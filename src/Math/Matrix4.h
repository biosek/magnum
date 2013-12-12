#ifndef Magnum_Math_Matrix4_h
#define Magnum_Math_Matrix4_h
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
 * @brief Class Magnum::Math::Matrix4
 */

#include "Math/Matrix.h"
#include "Math/Vector4.h"

#ifdef _WIN32 /* I so HATE windef.h */
#undef near
#undef far
#endif

namespace Magnum { namespace Math {

/**
@brief 4x4 matrix
@tparam T   Underlying data type

Represents 3D transformation. See @ref matrix-vector and @ref transformations
for brief introduction.
@see Magnum::Matrix4, Magnum::Matrix4d, DualQuaternion,
    SceneGraph::MatrixTransformation3D
@configurationvalueref{Magnum::Math::Matrix4}
 */
template<class T> class Matrix4: public Matrix<4, T> {
    public:
        /**
         * @brief 3D translation
         * @param vector    Translation vector
         *
         * @see translation(), DualQuaternion::translation(),
         *      Matrix3::translation(const Vector2&), Vector3::xAxis(),
         *      Vector3::yAxis(), Vector3::zAxis()
         */
        constexpr static Matrix4<T> translation(const Vector3<T>& vector) {
            return {{      T(1),       T(0),       T(0), T(0)},
                    {      T(0),       T(1),       T(0), T(0)},
                    {      T(0),       T(0),       T(1), T(0)},
                    {vector.x(), vector.y(), vector.z(), T(1)}};
        }

        /**
         * @brief 3D scaling
         * @param vector    Scaling vector
         *
         * @see rotationScaling() const, Matrix3::scaling(const Vector2&),
         *      Vector3::xScale(), Vector3::yScale(), Vector3::zScale()
         */
        constexpr static Matrix4<T> scaling(const Vector3<T>& vector) {
            return {{vector.x(),       T(0),       T(0), T(0)},
                    {      T(0), vector.y(),       T(0), T(0)},
                    {      T(0),       T(0), vector.z(), T(0)},
                    {      T(0),       T(0),       T(0), T(1)}};
        }

        /**
         * @brief 3D rotation around arbitrary axis
         * @param angle             Rotation angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         *
         * Expects that the rotation axis is normalized. If possible, use
         * faster alternatives like rotationX(), rotationY() and rotationZ().
         * @see rotation() const, Quaternion::rotation(), DualQuaternion::rotation(),
         *      Matrix3::rotation(Rad), Vector3::xAxis(), Vector3::yAxis(),
         *      Vector3::zAxis(), Vector::isNormalized()
         */
        static Matrix4<T> rotation(Rad<T> angle, const Vector3<T>& normalizedAxis);

        /**
         * @brief 3D rotation around X axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::xAxis())`.
         * @see rotation(Rad, const Vector3&), rotationY(), rotationZ(),
         *      rotation() const, Quaternion::rotation(), Matrix3::rotation(Rad)
         */
        static Matrix4<T> rotationX(Rad<T> angle);

        /**
         * @brief 3D rotation around Y axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::yAxis())`.
         * @see rotation(Rad, const Vector3&), rotationX(), rotationZ(),
         *      rotation() const, Quaternion::rotation(), Matrix3::rotation(Rad)
         */
        static Matrix4<T> rotationY(Rad<T> angle);

        /**
         * @brief 3D rotation matrix around Z axis
         * @param angle Rotation angle (counterclockwise)
         *
         * Faster than calling `Matrix4::rotation(angle, Vector3::zAxis())`.
         * @see rotation(Rad, const Vector3&), rotationX(), rotationY(),
         *      rotation() const, Quaternion::rotation(), Matrix3::rotation(Rad)
         */
        static Matrix4<T> rotationZ(Rad<T> angle);

        /**
         * @brief 3D reflection matrix
         * @param normal    Normal of the plane through which to reflect
         *
         * Expects that the normal is normalized.
         * @see Matrix3::reflection(), Vector::isNormalized()
         */
        static Matrix4<T> reflection(const Vector3<T>& normal);

        /**
         * @brief 3D orthographic projection matrix
         * @param size      Size of the view
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * @see perspectiveProjection(), Matrix3::projection()
         */
        static Matrix4<T> orthographicProjection(const Vector2<T>& size, T near, T far);

        /**
         * @brief 3D perspective projection matrix
         * @param size      Size of near clipping plane
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * @see orthographicProjection(), Matrix3::projection()
         */
        static Matrix4<T> perspectiveProjection(const Vector2<T>& size, T near, T far);

        /**
         * @brief 3D perspective projection matrix
         * @param fov           Field of view angle (horizontal)
         * @param aspectRatio   Aspect ratio
         * @param near          Near clipping plane
         * @param far           Far clipping plane
         *
         * @see orthographicProjection(), Matrix3::projection()
         */
        static Matrix4<T> perspectiveProjection(Rad<T> fov, T aspectRatio, T near, T far) {
            const T xyScale = 2*std::tan(fov.toUnderlyingType()/2)*near;
            return perspectiveProjection(Vector2<T>(xyScale, xyScale/aspectRatio), near, far);
        }

        /**
         * @brief Create matrix from rotation/scaling part and translation part
         * @param rotationScaling   Rotation/scaling part (upper-left 3x3
         *      matrix)
         * @param translation       Translation part (first three elements of
         *      fourth column)
         *
         * @see rotationScaling() const, translation() const
         */
        constexpr static Matrix4<T> from(const Matrix<3, T>& rotationScaling, const Vector3<T>& translation) {
            return {{rotationScaling[0], T(0)},
                    {rotationScaling[1], T(0)},
                    {rotationScaling[2], T(0)},
                    {       translation, T(1)}};
        }

        /** @copydoc Matrix::Matrix(ZeroType) */
        constexpr explicit Matrix4(typename Matrix<4, T>::ZeroType): Matrix<4, T>(Matrix<4, T>::Zero) {}

        /**
         * @brief Default constructor
         *
         * Creates identity matrix. You can also explicitly call this
         * constructor with `Matrix4 m(Matrix4::Identity);`. Optional parameter
         * @p value allows you to specify value on diagonal.
         * @todo Use constexpr implementation in Matrix, when done
         */
        constexpr /*implicit*/ Matrix4(typename Matrix<4, T>::IdentityType = (Matrix<4, T>::Identity), T value = T(1)): Matrix<4, T>(
            Vector<4, T>(value,  T(0),  T(0),  T(0)),
            Vector<4, T>( T(0), value,  T(0),  T(0)),
            Vector<4, T>( T(0),  T(0), value,  T(0)),
            Vector<4, T>( T(0),  T(0),  T(0), value)
        ) {}

        /** @brief %Matrix from column vectors */
        constexpr /*implicit*/ Matrix4(const Vector4<T>& first, const Vector4<T>& second, const Vector4<T>& third, const Vector4<T>& fourth): Matrix<4, T>(first, second, third, fourth) {}

        /** @copydoc Matrix::Matrix(const RectangularMatrix<size, size, U>&) */
        template<class U> constexpr explicit Matrix4(const RectangularMatrix<4, 4, U>& other): Matrix<4, T>(other) {}

        /** @brief Construct matrix from external representation */
        #ifndef CORRADE_GCC44_COMPATIBILITY
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(std::declval<U>()))> constexpr explicit Matrix4(const U& other): Matrix<4, T>(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(other)) {}
        #else
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(*static_cast<const U*>(nullptr)))> constexpr explicit Matrix4(const U& other): Matrix<4, T>(Implementation::RectangularMatrixConverter<4, 4, T, U>::from(other)) {}
        #endif

