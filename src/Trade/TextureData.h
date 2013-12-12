#ifndef Magnum_Trade_TextureData_h
#define Magnum_Trade_TextureData_h
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
 * @brief Class Magnum::Trade::TextureData
 */

#include "Array.h"
#include "Sampler.h"
#include "magnumVisibility.h"

namespace Magnum { namespace Trade {

/**
@brief %Texture data
*/
class TextureData {
    public:
        /**
         * @brief Texture type
         *
         * @see type()
         */
        enum class Type: UnsignedByte {
            Texture1D,  /**< One-dimensional texture */
            Texture2D,  /**< Two-dimensional texture */
            Texture3D,  /**< Three-dimensional texture */
            Cube        /**< Cube map texture */
        };

        /**
         * @brief Constructor
         * @param type                  Texture type
         * @param minificationFilter    Minification filter
         * @param magnificationFilter   Magnification filter
         * @param mipmapFilter          Mipmap filter
         * @param wrapping              Wrapping
         * @param image                 Texture image ID
         */
        TextureData(Type type, Sampler::Filter minificationFilter, Sampler::Filter magnificationFilter, Sampler::Mipmap mipmapFilter, Array3D<Sampler::Wrapping> wrapping, UnsignedInt image): _type(type), _minificationFilter(minificationFilter), _magnificationFilter(magnificationFilter), _mipmapFilter(mipmapFilter), _wrapping(wrapping), _image(image) {}

        /** @brief Copying is not allowed */
        TextureData(const TextureData&) = delete;

        /** @brief Move constructor */
        TextureData(TextureData&&);

        /** @brief Copying is not allowed */
        TextureData& operator=(const TextureData&) = delete;

        /** @brief Move assignment */
        TextureData& operator=(TextureData&&);

        /** @brief Texture type */
        Type type() const { return _type; }

        /** @brief Minification filter */
        Sampler::Filter minificationFilter() const { return _minificationFilter; }

        /** @brief Magnification filter */
        Sampler::Filter magnificationFilter() const { return _magnificationFilter; }

        /** @brief Mipmap filter */
        Sampler::Mipmap mipmapFilter() const { return _mipmapFilter; }

        /** @brief Wrapping */
        Array3D<Sampler::Wrapping> wrapping() const { return _wrapping; }

        /**
         * @brief Image ID
         *
         * ID of 1D, 2D or 3D image based on texture type. If type is
         * @ref Type::Cube the function returns first of six consecutive
         * IDs of cube map sides, ordered +X, -X, +Y, -Y, +Z, -Z.
         * @see @ref type(), @ref AbstractImporter::image1D(),
         *      @ref AbstractImporter::image2D(), @ref AbstractImporter::image3D()
         */
        UnsignedInt image() const { return _image; }

    private:
        Type _type;
        Sampler::Filter _minificationFilter, _magnificationFilter;
        Sampler::Mipmap _mipmapFilter;
        Array3D<Sampler::Wrapping> _wrapping;
        UnsignedInt _image;
};

/* GCC 4.4 doesn't like it defaulted */
inline TextureData::TextureData(TextureData&& other): _type(other._type), _minificationFilter(other._minificationFilter), _magnificationFilter(other._magnificationFilter), _mipmapFilter(other._mipmapFilter), _wrapping(other._wrapping), _image(other._image) {}

/* GCC 4.5 doesn't like it defaulted */
inline TextureData& TextureData::operator=(TextureData&& other) {
    std::swap(_type, other._type);
    std::swap(_minificationFilter, other._minificationFilter);
    std::swap(_magnificationFilter, other._magnificationFilter);
    std::swap(_mipmapFilter, other._mipmapFilter);
    std::swap(_wrapping, other._wrapping);
    std::swap(_image, other._image);
    return *this;
}

/** @debugoperator{Magnum::Trade::TextureData} */
Debug MAGNUM_EXPORT operator<<(Debug debug, TextureData::Type value);

}}

#endif
