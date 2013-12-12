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

#include "Math/Vector3.h"

struct Vec3 {
    float x, y, z;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct VectorConverter<3, float, Vec3> {
    constexpr static Vector<3, Float> from(const Vec3& other) {
        return {other.x, other.y, other.z};
    }

    constexpr static Vec3 to(const Vector<3, Float>& other) {
        return {other[0], other[1], other[2]};
    }
};

}

namespace Test {

class Vector3Test: public Corrade::TestSuite::Tester {
    public:
        Vector3Test();

        void construct();
        void constructDefault();
        void constructOneValue();
        void constructParts();
        void constructConversion();
        void constructCopy();

        void convert();

        void access();
        void cross();
        void axes();
        void scales();
        void twoComponent();

        void swizzleType();
        void debug();
        void configuration();
};

typedef Math::Vector3<Float> Vector3;
typedef Math::Vector3<Int> Vector3i;
typedef Math::Vector2<Float> Vector2;

Vector3Test::Vector3Test() {
    addTests<Vector3Test>({&Vector3Test::construct,
              &Vector3Test::constructDefault,
              &Vector3Test::constructOneValue,
              &Vector3Test::constructParts,
              &Vector3Test::constructConversion,
              &Vector3Test::constructCopy,

              &Vector3Test::convert,

              &Vector3Test::access,
              &Vector3Test::cross,
              &Vector3Test::axes,
              &Vector3Test::scales,
              &Vector3Test::twoComponent,

              &Vector3Test::swizzleType,
              &Vector3Test::debug,
              &Vector3Test::configuration});
}

void Vector3Test::construct() {
    #ifndef CORRADE_GCC44_COMPATIBILITY
    constexpr Vector3 a = {1.0f, 2.5f, -3.0f};
    #else
    constexpr Vector3 a(1.0f, 2.5f, -3.0f); /* Ambiguity with default copy constructor */
    #endif
    CORRADE_COMPARE(a, (Vector<3, Float>(1.0f, 2.5f, -3.0f)));
}

void Vector3Test::constructDefault() {
    constexpr Vector3 a;
    CORRADE_COMPARE(a, Vector3(0.0f, 0.0f, 0.0f));
}

void Vector3Test::constructOneValue() {
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vector3 a(-3.0f);
    CORRADE_COMPARE(a, Vector3(-3.0f, -3.0f, -3.0f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Vector3>::value));
}

void Vector3Test::constructParts() {
    constexpr Vector2 a(1.0f, 2.0f);
    constexpr Vector3 b = {a, 3.0f};
    CORRADE_COMPARE(b, Vector3(1.0f, 2.0f, 3.0f));
}

void Vector3Test::constructConversion() {
    constexpr Vector3 a(1.0f, 2.5f, -3.0f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vector3i b(a);
    CORRADE_COMPARE(b, Vector3i(1, 2, -3));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector3, Vector3i>::value));
}

void Vector3Test::constructCopy() {
    constexpr Vector<3, Float> a(1.0f, 2.5f, -3.0f);
    constexpr Vector3 b(a);
    CORRADE_COMPARE(b, Vector3(1.0f, 2.5f, -3.0f));
}

void Vector3Test::convert() {
    constexpr Vec3 a{1.5f, 2.0f, -3.5f};
    constexpr Vector3 b(1.5f, 2.0f, -3.5f);

    constexpr Vector3 c(a);
    CORRADE_COMPARE(c, b);

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vec3 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vec3, Vector3>::value));
    {
        #ifdef CORRADE_GCC44_COMPATIBILITY
        CORRADE_EXPECT_FAIL("GCC 4.4 doesn't have explicit conversion operators");
        #endif
        CORRADE_VERIFY(!(std::is_convertible<Vector3, Vec3>::value));
    }
}

void Vector3Test::access() {
    Vector3 vec(1.0f, -2.0f, 5.0f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.r(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(vec.g(), -2.0f);
    CORRADE_COMPARE(vec.z(), 5.0f);
    CORRADE_COMPARE(vec.b(), 5.0f);

    constexpr Vector3 cvec(1.0f, -2.0f, 5.0f);
    constexpr Float x = cvec.x();
    constexpr Float r = cvec.r();
    constexpr Float y = cvec.y();
    constexpr Float g = cvec.g();
    constexpr Float z = cvec.z();
    constexpr Float b = cvec.b();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(r, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
    CORRADE_COMPARE(g, -2.0f);
    CORRADE_COMPARE(z, 5.0f);
    CORRADE_COMPARE(b, 5.0f);
}

void Vector3Test::cross() {
    Vector3i a(1, -1, 1);
    Vector3i b(4, 3, 7);

    CORRADE_COMPARE(Vector3i::cross(a, b), Vector3i(-10, -3, 7));
}

void Vector3Test::axes() {
    constexpr Vector3 x = Vector3::xAxis(5.0f);
    constexpr Vector3 y = Vector3::yAxis(6.0f);
    constexpr Vector3 z = Vector3::zAxis(7.0f);
    CORRADE_COMPARE(x, Vector3(5.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(y, Vector3(0.0f, 6.0f, 0.0f));
    CORRADE_COMPARE(z, Vector3(0.0f, 0.0f, 7.0f));
}

void Vector3Test::scales() {
    constexpr Vector3 x = Vector3::xScale(-5.0f);
    constexpr Vector3 y = Vector3::yScale(-0.2f);
    constexpr Vector3 z = Vector3::zScale(71.0f);
    CORRADE_COMPARE(x, Vector3(-5.0f, 1.0f, 1.0f));
    CORRADE_COMPARE(y, Vector3(1.0f, -0.2f, 1.0f));
    CORRADE_COMPARE(z, Vector3(1.0f, 1.0f, 71.0f));
}

void Vector3Test::twoComponent() {
    Vector3 a(1.0f, 2.0f, 3.0f);
    CORRADE_COMPARE(a.xy(), Vector2(1.0f, 2.0f));

    constexpr Vector3 b(1.0f, 2.0f, 3.0f);
    constexpr Vector2 c = b.xy();
    constexpr Float d = b.xy().y();
    CORRADE_COMPARE(c, Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(d, 2.0f);
}

void Vector3Test::swizzleType() {
    constexpr Vector<4, Int> orig;

    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr
    #else
    const
    #endif
    auto b = swizzle<'y', 'z', 'a'>(orig);
    CORRADE_VERIFY((std::is_same<decltype(b), const Vector3i>::value));
}

void Vector3Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector3(0.5f, 15.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1)\n");
}

void Vector3Test::configuration() {
    Corrade::Utility::Configuration c;

    Vector3 vec(3.0f, 3.125f, 9.55f);
    std::string value("3 3.125 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector3>("vector"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector3Test)