        /** @brief Copy constructor */
        constexpr Matrix4(const RectangularMatrix<4, 4, T>& other): Matrix<4, T>(other) {}

        /**
         * @brief Check whether the matrix represents rigid transformation
         *
         * Rigid transformation consists only of rotation and translation (i.e.
         * no scaling or projection).
         * @see isOrthogonal()
         */
        bool isRigidTransformation() const {
            return rotationScaling().isOrthogonal() && row(3) == Vector4<T>(T(0), T(0), T(0), T(1));
        }

        /**
         * @brief 3D rotation and scaling part of the matrix
         *
         * Upper-left 3x3 part of the matrix.
         * @see from(const Matrix<3, T>&, const Vector3&), rotation() const,
         *      rotationNormalized(), @ref uniformScaling(),
         *      rotation(T, const Vector3&), Matrix3::rotationScaling() const
         */
        /* Not Matrix3, because it is for affine 2D transformations */
        constexpr Matrix<3, T> rotationScaling() const {
            return {(*this)[0].xyz(),
                    (*this)[1].xyz(),
                    (*this)[2].xyz()};
        }

        /**
         * @brief 3D rotation part of the matrix assuming there is no scaling
         *
         * Similar to @ref rotationScaling(), but additionally checks that the
         * base vectors are normalized.
         * @see rotation() const, @ref uniformScaling(),
         *      @ref Matrix3::rotationNormalized()
         * @todo assert also orthogonality or this is good enough?
         */
        /* Not Matrix3, because it is for affine 2D transformations */
        Matrix<3, T> rotationNormalized() const {
            CORRADE_ASSERT((*this)[0].xyz().isNormalized() && (*this)[1].xyz().isNormalized() && (*this)[2].xyz().isNormalized(),
                           "Math::Matrix4::rotationNormalized(): the rotation part is not normalized", {});
            return {(*this)[0].xyz(),
                    (*this)[1].xyz(),
                    (*this)[2].xyz()};
        }

