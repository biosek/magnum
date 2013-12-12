#ifndef Magnum_AbstractFramebuffer_h
#define Magnum_AbstractFramebuffer_h
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
 * @brief Class Magnum::AbstractFramebuffer
 */

#include <Containers/EnumSet.h>

#include "Math/Range.h"
#include "Magnum.h"
#include "OpenGL.h"

#ifdef CORRADE_GCC45_COMPATIBILITY
#include "Buffer.h"
#include "ColorFormat.h"
#endif

namespace Magnum {

/**
@brief Mask for framebuffer clearing

@see AbstractFramebuffer, FramebufferClearMask
*/
enum class FramebufferClear: GLbitfield {
    Color = GL_COLOR_BUFFER_BIT,    /**< Color */
    Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value */
    Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil value */
};

/**
@brief Mask for clearing

@see AbstractFramebuffer::clear()
*/
typedef Containers::EnumSet<FramebufferClear, GLbitfield,
    GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> FramebufferClearMask;

/**
@brief Mask for framebuffer blitting

@see AbstractFramebuffer, FramebufferBlitMask
@requires_gl30 %Extension @extension{ARB,framebuffer_object}
@requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
    @es_extension{NV,framebuffer_blit}
*/
enum class FramebufferBlit: GLbitfield {
    ColorBuffer = GL_COLOR_BUFFER_BIT,    /**< Color buffer */
    DepthBuffer = GL_DEPTH_BUFFER_BIT,    /**< Depth buffer */
    StencilBuffer = GL_STENCIL_BUFFER_BIT /**< Stencil buffer */
};

/**
@brief Mask for framebuffer blitting

@see AbstractFramebuffer::blit()
@requires_gl30 %Extension @extension{ARB,framebuffer_object}
@requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
    @es_extension{NV,framebuffer_blit}
*/
typedef Containers::EnumSet<FramebufferBlit, GLbitfield,
    GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> FramebufferBlitMask;

/**
@brief %Framebuffer blit filtering

@see AbstractFramebuffer::blit()
@requires_gl30 %Extension @extension{ARB,framebuffer_object}
@requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
    @es_extension{NV,framebuffer_blit}
*/
enum class FramebufferBlitFilter: GLenum {
    Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */
    Linear = GL_LINEAR      /**< Linear interpolation filtering */
};

/**
@brief Target for binding framebuffer

@see DefaultFramebuffer::bind(), Framebuffer::bind()
@requires_gl30 %Extension @extension{ARB,framebuffer_object}
*/
enum class FramebufferTarget: GLenum {
    /**
     * For reading only.
     * @requires_gles30 %Extension @es_extension{APPLE,framebuffer_multisample},
     *      @es_extension{ANGLE,framebuffer_blit} or @es_extension{NV,framebuffer_blit}
     */
    #ifndef MAGNUM_TARGET_GLES2
    Read = GL_READ_FRAMEBUFFER,
    #else
    Read = GL_READ_FRAMEBUFFER_APPLE,
    #endif

    /**
     * For drawing only.
     * @requires_gles30 %Extension @es_extension{APPLE,framebuffer_multisample},
     *      @es_extension{ANGLE,framebuffer_blit} or @es_extension{NV,framebuffer_blit}
     */
    #ifndef MAGNUM_TARGET_GLES2
    Draw = GL_DRAW_FRAMEBUFFER,
    #else
    Draw = GL_DRAW_FRAMEBUFFER_APPLE,
    #endif

    ReadDraw = GL_FRAMEBUFFER       /**< For both reading and drawing. */
};

/**
@brief Base for default and named framebuffers

See DefaultFramebuffer and Framebuffer for more information.

@section AbstractFramebuffer-performance-optimization Performance optimizations and security

The engine tracks currently bound framebuffer and current viewport to avoid
unnecessary calls to @fn_gl{BindFramebuffer} and @fn_gl{Viewport} when
switching framebuffers. %Framebuffer limits and implementation-defined values
(such as @ref maxViewportSize()) are cached, so repeated queries don't result
in repeated @fn_gl{Get} calls.

If @extension{ARB,robustness} is available, read() operations are protected
from buffer overflow.
@todo @extension{ARB,viewport_array} (and `GL_MAX_VIEWPORTS`)
*/
class MAGNUM_EXPORT AbstractFramebuffer {
    friend class Context;

