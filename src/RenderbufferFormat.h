#ifndef Magnum_RenderbufferFormat_h
#define Magnum_RenderbufferFormat_h
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

/** @file
 * @brief Enum Magnum::RenderbufferFormat
 */

#include "OpenGL.h"

namespace Magnum {

/**
@brief Internal renderbuffer format

@see Renderbuffer
@requires_gl30 %Extension @extension{ARB,framebuffer_object}
@todo RGB, RGB8 ES only (ES3 + @es_extension{OES,rgb8_rgba8})
 */
enum class RenderbufferFormat: GLenum {
    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned, size implementation-dependent.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref Magnum::RenderbufferFormat "RenderbufferFormat::R8".
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Use exactly specified format in OpenGL ES instead.
     */
    Red = GL_RED,
    #endif

    /**
     * Red component, normalized unsigned byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
     */
    #ifndef MAGNUM_TARGET_GLES2
    R8 = GL_R8,
    #else
    R8 = GL_R8_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red and green component, normalized unsigned, size
     * implementation-dependent.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref Magnum::RenderbufferFormat "RenderbufferFormat::RG8".
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Use exactly specified format in OpenGL ES instead.
     */
    RG = GL_RG,
    #endif

    /**
     * Red and green component, each normalized unsigned byte.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gles30 %Extension @es_extension{EXT,texture_rg}
     */
    #ifndef MAGNUM_TARGET_GLES2
    RG8 = GL_RG8,
    #else
    RG8 = GL_RG8_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGBA, normalized unsigned, size implementation-dependent.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref Magnum::RenderbufferFormat "RenderbufferFormat::RGBA8".
     * @requires_gl Use exactly specified format in OpenGL ES 2.0 instead.
     */
    RGBA = GL_RGBA,
    #endif

    /**
     * RGBA, each component normalized unsigned byte.
     * @requires_gles30 %Extension @es_extension{ARM,rgba8} or @es_extension{OES,rgb8_rgba8}
     */
    #ifndef MAGNUM_TARGET_GLES2
    RGBA8 = GL_RGBA8,
    #else
    RGBA8 = GL_RGBA8_OES,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, normalized unsigned short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    R16 = GL_R16,

    /**
     * Red and green component, each normalized unsigned short.
     * @requires_gl30 %Extension @extension{ARB,texture_rg}
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RG16 = GL_RG16,

    /**
     * RGB, each component normalized unsigned short.
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RGB16 = GL_RGB16,

    /**
     * RGBA, each component normalized unsigned short.
     * @requires_gl Only byte-sized normalized formats are available in OpenGL
     *      ES.
     */
    RGBA16 = GL_RGBA16,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Red component, non-normalized unsigned byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R8UI = GL_R8UI,

    /**
     * Red and green component, each non-normalized unsigned byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG8UI = GL_RG8UI,

    /**
     * RGBA, each component non-normalized unsigned byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA8UI = GL_RGBA8UI,

    /**
     * Red component, non-normalized signed byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R8I = GL_R8I,

    /**
     * Red and green component, each non-normalized signed byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG8I = GL_RG8I,

    /**
     * RGBA, each component non-normalized signed byte.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA8I = GL_RGBA8I,

    /**
     * Red component, non-normalized unsigned short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R16UI = GL_R16UI,

    /**
     * Red and green component, each non-normalized unsigned short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG16UI = GL_RG16UI,

    /**
     * RGBA, each component non-normalized unsigned short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA16UI = GL_RGBA16UI,

    /**
     * Red component, non-normalized signed short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R16I = GL_R16I,

    /**
     * Red and green component, each non-normalized signed short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG16I = GL_RG16I,

    /**
     * RGBA, each component non-normalized signed short.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA16I = GL_RGBA16I,

    /**
     * Red component, non-normalized unsigned int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R32UI = GL_R32UI,

    /**
     * Red and green component, each non-normalized unsigned int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG32UI = GL_RG32UI,

    /**
     * RGBA, each component non-normalized unsigned int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA32UI = GL_RGBA32UI,

    /**
     * Red component, non-normalized signed int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    R32I = GL_R32I,

    /**
     * Red and green component, each non-normalized signed int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RG32I = GL_RG32I,

    /**
     * RGBA, each component non-normalized signed int.
     * @requires_gl30 %Extension @extension{EXT,texture_integer}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGBA32I = GL_RGBA32I,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Red component, half float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gl Only (non)normalized integral formats are available in
     *      OpenGL ES.
     */
    R16F = GL_R16F,

    /**
     * Red and green component, each half float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gl Only (non)normalized integral formats are available in
     *      OpenGL ES.
     */
    RG16F = GL_RG16F,

    /**
     * RGBA, each component half float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gl Only (non)normalized integral formats are available in
     *      OpenGL ES.
     */
    RGBA16F = GL_RGBA16F,

    /**
     * Red component, float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gl Only (non)normalized integral formats are available in
     *      OpenGL ES.
     */
    R32F = GL_R32F,

    /**
     * Red and green component, each float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gl Only (non)normalized integral formats are available in
     *      OpenGL ES.
     */
    RG32F = GL_RG32F,

    /**
     * RGBA, each component float.
     * @requires_gl30 %Extension @extension{ARB,texture_float}
     * @requires_gl Only (non)normalized integral formats are available in
     *      OpenGL ES.
     */
    RGBA32F = GL_RGBA32F,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * RGBA, normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gles30 Usable only as internal texture format in OpenGL ES
     *      2.0, see @ref Magnum::TextureFormat "TextureFormat::RGB10A2".
     */
    RGB10A2 = GL_RGB10_A2,

