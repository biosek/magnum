#ifndef Magnum_Trade_AbstractMaterialData_h
#define Magnum_Trade_AbstractMaterialData_h
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
 * @brief Class @ref Magnum::Trade::AbstractMaterialData, enum @ref Magnum::Trade::MaterialType
 */

#include "magnumVisibility.h"
#include "Magnum.h"

namespace Magnum { namespace Trade {

/**
@brief Material type

@see @ref AbstractMaterialData::type()
*/
enum class MaterialType: UnsignedByte {
    Phong       /**< Phong shading */
};

/**
@brief Base for material data

Subclasses provide access to parameters for given material type.
*/
class MAGNUM_EXPORT AbstractMaterialData {
    public:
        /**
         * @brief Constructor
         * @param type      Material type
         */
        explicit AbstractMaterialData(MaterialType type);

        /** @brief Destructor */
        virtual ~AbstractMaterialData() = 0;

        /** @brief Copying is not allowed */
        AbstractMaterialData(const AbstractMaterialData&) = delete;

        /** @brief Move constructor */
        AbstractMaterialData(AbstractMaterialData&&);

        /** @brief Copying is not allowed */
        AbstractMaterialData& operator=(const AbstractMaterialData&) = delete;

        /** @brief Move assignment */
        AbstractMaterialData& operator=(AbstractMaterialData&&);

        /** @brief Material type */
        MaterialType type() const { return _type; }

    private:
        MaterialType _type;
};

/** @debugoperator{Magnum::Trade::AbstractMaterialData} */
Debug MAGNUM_EXPORT operator<<(Debug debug, MaterialType value);

}}

#endif
