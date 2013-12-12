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

#include <sstream>
#include <TestSuite/Tester.h>

#include "Shapes/shapeImplementation.h"

namespace Magnum { namespace Shapes { namespace Test {

class ShapeImplementationTest: public TestSuite::Tester {
    public:
        ShapeImplementationTest();

        void debug();
};

ShapeImplementationTest::ShapeImplementationTest() {
    addTests<ShapeImplementationTest>({&ShapeImplementationTest::debug});
}

void ShapeImplementationTest::debug() {
    std::ostringstream o;
    Debug(&o) << Implementation::ShapeDimensionTraits<2>::Type::Composition;
    CORRADE_COMPARE(o.str(), "Shapes::Shape2D::Type::Composition\n");

    o.str({});
    Debug(&o) << Implementation::ShapeDimensionTraits<3>::Type::Plane;
    CORRADE_COMPARE(o.str(), "Shapes::Shape3D::Type::Plane\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::ShapeImplementationTest)
