#ifndef Magnum_Math_Complex_h
#define Magnum_Math_Complex_h
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

/** @file
 * @brief Class Magnum::Math::Complex
 */

#include <limits>
#include <Utility/Assert.h>
#include <Utility/Debug.h>

#include "Math/Matrix.h"
#include "Math/Vector2.h"

namespace Magnum { namespace Math {

namespace Implementation {
    /* No assertions fired, for internal use. Not private member because used
       from outside the class. */
    template<class T> constexpr static Complex<T> complexFromMatrix(const Matrix<2, T>& matrix) {
        return {matrix[0][0], matrix[0][1]};
    }
}

/**
@brief %Complex number
@tparam T   Data type

Represents 2D rotation. See @ref transformations for brief introduction.
@see Magnum::Complex, Magnum::Complexd, Matrix3
*/
template<class T> class Complex {
    public:
        typedef T Type;                         /**< @brief Underlying data type */

        /**
         * @brief Dot product
         *
         * @f[
         *      c_0 \cdot c_1 = a_0 a_1 + b_0 b_1
         * @f]
         * @see dot() const
         */
        static T dot(const Complex<T>& a, const Complex<T>& b) {
            return a._real*b._real + a._imaginary*b._imaginary;
        }

        /**
         * @brief Angle between normalized complex numbers
         *
         * Expects that both complex numbers are normalized. @f[
         *      \theta = acos \left( \frac{Re(c_0 \cdot c_1))}{|c_0| |c_1|} \right) = acos (a_0 a_1 + b_0 b_1)
         * @f]
         * @see isNormalized(), Quaternion::angle(), Vector::angle()
         */
        static Rad<T> angle(const Complex<T>& normalizedA, const Complex<T>& normalizedB) {
            CORRADE_ASSERT(normalizedA.isNormalized() && normalizedB.isNormalized(),
                           "Math::Complex::angle(): complex numbers must be normalized", Rad<T>(std::numeric_limits<T>::quiet_NaN()));
            return Rad<T>(std::acos(normalizedA._real*normalizedB._real + normalizedA._imaginary*normalizedB._imaginary));
        }

        /**
         * @brief Rotation complex number
         * @param angle             Rotation angle (counterclockwise)
         *
         * @f[
         *      c = cos \theta + i sin \theta
         * @f]
         * @see angle(), Matrix3::rotation(), Quaternion::rotation()
         */
        static Complex<T> rotation(Rad<T> angle) {
            return {std::cos(angle.toUnderlyingType()), std::sin(angle.toUnderlyingType())};
        }

        /**
         * @brief Create complex number from rotation matrix
         *
         * Expects that the matrix is orthogonal (i.e. pure rotation).
         * @see toMatrix(), DualComplex::fromMatrix(), Matrix::isOrthogonal()
         */
        static Complex<T> fromMatrix(const Matrix<2, T>& matrix) {
            CORRADE_ASSERT(matrix.isOrthogonal(),
                "Math::Complex::fromMatrix(): the matrix is not orthogonal", {});
            return Implementation::complexFromMatrix(matrix);
        }

        /**
         * @brief Default constructor
         *
         * Constructs unit complex number. @f[
         *      c = 1 + i0
         * @f]
         */
        constexpr /*implicit*/ Complex(): _real(T(1)), _imaginary(T(0)) {}

        /**
         * @brief Construct complex number from real and imaginary part
         *
         * @f[
         *      c = a + ib
         * @f]
         */
        constexpr /*implicit*/ Complex(T real, T imaginary): _real(real), _imaginary(imaginary) {}

        /**
         * @brief Construct complex number from vector
         *
         * To be used in transformations later. @f[
         *      c = v_x + iv_y
         * @f]
         * @see operator Vector2(), transformVector()
         */
        constexpr explicit Complex(const Vector2<T>& vector): _real(vector.x()), _imaginary(vector.y()) {}

        /** @brief Equality comparison */
        bool operator==(const Complex<T>& other) const {
            return TypeTraits<T>::equals(_real, other._real) &&
                   TypeTraits<T>::equals(_imaginary, other._imaginary);
        }

        /** @brief Non-equality comparison */
        bool operator!=(const Complex<T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Whether the complex number is normalized
         *
         * Complex number is normalized if it has unit length: @f[
         *      |c \cdot c - 1| < 2 \epsilon + \epsilon^2 \cong 2 \epsilon
         * @f]
         * @see dot(), normalized()
         */
        bool isNormalized() const {
            return Implementation::isNormalizedSquared(dot());
        }

        /** @brief Real part */
        constexpr T real() const { return _real; }

        /** @brief Imaginary part */
        constexpr T imaginary() const { return _imaginary; }

        /**
         * @brief Convert complex number to vector
         *
         * @f[
         *      \boldsymbol v = \begin{pmatrix} a \\ b \end{pmatrix}
         * @f]
         */
        #ifndef CORRADE_GCC44_COMPATIBILITY
        constexpr explicit operator Vector2<T>() const {
        #else
        constexpr operator Vector2<T>() const {
        #endif
            return {_real, _imaginary};
        }

        /**
         * @brief Rotation angle of complex number
         *
         * @f[
         *      \theta = atan2(b, a)
         * @f]
         * @see rotation()
         */
        Rad<T> angle() const {
            return Rad<T>(std::atan2(_imaginary, _real));
        }

        /**
         * @brief Convert complex number to rotation matrix
         *
         * @f[
         *      M = \begin{pmatrix}
         *               a & -b \\
         *               b &  a
         *          \end{pmatrix}
         * @f]
         * @see fromMatrix(), DualComplex::toMatrix(),
         *      Matrix3::from(const Matrix<2, T>&, const Vector2<T>&)
         */
        Matrix<2, T> toMatrix() const {
            return {Vector<2, T>(_real, _imaginary),
                    Vector<2, T>(-_imaginary, _real)};
        }

        /**
         * @brief Add complex number and assign
         *
         * The computation is done in-place. @f[
         *      c_0 + c_1 = (a_0 + a_1) + i(b_0 + b_1)
         * @f]
         */
        Complex<T>& operator+=(const Complex<T>& other) {
            _real += other._real;
            _imaginary += other._imaginary;
            return *this;
        }

        /**
         * @brief Add complex number
         *
         * @see operator+=()
         */
        Complex<T> operator+(const Complex<T>& other) const {
            return Complex<T>(*this) += other;
        }

        /**
         * @brief Negated complex number
         *
         * @f[
         *      -c = -a -ib
         * @f]
         */
        Complex<T> operator-() const {
            return {-_real, -_imaginary};
        }

        /**
         * @brief Subtract complex number and assign
         *
         * The computation is done in-place. @f[
         *      c_0 - c_1 = (a_0 - a_1) + i(b_0 - b_1)
         * @f]
         */
        Complex<T>& operator-=(const Complex<T>& other) {
            _real -= other._real;
            _imaginary -= other._imaginary;
            return *this;
        }

        /**
         * @brief Subtract complex number
         *
         * @see operator-=()
         */
        Complex<T> operator-(const Complex<T>& other) const {
            return Complex<T>(*this) -= other;
        }

        /**
         * @brief Multiply with scalar and assign
         *
         * The computation is done in-place. @f[
         *      c \cdot t = ta + itb
         * @f]
         */
        Complex<T>& operator*=(T scalar) {
            _real *= scalar;
            _imaginary *= scalar;
            return *this;
        }

        /**
         * @brief Multiply with scalar
         *
         * @see operator*=(T)
         */
        Complex<T> operator*(T scalar) const {
            return Complex<T>(*this) *= scalar;
        }

        /**
         * @brief Divide with scalar and assign
         *
         * The computation is done in-place. @f[
         *      \frac c t = \frac a t + i \frac b t
         * @f]
         */
        Complex<T>& operator/=(T scalar) {
            _real /= scalar;
            _imaginary /= scalar;
            return *this;
        }

        /**
         * @brief Divide with scalar
         *
         * @see operator/=(T)
         */
        Complex<T> operator/(T scalar) const {
            return Complex<T>(*this) /= scalar;
        }

        /**
         * @brief Multiply with complex number
         *
         * @f[
         *      c_0 c_1 = (a_0 + ib_0)(a_1 + ib_1) = (a_0 a_1 - b_0 b_1) + i(a_1 b_0 + a_0 b_1)
         * @f]
         */
        Complex<T> operator*(const Complex<T>& other) const {
            return {_real*other._real - _imaginary*other._imaginary,
                    _imaginary*other._real + _real*other._imaginary};
        }

        /**
         * @brief Dot product of the complex number
         *
         * Should be used instead of length() for comparing complex number length
         * with other values, because it doesn't compute the square root. @f[
         *      c \cdot c = a^2 + b^2
         * @f]
         * @see dot(const Complex&, const Complex&), isNormalized()
         */
        T dot() const {
            return dot(*this, *this);
        }

        /**
         * @brief %Complex number length
         *
         * See also dot() const which is faster for comparing length with other
         * values. @f[
         *      |c| = \sqrt{c \cdot c}
         * @f]
         * @see isNormalized()
         */
        T length() const {
            /** @todo Remove when NaCl's newlib has this fixed */
            #ifndef CORRADE_TARGET_NACL_NEWLIB
            return std::hypot(_real, _imaginary);
            #else
            return std::sqrt(dot());
            #endif
        }

        /**
         * @brief Normalized complex number (of unit length)
         *
         * @see isNormalized()
         */
        Complex<T> normalized() const {
            return (*this)/length();
        }

        /**
         * @brief Conjugated complex number
         *
         * @f[
         *      c^* = a - ib
         * @f]
         */
        Complex<T> conjugated() const {
            return {_real, -_imaginary};
        }

        /**
         * @brief Inverted complex number
         *
         * See invertedNormalized() which is faster for normalized
         * complex numbers. @f[
         *      c^{-1} = \frac{c^*}{|c|^2} = \frac{c^*}{c \cdot c}
         * @f]
         */
        Complex<T> inverted() const {
            return conjugated()/dot();
        }

        /**
         * @brief Inverted normalized complex number
         *
         * Equivalent to conjugated(). Expects that the complex number is
         * normalized. @f[
         *      c^{-1} = \frac{c^*}{c \cdot c} = c^*
         * @f]
         * @see isNormalized(), inverted()
         */
        Complex<T> invertedNormalized() const {
            CORRADE_ASSERT(isNormalized(),
                           "Math::Complex::invertedNormalized(): complex number must be normalized",
                           Complex<T>(std::numeric_limits<T>::quiet_NaN(), {}));
            return conjugated();
        }

        /**
         * @brief Rotate vector with complex number
         *
         * @f[
         *      v' = c v = c (v_x + iv_y)
         * @f]
         * @see Complex(const Vector2&), operator Vector2(), Matrix3::transformVector()
         */
        Vector2<T> transformVector(const Vector2<T>& vector) const {
            return Vector2<T>((*this)*Complex<T>(vector));
        }

    private:
        T _real, _imaginary;
};

/** @relates Complex
@brief Multiply scalar with complex

Same as Complex::operator*(T) const.
*/
template<class T> inline Complex<T> operator*(T scalar, const Complex<T>& complex) {
    return complex*scalar;
}

/** @relates Complex
@brief Divide complex with number and invert

@f[
    \frac t c = \frac t a + i \frac t b
@f]
@see Complex::operator/()
*/
template<class T> inline Complex<T> operator/(T scalar, const Complex<T>& complex) {
    return {scalar/complex.real(), scalar/complex.imaginary()};
}

/** @debugoperator{Magnum::Math::Complex} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Complex<T>& value) {
    debug << "Complex(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.real() << ", " << value.imaginary() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Complex<Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Complex<Double>&);
#endif
#endif

}}

#endif
