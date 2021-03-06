#ifndef Magnum_Version_h
#define Magnum_Version_h
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

/** @file Version.h
 * Enum @ref Magnum::Version, function @ref version()
 */

#include <utility>

#include "Magnum.h"
#include "magnumVisibility.h"

namespace Magnum {

/**
@brief OpenGL version

@see @ref Context, @ref MAGNUM_ASSERT_VERSION_SUPPORTED()
*/
enum class Version: Int {
    None = 0xFFFF,                  /**< @brief Unspecified */
    #ifndef MAGNUM_TARGET_GLES
    GL210 = 210,                    /**< @brief OpenGL 2.1 / GLSL 1.20 */
    GL300 = 300,                    /**< @brief OpenGL 3.0 / GLSL 1.30 */
    GL310 = 310,                    /**< @brief OpenGL 3.1 / GLSL 1.40 */
    GL320 = 320,                    /**< @brief OpenGL 3.2 / GLSL 1.50 */
    GL330 = 330,                    /**< @brief OpenGL 3.3, GLSL 3.30 */
    GL400 = 400,                    /**< @brief OpenGL 4.0, GLSL 4.00 */
    GL410 = 410,                    /**< @brief OpenGL 4.1, GLSL 4.10 */
    GL420 = 420,                    /**< @brief OpenGL 4.2, GLSL 4.20 */
    GL430 = 430,                    /**< @brief OpenGL 4.3, GLSL 4.30 */
    GL440 = 440,                    /**< @brief OpenGL 4.4, GLSL 4.40 */
    #endif

    /**
     * @brief OpenGL ES 2.0, GLSL ES 1.00
     *
     * All the functionality is present in OpenGL 4.2 (extension
     * @extension{ARB,ES2_compatibility}), so on desktop OpenGL this is
     * equivalent to @ref Version::GL410.
     */
    #ifndef MAGNUM_TARGET_GLES
    GLES200 = 410,
    #else
    GLES200 = 200,
    #endif

    /**
     * @brief OpenGL ES 3.0, GLSL ES 3.00
     *
     * All the functionality is present in OpenGL 4.3 (extension
     * @extension{ARB,ES3_compatibility}), so on desktop OpenGL this is the
     * equivalent to @ref Version::GL430.
     */
    #ifndef MAGNUM_TARGET_GLES
    GLES300 = 430
    #else
    GLES300 = 300
    #endif
};

/** @brief Enum value from major and minor version number */
inline Version version(Int major, Int minor) {
    return Version(major*100 + minor*10);
}

/** @brief Major and minor version number from enum value */
inline std::pair<Int, Int> version(Version version) {
    return {Int(version)/100, (Int(version)%100)/10};
}

/** @debugoperator{Magnum::Context} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Version value);

}

#endif
