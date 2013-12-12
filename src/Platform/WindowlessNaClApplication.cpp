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

#include "WindowlessNaClApplication.h"

#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/completion_callback.h>
#include <Utility/NaClStreamBuffer.h>

#include "Context.h"

namespace Magnum { namespace Platform {

struct WindowlessNaClApplication::ConsoleDebugOutput {
    explicit ConsoleDebugOutput(pp::Instance* instance);

    Utility::NaClConsoleStreamBuffer debugBuffer, warningBuffer, errorBuffer;
    std::ostream debugOutput, warningOutput, errorOutput;
};

WindowlessNaClApplication::ConsoleDebugOutput::ConsoleDebugOutput(pp::Instance* instance): debugBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Log), warningBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Warning), errorBuffer(instance, Utility::NaClConsoleStreamBuffer::LogLevel::Error), debugOutput(&debugBuffer), warningOutput(&warningBuffer), errorOutput(&errorBuffer) {
    /* Inform about this change on standard output */
    Debug() << "Platform::WindowlessNaClApplication: redirecting Debug, Warning and Error output to JavaScript console";

    Debug::setOutput(&debugOutput);
    Warning::setOutput(&warningOutput);
    Error::setOutput(&errorOutput);
}

/** @todo Delegating constructor when support for GCC 4.6 is dropped */

WindowlessNaClApplication::WindowlessNaClApplication(const Arguments& arguments, const Configuration& configuration): Instance(arguments), Graphics3DClient(this), graphics(nullptr), c(nullptr) {
    debugOutput = new ConsoleDebugOutput(this);
    createContext(configuration);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessNaClApplication::WindowlessNaClApplication(const Arguments& arguments): Instance(arguments), Graphics3DClient(this), c(nullptr) {
    debugOutput = new ConsoleDebugOutput(this);
    /* GCC 4.5 can't handle {} here (wtf) */
    createContext(Configuration());
}
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
WindowlessNaClApplication::WindowlessNaClApplication(const Arguments& arguments, std::nullptr_t)
#else
WindowlessNaClApplication::WindowlessNaClApplication(const Arguments& arguments, void*)
#endif
    : Instance(arguments), Graphics3DClient(this), graphics(nullptr), c(nullptr)
{
    debugOutput = new ConsoleDebugOutput(this);
}

void WindowlessNaClApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) {
        Error() << "Platform::WindowlessNaClApplication::createContext(): cannot create context";
        std::exit(1);
    }
}

bool WindowlessNaClApplication::tryCreateContext(const Configuration&) {
    CORRADE_ASSERT(!c, "Platform::WindowlessNaClApplication::tryCreateContext(): context already created", false);

    const std::int32_t attributes[] = {
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
        PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
        PP_GRAPHICS3DATTRIB_WIDTH, 1,
        PP_GRAPHICS3DATTRIB_HEIGHT, 1,
        PP_GRAPHICS3DATTRIB_NONE
    };

    graphics = new pp::Graphics3D(this, attributes);
    if(graphics->is_null()) {
        delete graphics;
        graphics = nullptr;
        return false;
    }
    if(!BindGraphics(*graphics)) {
        Error() << "Platform::WindowlessNaClApplication::tryCreateContext(): cannot bind graphics";
        std::exit(1);
    }

    glSetCurrentContextPPAPI(graphics->pp_resource());

    c = new Context;
    return true;
}

WindowlessNaClApplication::~WindowlessNaClApplication() {
    delete c;
    delete graphics;
    delete debugOutput;
}

bool WindowlessNaClApplication::Init(uint32_t , const char* , const char*) {
    return exec() == 0;
}

}}
