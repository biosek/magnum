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
#include <Utility/Configuration.h>

#include "Math/Range.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Math/Geometry/Rectangle.h"
#endif

namespace Magnum { namespace Math { namespace Test {

class RangeTest: public Corrade::TestSuite::Tester {
    public:
        RangeTest();

        void construct();
        void constructDefault();
        void constructFromSize();
        void constructConversion();
        void constructCopy();

        void access();
        void compare();
        void size();
        void center();

        void translated();
        void padded();
        void scaled();

        void subclassTypes();
        void subclass();

        #ifdef MAGNUM_BUILD_DEPRECATED
        void deprecated();
        #endif

        void debug();
        void configuration();
};

#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Range1D<Float> Range1D;
#else
typedef Math::Range<1, Float> Range1D;
#endif
typedef Math::Range2D<Float> Range2D;
typedef Math::Range3D<Float> Range3D;
#ifndef CORRADE_GCC46_COMPATIBILITY
typedef Math::Range1D<Int> Range1Di;
#else
typedef Math::Range<1, Int> Range1Di;
#endif
typedef Math::Range2D<Int> Range2Di;
typedef Math::Range3D<Int> Range3Di;
typedef Vector2<Int> Vector2i;
typedef Vector3<Int> Vector3i;

RangeTest::RangeTest() {
    addTests<RangeTest>({&RangeTest::construct,
              &RangeTest::constructDefault,
              &RangeTest::constructFromSize,
              &RangeTest::constructConversion,
              &RangeTest::constructCopy,

              &RangeTest::access,
              &RangeTest::compare,
              &RangeTest::size,
              &RangeTest::center,

              &RangeTest::translated,
              &RangeTest::padded,
              &RangeTest::scaled,

              &RangeTest::subclassTypes,
              &RangeTest::subclass,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &RangeTest::deprecated,
              #endif

              &RangeTest::debug,
              &RangeTest::configuration});
}

void RangeTest::construct() {
    constexpr Range1Di a(3, 23);
    constexpr Range2Di b({3, 5}, {23, 78});
    constexpr Range3Di c({3, 5, -7}, {23, 78, 2});

    CORRADE_COMPARE(a, (Range<1, Int>(3, 23)));
    CORRADE_COMPARE(b, (Range<2, Int>({3, 5}, {23, 78})));
    CORRADE_COMPARE(c, (Range<3, Int>({3, 5, -7}, {23, 78, 2})));
}

void RangeTest::constructDefault() {
    constexpr Range1Di a;
    constexpr Range2Di b;
    constexpr Range3Di c;

    CORRADE_COMPARE(a, Range1Di(0, 0));
    CORRADE_COMPARE(b, Range2Di({0, 0}, {0, 0}));
    CORRADE_COMPARE(c, Range3Di({0, 0, 0}, {0, 0, 0}));
}

void RangeTest::constructFromSize() {
    CORRADE_COMPARE(Range1Di::fromSize(3, 23), Range1Di(3, 26));
    CORRADE_COMPARE(Range2Di::fromSize({3, 5}, {23, 78}), Range2Di({3, 5}, {26, 83}));
    CORRADE_COMPARE(Range3Di::fromSize({3, 5, -7}, {23, 78, 9}), Range3Di({3, 5, -7}, {26, 83, 2}));
}

void RangeTest::constructConversion() {
    constexpr Range1D a(1.3f, -15.0f);
    constexpr Range2D b({1.3f, 2.7f}, {-15.0f, 7.0f});
    constexpr Range3D c({1.3f, 2.7f, -1.5f}, {-15.0f, 7.0f, 0.3f});

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Range1Di d(a);
    CORRADE_COMPARE(d, Range1Di(1, -15));

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Range2Di e(b);
    CORRADE_COMPARE(e, Range2Di({1, 2}, {-15, 7}));

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Range3Di f(c);
    CORRADE_COMPARE(f, Range3Di({1, 2, -1}, {-15, 7, 0}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Range<2, Float>, Range<2, Int>>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range1D, Range1Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range2D, Range2Di>::value));
    CORRADE_VERIFY(!(std::is_convertible<Range3D, Range3Di>::value));
}

void RangeTest::constructCopy() {
    constexpr Range1Di a(3, 23);
    constexpr Range2Di b({3, 5}, {23, 78});
    constexpr Range3Di c({3, 5, -7}, {23, 78, 2});

    constexpr Range1Di d(a);
    constexpr Range2Di e(b);
    constexpr Range3Di f(c);

    CORRADE_COMPARE(d, Range1Di(3, 23));
    CORRADE_COMPARE(e, Range2Di({3, 5}, {23, 78}));
    CORRADE_COMPARE(f, Range3Di({3, 5, -7}, {23, 78, 2}));
}

void RangeTest::access() {
    Range1Di line(34, 47);
    Range2Di rect({34, 23}, {47, 30});
    Range3Di cube({34, 23, -17}, {47, 30, 12});

    constexpr Range1Di cline(34, 47);
    constexpr Range2Di crect({34, 23}, {47, 30});
    constexpr Range3Di ccube({34, 23, -17}, {47, 30, 12});

    CORRADE_COMPARE(line.min(), 34);
    CORRADE_COMPARE(cline.min(), 34);
    CORRADE_COMPARE(line.max(), 47);
    CORRADE_COMPARE(cline.max(), 47);

    CORRADE_COMPARE(rect.bottomLeft(), Vector2i(34, 23));
    CORRADE_COMPARE(rect.topRight(), Vector2i(47, 30));
    constexpr Vector2i bottomLeft = crect.bottomLeft();
    constexpr Vector2i topRight = crect.topRight();
    CORRADE_COMPARE(bottomLeft, Vector2i(34, 23));
    CORRADE_COMPARE(topRight, Vector2i(47, 30));

    CORRADE_COMPARE(rect.left(), 34);
    CORRADE_COMPARE(rect.right(), 47);
    CORRADE_COMPARE(rect.bottom(), 23);
    CORRADE_COMPARE(rect.top(), 30);
    constexpr Int left2 = crect.left();
    constexpr Int right2 = crect.right();
    constexpr Int bottom2 = crect.bottom();
    constexpr Int top2 = crect.top();
    CORRADE_COMPARE(left2, 34);
    CORRADE_COMPARE(right2, 47);
    CORRADE_COMPARE(bottom2, 23);
    CORRADE_COMPARE(top2, 30);

    CORRADE_COMPARE(cube.backBottomLeft(), Vector3i(34, 23, -17));
    CORRADE_COMPARE(cube.frontTopRight(), Vector3i(47, 30, 12));
    constexpr Vector3i backBottomLeft = ccube.backBottomLeft();
    constexpr Vector3i frontTopRight = ccube.frontTopRight();
    CORRADE_COMPARE(backBottomLeft, Vector3i(34, 23, -17));
    CORRADE_COMPARE(frontTopRight, Vector3i(47, 30, 12));

    CORRADE_COMPARE(cube.left(), 34);
    CORRADE_COMPARE(cube.right(), 47);
    CORRADE_COMPARE(cube.bottom(), 23);
    CORRADE_COMPARE(cube.top(), 30);
    CORRADE_COMPARE(cube.back(), -17);
    CORRADE_COMPARE(cube.front(), 12);
    constexpr Int left3 = ccube.left();
    constexpr Int right3 = ccube.right();
    constexpr Int bottom3 = ccube.bottom();
    constexpr Int top3 = ccube.top();
    constexpr Int back3 = ccube.back();
    constexpr Int front3 = ccube.front();
    CORRADE_COMPARE(left3, 34);
    CORRADE_COMPARE(right3, 47);
    CORRADE_COMPARE(bottom3, 23);
    CORRADE_COMPARE(top3, 30);
    CORRADE_COMPARE(back3, -17);
    CORRADE_COMPARE(front3, 12);

    CORRADE_COMPARE(rect.bottomRight(), Vector2i(47, 23));
    CORRADE_COMPARE(rect.topLeft(), Vector2i(34, 30));

    CORRADE_COMPARE(cube.backBottomRight(), Vector3i(47, 23, -17));
    CORRADE_COMPARE(cube.backTopLeft(), Vector3i(34, 30, -17));
    CORRADE_COMPARE(cube.backTopRight(), Vector3i(47, 30, -17));
    CORRADE_COMPARE(cube.frontBottomLeft(), Vector3i(34, 23, 12));
    CORRADE_COMPARE(cube.frontBottomRight(), Vector3i(47, 23, 12));
    CORRADE_COMPARE(cube.frontTopLeft(), Vector3i(34, 30, 12));
}

void RangeTest::compare() {
    CORRADE_VERIFY(Range2Di({34, 23}, {47, 30}) == Range2Di({34, 23}, {47, 30}));
    CORRADE_VERIFY(Range2Di({34, 23}, {47, 30}) != Range2Di({34, 23}, {48, 30}));
    CORRADE_VERIFY(Range2Di({34, 23}, {47, 30}) != Range2Di({35, 23}, {47, 30}));

    CORRADE_VERIFY(Range1D(1.0f, 1.0f) != Range1D(1.0f + TypeTraits<Float>::epsilon()*2, 1.0f));
    CORRADE_VERIFY(Range1D(1.0f, 1.0f) != Range1D(1.0f, 1.0f + TypeTraits<Float>::epsilon()*2));
    CORRADE_VERIFY(Range1D(1.0f, 1.0f) == Range1D(1.0f + TypeTraits<Float>::epsilon()/2.0f,
                                                  1.0f + TypeTraits<Float>::epsilon()/2.0f));
}

void RangeTest::size() {
    const Range1Di line(34, 47);
    const Range2Di rect({34, 23}, {47, 30});
    const Range3Di cube({34, 23, -17}, {47, 30, 12});

    CORRADE_COMPARE(line.size(), 13);
    CORRADE_COMPARE(rect.size(), Vector2i(13, 7));
    CORRADE_COMPARE(cube.size(), Vector3i(13, 7, 29));

    CORRADE_COMPARE(rect.sizeX(), 13);
    CORRADE_COMPARE(rect.sizeY(), 7);

    CORRADE_COMPARE(cube.sizeX(), 13);
    CORRADE_COMPARE(cube.sizeY(), 7);
    CORRADE_COMPARE(cube.sizeZ(), 29);
}

void RangeTest::center() {
    const Range1Di line(34, 47);
    const Range2Di rect({34, 23}, {47, 30});
    const Range3Di cube({34, 23, -17}, {47, 30, 12});

    CORRADE_COMPARE(line.center(), 40);
    CORRADE_COMPARE(rect.center(), Vector2i(40, 26));
    CORRADE_COMPARE(cube.center(), Vector3i(40, 26, -2));

    CORRADE_COMPARE(rect.centerX(), 40);
    CORRADE_COMPARE(rect.centerY(), 26);

    CORRADE_COMPARE(cube.centerX(), 40);
    CORRADE_COMPARE(cube.centerY(), 26);
    CORRADE_COMPARE(cube.centerZ(), -2);
}

void RangeTest::translated() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({17, 63}, {30, 70});

