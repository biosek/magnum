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

#include "ColorFormat.h"
#include "Trade/ImageData.h"

namespace Magnum { namespace Trade { namespace Test {

class ImageDataTest: public TestSuite::Tester {
    public:
        explicit ImageDataTest();

        void moveConstructor();
        void moveAssignment();
        void toReference();
        void release();
};

ImageDataTest::ImageDataTest() {
    addTests<ImageDataTest>({&ImageDataTest::moveConstructor,
              &ImageDataTest::moveAssignment,
              &ImageDataTest::toReference,
              &ImageDataTest::release});
}

void ImageDataTest::moveConstructor() {
    unsigned char* data = new unsigned char[3];
    Trade::ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);

    Trade::ImageData2D b(std::move(a));
    CORRADE_VERIFY(!a.data());
    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_VERIFY(b.data() == data);
}

void ImageDataTest::moveAssignment() {
    unsigned char* data = new unsigned char[3];
    Trade::ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);

    Trade::ImageData2D b(ColorFormat::Red, ColorType::UnsignedByte, {}, nullptr);
    b = std::move(a);
    CORRADE_VERIFY(!a.data());
    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_VERIFY(b.data() == data);
}

void ImageDataTest::toReference() {
    unsigned char* data = new unsigned char[3];
    const Trade::ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 3}, data);

    ImageReference2D b = a;
    CORRADE_COMPARE(b.format(), ColorFormat::Red);
    CORRADE_COMPARE(b.type(), ColorType::UnsignedByte);
    CORRADE_COMPARE(b.size(), Vector2i(1, 3));
    CORRADE_COMPARE(b.data(), data);

    CORRADE_VERIFY((std::is_convertible<const Trade::ImageData2D&, ImageReference2D>::value));
    {
        #ifdef CORRADE_GCC47_COMPATIBILITY
        CORRADE_EXPECT_FAIL("Rvalue references for *this are not supported in GCC < 4.8.1.");
        #endif
        CORRADE_VERIFY(!(std::is_convertible<const Trade::ImageData2D, ImageReference2D>::value));
        CORRADE_VERIFY(!(std::is_convertible<const Trade::ImageData2D&&, ImageReference2D>::value));
    }
}

void ImageDataTest::release() {
    unsigned char data[] = {'b', 'e', 'e', 'r'};
    ImageData2D a(ColorFormat::Red, ColorType::UnsignedByte, {1, 4}, data);
    const unsigned char* const pointer = a.release();
    CORRADE_COMPARE(pointer, data);
    CORRADE_VERIFY(!a.data());
    CORRADE_VERIFY(a.size().isZero());
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageDataTest)
