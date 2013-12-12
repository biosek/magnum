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

#include "AbstractTexture.h"

#include "BufferImage.h"
#include "ColorFormat.h"
#include "Context.h"
#include "Extensions.h"
#include "Image.h"
#include "Shader.h"
#include "TextureFormat.h"
#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

AbstractTexture::BindImplementation AbstractTexture::bindImplementation =
    &AbstractTexture::bindImplementationDefault;
AbstractTexture::ParameteriImplementation AbstractTexture::parameteriImplementation =
    &AbstractTexture::parameterImplementationDefault;
AbstractTexture::ParameterfImplementation AbstractTexture::parameterfImplementation =
    &AbstractTexture::parameterImplementationDefault;
AbstractTexture::ParameterfvImplementation AbstractTexture::parameterfvImplementation =
    &AbstractTexture::parameterImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::GetLevelParameterivImplementation AbstractTexture::getLevelParameterivImplementation =
    &AbstractTexture::getLevelParameterImplementationDefault;
#endif
AbstractTexture::MipmapImplementation AbstractTexture::mipmapImplementation =
    &AbstractTexture::mipmapImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::Storage1DImplementation AbstractTexture::storage1DImplementation =
    &AbstractTexture::storageImplementationFallback;
#endif
AbstractTexture::Storage2DImplementation AbstractTexture::storage2DImplementation =
    &AbstractTexture::storageImplementationFallback;
AbstractTexture::Storage3DImplementation AbstractTexture::storage3DImplementation =
    &AbstractTexture::storageImplementationFallback;
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::GetImageImplementation AbstractTexture::getImageImplementation =
    &AbstractTexture::getImageImplementationDefault;
AbstractTexture::Image1DImplementation AbstractTexture::image1DImplementation =
    &AbstractTexture::imageImplementationDefault;
#endif
AbstractTexture::Image2DImplementation AbstractTexture::image2DImplementation =
    &AbstractTexture::imageImplementationDefault;
AbstractTexture::Image3DImplementation AbstractTexture::image3DImplementation =
    &AbstractTexture::imageImplementationDefault;
#ifndef MAGNUM_TARGET_GLES
AbstractTexture::SubImage1DImplementation AbstractTexture::subImage1DImplementation =
    &AbstractTexture::subImageImplementationDefault;
#endif
AbstractTexture::SubImage2DImplementation AbstractTexture::subImage2DImplementation =
    &AbstractTexture::subImageImplementationDefault;
AbstractTexture::SubImage3DImplementation AbstractTexture::subImage3DImplementation =
    &AbstractTexture::subImageImplementationDefault;

AbstractTexture::InvalidateImageImplementation AbstractTexture::invalidateImageImplementation = &AbstractTexture::invalidateImageImplementationNoOp;
AbstractTexture::InvalidateSubImageImplementation AbstractTexture::invalidateSubImageImplementation = &AbstractTexture::invalidateSubImageImplementationNoOp;

Int AbstractTexture::maxLayers() { return Shader::maxCombinedTextureImageUnits(); }

#ifndef MAGNUM_TARGET_GLES
Int AbstractTexture::maxColorSamples() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        return 0;

    GLint& value = Context::current()->state().texture->maxColorSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &value);

    return value;
}

Int AbstractTexture::maxDepthSamples() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        return 0;

    GLint& value = Context::current()->state().texture->maxDepthSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &value);

    return value;
}

Int AbstractTexture::maxIntegerSamples() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        return 0;

    GLint& value = Context::current()->state().texture->maxIntegerSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &value);

    return value;
}
#endif

void AbstractTexture::destroy() {
    /* Moved out */
    if(!_id) return;

    /* Remove all bindings */
    std::vector<GLuint>& bindings = Context::current()->state().texture->bindings;
    for(auto it = bindings.begin(); it != bindings.end(); ++it)
        if(*it == _id) *it = 0;

    glDeleteTextures(1, &_id);
}

void AbstractTexture::move() {
    _id = 0;
}

AbstractTexture::AbstractTexture(GLenum target): _target(target) {
    glGenTextures(1, &_id);
}

AbstractTexture::~AbstractTexture() { destroy(); }

AbstractTexture::AbstractTexture(AbstractTexture&& other): _target(other._target), _id(other._id) {
    other.move();
}

AbstractTexture& AbstractTexture::operator=(AbstractTexture&& other) {
    destroy();

    _target = other._target;
    _id = other._id;

    other.move();
    return *this;
}

