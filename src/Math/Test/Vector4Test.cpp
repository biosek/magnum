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

#include "Math/Vector4.h"

struct Vec4 {
    float x, y, z, w;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct VectorConverter<4, float, Vec4> {
    constexpr static Vector<4, Float> from(const Vec4& other) {
        return {other.x, other.y, other.z, other.w};
    }

    constexpr static Vec4 to(const Vector<4, Float>& other) {
        return {other[0], other[1], other[2], other[3]};
    }
};

}

namespace Test {

class Vector4Test: public Corrade::TestSuite::Tester {
    public:
        Vector4Test();

        void construct();
        void constructDefault();
        void constructOneValue();
        void constructParts();
        void constructConversion();
        void constructCopy();

        void convert();

        void access();
        void threeComponent();
        void twoComponent();

        void swizzleType();
        void debug();
        void configuration();
};

typedef Math::Vector4<Float> Vector4;
typedef Math::Vector4<Int> Vector4i;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector2<Float> Vector2;

Vector4Test::Vector4Test() {
    addTests<Vector4Test>({&Vector4Test::construct,
              &Vector4Test::constructDefault,
              &Vector4Test::constructOneValue,
              &Vector4Test::constructParts,
              &Vector4Test::constructConversion,
              &Vector4Test::constructCopy,

              &Vector4Test::convert,

              &Vector4Test::access,
              &Vector4Test::threeComponent,
              &Vector4Test::twoComponent,

              &Vector4Test::swizzleType,
              &Vector4Test::debug,
              &Vector4Test::configuration});
}

void Vector4Test::construct() {
    #ifndef CORRADE_GCC44_COMPATIBILITY
    constexpr Vector4 a = {1.0f, -2.5f, 3.0f, 4.1f};
    #else
    constexpr Vector4 a(1.0f, -2.5f, 3.0f, 4.1f); /* Ambiguity with default copy constructor */
    #endif
    CORRADE_COMPARE(a, (Vector<4, Float>(1.0f, -2.5f, 3.0f, 4.1f)));
}

void Vector4Test::constructDefault() {
    constexpr Vector4 a;
    CORRADE_COMPARE(a, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
}

void Vector4Test::constructOneValue() {
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vector4 a(4.3f);
    CORRADE_COMPARE(a, Vector4(4.3f, 4.3f, 4.3f, 4.3f));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Vector4>::value));
}

void Vector4Test::constructParts() {
    constexpr Vector3 a(1.0f, 2.0f, 3.0f);
    constexpr Vector4 b = {a, 4.0f};
    CORRADE_COMPARE(b, Vector4(1.0f, 2.0f, 3.0f, 4.0f));
}

void Vector4Test::constructConversion() {
    constexpr Vector4 a(1.0f, -2.5f, 3.0f, 4.1f);
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vector4i b(a);
    CORRADE_COMPARE(b, Vector4i(1, -2, 3, 4));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector4, Vector4i>::value));
}

void Vector4Test::constructCopy() {
    constexpr Vector<4, Float> a(1.0f, -2.5f, 3.0f, 4.1f);
    constexpr Vector4 b(a);
    CORRADE_COMPARE(b, Vector4(1.0f, -2.5f, 3.0f, 4.1f));
}

void Vector4Test::convert() {
    constexpr Vec4 a{1.5f, 2.0f, -3.5f, -0.5f};
    constexpr Vector4 b(1.5f, 2.0f, -3.5f, -0.5f);

    constexpr Vector4 c(a);
    CORRADE_COMPARE(c, b);

    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr /* Not constexpr under GCC < 4.7 */
    #endif
    Vec4 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);
    CORRADE_COMPARE(d.w, a.w);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vec4, Vector4>::value));
    {
        #ifdef CORRADE_GCC44_COMPATIBILITY
        CORRADE_EXPECT_FAIL("GCC 4.4 doesn't have explicit conversion operators");
        #endif
        CORRADE_VERIFY(!(std::is_convertible<Vector4, Vec4>::value));
    }
}

void Vector4Test::access() {
    Vector4 vec(1.0f, -2.0f, 5.0f, 0.5f);
    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.r(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(vec.g(), -2.0f);
    CORRADE_COMPARE(vec.z(), 5.0f);
    CORRADE_COMPARE(vec.b(), 5.0f);
    CORRADE_COMPARE(vec.w(), 0.5f);
    CORRADE_COMPARE(vec.a(), 0.5f);

    constexpr Vector4 cvec(1.0f, -2.0f, 5.0f, 0.5f);
    constexpr Float x = cvec.x();
    constexpr Float r = cvec.r();
    constexpr Float y = cvec.y();
    constexpr Float g = cvec.g();
    constexpr Float z = cvec.z();
    constexpr Float b = cvec.b();
    constexpr Float w = cvec.w();
    constexpr Float a = cvec.a();
    CORRADE_COMPARE(x, 1.0f);
    CORRADE_COMPARE(r, 1.0f);
    CORRADE_COMPARE(y, -2.0f);
    CORRADE_COMPARE(g, -2.0f);
    CORRADE_COMPARE(z, 5.0f);
    CORRADE_COMPARE(b, 5.0f);
    CORRADE_COMPARE(w, 0.5f);
    CORRADE_COMPARE(a, 0.5f);
}

void Vector4Test::threeComponent() {
    Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    CORRADE_COMPARE(a.xyz(), Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(a.rgb(), Vector3(1.0f, 2.0f, 3.0f));

    constexpr Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);
    constexpr Vector3 c = b.xyz();
    constexpr Float d = b.xyz().y();
    CORRADE_COMPARE(c, Vector3(1.0f, 2.0f, 3.0f));
    CORRADE_COMPARE(d, 2.0f);
}

void Vector4Test::twoComponent() {
    Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    CORRADE_COMPARE(a.xy(), Vector2(1.0f, 2.0f));

    constexpr Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);
    constexpr Vector2 c = b.xy();
    constexpr Float d = b.xy().x();
    CORRADE_COMPARE(c, Vector2(1.0f, 2.0f));
    CORRADE_COMPARE(d, 1.0f);
}

void Vector4Test::swizzleType() {
    constexpr Vector4i orig;

    #if !defined(CORRADE_GCC45_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
    constexpr
    #else
    const
    #endif
    auto c = swizzle<'y', 'a', 'y', 'x'>(orig);
    CORRADE_VERIFY((std::is_same<decltype(c), const Vector4i>::value));
}

void Vector4Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1, 1)\n");
}

void Vector4Test::configuration() {
    Corrade::Utility::Configuration c;

    Vector4 vec(3.0f, 3.125f, 9.0f, 9.55f);
    std::string value("3 3.125 9 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector4>("vector"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector4Test)
