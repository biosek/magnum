#ifndef Magnum_Math_Range_h
#define Magnum_Math_Range_h
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
 * @brief Class @ref Magnum::Math::Range, @ref Magnum::Math::Range2D, @ref Magnum::Math::Range3D, alias @ref Magnum::Math::Range1D
 */

#include "Math/Vector3.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<UnsignedInt, class> struct RangeTraits;

    template<class T> struct RangeTraits<1, T> { typedef Vector<1, T> Type; };
    template<class T> struct RangeTraits<2, T> { typedef Vector2<T> Type; };
    template<class T> struct RangeTraits<3, T> { typedef Vector3<T> Type; };
}

/**
@brief N-dimensional range

Axis-aligned line (in 1D), rectangle (in 2D) or cube (in 3D). Minimal
coordinate is inclusive, maximal exclusive. See @ref Range1D, @ref Range2D and
@ref Range3D specializations for given dimension count.
*/
template<UnsignedInt dimensions, class T> class Range {
    template<UnsignedInt, class> friend class Range;

    public:
        /**
         * @brief Underlying vector type
         *
         * `T` in 1D, @ref Vector2<T> in 2D, @ref Vector3<T> in 3D.
         */
        typedef typename Implementation::RangeTraits<dimensions, T>::Type VectorType;

        /**
         * Create range from minimal coordinates and size
         * @param min   Minimal coordinates
         * @param size  Range size
         */
        static Range<dimensions, T> fromSize(const VectorType& min, const VectorType& size) {
            return {min, min+size};
        }

        /**
         * @brief Construct zero range
         *
         * Construct zero-size range positioned at origin.
         */
        constexpr Range(): _min{}, _max{} {}

        /** @brief Construct range from minimal and maximal coordinates */
        constexpr Range(const VectorType& min, const VectorType& max): _min(min), _max(max) {}

        /** @brief Copy constructor */
        constexpr Range(const Range<dimensions, T>&) = default;

        /**
         * @brief Construct range from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * Range2D<Float> floatingPoint({1.3f, 2.7f}, {-15.0f, 7.0f});
         * Range2D<Byte> integral(floatingPoint); // {{1, 2}, {-15, 7}}
         * @endcode
         */
        template<class U> constexpr explicit Range(const Range<dimensions, U>& other): _min(other._min), _max(other._max) {}

        /** @brief Equality comparison */
        constexpr bool operator==(const Range<dimensions, T>& other) const {
            return _min == other._min && _max == other._max;
        }

        /** @brief Non-equality comparison */
        constexpr bool operator!=(const Range<dimensions, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Minimal coordinates (inclusive)
         *
         * @see @ref size(), @ref Range2D::bottomLeft(),
         *      @ref Range3D::backBottomLeft()
         */
        VectorType& min() { return _min; }
        constexpr const VectorType min() const { return _min; } /**< @overload */

        /**
         * @brief Maximal coordinates (exclusive)
         *
         * @see @ref size(), @ref Range2D::topRight(),
         *      @ref Range3D::frontTopRight()
         */
        VectorType& max() { return _max; }
        constexpr const VectorType max() const { return _max; } /**< @overload */

        /**
         * @brief Range size
         *
         * @see @ref min(), @ref max(), @ref Range2D::sizeX(),
         *      @ref Range2D::sizeY(), @ref Range3D::sizeX(),
         *      @ref Range3D::sizeY(), @ref Range3D::sizeZ(), @ref center()
         */
        VectorType size() const { return _max - _min; }

        /**
         * @brief Range center
         *
         * @see @ref Range2D::centerX(), @ref Range2D::centerY(),
         *      @ref Range3D::centerX(), @ref Range3D::centerY(),
         *      @ref Range3D::centerZ(), @ref size()
         */
        VectorType center() const { return (_min + _max)/T(2); }

        /**
         * @brief Translated range
         *
         * Translates the minimal and maximal coordinates by given amount. Size
         * remains the same.
         * @see @ref padded()
         */
        Range<dimensions, T> translated(const VectorType& vector) const;

        /**
         * @brief Padded rage
         *
         * Translates the minimal and maximal coordinates by given amount.
         * Center remains the same.
         * @see @ref translated()
         */
        Range<dimensions, T> padded(const VectorType& padding) const;

        /**
         * @brief Scaled range
         *
         * Multiplies the minimal and maximal coordinates by given amount.
         * @see @ref padded()
         */
        Range<dimensions, T> scaled(const VectorType& scaling) const;

    private:
        VectorType _min, _max;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(dimensions, Type, VectorType)  \
    static Type<T> fromSize(const VectorType<T>& min, const VectorType<T>& size) { \
        return Range<dimensions, T>::fromSize(min, size);                   \
    }                                                                       \
    Type<T> translated(const VectorType<T>& vector) const {                 \
        return Range<dimensions, T>::translated(vector);                    \
    }                                                                       \
    Type<T> padded(const VectorType<T>& padding) const {                    \
        return Range<dimensions, T>::padded(padding);                       \
    }                                                                       \
    Type<T> scaled(const VectorType<T>& scaling) const {                    \
        return Range<dimensions, T>::scaled(scaling);                       \
    }
#endif

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief One-dimensional range

Convenience alternative to <tt>%Range<1, T></tt>. See @ref Range for more
information.
@note Not available on GCC < 4.7. Use <tt>%Range<1, T></tt> instead.
*/
#ifndef CORRADE_MSVC2013_COMPATIBILITY /* Apparently cannot have multiply defined aliases */
template<class T> using Range1D = Range<1, T>;
#endif
#endif

/**
@brief Two-dimensional range

See @ref Range for more information.
@see @ref Range1D, @ref Range3D
*/
template<class T> class Range2D: public Range<2, T> {
    public:
        /** @copydoc Range() */
        constexpr /*implicit*/ Range2D() {}

        /** @copydoc Range(const VectorType&, const VectorType&) */
        constexpr /*implicit*/ Range2D(const Vector2<T>& min, const Vector2<T>& max): Range<2, T>(min, max) {}

        /** @copydoc Range(const Range&) */
        constexpr /*implicit*/ Range2D(const Range<2, T>& other): Range<2, T>(other) {}

        /** @copydoc Range(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Range2D(const Range2D<U>& other): Range<2, T>(other) {}

        /**
         * @brief Bottom left corner
         *
         * Equivalent to @ref min().
         */
        Vector2<T>& bottomLeft() { return Range<2, T>::min(); }
        constexpr Vector2<T> bottomLeft() const { return Range<2, T>::min(); } /**< @overload */

        /** @brief Bottom right corner */
        constexpr Vector2<T> bottomRight() const {
            return {Range<2, T>::max().x(), Range<2, T>::min().y()};
        }

        /** @brief Top left corner */
        constexpr Vector2<T> topLeft() const {
            return {Range<2, T>::min().x(), Range<2, T>::max().y()};
        }

        /**
         * @brief Top right corner
         *
         * Equivalent to @ref max().
         */
        Vector2<T>& topRight() { return Range<2, T>::max(); }
        constexpr Vector2<T> topRight() const { return Range<2, T>::max(); } /**< @overload */

        /** @brief Left edge */
        T& left() { return Range<2, T>::min().x(); }
        constexpr T left() const { return Range<2, T>::min().x(); } /**< @overload */

        /** @brief Right edge */
        T& right() { return Range<2, T>::max().x(); }
        constexpr T right() const { return Range<2, T>::max().x(); } /**< @overload */

        /** @brief Bottom edge */
        T& bottom() { return Range<2, T>::min().y(); }
        constexpr T bottom() const { return Range<2, T>::min().y(); } /**< @overload */

        /** @brief Top edge */
        T& top() { return Range<2, T>::max().y(); }
        constexpr T top() const { return Range<2, T>::max().y(); } /**< @overload */

        /**
         * @brief %Range width
         *
         * @see @ref size()
         */
        T sizeX() const {
            return Range<2, T>::max().x() - Range<2, T>::min().x();
        }

        /**
         * @brief %Range height
         *
         * @see @ref size()
         */
        T sizeY() const {
            return Range<2, T>::max().y() - Range<2, T>::min().y();
        }

        /**
         * @brief %Range center on X axis
         *
         * @see @ref center()
         */
        T centerX() const {
            return (Range<2, T>::min().x() + Range<2, T>::max().x())/T(2);
        }

        /**
         * @brief %Range center on Y axis
         *
         * @see @ref center()
         */
        T centerY() const {
            return (Range<2, T>::min().y() + Range<2, T>::max().y())/T(2);
        }

        MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(2, Range2D, Vector2)
};

/**
@brief Two-dimensional range

See @ref Range for more information.
@see @ref Range1D, @ref Range2D
*/
template<class T> class Range3D: public Range<3, T> {
    public:
        /** @copydoc Range() */
        constexpr /*implicit*/ Range3D() {}

        /** @copydoc Range(const VectorType&, const VectorType&) */
        constexpr /*implicit*/ Range3D(const Vector3<T>& min, const Vector3<T>& max): Range<3, T>(min, max) {}

        /** @copydoc Range(const Range&) */
        constexpr /*implicit*/ Range3D(const Range<3, T>& other): Range<3, T>(other) {}

        /** @copydoc Range(const Range<dimensions, U>&) */
        template<class U> constexpr explicit Range3D(const Range3D<U>& other): Range<3, T>(other) {}

        /**
         * @brief Back bottom left corner
         *
         * Equivalent to @ref min().
         */
        Vector3<T>& backBottomLeft() { return Range<3, T>::min(); }
        constexpr Vector3<T> backBottomLeft() const { return Range<3, T>::min(); } /**< @overload */

        /** @brief Back bottom right corner */
        constexpr Vector3<T> backBottomRight() const {
            return {Range<3, T>::max().x(), Range<3, T>::min().y(), Range<3, T>::min().z()};
        }

        /** @brief Back top right corner */
        constexpr Vector3<T> backTopLeft() const {
            return {Range<3, T>::min().x(), Range<3, T>::max().y(), Range<3, T>::min().z()};
        }

        /** @brief Back top right corner */
        constexpr Vector3<T> backTopRight() const {
            return {Range<3, T>::max().x(), Range<3, T>::max().y(), Range<3, T>::min().z()};
        }

        /**
         * @brief Front top right corner
         *
         * Equivalent to @ref max().
         */
        Vector3<T>& frontTopRight() { return Range<3, T>::max(); }
        constexpr Vector3<T> frontTopRight() const { return Range<3, T>::max(); } /**< @overload */

        /** @brief Front top left corner */
        constexpr Vector3<T> frontTopLeft() const {
            return {Range<3, T>::min().x(), Range<3, T>::max().y(), Range<3, T>::max().z()};
        }

        /** @brief Front bottom right corner */
        constexpr Vector3<T> frontBottomRight() const {
            return {Range<3, T>::max().x(), Range<3, T>::min().y(), Range<3, T>::max().z()};
        }

        /** @brief Front bottom left corner */
        constexpr Vector3<T> frontBottomLeft() const {
            return {Range<3, T>::min().x(), Range<3, T>::min().y(), Range<3, T>::max().z()};
        }

        /** @brief Left edge */
        T& left() { return Range<3, T>::min().x(); }
        constexpr T left() const { return Range<3, T>::min().x(); } /**< @overload */

        /** @brief Right edge */
        T& right() { return Range<3, T>::max().x(); }
        constexpr T right() const { return Range<3, T>::max().x(); } /**< @overload */

        /** @brief Bottom edge */
        T& bottom() { return Range<3, T>::min().y(); }
        constexpr T bottom() const { return Range<3, T>::min().y(); } /**< @overload */

        /** @brief Top edge */
        T& top() { return Range<3, T>::max().y(); }
        constexpr T top() const { return Range<3, T>::max().y(); } /**< @overload */

        /** @brief Back edge */
        T& back() { return Range<3, T>::min().z(); }
        constexpr T back() const { return Range<3, T>::min().z(); } /**< @overload */

        /** @brief Front edge */
        T& front() { return Range<3, T>::max().z(); }
        constexpr T front() const { return Range<3, T>::max().z(); } /**< @overload */

        /**
         * @brief %Range width
         *
         * @see @ref size()
         */
        T sizeX() const {
            return Range<3, T>::max().x() - Range<3, T>::min().x();
        }

        /**
         * @brief %Range height
         *
         * @see @ref size()
         */
        T sizeY() const {
            return Range<3, T>::max().y() - Range<3, T>::min().y();
        }

        /**
         * @brief %Range depth
         *
         * @see @ref size()
         */
        T sizeZ() const {
            return Range<3, T>::max().z() - Range<3, T>::min().z();
        }

        /**
         *
         * @brief %Range center on X axis
         *
         * @see @ref center()
         */
        T centerX() const {
            return (Range<3, T>::min().x() + Range<3, T>::max().x())/T(2);
        }

        /**
         * @brief %Range center on Y axis
         *
         * @see @ref center()
         */
        T centerY() const {
            return (Range<3, T>::min().y() + Range<3, T>::max().y())/T(2);
        }

        /**
         * @brief %Range center on Z axis
         *
         * @see @ref center()
         */
        T centerZ() const {
            return (Range<3, T>::min().z() + Range<3, T>::max().z())/T(2);
        }

        MAGNUM_RANGE_SUBCLASS_IMPLEMENTATION(3, Range3D, Vector3)
};

/** @debugoperator{Magnum::Math::Range} */
template<UnsignedInt dimensions, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Range<dimensions, T>& value) {
    debug << "Range({";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.min()[0];
    for(UnsignedInt i = 1; i != dimensions; ++i) debug << ", " << value.min()[i];
    debug << "}, {" << value.max()[0];
    for(UnsignedInt i = 1; i != dimensions; ++i) debug << ", " << value.max()[i];
    debug << "})";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

template<UnsignedInt dimensions, class T> Range<dimensions, T> Range<dimensions, T>::translated(const VectorType& vector) const {
    return {_min + vector, _max + vector};
}

template<UnsignedInt dimensions, class T> Range<dimensions, T> Range<dimensions, T>::padded(const VectorType& padding) const {
    return {_min - padding, _max + padding};
}

template<UnsignedInt dimensions, class T> Range<dimensions, T> Range<dimensions, T>::scaled(const VectorType& scaling) const {
    return {_min*scaling, _max*scaling};
}

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::Range} */
template<Magnum::UnsignedInt dimensions, class T> struct ConfigurationValue<Magnum::Math::Range<dimensions, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::Range<dimensions, T>& value, const ConfigurationValueFlags flags) {
        return ConfigurationValue<Magnum::Math::Vector<dimensions*2, T>>::toString(
            reinterpret_cast<const Magnum::Math::Vector<dimensions*2, T>&>(value), flags);
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::Range<dimensions, T> fromString(const std::string& stringValue, const ConfigurationValueFlags flags) {
        const auto vec = ConfigurationValue<Magnum::Math::Vector<dimensions*2, T>>::fromString(stringValue, flags);
        return *reinterpret_cast<const Magnum::Math::Range<dimensions, T>*>(vec.data());
    }
};

/** @configurationvalue{Magnum::Math::Range2D} */
template<class T> struct ConfigurationValue<Magnum::Math::Range2D<T>>: public ConfigurationValue<Magnum::Math::Range<2, T>> {};

/** @configurationvalue{Magnum::Math::Range3D} */
template<class T> struct ConfigurationValue<Magnum::Math::Range3D<T>>: public ConfigurationValue<Magnum::Math::Range<3, T>> {};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Int>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Int>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::Range<3, Magnum::Double>>;
#endif
#endif

}}

#endif