    CORRADE_COMPARE(a.translated({-17, 40}), b);
    CORRADE_COMPARE(a.size(), b.size());
}

void RangeTest::padded() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({31, 28}, {50, 25});

    CORRADE_COMPARE(a.padded({3, -5}), b);
    CORRADE_COMPARE(a.center(), b.center());
}

void RangeTest::scaled() {
    Range2Di a({34, 23}, {47, 30});
    Range2Di b({68, -69}, {94, -90});

    CORRADE_COMPARE(a.scaled({2, -3}), b);
}

template<class T> class BasicRect: public Math::Range<2, T> {
    public:
        /* MSVC 2013 can't cope with {} here */
        template<class ...U> BasicRect(U&&... args): Math::Range<2, T>(std::forward<U>(args)...) {}

        MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(2, BasicRect, Vector2)
};

typedef BasicRect<Int> Recti;

void RangeTest::subclassTypes() {
    const Vector2i a;
    CORRADE_VERIFY((std::is_same<decltype(Recti::fromSize(a, a)), Recti>::value));

    const Recti r;
    CORRADE_VERIFY((std::is_same<decltype(r.translated(a)), Recti>::value));
    CORRADE_VERIFY((std::is_same<decltype(r.padded(a)), Recti>::value));
    CORRADE_VERIFY((std::is_same<decltype(r.scaled(a)), Recti>::value));
}

void RangeTest::subclass() {
    CORRADE_COMPARE(Recti::fromSize({3, 5}, {23, 78}),
                    Recti(Vector2i{3, 5}, Vector2i{26, 83}));

    CORRADE_COMPARE(Recti(Vector2i{34, 23}, Vector2i{47, 30}).translated({-17, 40}),
                    Recti(Vector2i{17, 63}, Vector2i{30, 70}));
    CORRADE_COMPARE(Recti(Vector2i{34, 23}, Vector2i{47, 30}).padded({3, -5}),
                    Recti(Vector2i{31, 28}, Vector2i{50, 25}));
    CORRADE_COMPARE(Recti(Vector2i{34, 23}, Vector2i{47, 30}).scaled({2, -3}),
                    Recti(Vector2i{68, -69}, Vector2i{94, -90}));
}

void RangeTest::debug() {
    std::ostringstream o;
    Debug(&o) << Range2Di({34, 23}, {47, 30});

    CORRADE_COMPARE(o.str(), "Range({34, 23}, {47, 30})\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void RangeTest::deprecated() {
    typedef Geometry::Rectangle<Float> Rectangle;
    typedef Geometry::Rectangle<Int> Rectanglei;

    Rectanglei a({45, 23}, {-17, 35});
    CORRADE_COMPARE(Rectanglei(), Range2Di({0, 0}, {0, 0}));
    CORRADE_COMPARE(a, Range2Di({45, 23}, {-17, 35}));
    CORRADE_COMPARE(Rectanglei(a), Range2Di({45, 23}, {-17, 35}));
    CORRADE_COMPARE(Rectangle(a), Range2D({45.0f, 23.0f}, {-17.0f, 35.0f}));

    CORRADE_COMPARE(a.width(), -62);
    CORRADE_COMPARE(a.height(), 12);

    CORRADE_VERIFY(!(std::is_convertible<Rectangle, Rectanglei>::value));

    Corrade::Utility::Configuration c;
    Rectangle rect({3.0f, 3.125f}, {9.0f, 9.55f});
    std::string value("3 3.125 9 9.55");

    c.setValue("rectangle", rect);
    CORRADE_COMPARE(c.value("rectangle"), value);
    CORRADE_COMPARE(c.value<Rectangle>("rectangle"), rect);
}
#endif

void RangeTest::configuration() {
    Corrade::Utility::Configuration c;

    Range2D rect({3.0f, 3.125f}, {9.0f, 9.55f});
    std::string value("3 3.125 9 9.55");

    c.setValue("rectangle", rect);
    CORRADE_COMPARE(c.value("rectangle"), value);
    CORRADE_COMPARE(c.value<Range2D>("rectangle"), rect);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::RangeTest)
