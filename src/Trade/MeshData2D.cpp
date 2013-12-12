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

#include "MeshData2D.h"

#include "Math/Vector2.h"

namespace Magnum { namespace Trade {

MeshData2D::MeshData2D(MeshPrimitive primitive, std::vector<UnsignedInt> indices, std::vector<std::vector<Vector2>> positions, std::vector<std::vector<Vector2>> textureCoords2D): _primitive(primitive), _indices(std::move(indices)), _positions(std::move(positions)), _textureCoords2D(std::move(textureCoords2D)) {
    CORRADE_ASSERT(!_positions.empty(), "Trade::MeshData2D: no position array specified", );
}

/* GCC 4.4 doesn't like it defaulted */
MeshData2D::MeshData2D(MeshData2D&& other): _primitive(other._primitive), _indices(std::move(other._indices)), _positions(std::move(other._positions)), _textureCoords2D(std::move(other._textureCoords2D)) {}

MeshData2D::~MeshData2D() = default;

/* GCC 4.5 doesn't like it defaulted */
MeshData2D& MeshData2D::operator=(MeshData2D&& other) {
    std::swap(_primitive, other._primitive);
    std::swap(_indices, other._indices);
    std::swap(_positions, other._positions);
    std::swap(_textureCoords2D, other._textureCoords2D);
    return *this;
}

std::vector<UnsignedInt>& MeshData2D::indices() {
    CORRADE_ASSERT(isIndexed(), "Trade::MeshData2D::indices(): the mesh is not indexed", _indices);
    return _indices;
}

const std::vector<UnsignedInt>& MeshData2D::indices() const {
    CORRADE_ASSERT(isIndexed(), "Trade::MeshData2D::indices(): the mesh is not indexed", _indices);
    return _indices;
}

std::vector<Vector2>& MeshData2D::positions(const UnsignedInt id) {
    CORRADE_ASSERT(id < positionArrayCount(), "Trade::MeshData2D::positions(): index out of range", _positions[id]);
    return _positions[id];
}

const std::vector<Vector2>& MeshData2D::positions(const UnsignedInt id) const {
    CORRADE_ASSERT(id < positionArrayCount(), "Trade::MeshData2D::positions(): index out of range", _positions[id]);
    return _positions[id];
}

std::vector<Vector2>& MeshData2D::textureCoords2D(const UnsignedInt id) {
    CORRADE_ASSERT(id < textureCoords2DArrayCount(), "Trade::MeshData2D::textureCoords2D(): index out of range", _textureCoords2D[id]);
    return _textureCoords2D[id];
}

const std::vector<Vector2>& MeshData2D::textureCoords2D(const UnsignedInt id) const {
    CORRADE_ASSERT(id < textureCoords2DArrayCount(), "Trade::MeshData2D::textureCoords2D(): index out of range", _textureCoords2D[id]);
    return _textureCoords2D[id];
}

}}
