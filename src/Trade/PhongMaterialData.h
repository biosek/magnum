#ifndef Magnum_Trade_PhongMaterialData_h
#define Magnum_Trade_PhongMaterialData_h
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
 * @brief Class Magnum::Trade::PhongMaterialData
 */

#include "Math/Vector3.h"
#include "Magnum.h"
#include "AbstractMaterialData.h"

namespace Magnum { namespace Trade {

/**
@brief Phong material data
*/
class MAGNUM_EXPORT PhongMaterialData: public AbstractMaterialData {
    public:
        enum: UnsignedInt {
            AmbientTextureID = 0,   /**< Ambient texture ID for mapping with texture coordinates */
            DiffuseTextureID = 1,   /**< Diffuse texture ID for mapping with texture coordinates */
            SpecularTextureID = 3   /**< Specular texture ID for mapping with texture coordinates */
        };

        /**
         * @brief Material flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            AmbientTexture = 1 << 0,    /**< The material has ambient texture instead of color */
            DiffuseTexture = 1 << 1,    /**< The material has diffuse texture instead of color */
            SpecularTexture = 1 << 2    /**< The material has specular texture instead of color */
        };

        /**
         * @brief Material flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag, UnsignedByte> Flags;

        /**
         * @brief Constructor
         * @param flags         Material flags
         * @param shininess     Shininess
         *
         * Colors and textures should be specified using member functions based
         * on what flags are set.
         */
        explicit PhongMaterialData(Flags flags, Float shininess): AbstractMaterialData(MaterialType::Phong), _shininess(shininess), _flags(flags) {}

        /** @brief Material flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Ambient color
         *
         * Available only if the material doesn't have @ref Flag::AmbientTexture.
         * @see @ref flags()
         */
        Vector3& ambientColor();
        Vector3 ambientColor() const; /**< @overload */

        /**
         * @brief Ambient texture ID
         *
         * Available only if the material has @ref Flag::AmbientTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt& ambientTexture();
        UnsignedInt ambientTexture() const; /**< @overload */

        /**
         * @brief Diffuse color
         *
         * Available only if the material doesn't have @ref Flag::DiffuseTexture.
         * @see @ref flags()
         */
        Vector3& diffuseColor();
        Vector3 diffuseColor() const; /**< @overload */

        /**
         * @brief Diffuse texture ID
         *
         * Available only if the material has @ref Flag::DiffuseTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt& diffuseTexture();
        UnsignedInt diffuseTexture() const; /**< @overload */

        /**
         * @brief Specular color
         *
         * Available only if the material doesn't have @ref Flag::SpecularTexture.
         * @see @ref flags()
         */
        Vector3& specularColor();
        Vector3 specularColor() const; /**< @overload */

        /**
         * @brief Specular texture ID
         *
         * Available only if the material has @ref Flag::SpecularTexture.
         * @see @ref flags(), @ref AbstractImporter::texture()
         */
        UnsignedInt& specularTexture();
        UnsignedInt specularTexture() const; /**< @overload */

        /** @brief Shininess */
        Float shininess() const { return _shininess; }

    private:
        #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
        union Source {
            Source() {}

            public:
                Vector3& color() { return _color; }
                UnsignedInt& texture() { return _texture; }

            private:
                Vector3 _color;
                UnsignedInt _texture;
        };
        #else
        class Source {
            public:
                Vector3& color() { return *reinterpret_cast<Vector3*>(&data); }
                UnsignedInt& texture() { return *reinterpret_cast<UnsignedInt*>(&data); }

            private:
                char data[sizeof(Vector3)];
        };
        #endif

        Source _ambient,
            _diffuse,
            _specular;
        Float _shininess;
        Flags _flags;
};

CORRADE_ENUMSET_OPERATORS(PhongMaterialData::Flags)

/* Ugly as hell. */

inline Vector3 PhongMaterialData::ambientColor() const {
    return const_cast<PhongMaterialData*>(this)->ambientColor();
}

inline UnsignedInt PhongMaterialData::ambientTexture() const {
    return const_cast<PhongMaterialData*>(this)->ambientTexture();
}

inline Vector3 PhongMaterialData::diffuseColor() const {
    return const_cast<PhongMaterialData*>(this)->diffuseColor();
}

inline UnsignedInt PhongMaterialData::diffuseTexture() const {
    return const_cast<PhongMaterialData*>(this)->diffuseTexture();
}

inline Vector3 PhongMaterialData::specularColor() const {
    return const_cast<PhongMaterialData*>(this)->specularColor();
}

inline UnsignedInt PhongMaterialData::specularTexture() const {
    return const_cast<PhongMaterialData*>(this)->specularTexture();
}

}}

#endif