void AbstractTexture::bind(Int layer) {
    Implementation::TextureState* const textureState = Context::current()->state().texture;

    /* If already bound in given layer, nothing to do */
    if(textureState->bindings[layer] == _id) return;

    (this->*bindImplementation)(layer);
}

void AbstractTexture::bindImplementationDefault(GLint layer) {
    Implementation::TextureState* const textureState = Context::current()->state().texture;

    /* Change to given layer, if not already there */
    if(textureState->currentLayer != layer)
        glActiveTexture(GL_TEXTURE0 + (textureState->currentLayer = layer));

    /* Bind the texture to the layer */
    glBindTexture(_target, (textureState->bindings[layer] = _id));
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::bindImplementationDSA(GLint layer) {
    glBindMultiTextureEXT(GL_TEXTURE0 + layer, _target, (Context::current()->state().texture->bindings[layer] = _id));
}
#endif

AbstractTexture& AbstractTexture::setMinificationFilter(Sampler::Filter filter, Sampler::Mipmap mipmap) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE || mipmap == Sampler::Mipmap::Base, "AbstractTexture: rectangle textures cannot have mipmaps", *this);
    #endif

    (this->*parameteriImplementation)(GL_TEXTURE_MIN_FILTER, GLint(filter)|GLint(mipmap));
    return *this;
}

AbstractTexture& AbstractTexture::generateMipmap() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(_target != GL_TEXTURE_RECTANGLE, "AbstractTexture: rectangle textures cannot have mipmaps", *this);
    #endif

    (this->*mipmapImplementation)();
    return *this;
}

void AbstractTexture::mipmapImplementationDefault() {
    bindInternal();
    glGenerateMipmap(_target);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::mipmapImplementationDSA() {
    glGenerateTextureMipmapEXT(_id, _target);
}
#endif

void AbstractTexture::bindInternal() {
    Implementation::TextureState* const textureState = Context::current()->state().texture;

    /* If the texture is already bound in current layer, nothing to do */
    if(textureState->bindings[textureState->currentLayer] == _id)
        return;

    /* Set internal layer as active if not already */
    const GLint internalLayer = maxLayers()-1;
    if(textureState->currentLayer != internalLayer)
        glActiveTexture(GL_TEXTURE0 + (textureState->currentLayer = internalLayer));

    /* Bind the texture to internal layer, if not already */
    if(textureState->bindings[internalLayer] != _id)
        glBindTexture(_target, (textureState->bindings[internalLayer] = _id));
}

void AbstractTexture::initializeContextBasedFunctionality(Context& context) {
    /* Resize bindings array to hold all possible layers */
    context.state().texture->bindings.resize(maxLayers());

    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "AbstractTexture: using" << Extensions::GL::EXT::direct_state_access::string() << "features";

        bindImplementation = &AbstractTexture::bindImplementationDSA;
        parameteriImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfvImplementation = &AbstractTexture::parameterImplementationDSA;
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDSA;
        mipmapImplementation = &AbstractTexture::mipmapImplementationDSA;
        getImageImplementation = &AbstractTexture::getImageImplementationDSA;
        image1DImplementation = &AbstractTexture::imageImplementationDSA;
        image2DImplementation = &AbstractTexture::imageImplementationDSA;
        image3DImplementation = &AbstractTexture::imageImplementationDSA;
        subImage1DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage2DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDSA;
    }

    if(context.isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>()) {
        Debug() << "AbstractTexture: using" << Extensions::GL::ARB::invalidate_subdata::string() << "features";

        invalidateImageImplementation = &AbstractTexture::invalidateImageImplementationARB;
        invalidateSubImageImplementation = &AbstractTexture::invalidateSubImageImplementationARB;
    }

    if(context.isExtensionSupported<Extensions::GL::ARB::robustness>() &&
       !context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        Debug() << "AbstractTexture: using" << Extensions::GL::ARB::robustness::string() << "features";

        getImageImplementation = &AbstractTexture::getImageImplementationRobustness;
    }

    if(context.isExtensionSupported<Extensions::GL::ARB::texture_storage>()) {
        Debug() << "AbstractTexture: using" << Extensions::GL::ARB::texture_storage::string() << "features";

        if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            storage1DImplementation = &AbstractTexture::storageImplementationDSA;
            storage2DImplementation = &AbstractTexture::storageImplementationDSA;
            storage3DImplementation = &AbstractTexture::storageImplementationDSA;
        } else {
            storage1DImplementation = &AbstractTexture::storageImplementationDefault;
            storage2DImplementation = &AbstractTexture::storageImplementationDefault;
            storage3DImplementation = &AbstractTexture::storageImplementationDefault;
        }
    }
    #else
    static_cast<void>(context);
    #endif
}

