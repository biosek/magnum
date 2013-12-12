#ifndef Magnum_Trade_MeshData3D_h
#define Magnum_Trade_MeshData3D_h
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
 * @brief Class Magnum::Trade::MeshData3D
 */

#include <vector>

#include "Magnum.h"

#include "magnumVisibility.h"

#ifdef CORRADE_GCC45_COMPATIBILITY
#include "Mesh.h"
#endif

namespace Magnum { namespace Trade {

/**
@brief Three-dimensional mesh data

Provides access to mesh data and additional information, such as primitive
type.
@see MeshData2D
*/
class MAGNUM_EXPORT MeshData3D {
    public:
        /**
         * @brief Constructor
         * @param primitive         Primitive
         * @param indices           Index array or empty array, if the mesh is
         *      not indexed
         * @param positions         Position arrays. At least one position
         *      array should be present.
         * @param normals           Normal arrays, if present
         * @param textureCoords2D   Two-dimensional texture coordinate arrays,
         *      if present
         */
        explicit MeshData3D(MeshPrimitive primitive, std::vector<UnsignedInt> indices, std::vector<std::vector<Vector3>> positions, std::vector<std::vector<Vector3>> normals, std::vector<std::vector<Vector2>> textureCoords2D);

        /** @brief Copying is not allowed */
        MeshData3D(const MeshData3D&) = delete;

        /** @brief Move constructor */
        MeshData3D(MeshData3D&&);

        ~MeshData3D();

        /** @brief Copying is not allowed */
        MeshData3D& operator=(const MeshData3D&) = delete;

        /** @brief Move assignment */
        MeshData3D& operator=(MeshData3D&&);

        /** @brief Primitive */
        MeshPrimitive primitive() const { return _primitive; }

        /** @brief Whether the mesh is indexed */
        bool isIndexed() const { return !_indices.empty(); }

        /**
         * @brief Indices
         *
         * @see isIndexed()
         */
        std::vector<UnsignedInt>& indices();
        const std::vector<UnsignedInt>& indices() const; /**< @overload */

        /**
         * @brief Count of position arrays
         *
         * There is always at least one.
         */
        UnsignedInt positionArrayCount() const { return _positions.size(); }

        /**
         * @brief Positions
         * @param id    Position array ID
         *
         * @see positionArrayCount()
         */
        std::vector<Vector3>& positions(UnsignedInt id);
        const std::vector<Vector3>& positions(UnsignedInt id) const; /**< @overload */

        /** @brief Whether the data contain any normals */
        bool hasNormals() const { return !_normals.empty(); }

        /** @brief Count of normal arrays */
        UnsignedInt normalArrayCount() const { return _normals.size(); }

        /**
         * @brief Normals
         * @param id    Normal array ID
         *
         * @see normalArrayCount()
         */
        std::vector<Vector3>& normals(UnsignedInt id);
        const std::vector<Vector3>& normals(UnsignedInt id) const; /**< @overload */

        /** @brief Whether the data contain any 2D texture coordinates */
        bool hasTextureCoords2D() const { return !_textureCoords2D.empty(); }

        /** @brief Count of 2D texture coordinate arrays */
        UnsignedInt textureCoords2DArrayCount() const { return _textureCoords2D.size(); }

        /**
         * @brief 2D texture coordinates
         * @param id    Texture coordinate array ID
         *
         * @see textureCoords2DArrayCount()
         */
        std::vector<Vector2>& textureCoords2D(UnsignedInt id);
        const std::vector<Vector2>& textureCoords2D(UnsignedInt id) const; /**< @overload */

    private:
        MeshPrimitive _primitive;
        std::vector<UnsignedInt> _indices;
        std::vector<std::vector<Vector3>> _positions;
        std::vector<std::vector<Vector3>> _normals;
        std::vector<std::vector<Vector2>> _textureCoords2D;
};

}}

#endif