        /**
         * @brief 3D rotation part of the matrix
         *
         * Normalized upper-left 3x3 part of the matrix. Expects uniform
         * scaling.
         * @see rotationNormalized(), rotationScaling() const,
         *      @ref uniformScaling(), rotation(T, const Vector3&),
         *      Matrix3::rotation() const
         */
        /* Not Matrix3, because it is for affine 2D transformations */
        Matrix<3, T> rotation() const;

        /**
         * @brief Uniform scaling part of the matrix, squared
         *
         * Squared length of vectors in upper-left 3x3 part of the matrix.
         * Expects that the scaling is the same in all axes. Faster alternative
         * to @ref uniformScaling(), because it doesn't compute the square
         * root.
         * @see @ref rotationScaling(), @ref rotation(),
         *      @ref rotationNormalized(), @ref scaling(const Vector3&),
         *      @ref Matrix3::uniformScaling()
         */
        T uniformScalingSquared() const;

        /**
         * @brief Uniform scaling part of the matrix
         *
         * Length of vectors in upper-left 3x3 part of the matrix. Expects that
         * the scaling is the same in all axes. Use faster alternative
         * @ref uniformScalingSquared() where possible.
         * @see @ref rotationScaling(), @ref rotation(),
         *      @ref rotationNormalized(), @ref scaling(const Vector3&),
         *      @ref Matrix3::uniformScaling()
         */
        T uniformScaling() const { return std::sqrt(uniformScalingSquared()); }

        /**
         * @brief Right-pointing 3D vector
         *
         * First three elements of first column.
         * @see up(), backward(), Vector3::xAxis(), Matrix3::right()
         */
        Vector3<T>& right() { return (*this)[0].xyz(); }
        constexpr Vector3<T> right() const { return (*this)[0].xyz(); } /**< @overload */

        /**
         * @brief Up-pointing 3D vector
         *
         * First three elements of second column.
         * @see right(), backward(), Vector3::yAxis(), Matrix3::up()
         */
        Vector3<T>& up() { return (*this)[1].xyz(); }
        constexpr Vector3<T> up() const { return (*this)[1].xyz(); } /**< @overload */

        /**
         * @brief Backward-pointing 3D vector
         *
         * First three elements of third column.
         * @see right(), up(), Vector3::yAxis()
         */
        Vector3<T>& backward() { return (*this)[2].xyz(); }
        constexpr Vector3<T> backward() const { return (*this)[2].xyz(); } /**< @overload */