ColorFormat AbstractTexture::imageFormatForInternalFormat(const TextureFormat internalFormat) {
    switch(internalFormat) {
        case TextureFormat::Red:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8:
        case TextureFormat::R8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R16:
        case TextureFormat::R16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16F:
        case TextureFormat::R32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRed:
        case TextureFormat::CompressedRedRtgc1:
        case TextureFormat::CompressedSignedRedRgtc1:
        #endif
            return ColorFormat::Red;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8UI:
        case TextureFormat::R8I:
        case TextureFormat::R16UI:
        case TextureFormat::R16I:
        case TextureFormat::R32UI:
        case TextureFormat::R32I:
            return ColorFormat::RedInteger;
        #endif

        case TextureFormat::RG:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG8:
        case TextureFormat::RG8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RG16:
        case TextureFormat::RG16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRG:
        case TextureFormat::CompressedRGRgtc2:
        case TextureFormat::CompressedSignedRGRgtc2:
        #endif
            return ColorFormat::RG;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG8UI:
        case TextureFormat::RG8I:
        case TextureFormat::RG16UI:
        case TextureFormat::RG16I:
        case TextureFormat::RG32UI:
        case TextureFormat::RG32I:
            return ColorFormat::RGInteger;
        #endif

        case TextureFormat::RGB:
        case TextureFormat::RGB8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB16:
        case TextureFormat::RGB16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R3B3G2:
        case TextureFormat::RGB4:
        case TextureFormat::RGB5:
        #endif
        case TextureFormat::RGB565:
        case TextureFormat::RGB10:
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB12:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R11FG11FB10F:
        case TextureFormat::RGB9E5:
        #endif
        case TextureFormat::SRGB:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGB:
        case TextureFormat::CompressedRGBBptcUnsignedFloat:
        case TextureFormat::CompressedRGBBptcSignedFloat:
        #endif
            return ColorFormat::RGB;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB8UI:
        case TextureFormat::RGB8I:
        case TextureFormat::RGB16UI:
        case TextureFormat::RGB16I:
        case TextureFormat::RGB32UI:
        case TextureFormat::RGB32I:
            return ColorFormat::RGBInteger;
        #endif

        case TextureFormat::RGBA:
        case TextureFormat::RGBA8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA16:
        case TextureFormat::RGBA16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA2:
        #endif
        case TextureFormat::RGBA4:
        case TextureFormat::RGB5A1:
        case TextureFormat::RGB10A2:
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA12:
        #endif
        case TextureFormat::SRGBAlpha:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8Alpha8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBA:
        case TextureFormat::CompressedRGBABptcUnorm:
        case TextureFormat::CompressedSRGBAlphaBptcUnorm:
        #endif
            return ColorFormat::RGBA;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA8UI:
        case TextureFormat::RGBA8I:
        case TextureFormat::RGBA16UI:
        case TextureFormat::RGBA16I:
        case TextureFormat::RGBA32UI:
        case TextureFormat::RGBA32I:
        case TextureFormat::RGB10A2UI:
            return ColorFormat::RGBAInteger;
        #endif

        #ifdef MAGNUM_TARGET_GLES2
        case TextureFormat::Luminance:
            return ColorFormat::Luminance;
        case TextureFormat::LuminanceAlpha:
            return ColorFormat::LuminanceAlpha;
        #endif

        case TextureFormat::DepthComponent:
        case TextureFormat::DepthComponent16:
        case TextureFormat::DepthComponent24:
        case TextureFormat::DepthComponent32:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::DepthComponent32F:
        #endif
            return ColorFormat::DepthComponent;

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::StencilIndex8:
            return ColorFormat::StencilIndex;
        #endif

        case TextureFormat::DepthStencil:
        case TextureFormat::Depth24Stencil8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::Depth32FStencil8:
        #endif
            return ColorFormat::DepthStencil;
    }

    CORRADE_ASSERT_UNREACHABLE();
}

