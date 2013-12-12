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

#include "Math/DualComplex.h"
#include "Math/DualQuaternion.h"

namespace Magnum { namespace Math { namespace Test {

class DualComplexTest: public Corrade::TestSuite::Tester {
    public:
        explicit DualComplexTest();

        void construct();
        void constructDefault();
        void constructFromVector();
        void constructCopy();

        void isNormalized();

        void multiply();

        void lengthSquared();
        void length();
        void normalized();

        void complexConjugated();
        void dualConjugated();
        void conjugated();
        void inverted();
        void invertedNormalized();

        void rotation();
        void translation();
        void combinedTransformParts();
        void matrix();
        void transformPoint();

        void debug();
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Complex<Float> Complex;
typedef Math::Dual<Float> Dual;
typedef Math::DualComplex<Float> DualComplex;
typedef Math::Matrix3<Float> Matrix3;
typedef Math::Vector2<Float> Vector2;

DualComplexTest::DualComplexTest() {
    addTests<DualComplexTest>({&DualComplexTest::construct,
              &DualComplexTest::constructDefault,
              &DualComplexTest::constructFromVector,
              &DualComplexTest::constructCopy,

              &DualComplexTest::isNormalized,

              &DualComplexTest::multiply,

              &DualComplexTest::lengthSquared,
              &DualComplexTest::length,
              &DualComplexTest::normalized,

              &DualComplexTest::complexConjugated,
              &DualComplexTest::dualConjugated,
              &DualComplexTest::conjugated,
              &DualComplexTest::inverted,
              &DualComplexTest::invertedNormalized,

              &DualComplexTest::rotation,
              &DualComplexTest::translation,
              &DualComplexTest::combinedTransformParts,
              &DualComplexTest::matrix,
              &DualComplexTest::transformPoint,

              &DualComplexTest::debug});
}

void DualComplexTest::construct() {
    constexpr DualComplex a({-1.0f, 2.5f}, {3.0f, -7.5f});
    CORRADE_COMPARE(a, DualComplex({-1.0f, 2.5f}, {3.0f, -7.5f}));

    constexpr Complex b = a.real();
    constexpr Complex c = a.dual();
    CORRADE_COMPARE(b, Complex(-1.0f, 2.5f));
    CORRADE_COMPARE(c, Complex(3.0f, -7.5f));

    constexpr DualComplex d(Complex(-1.0f, 2.5f));
    CORRADE_COMPARE(d, DualComplex({-1.0f, 2.5f}, {0.0f, 0.0f}));
}

void DualComplexTest::constructDefault() {
    constexpr DualComplex a;
    CORRADE_COMPARE(a, DualComplex({1.0f, 0.0f}, {0.0f, 0.0f}));
    CORRADE_COMPARE(a.length(), 1.0f);
}

void DualComplexTest::constructFromVector() {
    constexpr DualComplex a(Vector2(1.5f, -3.0f));
    CORRADE_COMPARE(a, DualComplex({1.0f, 0.0f}, {1.5f, -3.0f}));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Vector2, DualComplex>::value));
}

void DualComplexTest::constructCopy() {
    constexpr Math::Dual<Complex> a({-1.0f, 2.5f}, {3.0f, -7.5f});
    constexpr DualComplex b(a);
    CORRADE_COMPARE(b, DualComplex({-1.0f, 2.5f}, {3.0f, -7.5f}));
}

void DualComplexTest::isNormalized() {
    CORRADE_VERIFY(!DualComplex({2.0f, 1.0f}, {}).isNormalized());
    CORRADE_VERIFY((DualComplex::rotation(Deg(23.0f))*DualComplex::translation({6.0f, 3.0f})).isNormalized());
}

void DualComplexTest::multiply() {
    DualComplex a({-1.5f,  2.0f}, { 3.0f, -6.5f});
    DualComplex b({ 2.0f, -7.5f}, {-0.5f,  1.0f});;
    CORRADE_COMPARE(a*b, DualComplex({12.0f, 15.25f}, {1.75f, -9.0f}));
}

void DualComplexTest::lengthSquared() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.lengthSquared(), 10.0f);
}

void DualComplexTest::length() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.length(), 3.162278f);
}

void DualComplexTest::normalized() {
    DualComplex a({-1.0f, 3.0f}, {0.5f, -2.0f});
    DualComplex b({-0.316228f, 0.948683f}, {0.5f, -2.0f});
    CORRADE_COMPARE(a.normalized().length(), 1.0f);
    CORRADE_COMPARE(a.normalized(), b);
}

void DualComplexTest::complexConjugated() {
    DualComplex a({-1.0f,  2.5f}, {3.0f, -7.5f});
    DualComplex b({-1.0f, -2.5f}, {3.0f,  7.5f});
    CORRADE_COMPARE(a.complexConjugated(), b);
}

void DualComplexTest::dualConjugated() {
    DualComplex a({-1.0f, 2.5f}, { 3.0f, -7.5f});
    DualComplex b({-1.0f, 2.5f}, {-3.0f,  7.5f});
    CORRADE_COMPARE(a.dualConjugated(), b);
}