        /**
         * @brief 3D translation part of the matrix
         *
         * First three elements of fourth column.
         * @see from(const Matrix<3, T>&, const Vector3&),
         *      translation(const Vector3&), Matrix3::translation()
         */
        Vector3<T>& translation() { return (*this)[3].xyz(); }
        constexpr Vector3<T> translation() const { return (*this)[3].xyz(); } /**< @overload */

        /**
         * @brief Inverted rigid transformation matrix
         *
         * Expects that the matrix represents rigid transformation.
         * Significantly faster than the general algorithm in inverted().
         * @see isRigidTransformation(), invertedOrthogonal(),
         *      rotationScaling() const, translation() const
         */
        Matrix4<T> invertedRigid() const;

        /**
         * @brief Transform 3D vector with the matrix
         *
         * Unlike in transformVector(), translation is not involved in the
         * transformation. @f[
         *      \boldsymbol v' = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ v_z \\ 0 \end{pmatrix}
         * @f]
         * @see Quaternion::transformVector(), Matrix3::transformVector()
         * @todo extract 3x3 matrix and multiply directly? (benchmark that)
         */
        Vector3<T> transformVector(const Vector3<T>& vector) const {
            /* Workaround for GCC 4.4 strict-aliasing fascism */
            #ifndef CORRADE_GCC44_COMPATIBILITY
            return ((*this)*Vector4<T>(vector, T(0))).xyz();
            #else
            const auto v = (*this)*Vector4<T>(vector, T(0));
            return v.xyz();
            #endif
        }

        /**
         * @brief Transform 3D point with the matrix
         *
         * Unlike in transformVector(), translation is also involved in the
         * transformation. @f[
         *      \boldsymbol v' = \boldsymbol M \begin{pmatrix} v_x \\ v_y \\ v_z \\ 1 \end{pmatrix}
         * @f]
         * @see DualQuaternion::transformPoint(), Matrix3::transformPoint()
         */
        Vector3<T> transformPoint(const Vector3<T>& vector) const {
            /* Workaround for GCC 4.4 strict-aliasing fascism */
            #ifndef CORRADE_GCC44_COMPATIBILITY
            return ((*this)*Vector4<T>(vector, T(1))).xyz();
            #else
            const auto v = (*this)*Vector4<T>(vector, T(1));
            return v.xyz();
            #endif
        }

        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(4, 4, Matrix4<T>)
        MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(4, Matrix4, Vector4)
};

MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(4, Matrix4)

