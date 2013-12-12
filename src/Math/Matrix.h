#ifndef Magnum_Math_Matrix_h
#define Magnum_Math_Matrix_h
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
 * @brief Class Magnum::Math::Matrix
 */

#include "RectangularMatrix.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t size, class T> class MatrixDeterminant;
}

/**
@brief Square matrix
@tparam size    %Matrix size
@tparam T       Data type

See @ref matrix-vector for brief introduction.

@configurationvalueref{Magnum::Math::Matrix}
@see @ref Matrix2x2, @ref Matrix3x3, @ref Matrix4x4
 */
template<std::size_t size, class T> class Matrix: public RectangularMatrix<size, size, T> {
    public:
        const static std::size_t Size = size; /**< @brief %Matrix size */

        /** @brief Pass to constructor to create zero-filled matrix */
        enum ZeroType { Zero };

        /**
         * @brief Zero-filled matrix constructor
         *
         * Use this constructor by calling `Matrix m(Matrix::Zero);`.
         */
        constexpr explicit Matrix(ZeroType) {}

        /** @brief Pass to constructor to create identity matrix */
        enum IdentityType { Identity };

        /**
         * @brief Default constructor
         *
         * You can also explicitly call this constructor with
         * `Matrix m(Matrix::Identity);`. Optional parameter @p value allows
         * you to specify value on diagonal.
         * @todo use constexpr fromDiagonal() for this when it's done
         */
        /*implicit*/ Matrix(IdentityType = Identity, T value = T(1)) {
            for(std::size_t i = 0; i != size; ++i)
                (*this)[i][i] = value;
        }

        /**
         * @brief %Matrix from column vectors
         * @param first First column vector
         * @param next  Next column vectors
         */
        template<class ...U> constexpr /*implicit*/ Matrix(const Vector<size, T>& first, const U&... next): RectangularMatrix<size, size, T>(first, next...) {}

        /**
         * @brief Construct matrix from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Matrix2x2<Float> floatingPoint({1.3f, 2.7f},
         *                                {-15.0f, 7.0f});
         * Matrix2x2<Byte> integral(floatingPoint);
         * // integral == {{1, 2}, {-15, 7}}
         * @endcode
         */
        template<class U> constexpr explicit Matrix(const RectangularMatrix<size, size, U>& other): RectangularMatrix<size, size, T>(other) {}

        /** @brief Construct matrix from external representation */
        #ifndef CORRADE_GCC44_COMPATIBILITY
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<size, size, T, U>::from(std::declval<U>()))> constexpr explicit Matrix(const U& other): RectangularMatrix<size, size, T>(Implementation::RectangularMatrixConverter<size, size, T, U>::from(other)) {}
        #else
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<size, size, T, U>::from(*static_cast<const U*>(nullptr)))> constexpr explicit Matrix(const U& other): RectangularMatrix<size, size, T>(Implementation::RectangularMatrixConverter<size, size, T, U>::from(other)) {}
        #endif

        /** @brief Copy constructor */
        constexpr Matrix(const RectangularMatrix<size, size, T>& other): RectangularMatrix<size, size, T>(other) {}

        /**
         * @brief Whether the matrix is orthogonal
         *
         * The matrix is orthogonal if its transpose is equal to its inverse: @f[
         *      Q^T = Q^{-1}
         * @f]
         * @see transposed(), inverted(), Matrix3::isRigidTransformation(),
         *      Matrix4::isRigidTransformation()
         */
        bool isOrthogonal() const;

        /**
         * @brief Trace of the matrix
         *
         * @f[
         * tr(A) = \sum_{i=1}^n a_{i,i}
         * @f]
         */
        T trace() const { return RectangularMatrix<size, size, T>::diagonal().sum(); }

        /** @brief %Matrix without given column and row */
        Matrix<size-1, T> ij(std::size_t skipCol, std::size_t skipRow) const;

        /**
         * @brief Determinant
         *
         * Computed recursively using Laplace's formula: @f[
         *      \det(A) = \sum_{j=1}^n (-1)^{i+j} a_{i,j} \det(A^{i,j})
         * @f] @f$ A^{i, j} @f$ is matrix without i-th row and j-th column, see
         * ij(). The formula is expanded down to 2x2 matrix, where the
         * determinant is computed directly: @f[
         *      \det(A) = a_{0, 0} a_{1, 1} - a_{1, 0} a_{0, 1}
         * @f]
         */
        T determinant() const { return Implementation::MatrixDeterminant<size, T>()(*this); }

        /**
         * @brief Inverted matrix
         *
         * Computed using Cramer's rule: @f[
         *      A^{-1} = \frac{1}{\det(A)} Adj(A)
         * @f]
         * See invertedOrthogonal(), Matrix3::invertedRigid() and Matrix4::invertedRigid()
         * which are faster alternatives for particular matrix types.
         */
        Matrix<size, T> inverted() const;

        /**
         * @brief Inverted orthogonal matrix
         *
         * Equivalent to transposed(), expects that the matrix is orthogonal. @f[
         *      A^{-1} = A^T
         * @f]
         * @see inverted(), isOrthogonal(), Matrix3::invertedRigid(),
         *      Matrix4::invertedRigid()
         */
        Matrix<size, T> invertedOrthogonal() const {
            CORRADE_ASSERT(isOrthogonal(),
                "Math::Matrix::invertedOrthogonal(): the matrix is not orthogonal", {});
            return RectangularMatrix<size, size, T>::transposed();
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Reimplementation of functions to return correct type */
        Matrix<size, T> operator*(const Matrix<size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        template<std::size_t otherCols> RectangularMatrix<otherCols, size, T> operator*(const RectangularMatrix<otherCols, size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        Vector<size, T> operator*(const Vector<size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(size, size, Matrix<size, T>)
        #endif
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief 2x2 matrix

Convenience alternative to <tt>%Matrix<2, T></tt>. See @ref Matrix for more
information.
@note Not available on GCC < 4.7. Use <tt>%Matrix<2, T></tt> instead.
@see @ref Magnum::Matrix2x2, @ref Magnum::Matrix2x2d
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
template<class T> using Matrix2x2 = Matrix<2, T>;
#endif

/**
@brief 3x3 matrix

Convenience alternative to <tt>%Matrix<3, T></tt>. See @ref Matrix for more
information. Note that this is different from @ref Matrix3, which contains
additional functions for transformations in 2D.
@note Not available on GCC < 4.7. Use <tt>%Matrix<3, T></tt> instead.
@see @ref Magnum::Matrix3x3, @ref Magnum::Matrix3x3d
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
template<class T> using Matrix3x3 = Matrix<3, T>;
#endif

/**
@brief 4x4 matrix

Convenience alternative to <tt>%Matrix<4, T></tt>. See @ref Matrix for more
information. Note that this is different from @ref Matrix4, which contains
additional functions for transformations in 3D.
@note Not available on GCC < 4.7. Use <tt>%Matrix<3, T></tt> instead.
@see @ref Magnum::Matrix4x4, @ref Magnum::Matrix4x4d
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
template<class T> using Matrix4x4 = Matrix<4, T>;
#endif
#endif

MAGNUM_MATRIX_OPERATOR_IMPLEMENTATION(Matrix<size, T>)

/** @debugoperator{Magnum::Math::Matrix} */
template<std::size_t size, class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Matrix<size, T>& value) {
    return debug << static_cast<const RectangularMatrix<size, size, T>&>(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(size, Type, VectorType)       \
    VectorType<T>& operator[](std::size_t col) {                            \
        return static_cast<VectorType<T>&>(Matrix<size, T>::operator[](col)); \
    }                                                                       \
    constexpr const VectorType<T> operator[](std::size_t col) const {       \
        return VectorType<T>(Matrix<size, T>::operator[](col));             \
    }                                                                       \
    VectorType<T> row(std::size_t row) const {                              \
        return VectorType<T>(Matrix<size, T>::row(row));                    \
    }                                                                       \
                                                                            \
    Type<T> operator*(const Matrix<size, T>& other) const {                 \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
    template<std::size_t otherCols> RectangularMatrix<otherCols, size, T> operator*(const RectangularMatrix<otherCols, size, T>& other) const { \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
    VectorType<T> operator*(const Vector<size, T>& other) const {           \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
                                                                            \
    Type<T> transposed() const { return Matrix<size, T>::transposed(); }    \
    Type<T> inverted() const { return Matrix<size, T>::inverted(); }        \
    Type<T> invertedOrthogonal() const {                                    \
        return Matrix<size, T>::invertedOrthogonal();                       \
    }

namespace Implementation {

template<std::size_t size, class T> class MatrixDeterminant {
    public:
        T operator()(const Matrix<size, T>& m);
};

template<std::size_t size, class T> T MatrixDeterminant<size, T>::operator()(const Matrix<size, T>& m) {
    T out(0);

    for(std::size_t col = 0; col != size; ++col)
        out += ((col & 1) ? -1 : 1)*m[col][0]*m.ij(col, 0).determinant();

    return out;
}

template<class T> class MatrixDeterminant<2, T> {
    public:
        constexpr T operator()(const Matrix<2, T>& m) const {
            return m[0][0]*m[1][1] - m[1][0]*m[0][1];
        }
};

template<class T> class MatrixDeterminant<1, T> {
    public:
        constexpr T operator()(const Matrix<1, T>& m) const {
            return m[0][0];
        }
};

}
#endif

template<std::size_t size, class T> bool Matrix<size, T>::isOrthogonal() const {
    /* Normality */
    for(std::size_t i = 0; i != size; ++i)
        if(!(*this)[i].isNormalized()) return false;

    /* Orthogonality */
    for(std::size_t i = 0; i != size-1; ++i)
        for(std::size_t j = i+1; j != size; ++j)
            if(Vector<size, T>::dot((*this)[i], (*this)[j]) > TypeTraits<T>::epsilon())
                return false;

    return true;
}

template<std::size_t size, class T> Matrix<size-1, T> Matrix<size, T>::ij(const std::size_t skipCol, const std::size_t skipRow) const {
    Matrix<size-1, T> out(Matrix<size-1, T>::Zero);

    for(std::size_t col = 0; col != size-1; ++col)
        for(std::size_t row = 0; row != size-1; ++row)
            out[col][row] = (*this)[col + (col >= skipCol)]
                                    [row + (row >= skipRow)];

    return out;
}

template<std::size_t size, class T> Matrix<size, T> Matrix<size, T>::inverted() const {
    Matrix<size, T> out(Zero);

    const T _determinant = determinant();

    for(std::size_t col = 0; col != size; ++col)
        for(std::size_t row = 0; row != size; ++row)
            out[col][row] = (((row+col) & 1) ? -1 : 1)*ij(row, col).determinant()/_determinant;

    return out;
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix} */
    template<std::size_t size, class T> struct ConfigurationValue<Magnum::Math::Matrix<size, T>>: public ConfigurationValue<Magnum::Math::RectangularMatrix<size, size, T>> {};
}}

#endif
