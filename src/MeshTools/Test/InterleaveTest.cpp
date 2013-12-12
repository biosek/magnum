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

#include "Utility/Endianness.h"
#include "Utility/Debug.h"
#include "MeshTools/Interleave.h"

namespace Magnum { namespace MeshTools { namespace Test {

class InterleaveTest: public Corrade::TestSuite::Tester {
    public:
        InterleaveTest();

        void attributeCount();
        void attributeCountGaps();
        void stride();
        void strideGaps();
        void write();
        void writeGaps();
};

InterleaveTest::InterleaveTest() {
    addTests<InterleaveTest>({&InterleaveTest::attributeCount,
              &InterleaveTest::attributeCountGaps,
              &InterleaveTest::stride,
              &InterleaveTest::strideGaps,
              &InterleaveTest::write,
              &InterleaveTest::writeGaps});
}

void InterleaveTest::attributeCount() {
    std::stringstream ss;
    Error::setOutput(&ss);
    CORRADE_COMPARE((Implementation::Interleave::attributeCount(std::vector<Byte>{0, 1, 2},
        std::vector<Byte>{0, 1, 2, 3, 4, 5})), std::size_t(0));
    CORRADE_COMPARE(ss.str(), "MeshTools::interleave(): attribute arrays don't have the same length, expected 3 but got 6\n");

    CORRADE_COMPARE((Implementation::Interleave::attributeCount(std::vector<Byte>{0, 1, 2},
        std::vector<Byte>{3, 4, 5})), std::size_t(3));
}

void InterleaveTest::attributeCountGaps() {
    CORRADE_COMPARE((Implementation::Interleave::attributeCount(std::vector<Byte>{0, 1, 2}, 3,
        std::vector<Byte>{3, 4, 5}, 5)), std::size_t(3));

    /* No arrays from which to get size */
    CORRADE_COMPARE(Implementation::Interleave::attributeCount(3, 5), ~std::size_t(0));
}

void InterleaveTest::stride() {
    CORRADE_COMPARE(Implementation::Interleave::stride(std::vector<Byte>()), std::size_t(1));
    CORRADE_COMPARE(Implementation::Interleave::stride(std::vector<Int>()), std::size_t(4));
    CORRADE_COMPARE((Implementation::Interleave::stride(std::vector<Byte>(), std::vector<Int>())), std::size_t(5));
}

void InterleaveTest::strideGaps() {
    CORRADE_COMPARE((Implementation::Interleave::stride(2, std::vector<Byte>(), 1, std::vector<Int>(), 12)), std::size_t(20));
}

void InterleaveTest::write() {
    std::size_t attributeCount;
    std::size_t stride;
    Containers::Array<char> data;
    std::tie(attributeCount, stride, data) = MeshTools::interleave(
        std::vector<Byte>{0, 1, 2},
        std::vector<Int>{3, 4, 5},
        std::vector<Short>{6, 7, 8});

    CORRADE_COMPARE(attributeCount, std::size_t(3));
    CORRADE_COMPARE(stride, std::size_t(7));
    if(!Utility::Endianness::isBigEndian()) {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00,
            0x01, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00,
            0x02, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00
        }));
    } else {
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06,
            0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x07,
            0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08
        }));
    }
}

void InterleaveTest::writeGaps() {
    std::size_t attributeCount;
    std::size_t stride;
    Containers::Array<char> data;
    std::tie(attributeCount, stride, data) = MeshTools::interleave(
        std::vector<Byte>{0, 1, 2}, 3,
        std::vector<Int>{3, 4, 5},
        std::vector<Short>{6, 7, 8}, 2);

    CORRADE_COMPARE(attributeCount, std::size_t(3));
    CORRADE_COMPARE(stride, std::size_t(12));

    if(!Utility::Endianness::isBigEndian()) {
        /*  byte, _____________gap, int___________________, short_____, _______gap */
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
        }));
    } else {
        /*  byte, _____________gap, ___________________int, _____short, _______gap */
        CORRADE_COMPARE(std::vector<char>(data.begin(), data.end()), (std::vector<char>{
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00
        }));
    }
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::InterleaveTest)
