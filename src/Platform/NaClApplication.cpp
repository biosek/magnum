/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "NaClApplication.h"

#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/fullscreen.h>
#include <ppapi/cpp/completion_callback.h>
#include <Utility/NaClStreamBuffer.h>

#include "Context.h"
#include "Platform/ScreenedApplication.hpp"

namespace Magnum { namespace Platform {

struct NaClApplication::ConsoleDebugOutput {
    explicit ConsoleDebugOutput(pp::Instance* instance);

    Utility::NaClConsoleStreamBuffer debugBuffer, warningBuffer, errorBuffer;
    std::ostream debugOutput, warningOutput, errorOutput;
};

NaClApplication::ConsoleDebugOutput::ConsoleDebugOutput(pp::Instance* instance): debugBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Log), warningBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Warning), errorBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Error), debugOutput(&debugBuffer), warningOutput(&warningBuffer), errorOutput(&errorBuffer) {
    /* Inform about this change on standard output */
    Debug() << "Platform::NaClApplication: redirecting Debug, Warning and Error output to JavaScript console";

    Debug::setOutput(&debugOutput);
    Warning::setOutput(&warningOutput);
    Error::setOutput(&errorOutput);
}

/** @todo Delegating constructor when support for GCC 4.6 is dropped */

NaClApplication::NaClApplication(const Arguments& arguments, const Configuration& configuration): Instance(arguments), Graphics3DClient(this), MouseLock(this), graphics(nullptr), fullscreen(nullptr), c(nullptr) {
    debugOutput = new ConsoleDebugOutput(this);
    createContext(configuration);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
NaClApplication::NaClApplication(const Arguments& arguments): Instance(arguments), Graphics3DClient(this), MouseLock(this), c(nullptr) {
    debugOutput = new ConsoleDebugOutput(this);
    /* GCC 4.5 can't handle {} here (wtf) */
    createContext(Configuration());
}
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
NaClApplication::NaClApplication(const Arguments& arguments, std::nullptr_t)
#else
NaClApplication::NaClApplication(const Arguments& arguments, void*)
#endif
    : Instance(arguments), Graphics3DClient(this), MouseLock(this), c(nullptr)
{
    debugOutput = new ConsoleDebugOutput(this);
}

void NaClApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) {
        Error() << "Platform::NaClApplication::createContext(): cannot create context";
        std::exit(1);
    }
}

bool NaClApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(!c, "Platform::NaClApplication::tryCreateContext(): context already created", false);

    viewportSize = configuration.size();

    const std::int32_t attributes[] = {
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
        PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
        PP_GRAPHICS3DATTRIB_SAMPLES, configuration.sampleCount(),
        PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, configuration.sampleCount() > 1 ? 1 : 0,
        PP_GRAPHICS3DATTRIB_WIDTH, configuration.size().x(),
        PP_GRAPHICS3DATTRIB_HEIGHT, configuration.size().y(),
        PP_GRAPHICS3DATTRIB_NONE
    };

    graphics = new pp::Graphics3D(this, attributes);
    if(graphics->is_null()) {
        delete graphics;
        graphics = nullptr;
        return false;
    }
    if(!BindGraphics(*graphics)) {
        Error() << "Platform::NaClApplication::tryCreateContext(): cannot bind graphics";
        std::exit(1);
    }

    fullscreen = new pp::Fullscreen(this);

    glSetCurrentContextPPAPI(graphics->pp_resource());

    /* Enable input handling for mouse and keyboard */
    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE|PP_INPUTEVENT_CLASS_WHEEL);
    RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);

    c = new Context;
    return true;
}

NaClApplication::~NaClApplication() {
    delete c;
    delete fullscreen;
    delete graphics;
    delete debugOutput;
}

bool NaClApplication::isFullscreen() {
    return fullscreen->IsFullscreen();
}

bool NaClApplication::setFullscreen(bool enabled) {
    /* Given fullscreen mode already set or switching to it is in progress, done */
    if(isFullscreen() == enabled || ((flags & Flag::FullscreenSwitchInProgress) && (flags & Flag::WillBeFullscreen) == enabled))
        return true;

    /* Switch to opposite fullscreen mode is in progress, can't revert it back */
    if((flags & Flag::FullscreenSwitchInProgress) && (flags & Flag::WillBeFullscreen) != enabled)
        return false;

    /* Set fullscreen */
    if(!fullscreen->SetFullscreen(enabled))
        return false;

    /* Set flags */
    flags |= Flag::FullscreenSwitchInProgress;
    enabled ? flags |= Flag::WillBeFullscreen : flags &= ~Flag::WillBeFullscreen;
    return true;
}