/** @debugoperator{Magnum::Math::Matrix4} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Matrix4<T>& value) {
    return debug << static_cast<const Matrix<4, T>&>(value);
}

template<class T> Matrix4<T> Matrix4<T>::rotation(const Rad<T> angle, const Vector3<T>& normalizedAxis) {
    CORRADE_ASSERT(normalizedAxis.isNormalized(),
                   "Math::Matrix4::rotation(): axis must be normalized", {});

    const T sine = std::sin(angle.toUnderlyingType());
    const T cosine = std::cos(angle.toUnderlyingType());
    const T oneMinusCosine = T(1) - cosine;

    const T xx = normalizedAxis.x()*normalizedAxis.x();
    const T xy = normalizedAxis.x()*normalizedAxis.y();
    const T xz = normalizedAxis.x()*normalizedAxis.z();
    const T yy = normalizedAxis.y()*normalizedAxis.y();
    const T yz = normalizedAxis.y()*normalizedAxis.z();
    const T zz = normalizedAxis.z()*normalizedAxis.z();

    return {
        {cosine + xx*oneMinusCosine,
            xy*oneMinusCosine + normalizedAxis.z()*sine,
                xz*oneMinusCosine - normalizedAxis.y()*sine,
                   T(0)},
        {xy*oneMinusCosine - normalizedAxis.z()*sine,
            cosine + yy*oneMinusCosine,
                yz*oneMinusCosine + normalizedAxis.x()*sine,
                   T(0)},
        {xz*oneMinusCosine + normalizedAxis.y()*sine,
            yz*oneMinusCosine - normalizedAxis.x()*sine,
                cosine + zz*oneMinusCosine,
                   T(0)},
        {T(0), T(0), T(0), T(1)}
    };
}

template<class T> Matrix4<T> Matrix4<T>::rotationX(const Rad<T> angle) {
    const T sine = std::sin(angle.toUnderlyingType());
    const T cosine = std::cos(angle.toUnderlyingType());

    return {{T(1),   T(0),   T(0), T(0)},
            {T(0), cosine,   sine, T(0)},
            {T(0),  -sine, cosine, T(0)},
            {T(0),   T(0),   T(0), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::rotationY(const Rad<T> angle) {
    const T sine = std::sin(angle.toUnderlyingType());
    const T cosine = std::cos(angle.toUnderlyingType());

    return {{cosine, T(0),  -sine, T(0)},
            {  T(0), T(1),   T(0), T(0)},
            {  sine, T(0), cosine, T(0)},
            {  T(0), T(0),   T(0), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::rotationZ(const Rad<T> angle) {
    const T sine = std::sin(angle.toUnderlyingType());
    const T cosine = std::cos(angle.toUnderlyingType());

    return {{cosine,   sine, T(0), T(0)},
            { -sine, cosine, T(0), T(0)},
            {  T(0),   T(0), T(1), T(0)},
            {  T(0),   T(0), T(0), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::reflection(const Vector3<T>& normal) {
    CORRADE_ASSERT(normal.isNormalized(),
                    "Math::Matrix4::reflection(): normal must be normalized", {});
    return from(Matrix<3, T>() - T(2)*normal*RectangularMatrix<1, 3, T>(normal).transposed(), {});
}

template<class T> Matrix4<T> Matrix4<T>::orthographicProjection(const Vector2<T>& size, const T near, const T far) {
    const Vector2<T> xyScale = T(2.0)/size;
    const T zScale = T(2.0)/(near-far);

    return {{xyScale.x(),        T(0),             T(0), T(0)},
            {       T(0), xyScale.y(),             T(0), T(0)},
            {       T(0),        T(0),           zScale, T(0)},
            {       T(0),        T(0), near*zScale-T(1), T(1)}};
}

template<class T> Matrix4<T> Matrix4<T>::perspectiveProjection(const Vector2<T>& size, const T near, const T far) {
    Vector2<T> xyScale = 2*near/size;
    T zScale = T(1.0)/(near-far);

    return {{xyScale.x(),        T(0),                 T(0),  T(0)},
            {       T(0), xyScale.y(),                 T(0),  T(0)},
            {       T(0),        T(0),    (far+near)*zScale, T(-1)},
            {       T(0),        T(0), T(2)*far*near*zScale,  T(0)}};
}

template<class T> inline Matrix<3, T> Matrix4<T>::rotation() const {
    CORRADE_ASSERT(TypeTraits<T>::equals((*this)[0].xyz().dot(), (*this)[1].xyz().dot()) &&
                   TypeTraits<T>::equals((*this)[1].xyz().dot(), (*this)[2].xyz().dot()),
        "Math::Matrix4::rotation(): the matrix doesn't have uniform scaling", {});
    return {(*this)[0].xyz().normalized(),
            (*this)[1].xyz().normalized(),
            (*this)[2].xyz().normalized()};
}

template<class T> T Matrix4<T>::uniformScalingSquared() const {
    const T scalingSquared = (*this)[0].xyz().dot();
    CORRADE_ASSERT(TypeTraits<T>::equals((*this)[1].xyz().dot(), scalingSquared) &&
                   TypeTraits<T>::equals((*this)[2].xyz().dot(), scalingSquared),
        "Math::Matrix4::uniformScaling(): the matrix doesn't have uniform scaling", {});
    return scalingSquared;
}

template<class T> Matrix4<T> Matrix4<T>::invertedRigid() const {
    CORRADE_ASSERT(isRigidTransformation(),
        "Math::Matrix4::invertedRigid(): the matrix doesn't represent rigid transformation", {});

    Matrix<3, T> inverseRotation = rotationScaling().transposed();
    return from(inverseRotation, inverseRotation*-translation());
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Matrix4} */
    template<class T> struct ConfigurationValue<Magnum::Math::Matrix4<T>>: public ConfigurationValue<Magnum::Math::Matrix<4, T>> {};
}}

#endif
