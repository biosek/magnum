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

#include <array>
#include <vector>
#include <Containers/Array.h>

#include "Buffer.h"
#include "Context.h"
#include "Extensions.h"
#include "Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class BufferGLTest: public AbstractOpenGLTester {
    public:
        explicit BufferGLTest();

        void construct();
        void data();
        void map();
        #ifdef MAGNUM_TARGET_GLES2
        void mapSub();
        #endif
        void mapRange();
        void mapRangeExplicitFlush();
        #ifndef MAGNUM_TARGET_GLES2
        void copy();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void invalidate();
        #endif
};

BufferGLTest::BufferGLTest() {
    addTests({&BufferGLTest::construct,
              &BufferGLTest::data,
              &BufferGLTest::map,
              #ifdef MAGNUM_TARGET_GLES2
              &BufferGLTest::mapSub,
              #endif
              &BufferGLTest::mapRange,
              &BufferGLTest::mapRangeExplicitFlush,
              #ifndef MAGNUM_TARGET_GLES2
              &BufferGLTest::copy,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &BufferGLTest::invalidate
              #endif
              });
}

void BufferGLTest::construct() {
    Buffer buffer;
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(buffer.targetHint(), Buffer::Target::Array);

    CORRADE_COMPARE(buffer.size(), 0);
    MAGNUM_VERIFY_NO_ERROR();
}

void BufferGLTest::data() {
    Buffer buffer;

    /* Plain array */
    constexpr Int data[] = {2, 7, 5, 13, 25};
    buffer.setData({data, 5}, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL vector */
    std::vector<Int> data2{2, 7, 5, 13, 25};
    buffer.setData(data2, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL array */
    std::array<Int, 5> data3{{2, 7, 5, 13, 25}};
    buffer.setData(data3, BufferUsage::StaticDraw);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::Array<Int> contents = buffer.data<Int>();
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(contents.size(), 5);
    CORRADE_COMPARE(contents[0], 2);
    CORRADE_COMPARE(contents[1], 7);
    CORRADE_COMPARE(contents[2], 5);
    CORRADE_COMPARE(contents[3], 13);
    CORRADE_COMPARE(contents[4], 25);
    #endif

    /* Plain array */
    constexpr Int subData[] = {125, 3, 15};
    buffer.setSubData(4, {subData, 3});
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL vector */
    std::vector<Int> subData2{125, 3, 15};
    buffer.setSubData(4, subData2);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /* STL array */
    std::array<Int, 3> subData3{{125, 3, 15}};
    buffer.setSubData(4, subData3);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(buffer.size(), 5*4);

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::Array<Int> subContents = buffer.subData<Int>(4, 3);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(subContents.size(), 3);
    CORRADE_COMPARE(subContents[0], 125);
    CORRADE_COMPARE(subContents[1], 3);
    CORRADE_COMPARE(subContents[2], 15);
    #endif
}

void BufferGLTest::map() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::mapbuffer>())
        CORRADE_SKIP(Extensions::GL::OES::mapbuffer::string() + std::string(" is not supported"));
    #endif
    Buffer buffer;

    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);

    #ifndef MAGNUM_TARGET_GLES2
    char* contents = reinterpret_cast<char*>(buffer.map(Buffer::MapAccess::ReadWrite));
    #else
    char* contents = reinterpret_cast<char*>(buffer.map(Buffer::MapAccess::WriteOnly));
    #endif
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(contents);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(contents[2], 5);
    #endif
    contents[3] = 107;

    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data<char>();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[3], 107);
    #endif
}

#ifdef MAGNUM_TARGET_GLES2
void BufferGLTest::mapSub() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::CHROMIUM::map_sub>())
        CORRADE_SKIP(Extensions::GL::CHROMIUM::map_sub::string() + std::string(" is not supported"));

    Buffer buffer;

    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);

    char* contents = reinterpret_cast<char*>(buffer.mapSub(1, 4, Buffer::MapAccess::WriteOnly));
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(contents);
    contents[3] = 107;

    buffer.unmapSub();
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
}
#endif

void BufferGLTest::mapRange() {
    #ifndef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    constexpr char data[] = {2, 7, 5, 13, 25};
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    char* contents = reinterpret_cast<char*>(buffer.map(1, 4, Buffer::MapFlag::Read|Buffer::MapFlag::Write));
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_VERIFY(contents);
    CORRADE_COMPARE(contents[2], 13);
    contents[3] = 107;

    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data<char>();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[4], 107);
    #endif
}

void BufferGLTest::mapRangeExplicitFlush() {
    #ifndef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::ARB::map_buffer_range::string() + std::string(" is not supported"));
    #else
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::map_buffer_range>())
        CORRADE_SKIP(Extensions::GL::EXT::map_buffer_range::string() + std::string(" is not supported"));
    #endif

    constexpr char data[] = {2, 7, 5, 13, 25};
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    /* Map, set byte, don't flush and unmap */
    char* contents = reinterpret_cast<char*>(buffer.map(1, 4, Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit));
    CORRADE_VERIFY(contents);
    contents[2] = 99;
    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /* Unflushed range _might_ not be changed, thus nothing to test */

    /* Map, set byte, flush and unmap */
    contents = reinterpret_cast<char*>(buffer.map(1, 4, Buffer::MapFlag::Write|Buffer::MapFlag::FlushExplicit));
    CORRADE_VERIFY(contents);
    contents[3] = 107;
    buffer.flushMappedRange(3, 1);
    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(buffer.unmap());
    MAGNUM_VERIFY_NO_ERROR();

    /* Flushed range should be changed */
    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    Containers::Array<char> changedContents = buffer.data<char>();
    CORRADE_COMPARE(changedContents.size(), 5);
    CORRADE_COMPARE(changedContents[4], 107);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void BufferGLTest::copy() {
    Buffer buffer1;
    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer1.setData(data, BufferUsage::StaticDraw);

    Buffer buffer2;
    buffer2.setData({nullptr, 5}, BufferUsage::StaticDraw);

    Buffer::copy(buffer1, buffer2, 1, 2, 3);
    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to verify the contents in ES? */
    #ifndef MAGNUM_TARGET_GLES
    const Containers::Array<char> subContents = buffer2.subData<char>(2, 3);
    CORRADE_COMPARE(subContents.size(), 3);
    CORRADE_COMPARE(subContents[0], 7);
    CORRADE_COMPARE(subContents[1], 5);
    CORRADE_COMPARE(subContents[2], 13);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void BufferGLTest::invalidate() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>())
        CORRADE_SKIP(Extensions::GL::ARB::invalidate_subdata::string() + std::string(" is not supported"));

    Buffer buffer;
    constexpr char data[] = {2, 7, 5, 13, 25};
    buffer.setData(data, BufferUsage::StaticDraw);

    /* Just test that no errors are emitted */

    buffer.invalidateSubData(3, 2);
    MAGNUM_VERIFY_NO_ERROR();

    buffer.invalidateData();
    MAGNUM_VERIFY_NO_ERROR();
}
#endif

}}

CORRADE_TEST_MAIN(Magnum::Test::BufferGLTest)
