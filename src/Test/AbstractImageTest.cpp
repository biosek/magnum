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

#include "AbstractImage.h"
#include "ColorFormat.h"

namespace Magnum { namespace Test {

class AbstractImageTest: public TestSuite::Tester {
    public:
        explicit AbstractImageTest();

        void debugFormat();
        void debugType();
};

AbstractImageTest::AbstractImageTest() {
    addTests<AbstractImageTest>({&AbstractImageTest::debugFormat,
              &AbstractImageTest::debugType});
}

void AbstractImageTest::debugFormat() {
    std::ostringstream o;
    Debug(&o) << ColorFormat::RGBA;
    CORRADE_COMPARE(o.str(), "ColorFormat::RGBA\n");
}

void AbstractImageTest::debugType() {
    std::ostringstream o;
    Debug(&o) << ColorType::UnsignedShort5551;
    CORRADE_COMPARE(o.str(), "ColorType::UnsignedShort5551\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::AbstractImageTest)
