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

#include <TestSuite/Tester.h>

#include "Math/Constants.h"
#include "Math/Functions.h"

namespace Magnum { namespace Math { namespace Test {

class ConstantsTest: public Corrade::TestSuite::Tester {
    public:
        ConstantsTest();

        void constantsFloat();
        void constantsDouble();
};

ConstantsTest::ConstantsTest() {
    addTests<ConstantsTest>({&ConstantsTest::constantsFloat,
              &ConstantsTest::constantsDouble});
}

void ConstantsTest::constantsFloat() {
    constexpr Float a = Constants<Float>::sqrt2();
    constexpr Float b = Constants<Float>::sqrt3();
    CORRADE_COMPARE(Math::pow<2>(a), 2.0f);
    CORRADE_COMPARE(Math::pow<2>(b), 3.0f);
}

void ConstantsTest::constantsDouble() {
    #ifndef MAGNUM_TARGET_GLES
    constexpr Double c = Constants<Double>::sqrt2();
    constexpr Double d = Constants<Double>::sqrt3();
    CORRADE_COMPARE(Math::pow<2>(c), 2.0);
    CORRADE_COMPARE(Math::pow<2>(d), 3.0);
    #else
    CORRADE_SKIP("Double precision is not supported when targeting OpenGL ES.");
    #endif
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::ConstantsTest)
