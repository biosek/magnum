#ifndef Magnum_BufferTexture_h
#define Magnum_BufferTexture_h
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

#ifndef MAGNUM_TARGET_GLES
/** @file
 * @brief Class @ref Magnum::BufferTexture, enum @ref Magnum::BufferTextureFormat
 */
#endif

#include "AbstractTexture.h"

#ifndef MAGNUM_TARGET_GLES
namespace Magnum {

/**
@brief Internal buffer texture format

@see @ref BufferTexture
*/
enum class BufferTextureFormat: GLenum {
    /** Red component, normalized unsigned byte. */
    R8 = GL_R8,

    /** Red and green component, each normalized unsigned byte. */
    RG8 = GL_RG8,

    /** RGBA, each component normalized unsigned byte. */
    RGBA8 = GL_RGBA8,

    /** Red component, normalized unsigned short. */
    R16 = GL_R16,

    /** Red and green component, each normalized unsigned short. */
    RG16 = GL_RG16,

    /** RGBA, each component normalized unsigned short. */
    RGBA16 = GL_RGBA16,

    /** Red component, non-normalized unsigned byte. */
    R8UI = GL_R8UI,

    /** Red and green component, each non-normalized unsigned byte. */
    RG8UI = GL_RG8UI,

    /** RGBA, each component non-normalized unsigned byte. */
    RGBA8UI = GL_RGBA8UI,

    /** Red component, non-normalized signed byte. */
    R8I = GL_R8I,

    /** Red and green component, each non-normalized signed byte. */
    RG8I = GL_RG8I,

    /** RGBA, each component non-normalized signed byte. */
    RGBA8I = GL_RGBA8I,

    /** Red component, non-normalized unsigned short. */
    R16UI = GL_R16UI,

    /** Red and green component, each non-normalized unsigned short. */
    RG16UI = GL_RG16UI,

    /** RGBA, each component non-normalized unsigned short. */
    RGBA16UI = GL_RGBA16UI,

    /** Red component, non-normalized signed short. */
    R16I = GL_R16I,

    /** Red and green component, each non-normalized signed short. */
    RG16I = GL_RG16I,

    /** RGBA, each component non-normalized signed short. */
    RGBA16I = GL_RGBA16I,

    /** Red component, non-normalized unsigned int. */
    R32UI = GL_R32UI,

    /** Red and green component, each non-normalized unsigned int. */
    RG32UI = GL_RG32UI,

    /**
     * RGB, each component non-normalized unsigned int.
     * @requires_gl40 %Extension @extension{ARB,texture_buffer_object_rgb32}
     */
    RGB32UI = GL_RGB32UI,

    /** RGBA, each component non-normalized unsigned int. */
    RGBA32UI = GL_RGBA32UI,

    /** Red component, non-normalized signed int. */
    R32I = GL_R32I,

    /** Red and green component, each non-normalized signed int. */
    RG32I = GL_RG32I,

    /**
     * RGB, each component non-normalized signed int.
     * @requires_gl40 %Extension @extension{ARB,texture_buffer_object_rgb32}
     */
    RGB32I = GL_RGB32I,

    /** RGBA, each component non-normalized signed int. */
    RGBA32I = GL_RGBA32I,

    /** Red component, half float. */
    R16F = GL_R16F,

    /** Red and green component, each half float. */
    RG16F = GL_RG16F,

    /** RGBA, each component half float. */
    RGBA16F = GL_RGBA16F,

    /** Red component, float. */
    R32F = GL_R32F,

    /** Red and green component, each float. */
    RG32F = GL_RG32F,

    /**
     * RGB, each component float.
     * @requires_gl40 %Extension @extension{ARB,texture_buffer_object_rgb32}
     */
    RGB32F = GL_RGB32F,

