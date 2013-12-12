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

#include "ObjectData3D.h"

namespace Magnum { namespace Trade {

ObjectData3D::ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, ObjectInstanceType3D instanceType, UnsignedInt instance): _children(std::move(children)), _transformation(transformation), _instanceType(instanceType), _instance(instance) {}

ObjectData3D::ObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation): _children(std::move(children)), _transformation(transformation), _instanceType(ObjectInstanceType3D::Empty), _instance(-1) {}

/* GCC 4.4 doesn't like it defaulted */
ObjectData3D::ObjectData3D(ObjectData3D&& other): _children(std::move(other._children)), _transformation(std::move(other._transformation)), _instanceType(std::move(other._instanceType)), _instance(std::move(other._instance)) {}

ObjectData3D::~ObjectData3D() = default;

/* GCC 4.5 doesn't like it defaulted */
ObjectData3D& ObjectData3D::operator=(ObjectData3D&& other) {
    std::swap(_children, other._children);
    std::swap(_transformation, other._transformation);
    std::swap(_instanceType, other._instanceType);
    std::swap(_instance, other._instance);
    return *this;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, ObjectInstanceType3D value) {
    switch(value) {
        #define _c(value) case ObjectInstanceType3D::value: return debug << "Trade::ObjectInstanceType3D::" #value;
        _c(Camera)
        _c(Light)
        _c(Mesh)
        _c(Empty)
        #undef _c
    }

    return debug << "Trade::ObjectInstanceType3D::(invalid)";
}
#endif

}}
