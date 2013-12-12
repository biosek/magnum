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

#include "WindowlessGlxApplication.h"

#include <Utility/Assert.h>
#include <Utility/Debug.h>

#include "Context.h"

#define None 0L // redef Xlib nonsense

namespace Magnum { namespace Platform {

/** @todo Delegating constructor when support for GCC 4.6 is dropped */

WindowlessGlxApplication::WindowlessGlxApplication(const Arguments&, const Configuration& configuration): c(nullptr) {
    createContext(configuration);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessGlxApplication::WindowlessGlxApplication(const Arguments&): c(nullptr) {
    /* GCC 4.5 can't handle {} here (wtf) */
    createContext(Configuration());
}
#endif

#ifndef CORRADE_GCC45_COMPATIBILITY
WindowlessGlxApplication::WindowlessGlxApplication(const Arguments&, std::nullptr_t)
#else
WindowlessGlxApplication::WindowlessGlxApplication(const Arguments&, void*)
#endif
    : c(nullptr) {}

void WindowlessGlxApplication::createContext(const Configuration&) {
    CORRADE_ASSERT(!c, "WindowlessGlxApplication::createContext(): context already created", );

    display = XOpenDisplay(nullptr);

    /* Check version */
    int major, minor;
    glXQueryVersion(display, &major, &minor);
    if(major == 1 && minor < 4) {
        Error() << "WindowlessGlxApplication: GLX version 1.4 or greater is required.";
        std::exit(1);
    }

    /* Choose config */
    int configCount = 0;
    static const int fbAttributes[] = { None };
    GLXFBConfig* configs = glXChooseFBConfig(display, DefaultScreen(display), fbAttributes, &configCount);
    if(!configCount) {
        Error() << "WindowlessGlxApplication: no supported framebuffer configuration found.";
        std::exit(1);
    }

    GLint contextAttributes[] = {
        #ifdef MAGNUM_TARGET_GLES
        GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
        #endif
        0
    };

    /** @todo Use some extension wrangler for this, not GLEW, as it apparently needs context to create context, yo dawg wtf. */
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
    context = glXCreateContextAttribsARB(display, configs[0], nullptr, True, contextAttributes);
    if(!context) {
        Error() << "WindowlessGlxApplication: cannot create context.";
        std::exit(1);
    }

    /* Create pbuffer */
    int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH,  32,
        GLX_PBUFFER_HEIGHT, 32,
        None
    };
    pbuffer = glXCreatePbuffer(display, configs[0], pbufferAttributes);

    XFree(configs);

    /* Set OpenGL context as current */
    if(!glXMakeContextCurrent(display, pbuffer, pbuffer, context)) {
        Error() << "WindowlessGlxApplication: cannot make context current";
        std::exit(1);
    }

    c = new Context;
}

WindowlessGlxApplication::~WindowlessGlxApplication() {
    delete c;

    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);
}

WindowlessGlxApplication::Configuration::Configuration() = default;
WindowlessGlxApplication::Configuration::~Configuration() = default;

}}