void DualComplexTest::conjugated() {
    DualComplex a({-1.0f,  2.5f}, { 3.0f, -7.5f});
    DualComplex b({-1.0f, -2.5f}, {-3.0f, -7.5f});
    CORRADE_COMPARE(a.conjugated(), b);
}

void DualComplexTest::inverted() {
    DualComplex a({-1.0f, 1.5f}, {3.0f, -7.5f});
    DualComplex b({-0.307692f, -0.461538f}, {4.384616f, -0.923077f});
    CORRADE_COMPARE(a*a.inverted(), DualComplex());
    CORRADE_COMPARE(a.inverted(), b);
}

void DualComplexTest::invertedNormalized() {
    DualComplex a({-0.316228f,  0.9486831f}, {     3.0f,    -2.5f});
    DualComplex b({-0.316228f, -0.9486831f}, {3.320391f, 2.05548f});

    std::ostringstream o;
    Error::setOutput(&o);
    DualComplex notInverted = DualComplex({-1.0f, -2.5f}, {}).invertedNormalized();
    CORRADE_VERIFY(notInverted != notInverted);
    CORRADE_COMPARE(o.str(), "Math::Complex::invertedNormalized(): complex number must be normalized\n");

    DualComplex inverted = a.invertedNormalized();
    CORRADE_COMPARE(a*inverted, DualComplex());
    CORRADE_COMPARE(inverted*a, DualComplex());
    CORRADE_COMPARE(inverted, b);
}

void DualComplexTest::rotation() {
    DualComplex a = DualComplex::rotation(Deg(120.0f));
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, DualComplex({-0.5f, 0.8660254f}, {0.0f, 0.0f}));
    CORRADE_COMPARE_AS(a.rotation().angle(), Deg(120.0f), Rad);

    /* Constexpr access to rotation */
    constexpr DualComplex b({-1.0f, 2.0f}, {});
    constexpr Complex c = b.rotation();
    CORRADE_COMPARE(c, Complex(-1.0f, 2.0f));
}

void DualComplexTest::translation() {
    Vector2 vec(1.5f, -3.5f);
    DualComplex a = DualComplex::translation(vec);
    CORRADE_COMPARE(a.length(), 1.0f);
    CORRADE_COMPARE(a, DualComplex({}, {1.5f, -3.5f}));
    CORRADE_COMPARE(a.translation(), vec);
}

void DualComplexTest::combinedTransformParts() {
    Vector2 translation = Vector2(-1.5f, 2.75f);
    DualComplex a = DualComplex::translation(translation)*DualComplex::rotation(Deg(23.0f));
    DualComplex b = DualComplex::rotation(Deg(23.0f))*DualComplex::translation(translation);

    CORRADE_COMPARE_AS(a.rotation().angle(), Deg(23.0f), Rad);
    CORRADE_COMPARE_AS(b.rotation().angle(), Deg(23.0f), Rad);
    CORRADE_COMPARE(a.translation(), translation);
    CORRADE_COMPARE(b.translation(), Complex::rotation(Deg(23.0f)).transformVector(translation));
}

void DualComplexTest::matrix() {
    DualComplex a = DualComplex::rotation(Deg(23.0f))*DualComplex::translation({2.0f, 3.0f});
    Matrix3 m = Matrix3::rotation(Deg(23.0f))*Matrix3::translation({2.0f, 3.0f});

    CORRADE_COMPARE(a.toMatrix(), m);

    std::ostringstream o;
    Error::setOutput(&o);
    DualComplex::fromMatrix(m*2);
    CORRADE_COMPARE(o.str(), "Math::DualComplex::fromMatrix(): the matrix doesn't represent rigid transformation\n");

    DualComplex b = DualComplex::fromMatrix(m);
    CORRADE_COMPARE(b, a);
}

void DualComplexTest::transformPoint() {
    DualComplex a = DualComplex::translation({2.0f, 3.0f})*DualComplex::rotation(Deg(23.0f));
    DualComplex b = DualComplex::rotation(Deg(23.0f))*DualComplex::translation({2.0f, 3.0f});
    Matrix3 m = Matrix3::translation({2.0f, 3.0f})*Matrix3::rotation(Deg(23.0f));
    Matrix3 n = Matrix3::rotation(Deg(23.0f))*Matrix3::translation({2.0f, 3.0f});
    Vector2 v(-3.6f, 0.7f);

    Vector2 transformedA = a.transformPoint(v);
    CORRADE_COMPARE(transformedA, m.transformPoint(v));
    CORRADE_COMPARE(transformedA, Vector2(-1.58733f, 2.237721f));

    Vector2 transformedB = b.transformPoint(v);
    CORRADE_COMPARE(transformedB, n.transformPoint(v));
    CORRADE_COMPARE(transformedB, Vector2(-2.918512f, 2.780698f));
}

void DualComplexTest::debug() {
    std::ostringstream o;

    Debug(&o) << DualComplex({-1.0f, -2.5f}, {-3.0f, -7.5f});
    CORRADE_COMPARE(o.str(), "DualComplex({-1, -2.5}, {-3, -7.5})\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualComplexTest)