    AbstractFramebuffer(const AbstractFramebuffer&) = delete;
    AbstractFramebuffer(AbstractFramebuffer&&) = delete;
    AbstractFramebuffer& operator=(const AbstractFramebuffer&) = delete;
    AbstractFramebuffer& operator=(AbstractFramebuffer&&) = delete;

    public:
        /** @todo `GL_IMPLEMENTATION_COLOR_READ_FORMAT`, `GL_IMPLEMENTATION_COLOR_READ_TYPE`, seems to be depending on currently bound FB (aargh) (@extension{ARB,ES2_compatibility}). */

        /**
         * @brief Max supported viewport size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setViewport(), @fn_gl{Get} with @def_gl{MAX_VIEWPORT_DIMS}
         */
        static Vector2i maxViewportSize();

        /**
         * @brief Max supported draw buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If ES extension @extension{NV,draw_buffers} is not
         * available, returns `0`.
         * @see @ref DefaultFramebuffer::mapForDraw(), @ref Framebuffer::mapForDraw(),
         *      @fn_gl{Get} with @def_gl{MAX_DRAW_BUFFERS}
         */
        static Int maxDrawBuffers();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported dual-source draw buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,blend_func_extended} is
         * not available, returns `0`.
         * @see @ref DefaultFramebuffer::mapForDraw(), @ref Framebuffer::mapForDraw(),
         *      @fn_gl{Get} with @def_gl{MAX_DUAL_SOURCE_DRAW_BUFFERS}
         * @requires_gl Multiple blending inputs are not available in
         *      OpenGL ES.
         */
        static Int maxDualSourceDrawBuffers();
        #endif