ColorType AbstractTexture::imageTypeForInternalFormat(const TextureFormat internalFormat) {
    switch(internalFormat) {
        case TextureFormat::Red:
        case TextureFormat::RG:
        case TextureFormat::RGB:
        case TextureFormat::RGBA:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8:
        case TextureFormat::RG8:
        #endif
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case TextureFormat::Luminance:
        case TextureFormat::LuminanceAlpha:
        #endif
        case TextureFormat::SRGB:
        case TextureFormat::SRGBAlpha:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8:
        case TextureFormat::SRGB8Alpha8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA2: /**< @todo really? */
        case TextureFormat::CompressedRed:
        case TextureFormat::CompressedRG:
        case TextureFormat::CompressedRGB:
        case TextureFormat::CompressedRGBA:
        case TextureFormat::CompressedRedRtgc1:
        case TextureFormat::CompressedRGRgtc2:
        case TextureFormat::CompressedRGBABptcUnorm:
        case TextureFormat::CompressedSRGBAlphaBptcUnorm:
        #endif
            return ColorType::UnsignedByte;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8Snorm:
        case TextureFormat::RG8Snorm:
        case TextureFormat::RGB8Snorm:
        case TextureFormat::RGBA8Snorm:
        case TextureFormat::R8I:
        case TextureFormat::RG8I:
        case TextureFormat::RGB8I:
        case TextureFormat::RGBA8I:
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedSignedRedRgtc1:
        case TextureFormat::CompressedSignedRGRgtc2:
        #endif
            return ColorType::Byte;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R16:
        case TextureFormat::RG16:
        case TextureFormat::RGB16:
        case TextureFormat::RGBA16:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16UI:
        case TextureFormat::RG16UI:
        case TextureFormat::RGB16UI:
        case TextureFormat::RGBA16UI:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB12: /**< @todo really? */
        #endif
        case TextureFormat::RGBA4: /**< @todo really? */
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA12: /**< @todo really? */
        #endif
            return ColorType::UnsignedShort;

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R16Snorm:
        case TextureFormat::RG16Snorm:
        case TextureFormat::RGB16Snorm:
        case TextureFormat::RGBA16Snorm:
        #endif
        case TextureFormat::R16I:
        case TextureFormat::RG16I:
        case TextureFormat::RGB16I:
        case TextureFormat::RGBA16I:
            return ColorType::Short;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            return ColorType::HalfFloat;

        case TextureFormat::R32UI:
        case TextureFormat::RG32UI:
        case TextureFormat::RGB32UI:
        case TextureFormat::RGBA32UI:
            return ColorType::UnsignedInt;

        case TextureFormat::R32I:
        case TextureFormat::RG32I:
        case TextureFormat::RGB32I:
        case TextureFormat::RGBA32I:
            return ColorType::Int;

        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBBptcUnsignedFloat:
        case TextureFormat::CompressedRGBBptcSignedFloat:
        #endif
            return ColorType::Float;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R3B3G2:
            return ColorType::UnsignedByte332;
        case TextureFormat::RGB4:
            return ColorType::UnsignedShort4444;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB5:
        #endif
        case TextureFormat::RGB5A1:
            return ColorType::UnsignedShort5551;

        case TextureFormat::RGB565:
            return ColorType::UnsignedShort565;

        case TextureFormat::RGB10:
        case TextureFormat::RGB10A2:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB10A2UI:
        #endif
            return ColorType::UnsignedInt2101010Rev; /**< @todo Rev for all? */

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R11FG11FB10F:
            return ColorType::UnsignedInt10F11F11FRev;
        case TextureFormat::RGB9E5:
            return ColorType::UnsignedInt5999Rev;
        #endif

        case TextureFormat::DepthComponent16:
            return ColorType::UnsignedShort;

        case TextureFormat::DepthComponent:
        case TextureFormat::DepthComponent24:
        case TextureFormat::DepthComponent32:
            return ColorType::UnsignedInt;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::DepthComponent32F:
            return ColorType::Float;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::StencilIndex8:
            return ColorType::UnsignedByte;
        #endif

        case TextureFormat::DepthStencil:
        case TextureFormat::Depth24Stencil8:
            return ColorType::UnsignedInt248;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::Depth32FStencil8:
            return ColorType::Float32UnsignedInt248Rev;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
}

void AbstractTexture::parameterImplementationDefault(GLenum parameter, GLint value) {
    bindInternal();
    glTexParameteri(_target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(GLenum parameter, GLint value) {
    glTextureParameteriEXT(_id, _target, parameter, value);
}
#endif

void AbstractTexture::parameterImplementationDefault(GLenum parameter, GLfloat value) {
    bindInternal();
    glTexParameterf(_target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(GLenum parameter, GLfloat value) {
    glTextureParameterfEXT(_id, _target, parameter, value);
}
#endif

void AbstractTexture::parameterImplementationDefault(GLenum parameter, const GLfloat* values) {
    bindInternal();
    glTexParameterfv(_target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(GLenum parameter, const GLfloat* values) {
    glTextureParameterfvEXT(_id, _target, parameter, values);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::getLevelParameterImplementationDefault(GLenum target, GLint level, GLenum parameter, GLint* values) {
    bindInternal();
    glGetTexLevelParameteriv(target, level, parameter, values);
}

void AbstractTexture::getLevelParameterImplementationDSA(GLenum target, GLint level, GLenum parameter, GLint* values) {
    glGetTextureLevelParameterivEXT(_id, target, level, parameter, values);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationFallback(const GLenum target, const GLsizei levels, const TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    CORRADE_INTERNAL_ASSERT(target == GL_TEXTURE_1D);

    const ColorFormat format = imageFormatForInternalFormat(internalFormat);
    const ColorType type = imageTypeForInternalFormat(internalFormat);

    for(GLsizei level = 0; level != levels; ++level) {
        (this->*image1DImplementation)(target, level, internalFormat, Math::max(Math::Vector<1, GLsizei>(1), size >> level), format, type, nullptr);
    }
}

void AbstractTexture::storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    bindInternal();
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage1D(target, levels, GLenum(internalFormat), size[0]);
    #else
    //glTexStorage2DEXT(target, levels, GLenum(internalFormat), size.x(), size.y());
    static_cast<void>(target);
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    #endif
}

void AbstractTexture::storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    glTextureStorage1DEXT(_id, target, levels, GLenum(internalFormat), size[0]);
}
#endif

void AbstractTexture::storageImplementationFallback(const GLenum target, const GLsizei levels, const TextureFormat internalFormat, const Vector2i& size) {
    const ColorFormat format = imageFormatForInternalFormat(internalFormat);
    const ColorType type = imageTypeForInternalFormat(internalFormat);

    /* Common code for classic types */
    #ifndef MAGNUM_TARGET_GLES
    if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE)
    #else
    if(target == GL_TEXTURE_2D)
    #endif
    {
        for(GLsizei level = 0; level != levels; ++level) {
            (this->*image2DImplementation)(target, level, internalFormat, Math::max(Vector2i(1), size >> level), format, type, nullptr);
        }

    /* Cube map additionally needs to specify all faces */
    } else if(target == GL_TEXTURE_CUBE_MAP) {
        for(GLsizei level = 0; level != levels; ++level) {
            const std::initializer_list<GLenum> faces = {
                GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
            };
            for(auto it = faces.begin(); it != faces.end(); ++it)
                (this->*image2DImplementation)(*it, level, internalFormat, Math::max(Vector2i(1), size >> level), format, type, nullptr);
        }

    #ifndef MAGNUM_TARGET_GLES
    /* Array texture is not scaled in "layer" dimension */
    } else if(target == GL_TEXTURE_1D_ARRAY) {
        for(GLsizei level = 0; level != levels; ++level) {
            (this->*image2DImplementation)(target, level, internalFormat, Math::max(Vector2i(1), size >> level), format, type, nullptr);
        }
    #endif

    /* No other targets are available */
    } else CORRADE_ASSERT_UNREACHABLE();
}

void AbstractTexture::storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size) {
    bindInternal();
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage2D(target, levels, GLenum(internalFormat), size.x(), size.y());
    #else
    //glTexStorage2DEXT(target, levels, GLenum(internalFormat), size.x(), size.y());
    static_cast<void>(target);
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector2i& size) {
    glTextureStorage2DEXT(_id, target, levels, GLenum(internalFormat), size.x(), size.y());
}
#endif

void AbstractTexture::storageImplementationFallback(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    const ColorFormat format = imageFormatForInternalFormat(internalFormat);
    const ColorType type = imageTypeForInternalFormat(internalFormat);

    /* Common code for classic type */
    #ifndef MAGNUM_TARGET_GLES2
    if(target == GL_TEXTURE_3D)
    #else
    if(target == GL_TEXTURE_3D_OES)
    #endif
    {
        for(GLsizei level = 0; level != levels; ++level) {
            (this->*image3DImplementation)(target, level, internalFormat, Math::max(Vector3i(1), size >> level), format, type, nullptr);
        }

    #ifndef MAGNUM_TARGET_GLES2
    /* Array texture is not scaled in "layer" dimension */
    }
    #ifndef MAGNUM_TARGET_GLES
    else if(target == GL_TEXTURE_2D_ARRAY || target == GL_TEXTURE_CUBE_MAP_ARRAY)
    #else
    else if(target == GL_TEXTURE_2D_ARRAY)
    #endif
    {
        for(GLsizei level = 0; level != levels; ++level) {
            (this->*image3DImplementation)(target, level, internalFormat, Math::max(Vector3i(1), size >> level), format, type, nullptr);
        }
    #endif

    /* No other targets are available */
    } else CORRADE_ASSERT_UNREACHABLE();
}

void AbstractTexture::storageImplementationDefault(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    bindInternal();
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage3D(target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    #else
    //glTexStorage3DEXT(target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    static_cast<void>(target);
    static_cast<void>(levels);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(GLenum target, GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    glTextureStorage3DEXT(_id, target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::getImageImplementationDefault(const GLenum target, const GLint level, const ColorFormat format, const ColorType type, const std::size_t, GLvoid* const data) {
    bindInternal();
    glGetTexImage(target, level, GLenum(format), GLenum(type), data);
}

void AbstractTexture::getImageImplementationDSA(const GLenum target, const GLint level, const ColorFormat format, const ColorType type, const std::size_t, GLvoid* const data) {
    glGetTextureImageEXT(_id, target, level, GLenum(format), GLenum(type), data);
}

void AbstractTexture::getImageImplementationRobustness(const GLenum target, const GLint level, const ColorFormat format, const ColorType type, const std::size_t dataSize, GLvoid* const data) {
    #ifndef MAGNUM_TARGET_GLES
    bindInternal();
    glGetnTexImageARB(target, level, GLenum(format), GLenum(type), dataSize, data);
    #else
    CORRADE_INTERNAL_ASSERT(false);
    static_cast<void>(target);
    static_cast<void>(level);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(dataSize);
    static_cast<void>(data);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data) {
    bindInternal();
    glTexImage1D(target, level, GLint(internalFormat), size[0], 0, GLenum(format), GLenum(type), data);
}

void AbstractTexture::imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data) {
    glTextureImage1DEXT(_id, target, level, GLint(internalFormat), size[0], 0, GLenum(format), GLenum(type), data);
}
#endif

void AbstractTexture::imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    bindInternal();
    glTexImage2D(target, level, GLint(internalFormat), size.x(), size.y(), 0, GLenum(format), GLenum(type), data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    glTextureImage2DEXT(_id, target, level, GLint(internalFormat), size.x(), size.y(), 0, GLenum(format), GLenum(type), data);
}
#endif

void AbstractTexture::imageImplementationDefault(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    bindInternal();
    /** @todo Get some extension wrangler instead to avoid linker errors to glTexImage3D() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexImage3D(target, level, GLint(internalFormat), size.x(), size.y(), size.z(), 0, GLenum(format), GLenum(type), data);
    #else
    static_cast<void>(target);
    static_cast<void>(level);
    static_cast<void>(internalFormat);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(data);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationDSA(GLenum target, GLint level, TextureFormat internalFormat, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    glTextureImage3DEXT(_id, target, level, GLint(internalFormat), size.x(), size.y(), size.z(), 0, GLenum(format), GLenum(type), data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDefault(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data) {
    bindInternal();
    glTexSubImage1D(target, level, offset[0], size[0], GLenum(format), GLenum(type), data);
}

void AbstractTexture::subImageImplementationDSA(GLenum target, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, ColorFormat format, ColorType type, const GLvoid* data) {
    glTextureSubImage1DEXT(_id, target, level, offset[0], size[0], GLenum(format), GLenum(type), data);
}
#endif

void AbstractTexture::subImageImplementationDefault(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    bindInternal();
    glTexSubImage2D(target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(GLenum target, GLint level, const Vector2i& offset, const Vector2i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    glTextureSubImage2DEXT(_id, target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}
#endif

void AbstractTexture::subImageImplementationDefault(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    bindInternal();
    /** @todo Get some extension wrangler instead to avoid linker errors to glTexSubImage3D() on ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glTexSubImage3D(target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
    #else
    static_cast<void>(target);
    static_cast<void>(level);
    static_cast<void>(offset);
    static_cast<void>(size);
    static_cast<void>(format);
    static_cast<void>(type);
    static_cast<void>(data);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDSA(GLenum target, GLint level, const Vector3i& offset, const Vector3i& size, ColorFormat format, ColorType type, const GLvoid* data) {
    glTextureSubImage3DEXT(_id, target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
}
#endif

void AbstractTexture::invalidateImageImplementationNoOp(GLint) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateImageImplementationARB(GLint level) {
    glInvalidateTexImage(_id, level);
}
#endif

void AbstractTexture::invalidateSubImageImplementationNoOp(GLint, const Vector3i&, const Vector3i&) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateSubImageImplementationARB(GLint level, const Vector3i& offset, const Vector3i& size) {
    glInvalidateTexSubImage(_id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z());
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> void AbstractTexture::image(GLenum target, GLint level, Image<dimensions>& image) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(this, target, level);
    const std::size_t dataSize = size.product()*image.pixelSize();
    char* data = new char[dataSize];
    (this->*getImageImplementation)(target, level, image.format(), image.type(), dataSize, data);
    image.setData(image.format(), image.type(), size, data);
}

template void MAGNUM_EXPORT AbstractTexture::image<1>(GLenum, GLint, Image<1>&);
template void MAGNUM_EXPORT AbstractTexture::image<2>(GLenum, GLint, Image<2>&);
template void MAGNUM_EXPORT AbstractTexture::image<3>(GLenum, GLint, Image<3>&);

template<UnsignedInt dimensions> void AbstractTexture::image(GLenum target, GLint level, BufferImage<dimensions>& image, BufferUsage usage) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(this, target, level);
    const std::size_t dataSize = size.product()*image.pixelSize();
    if(image.size() != size)
        image.setData(size, image.format(), image.type(), nullptr, usage);

    image.buffer().bind(Buffer::Target::PixelPack);
    (this->*getImageImplementation)(target, level, image.format(), image.type(), dataSize, nullptr);
}

template void MAGNUM_EXPORT AbstractTexture::image<1>(GLenum, GLint, BufferImage<1>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::image<2>(GLenum, GLint, BufferImage<2>&, BufferUsage);
template void MAGNUM_EXPORT AbstractTexture::image<3>(GLenum, GLint, BufferImage<3>&, BufferUsage);
#endif
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
Math::Vector<1, GLint> AbstractTexture::DataHelper<1>::imageSize(AbstractTexture* texture, GLenum target, GLint level) {
    Math::Vector<1, GLint> value;
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_WIDTH, &value[0]);
    return value;
}

Vector2i AbstractTexture::DataHelper<2>::imageSize(AbstractTexture* texture, GLenum target, GLint level) {
    Vector2i value;
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_WIDTH, &value[0]);
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_HEIGHT, &value[1]);
    return value;
}

Vector3i AbstractTexture::DataHelper<3>::imageSize(AbstractTexture* texture, GLenum target, GLint level) {
    Vector3i value;
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_WIDTH, &value[0]);
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_HEIGHT, &value[1]);
    (texture->*getLevelParameterivImplementation)(target, level, GL_TEXTURE_DEPTH, &value[2]);
    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::setImage(AbstractTexture* const texture, const GLenum target, const GLint level, const TextureFormat internalFormat, const ImageReference1D& image) {
    Buffer::unbind(Buffer::Target::PixelUnpack);
    (texture->*image1DImplementation)(target, level, internalFormat, image.size(), image.format(), image.type(), image.data());
}

void AbstractTexture::DataHelper<1>::setImage(AbstractTexture* const texture, const GLenum target, const GLint level, const TextureFormat internalFormat, BufferImage1D& image) {
    image.buffer().bind(Buffer::Target::PixelUnpack);
    (texture->*image1DImplementation)(target, level, internalFormat, image.size(), image.format(), image.type(), nullptr);
}

void AbstractTexture::DataHelper<1>::setSubImage(AbstractTexture* const texture, const GLenum target, const GLint level, const Math::Vector<1, GLint>& offset, const ImageReference1D& image) {
    Buffer::unbind(Buffer::Target::PixelUnpack);
    (texture->*subImage1DImplementation)(target, level, offset, image.size(), image.format(), image.type(), image.data());
}

void AbstractTexture::DataHelper<1>::setSubImage(AbstractTexture* const texture, const GLenum target, const GLint level, const Math::Vector<1, GLint>& offset, BufferImage1D& image) {
    image.buffer().bind(Buffer::Target::PixelUnpack);
    (texture->*subImage1DImplementation)(target, level, offset, image.size(), image.format(), image.type(), nullptr);
}
#endif

void AbstractTexture::DataHelper<2>::setImage(AbstractTexture* const texture, const GLenum target, const GLint level, const TextureFormat internalFormat, const ImageReference2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbind(Buffer::Target::PixelUnpack);
    #endif
    (texture->*image2DImplementation)(target, level, internalFormat, image.size(), image.format(), image.type(), image.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<2>::setImage(AbstractTexture* const texture, const GLenum target, const GLint level, const TextureFormat internalFormat, BufferImage2D& image) {
    image.buffer().bind(Buffer::Target::PixelUnpack);
    (texture->*image2DImplementation)(target, level, internalFormat, image.size(), image.format(), image.type(), nullptr);
}
#endif

void AbstractTexture::DataHelper<2>::setSubImage(AbstractTexture* const texture, const GLenum target, const GLint level, const Vector2i& offset, const ImageReference2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbind(Buffer::Target::PixelUnpack);
    #endif
    (texture->*subImage2DImplementation)(target, level, offset, image.size(), image.format(), image.type(), image.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<2>::setSubImage(AbstractTexture* const texture, const GLenum target, const GLint level, const Vector2i& offset, BufferImage2D& image) {
    image.buffer().bind(Buffer::Target::PixelUnpack);
    (texture->*subImage2DImplementation)(target, level, offset, image.size(), image.format(), image.type(), nullptr);
}
#endif

void AbstractTexture::DataHelper<3>::setImage(AbstractTexture* const texture, const GLenum target, const GLint level, const TextureFormat internalFormat, const ImageReference3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbind(Buffer::Target::PixelUnpack);
    #endif
    (texture->*image3DImplementation)(target, level, internalFormat, image.size(), image.format(), image.type(), image.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<3>::setImage(AbstractTexture* const texture, const GLenum target, const GLint level, const TextureFormat internalFormat, BufferImage3D& image) {
    image.buffer().bind(Buffer::Target::PixelUnpack);
    (texture->*image3DImplementation)(target, level, internalFormat, image.size(), image.format(), image.type(), nullptr);
}
#endif

void AbstractTexture::DataHelper<3>::setSubImage(AbstractTexture* const texture, const GLenum target, const GLint level, const Vector3i& offset, const ImageReference3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbind(Buffer::Target::PixelUnpack);
    #endif
    (texture->*subImage3DImplementation)(target, level, offset, image.size(), image.format(), image.type(), image.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<3>::setSubImage(AbstractTexture* const texture, const GLenum target, const GLint level, const Vector3i& offset, BufferImage3D& image) {
    image.buffer().bind(Buffer::Target::PixelUnpack);
    (texture->*subImage3DImplementation)(target, level, offset, image.size(), image.format(), image.type(), nullptr);
}
#endif

void AbstractTexture::DataHelper<2>::setWrapping(AbstractTexture* texture, const Array2D<Sampler::Wrapping>& wrapping) {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(texture->_target != GL_TEXTURE_RECTANGLE || ((wrapping.x() == Sampler::Wrapping::ClampToEdge || wrapping.x() == Sampler::Wrapping::ClampToBorder) && (wrapping.y() == Sampler::Wrapping::ClampToEdge || wrapping.y() == Sampler::Wrapping::ClampToEdge)), "AbstractTexture: rectangle texture wrapping must either clamp to border or to edge", );
    #endif

    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_S, GLint(wrapping.x()));
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_T, GLint(wrapping.y()));
}

void AbstractTexture::DataHelper<3>::setWrapping(AbstractTexture* texture, const Array3D<Sampler::Wrapping>& wrapping) {
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_S, GLint(wrapping.x()));
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_T, GLint(wrapping.y()));
    #ifndef MAGNUM_TARGET_GLES
    (texture->*parameteriImplementation)(GL_TEXTURE_WRAP_R, GLint(wrapping.z()));
    #endif
}
#endif

}