    /** RGBA, each component float. */
    RGBA32F = GL_RGBA32F
};

/**
@brief %Buffer texture

This texture is, unlike classic textures such as @ref Texture or
@ref CubeMapTexture, used as simple data source, without any unnecessary
interpolation and wrapping methods.

@section BufferTexture-usage Usage

%Texture data are stored in buffer and after binding the buffer to the texture
using @ref setBuffer(), you can fill the buffer at any time using data setting
functions in Buffer itself.

Note that the buffer is not managed (e.g. deleted on destruction) by the
texture, so you have to manage it on your own and ensure that it is available
for whole texture lifetime. On the other hand it allows you to use one buffer
for more textures or store more than one data in it.

Example usage:
@code
Buffer buffer;
BufferTexture texture;
texture.setBuffer(BufferTextureFormat::RGB32F, buffer);

constexpr static Vector3 data[] = {
    // ...
};
buffer.setData(data, BufferUsage::StaticDraw);
@endcode

The texture is bound to layer specified by shader via @ref bind(). In shader,
the texture is used via `samplerBuffer`, `isamplerBuffer` or `usamplerBuffer`.
Unlike in classic textures, coordinates for buffer textures are integer
coordinates passed to `texelFetch()`. See also @ref AbstractShaderProgram
documentation for more information.

@section BufferTexture-performance-optimization Performance optimizations

If extension @extension{EXT,direct_state_access} is available, @ref setBuffer()
functions use DSA to avoid unnecessary calls to @fn_gl{ActiveTexture} and
@fn_gl{BindTexture}. See
@ref AbstractTexture-performance-optimization "relevant section in AbstractTexture documentation"
and respective function documentation for more information.

@see @ref Texture, @ref CubeMapTexture, @ref CubeMapTextureArray
@requires_gl31 %Extension @extension{ARB,texture_buffer_object}
@requires_gl Texture buffers are not available in OpenGL ES.
*/
class MAGNUM_EXPORT BufferTexture: private AbstractTexture {
    friend class Context;

    BufferTexture(const BufferTexture&) = delete;
    BufferTexture(BufferTexture&&) = delete;
    BufferTexture& operator=(const BufferTexture&) = delete;
    BufferTexture& operator=(BufferTexture&&) = delete;

    public:
        explicit BufferTexture(): AbstractTexture(GL_TEXTURE_BUFFER) {}

        /** @copydoc AbstractTexture::bind() */
        void bind(Int layer) { AbstractTexture::bind(layer); }

        /**
         * @brief Set texture buffer
         * @param internalFormat    Internal format
         * @param buffer            %Buffer with data
         *
         * Binds given buffer to this texture. The buffer itself can be then
         * filled with data of proper format at any time using @ref Buffer "Buffer"'s
         * own data setting functions.
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexBuffer}
         *      or @fn_gl_extension{TextureBuffer,EXT,direct_state_access}
         */
        void setBuffer(BufferTextureFormat internalFormat, Buffer& buffer) {
            (this->*setBufferImplementation)(internalFormat, buffer);
        }

        /**
         * @brief Set texture buffer
         * @param internalFormat    Internal format
         * @param buffer            %Buffer
         * @param offset            Offset
         * @param size              Data size
         *
         * Binds range of given buffer to this texture. The buffer itself can
         * be then filled with data of proper format at any time using @ref Buffer "Buffer"'s
         * own data setting functions.
         * @requires_gl43 %Extension @extension{ARB,texture_buffer_range}
         * @see @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and @fn_gl{TexBufferRange}
         *      or @fn_gl_extension{TextureBufferRange,EXT,direct_state_access}
         */
        void setBuffer(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size) {
            (this->*setBufferRangeImplementation)(internalFormat, buffer, offset, size);
        }

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);

        typedef void(BufferTexture::*SetBufferImplementation)(BufferTextureFormat, Buffer&);
        void MAGNUM_LOCAL setBufferImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer);
        void MAGNUM_LOCAL setBufferImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer);
        static SetBufferImplementation setBufferImplementation;

        typedef void(BufferTexture::*SetBufferRangeImplementation)(BufferTextureFormat, Buffer&, GLintptr, GLsizeiptr);
        void MAGNUM_LOCAL setBufferRangeImplementationDefault(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        void MAGNUM_LOCAL setBufferRangeImplementationDSA(BufferTextureFormat internalFormat, Buffer& buffer, GLintptr offset, GLsizeiptr size);
        static SetBufferRangeImplementation setBufferRangeImplementation;
};

}
#endif

#endif