    /**
     * RGBA, non-normalized unsigned, each RGB component 10bit, alpha 2bit.
     * @requires_gl33 %Extension @extension{ARB,texture_rgb10_a2ui}
     * @requires_gles30 Only normalized integral formats are available in
     *      OpenGL ES 2.0.
     */
    RGB10A2UI = GL_RGB10_A2UI,
    #endif

    /** RGBA, normalized unsigned, each RGB component 5bit, alpha 1bit. */
    RGB5A1 = GL_RGB5_A1,

    /** RGBA, normalized unsigned, each component 4bit. */
    RGBA4 = GL_RGBA4,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * RGB, float, red and green 11bit, blue 10bit.
     * @requires_gl30 %Extension @extension{EXT,packed_float}
     * @requires_gl Usable only as internal texture format in OpenGL ES, see
     *      @ref Magnum::TextureFormat "TextureFormat::R11FG11FB10F".
     */
    R11FG11FB10F = GL_R11F_G11F_B10F,
    #endif

    /** RGB, normalized unsigned, red and blue 5bit, green 6bit. */
    RGB565 = GL_RGB565,

    /**
     * sRGBA, each component normalized unsigned byte.
     * @requires_gles30 %Extension @es_extension{EXT,sRGB}
     */
    #ifndef MAGNUM_TARGET_GLES2
    SRGB8Alpha8 = GL_SRGB8_ALPHA8,
    #else
    SRGB8Alpha8 = GL_SRGB8_ALPHA8_EXT,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Depth component, size implementation-dependent.
     * @todo is this allowed in core?
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref Magnum::RenderbufferFormat "RenderbufferFormat::DepthComponent16".
     * @requires_gl Use exactly specified format in OpenGL ES instead.
     */
    DepthComponent = GL_DEPTH_COMPONENT,
    #endif

    /** Depth component, 16bit. */
    DepthComponent16 = GL_DEPTH_COMPONENT16,

    /**
     * Depth component, 24bit.
     * @requires_gles30 %Extension @es_extension{OES,depth24}
     */
    #ifndef MAGNUM_TARGET_GLES2
    DepthComponent24 = GL_DEPTH_COMPONENT24,
    #else
    DepthComponent24 = GL_DEPTH_COMPONENT24_OES,
    #endif

    /**
     * Depth component, 32bit.
     * @requires_es_extension %Extension @es_extension{OES,depth32}
     */
    #ifndef MAGNUM_TARGET_GLES
    DepthComponent32 = GL_DEPTH_COMPONENT32,
    #else
    DepthComponent32 = GL_DEPTH_COMPONENT32_OES,
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Depth component, 32bit float.
     * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     */
    DepthComponent32F = GL_DEPTH_COMPONENT32F,
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Stencil index, size implementation-dependent.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref Magnum::RenderbufferFormat "RenderbufferFormat::StencilIndex8".
     * @requires_gl Use exactly specified format in OpenGL ES instead.
     */
    StencilIndex = GL_STENCIL_INDEX,
    #endif

    /**
     * 1-bit stencil index.
     * @requires_es_extension %Extension @es_extension{OES,stencil1}
     */
    #ifndef MAGNUM_TARGET_GLES
    StencilIndex1 = GL_STENCIL_INDEX1,
    #else
    StencilIndex1 = GL_STENCIL_INDEX1_OES,
    #endif

    /**
     * 4-bit stencil index.
     * @requires_es_extension %Extension @es_extension{OES,stencil4}
     */
    #ifndef MAGNUM_TARGET_GLES
    StencilIndex4 = GL_STENCIL_INDEX4,
    #else
    StencilIndex4 = GL_STENCIL_INDEX4_OES,
    #endif

    /** 8-bit stencil index. */
    StencilIndex8 = GL_STENCIL_INDEX8,

    #ifndef MAGNUM_TARGET_GLES
    /**
     * 16-bit stencil index.
     * @requires_gl At most 8bit stencil index is available in OpenGL ES.
     */
    StencilIndex16 = GL_STENCIL_INDEX16,

    /**
     * Depth and stencil component, size implementation-dependent.
     * @deprecated_gl Prefer to use the exactly specified version of this
     *      format, e.g. @ref Magnum::RenderbufferFormat "RenderbufferFormat::Depth24Stencil8".
     * @requires_gl Use exactly specified format in OpenGL ES instead.
     */
    DepthStencil = GL_DEPTH_STENCIL,
    #endif

    /**
     * 24bit depth and 8bit stencil component.
     * @requires_gles30 %Extension @es_extension{OES,packed_depth_stencil}
     */
    #ifdef MAGNUM_TARGET_GLES2
    Depth24Stencil8 = GL_DEPTH24_STENCIL8_OES
    #else
    Depth24Stencil8 = GL_DEPTH24_STENCIL8,

    /**
     * 32bit float depth component and 8bit stencil component.
     * @requires_gl30 %Extension @extension{ARB,depth_buffer_float}
     * @requires_gles30 Only integral depth textures are available in OpenGL ES
     *      2.0.
     */
    Depth32FStencil8 = GL_DEPTH32F_STENCIL8
    #endif
};

}

#endif