        /**
         * @brief Copy block of pixels
         * @param source            Source framebuffer
         * @param destination       Destination framebuffer
         * @param sourceRectangle   Source rectangle
         * @param destinationRectangle Destination rectangle
         * @param mask              Which buffers to perform blit operation on
         * @param filter            Interpolation filter
         *
         * Binds @p source framebuffer to @ref FramebufferTarget::Read and
         * @p destination framebuffer to @ref FramebufferTarget::Draw and
         * performs blitting operation. See @ref DefaultFramebuffer::mapForRead(),
         * @ref Framebuffer::mapForRead(), @ref DefaultFramebuffer::mapForDraw()
         * and @ref Framebuffer::mapForDraw() for specifying particular buffers
         * for blitting operation.
         * @see @fn_gl{BlitFramebuffer}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
         *      @es_extension{NV,framebuffer_blit}
         */
        static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);

        /**
         * @brief Copy block of pixels
         * @param source            Source framebuffer
         * @param destination       Destination framebuffer
         * @param rectangle         Source and destination rectangle
         * @param mask              Which buffers to perform blit operation on
         *
         * Convenience alternative to above function when source rectangle is
         * the same as destination rectangle. As the image is copied
         * pixel-by-pixel, no interpolation is needed and thus
         * @ref FramebufferBlitFilter::Nearest filtering is used by default.
         * @see @fn_gl{BlitFramebuffer}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
         *      @es_extension{NV,framebuffer_blit}
         */
        static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& rectangle, FramebufferBlitMask mask) {
            blit(source, destination, rectangle, rectangle, mask, FramebufferBlitFilter::Nearest);
        }

        /**
         * @brief Bind framebuffer for rendering
         *
         * Binds the framebuffer and updates viewport to saved dimensions.
         * @see setViewport(), DefaultFramebuffer::mapForRead(),
         *      Framebuffer::mapForRead(), DefaultFramebuffer::mapForDraw(),
         *      Framebuffer::mapForDraw(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Viewport}
         * @todo Bind internally to ReadDraw if separate binding points are not
         *      supported
         */
        void bind(FramebufferTarget target);

        /** @brief Viewport rectangle */
        Range2Di viewport() const { return _viewport; }

        /**
         * @brief Set viewport
         * @return Reference to self (for method chaining)
         *
         * Saves the viewport to be used at later time in bind(). If the
         * framebuffer is currently bound, updates the viewport to given
         * rectangle.
         * @see @ref maxViewportSize(), @fn_gl{Viewport}
         */
        AbstractFramebuffer& setViewport(const Range2Di& rectangle);

        /**
         * @brief Clear specified buffers in framebuffer
         * @param mask              Which buffers to clear
         *
         * To improve performance you can also use
         * DefaultFramebuffer::invalidate() / Framebuffer::invalidate() instead
         * of clearing given buffer if you will not use it anymore or fully
         * overwrite it later.
         * @see Renderer::setClearColor(), Renderer::setClearDepth(),
         *      Renderer::setClearStencil(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Clear}
         */
        void clear(FramebufferClearMask mask);

        /**
         * @brief Read block of pixels from framebuffer to image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param image             %Image where to put the data
         *
         * %Image parameters like format and type of pixel data are taken from
         * given image.
         *
         * If @extension{ARB,robustness} is available, the operation is
         * protected from buffer overflow.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{ReadPixels} or
         *      @fn_gl_extension{ReadnPixels,ARB,robustness}
         */
        void read(const Vector2i& offset, const Vector2i& size, Image2D& image);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Read block of pixels from framebuffer to buffer image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param image             %Buffer image where to put the data
         * @param usage             %Buffer usage
         *
         * See read(const Vector2i&, const Vector2i&, Image2D*) for more
         * information.
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
         */
        void read(const Vector2i& offset, const Vector2i& size, BufferImage2D& image, BufferUsage usage);
        #endif

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        explicit AbstractFramebuffer();
        ~AbstractFramebuffer();

        void MAGNUM_LOCAL bindInternal(FramebufferTarget target);
        FramebufferTarget MAGNUM_LOCAL bindInternal();
        void MAGNUM_LOCAL setViewportInternal();

        #ifdef MAGNUM_TARGET_GLES2
        static MAGNUM_LOCAL FramebufferTarget readTarget;
        static MAGNUM_LOCAL FramebufferTarget drawTarget;
        #endif

        typedef GLenum(AbstractFramebuffer::*CheckStatusImplementation)(FramebufferTarget);
        static CheckStatusImplementation checkStatusImplementation;

        typedef void(AbstractFramebuffer::*DrawBuffersImplementation)(GLsizei, const GLenum*);
        static MAGNUM_LOCAL DrawBuffersImplementation drawBuffersImplementation;

        typedef void(AbstractFramebuffer::*DrawBufferImplementation)(GLenum);
        static DrawBufferImplementation drawBufferImplementation;

        typedef void(AbstractFramebuffer::*ReadBufferImplementation)(GLenum);
        static ReadBufferImplementation readBufferImplementation;

        void MAGNUM_LOCAL invalidateImplementation(GLsizei count, GLenum* attachments);
        void MAGNUM_LOCAL invalidateImplementation(GLsizei count, GLenum* attachments, const Range2Di& rectangle);

        GLuint _id;
        Range2Di _viewport;

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);

        GLenum MAGNUM_LOCAL checkStatusImplementationDefault(FramebufferTarget target);
        #ifndef MAGNUM_TARGET_GLES
        GLenum MAGNUM_LOCAL checkStatusImplementationDSA(FramebufferTarget target);
        #endif

        void MAGNUM_LOCAL drawBuffersImplementationDefault(GLsizei count, const GLenum* buffers);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBuffersImplementationDSA(GLsizei count, const GLenum* buffers);
        #endif

        void MAGNUM_LOCAL drawBufferImplementationDefault(GLenum buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBufferImplementationDSA(GLenum buffer);
        #endif

        void MAGNUM_LOCAL readBufferImplementationDefault(GLenum buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL readBufferImplementationDSA(GLenum buffer);
        #endif

        typedef void(*ReadImplementation)(const Vector2i&, const Vector2i&, ColorFormat, ColorType, std::size_t, GLvoid*);
        static void MAGNUM_LOCAL readImplementationDefault(const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        static void MAGNUM_LOCAL readImplementationRobustness(const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        static ReadImplementation MAGNUM_LOCAL readImplementation;
};

inline AbstractFramebuffer::AbstractFramebuffer() = default;
inline AbstractFramebuffer::~AbstractFramebuffer() = default;

CORRADE_ENUMSET_OPERATORS(FramebufferClearMask)
CORRADE_ENUMSET_OPERATORS(FramebufferBlitMask)

}

#endif