void NaClApplication::DidChangeView(const pp::View& view) {
    /* Fullscreen switch in progress */
    if(flags & Flag::FullscreenSwitchInProgress) {
        /* Done, remove the progress flag */
        if(isFullscreen() == bool(flags & Flag::WillBeFullscreen)) {
            flags &= ~Flag::FullscreenSwitchInProgress;
            flags |= Flag::Redraw;
        }

        /* Don't process anything during the switch */
        else return;
    }

    Vector2i size(view.GetRect().width(), view.GetRect().height());

    /* Canvas resized */
    if(viewportSize != size) {
        graphics->ResizeBuffers(size.x(), size.y());
        viewportEvent(viewportSize = size);
    }

    drawEvent();
}

bool NaClApplication::HandleInputEvent(const pp::InputEvent& event) {
    /* Don't handle anything during switch from/to fullscreen */
    if(flags & Flag::FullscreenSwitchInProgress) return false;

    Flags tmpFlags = flags;

    switch(event.GetType()) {
        case PP_INPUTEVENT_TYPE_KEYDOWN:
        case PP_INPUTEVENT_TYPE_KEYUP: {
            pp::KeyboardInputEvent keyEvent(event);
            KeyEvent e(static_cast<KeyEvent::Key>(keyEvent.GetKeyCode()), static_cast<InputEvent::Modifier>(keyEvent.GetModifiers()));
            event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN ? keyPressEvent(e) : keyReleaseEvent(e);
            if(!e.isAccepted()) return false;
            break;
        }

        case PP_INPUTEVENT_TYPE_MOUSEDOWN:
        case PP_INPUTEVENT_TYPE_MOUSEUP: {
            pp::MouseInputEvent mouseEvent(event);
            MouseEvent e(static_cast<MouseEvent::Button>(mouseEvent.GetButton()), {mouseEvent.GetPosition().x(), mouseEvent.GetPosition().y()}, static_cast<InputEvent::Modifier>(mouseEvent.GetModifiers()));
            event.GetType() == PP_INPUTEVENT_TYPE_MOUSEDOWN ? mousePressEvent(e) : mouseReleaseEvent(e);
            if(!e.isAccepted()) return false;
            break;
        }

        case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
            pp::MouseInputEvent mouseEvent(event);
            MouseMoveEvent e({mouseEvent.GetPosition().x(), mouseEvent.GetPosition().y()},  {mouseEvent.GetMovement().x(), mouseEvent.GetMovement().y()}, static_cast<InputEvent::Modifier>(mouseEvent.GetModifiers()));
            mouseMoveEvent(e);
            if(!e.isAccepted()) return false;
            break;
        }

        default: return false;
    }

    /* Assume everything is properly sequential here */
    CORRADE_INTERNAL_ASSERT((tmpFlags & Flag::SwapInProgress) == (flags & Flag::SwapInProgress));

    /* Redraw, if it won't be handled after swap automatically */
    if((flags & Flag::Redraw) && !(flags & Flag::SwapInProgress)) {
        flags &= ~Flag::Redraw;
        drawEvent();
    }

    return true;
}

void NaClApplication::swapBuffers() {
    /* Swap already in progress, do nothing */
    if(flags & Flag::SwapInProgress) return;

    /* Swap buffers and call swapCallback() when done */
    flags |= Flag::SwapInProgress;
    graphics->SwapBuffers(pp::CompletionCallback(&swapCallback, this));
}

void NaClApplication::swapCallback(void* applicationInstance, std::int32_t) {
    NaClApplication* instance = static_cast<NaClApplication*>(applicationInstance);
    instance->flags &= ~Flag::SwapInProgress;

    /* Redraw, if requested */
    if(instance->flags & Flag::Redraw) {
        instance->flags &= ~Flag::Redraw;
        instance->drawEvent();
    }
}

void NaClApplication::setMouseLocked(bool enabled) {
    /* Already done, nothing to do */
    if(enabled == isMouseLocked()) return;

    if(enabled) LockMouse(pp::CompletionCallback(&mouseLockCallback, this));
    else UnlockMouse();
}

void NaClApplication::mouseLockCallback(void* applicationInstance, std::int32_t) {
    NaClApplication* instance = static_cast<NaClApplication*>(applicationInstance);
    instance->flags |= Flag::MouseLocked;
}

void NaClApplication::keyPressEvent(KeyEvent&) {}
void NaClApplication::keyReleaseEvent(KeyEvent&) {}
void NaClApplication::mousePressEvent(MouseEvent&) {}
void NaClApplication::mouseReleaseEvent(MouseEvent&) {}
void NaClApplication::mouseMoveEvent(MouseMoveEvent&) {}

template class BasicScreen<NaClApplication>;
template class BasicScreenedApplication<NaClApplication>;

}}
