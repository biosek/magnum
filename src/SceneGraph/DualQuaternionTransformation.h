#ifndef Magnum_SceneGraph_DualQuaternionTransformation_h
#define Magnum_SceneGraph_DualQuaternionTransformation_h
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
 * @brief Class Magnum::SceneGraph::BasicDualQuaternionTransformation, typedef Magnum::SceneGraph::DualQuaternionTransformation
 */

#include "Math/DualQuaternion.h"
#include "AbstractTranslationRotation3D.h"
#include "Object.h"

namespace Magnum { namespace SceneGraph {

/**
@brief Three-dimensional transformation implemented using dual quaternions

This class allows only rigid transformation (i.e. only rotation and
translation).
@see @ref DualQuaternionTransformation @ref scenegraph, Math::DualQuaternion, @ref BasicDualComplexTransformation
*/
template<class T> class BasicDualQuaternionTransformation: public AbstractBasicTranslationRotation3D<T> {
    public:
        /** @brief Underlying transformation type */
        typedef Math::DualQuaternion<T> DataType;

        /** @brief Object transformation */
        Math::DualQuaternion<T> transformation() const { return _transformation; }

        /**
         * @brief Set transformation
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual quaternion is normalized.
         * @see DualQuaternion::isNormalized()
         */
        Object<BasicDualQuaternionTransformation<T>>& setTransformation(const Math::DualQuaternion<T>& transformation) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualQuaternionTransformation::setTransformation(): the dual quaternion is not normalized",
                static_cast<Object<BasicDualQuaternionTransformation<T>>&>(*this));
            return setTransformationInternal(transformation);
        }

        /** @copydoc AbstractTranslationRotationScaling3D::resetTransformation() */
        Object<BasicDualQuaternionTransformation<T>>& resetTransformation() {
            return setTransformationInternal({});
        }

        /**
         * @brief Normalize rotation part
         * @return Reference to self (for method chaining)
         *
         * Normalizes the rotation part to prevent rounding errors when rotating
         * the object subsequently.
         * @see DualQuaternion::normalized()
         */
        Object<BasicDualQuaternionTransformation<T>>& normalizeRotation() {
            return setTransformationInternal(_transformation.normalized());
        }

        /**
         * @brief Multiply transformation
         * @param transformation    Transformation
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Expects that the dual quaternion is normalized.
         * @see DualQuaternion::isNormalized()
         */
        Object<BasicDualQuaternionTransformation<T>>& transform(const Math::DualQuaternion<T>& transformation, TransformationType type = TransformationType::Global) {
            CORRADE_ASSERT(transformation.isNormalized(),
                "SceneGraph::DualQuaternionTransformation::transform(): the dual quaternion is not normalized",
                static_cast<Object<BasicDualQuaternionTransformation<T>>&>(*this));
            return transformInternal(transformation, type);
        }

        /**
         * @copydoc AbstractTranslationRotationScaling3D::translate()
         * Same as calling transform() with DualQuaternion::translation().
         */
        Object<BasicDualQuaternionTransformation<T>>& translate(const Math::Vector3<T>& vector, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::DualQuaternion<T>::translation(vector), type);
        }

        /**
         * @brief Rotate object
         * @param angle             Angle (counterclockwise)
         * @param normalizedAxis    Normalized rotation axis
         * @param type              Transformation type
         * @return Reference to self (for method chaining)
         *
         * Same as calling transform() with DualQuaternion::rotation().
         * @see Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis(),
         *      normalizeRotation()
         */
        Object<BasicDualQuaternionTransformation<T>>& rotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type = TransformationType::Global) {
            return transformInternal(Math::DualQuaternion<T>::rotation(angle, normalizedAxis), type);
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Object<BasicDualQuaternionTransformation<T>>& rotateX(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return rotate(angle, Math::Vector3<T>::xAxis(), type);
        }
        Object<BasicDualQuaternionTransformation<T>>& rotateY(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return rotate(angle, Math::Vector3<T>::yAxis(), type);
        }
        Object<BasicDualQuaternionTransformation<T>>& rotateZ(Math::Rad<T> angle, TransformationType type = TransformationType::Global) {
            return rotate(angle, Math::Vector3<T>::zAxis(), type);
        }
        #endif

    protected:
        /* Allow construction only from Object */
        explicit BasicDualQuaternionTransformation();

    private:
        void doResetTransformation() override final { resetTransformation(); }

        void doTranslate(const Math::Vector3<T>& vector, TransformationType type) override final {
            translate(vector, type);
        }

        void doRotate(Math::Rad<T> angle, const Math::Vector3<T>& normalizedAxis, TransformationType type) override final {
            rotate(angle, normalizedAxis, type);
        }

        /* No assertions fired, for internal use */
        Object<BasicDualQuaternionTransformation<T>>& setTransformationInternal(const Math::DualQuaternion<T>& transformation) {
            /* Setting transformation is forbidden for the scene */
            /** @todo Assert for this? */
            /** @todo Do this in some common code so we don't need to include Object? */
            if(!static_cast<Object<BasicDualQuaternionTransformation<T>>*>(this)->isScene()) {
                _transformation = transformation;
                static_cast<Object<BasicDualQuaternionTransformation<T>>*>(this)->setDirty();
            }

            return static_cast<Object<BasicDualQuaternionTransformation<T>>&>(*this);
        }

        /* No assertions fired, for internal use */
        Object<BasicDualQuaternionTransformation<T>>& transformInternal(const Math::DualQuaternion<T>& transformation, TransformationType type) {
            return setTransformationInternal(type == TransformationType::Global ?
                transformation*_transformation : _transformation*transformation);
        }

        Math::DualQuaternion<T> _transformation;
};

template<class T> inline BasicDualQuaternionTransformation<T>::BasicDualQuaternionTransformation() = default;

/**
@brief Three-dimensional transformation for float scenes implemented using dual quaternions

@see @ref DualComplexTransformation
*/
typedef BasicDualQuaternionTransformation<Float> DualQuaternionTransformation;

namespace Implementation {

template<class T> struct Transformation<BasicDualQuaternionTransformation<T>> {
    static Math::DualQuaternion<T> fromMatrix(const Math::Matrix4<T>& matrix) {
        CORRADE_ASSERT(matrix.isRigidTransformation(),
            "SceneGraph::DualQuaternionTransformation: the matrix doesn't represent rigid transformation", {});
        return Math::DualQuaternion<T>::fromMatrix(matrix);
    }

    constexpr static Math::Matrix4<T> toMatrix(const Math::DualQuaternion<T>& transformation) {
        return transformation.toMatrix();
    }

    static Math::DualQuaternion<T> compose(const Math::DualQuaternion<T>& parent, const Math::DualQuaternion<T>& child) {
        return parent*child;
    }

    static Math::DualQuaternion<T> inverted(const Math::DualQuaternion<T>& transformation) {
        return transformation.invertedNormalized();
    }
};

}

#ifdef _WIN32
extern template class MAGNUM_SCENEGRAPH_EXPORT Object<BasicDualQuaternionTransformation<Float>>;
#endif

}}

#endif
