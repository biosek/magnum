#ifndef Magnum_Math_Angle_h
#define Magnum_Math_Angle_h
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
 * @brief Class Magnum::Math::Deg, Magnum::Math::Rad and related operators.
 */

#include <Utility/Debug.h>
#include <corradeCompatibility.h>

#include "Math/Constants.h"
#include "Math/Math.h"
#include "Math/Unit.h"

#include "magnumVisibility.h"

namespace Magnum { namespace Math {

/**
@brief Angle in degrees

Along with Rad provides convenience classes to make angle specification and
conversion less error-prone.

@section Rad-usage Usage

You can enter the value either by using literal:
@code
auto degrees = 60.0_degf;       // type is Deg<Float>
auto radians = 1.047_rad;       // type is Rad<Double>
@endcode

Or explicitly convert unitless value (such as output from some function) to
either degrees or radians:
@code
Double foo();

Deg<Float> degrees(35.0f);
Rad<Double> radians(foo());
//degrees = 60.0f;              // error, no implicit conversion
@endcode

The classes support all arithmetic operations, such as addition, subtraction
or multiplication/division by unitless number:
@code
auto a = 60.0_degf + 17.35_degf;
auto b = -a + 23.0_degf*4;
//auto c = 60.0_degf*45.0_degf; // error, undefined resulting unit
@endcode

It is also possible to compare angles with all comparison operators, but
comparison of degrees and radians is not possible without explicit conversion
to common type:
@code
Rad<Float> angle();

Deg<Float> x = angle();         // convert to degrees for easier comparison
if(x < 30.0_degf) foo();
//if(x > 1.57_radf) bar();      // error, both need to be of the same type
@endcode

It is possible to seamlessly convert between degrees and radians and explicitly
convert the value back to underlying type:
@code
Float sine(Rad<Float> angle);
Float a = sine(60.0_degf);      // the same as sine(1.047_radf)
Deg<Double> b = 1.047_rad;      // the same as 60.0_deg
Float d = Double(b);            // 60.0
//Float e = b;                  // error, no implicit conversion
@endcode

@section Rad-conversions Requirement of explicit conversion

The requirement of explicit conversions from and to unitless types helps to
reduce unit-based errors. Consider following example with implicit conversions
allowed:
@code
Float std::sin(Float angle);
Float sine(Rad<Float> angle);

Float a = 60.0f;                // degrees
sine(a);                        // silent error, sine() expected radians

auto b = 60.0_degf;             // degrees
std::sin(b);                    // silent error, std::sin() expected radians
@endcode

These silent errors are easily avoided by requiring explicit conversions:
@code
//sine(angleInDegrees);                     // compilation error
sine(Deg<Float>(angleInDegrees));           // explicitly specifying unit

//std::sin(angleInDegrees);                 // compilation error
std::sin(Float(Rad<Float>(angleInDegrees)); // required explicit conversion hints
                                            // to user that this case needs special
                                            // attention (i.e., conversion to radians)
@endcode

@see Magnum::Deg, Magnum::Degd
*/
template<class T> class Deg: public Unit<Deg, T> {
    public:
        /** @brief Default constructor */
        constexpr /*implicit*/ Deg() {}

        /** @brief Explicit constructor from unitless type */
        constexpr explicit Deg(T value): Unit<Math::Deg, T>(value) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Deg(Unit<Math::Deg, T> value): Unit<Math::Deg, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> constexpr explicit Deg(Unit<Math::Deg, U> value): Unit<Math::Deg, T>(value) {}

        /**
         * @brief Construct degrees from radians
         *
         * Performs conversion from radians to degrees, i.e.:
         * @f[
         *      deg = 180 \frac {rad} \pi
         * @f]
         */
        constexpr /*implicit*/ Deg(Unit<Rad, T> value);
};

#if !defined(CORRADE_GCC46_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
#ifndef MAGNUM_TARGET_GLES
/** @relates Deg
@brief Double-precision degree value literal

Example usage:
@code
Double cosine = Math::cos(60.0_deg);  // cosine = 0.5
Double cosine = Math::cos(1.047_rad); // cosine = 0.5
@endcode
@see Magnum::operator""_deg(), operator""_degf(), operator""_rad()
@note Not available on GCC < 4.7 and MSVC 2013. Use Deg::Deg(T) instead.
@requires_gl Only single-precision types are available in OpenGL ES.
*/
constexpr Deg<Double> operator "" _deg(long double value) { return Deg<Double>(value); }
#endif

/** @relates Deg
@brief Single-precision degree value literal

Example usage:
@code
Float tangent = Math::tan(60.0_degf);  // tangent = 1.732f
Float tangent = Math::tan(1.047_radf); // tangent = 1.732f
@endcode
@see Magnum::operator""_degf(), operator""_deg(), operator""_radf()
@note Not available on GCC < 4.7 and MSVC 2013. Use Deg::Deg(T) instead.
@requires_gl Only single-precision types are available in OpenGL ES.
*/
constexpr Deg<Float> operator "" _degf(long double value) { return Deg<Float>(value); }
#endif

/**
@brief Angle in radians

See Deg for more information.
@see Magnum::Rad, Magnum::Radd
*/
template<class T> class Rad: public Unit<Rad, T> {
    public:
        /** @brief Default constructor */
        constexpr /*implicit*/ Rad() {}

        /** @brief Construct from unitless type */
        constexpr explicit Rad(T value): Unit<Math::Rad, T>(value) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Rad(Unit<Math::Rad, T> value): Unit<Math::Rad, T>(value) {}

        /** @brief Construct from another underlying type */
        template<class U> constexpr explicit Rad(Unit<Math::Rad, U> value): Unit<Math::Rad, T>(value) {}

        /**
         * @brief Construct radians from degrees
         *
         * Performs conversion from degrees to radians, i.e.:
         * @f[
         *      rad = deg \frac \pi 180
         * @f]
         */
        constexpr /*implicit*/ Rad(Unit<Deg, T> value);
};

#if !defined(CORRADE_GCC46_COMPATIBILITY) && !defined(CORRADE_MSVC2013_COMPATIBILITY)
#ifndef MAGNUM_TARGET_GLES
/** @relates Rad
@brief Double-precision radian value literal

See operator""_rad() for more information.
@see Magnum::operator""_rad(), operator""_radf(), operator""_deg()
@note Not available on GCC < 4.7 and MSVC 2013. Use Rad::Rad(T) instead.
*/
constexpr Rad<Double> operator "" _rad(long double value) { return Rad<Double>(value); }
#endif

/** @relates Rad
@brief Single-precision radian value literal

See operator""_degf() for more information.
@see Magnum::operator""_radf(), operator""_rad(), operator""_degf()
@note Not available on GCC < 4.7 and MSVC 2013. Use Rad::Rad(T) instead.
*/
constexpr Rad<Float> operator "" _radf(long double value) { return Rad<Float>(value); }
#endif

template<class T> constexpr Deg<T>::Deg(Unit<Rad, T> value): Unit<Math::Deg, T>(T(180)*value.toUnderlyingType()/Math::Constants<T>::pi()) {}
template<class T> constexpr Rad<T>::Rad(Unit<Deg, T> value): Unit<Math::Rad, T>(value.toUnderlyingType()*Math::Constants<T>::pi()/T(180)) {}

/** @debugoperator{Magnum::Math::Rad} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Unit<Rad, T>& value) {
    debug << "Rad(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.toUnderlyingType() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/** @debugoperator{Magnum::Math::Deg} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Unit<Deg, T>& value) {
    debug << "Deg(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    debug << value.toUnderlyingType() << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Unit<Rad, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Unit<Deg, Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Unit<Rad, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const Unit<Deg, Double>&);
#endif
#endif

}}

#endif
