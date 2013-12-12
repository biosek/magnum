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

#include <Containers/Array.h>
#include <TestSuite/Tester.h>
#include <Utility/Directory.h>

#include "Text/AbstractFont.h"

#include "testConfigure.h"

namespace Magnum { namespace Text { namespace Test {

class AbstractFontTest: public TestSuite::Tester {
    public:
        explicit AbstractFontTest();

        void openSingleData();
        void openFile();
};

AbstractFontTest::AbstractFontTest() {
    addTests<AbstractFontTest>({&AbstractFontTest::openSingleData,
              &AbstractFontTest::openFile});
}

namespace {

class SingleDataFont: public Text::AbstractFont {
    public:
        explicit SingleDataFont(): opened(false) {}

        Features doFeatures() const override { return Feature::OpenData; }
        bool doIsOpened() const override { return opened; }
        void doClose() override {}

        std::pair<Float, Float> doOpenSingleData(const Containers::ArrayReference<const unsigned char> data, Float) override {
            opened = (data.size() == 1 && data[0] == 0xa5);
            return {};
        }

        UnsignedInt doGlyphId(char32_t) override { return 0; }

        Vector2 doGlyphAdvance(UnsignedInt) override { return {}; }

        std::unique_ptr<AbstractLayouter> doLayout(const GlyphCache&, Float, const std::string&) override {
            return {};
        }

        bool opened;
};

}

void AbstractFontTest::openSingleData() {
    /* doOpenData() should call doOpenSingleData() */
    SingleDataFont font;
    const unsigned char data[] = {0xa5};
    CORRADE_VERIFY(!font.isOpened());
    font.openData(std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>{{{}, data}}, 3.0f);
    CORRADE_VERIFY(font.isOpened());
}

void AbstractFontTest::openFile() {
    /* doOpenFile() should call doOpenSingleData() */
    SingleDataFont font;
    CORRADE_VERIFY(!font.isOpened());
    font.openFile(Utility::Directory::join(TEXT_TEST_DIR, "data.bin"), 3.0f);
    CORRADE_VERIFY(font.isOpened());
}

}}}

CORRADE_TEST_MAIN(Magnum::Text::Test::AbstractFontTest)
