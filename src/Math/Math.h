#ifndef Magnum_Math_Math_h
#define Magnum_Math_Math_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <cstddef>

#include "utilities.h"

/** @file
 * @brief Math constants and utilities
 */

namespace Magnum {

/**
 * @brief %Math library
 *
 * Template classes for matrix and vector calculations.
*/
namespace Math {

/** @brief Pi */
#define PI 3.1415926535

/**
 * @brief Integral power
 *
 * Returns integral power of base to the exponent.
 */
template<size_t exponent> inline constexpr size_t pow(size_t base) {
    return base*pow<exponent-1>(base);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> inline constexpr size_t pow<0>(size_t base) { return 1; }
#endif

/**
 * @brief Integral logarithm
 *
 * Returns integral logarithm of given number with given base.
 */
size_t MAGNUM_EXPORT log(size_t base, size_t number);

/**
 * @brief Angle in degrees
 *
 * Literal to make angle entering less error-prone. Converts the value to
 * radians at compile time. For example `180_deg` is converted to `3.14`.
 */
inline constexpr double operator "" _deg(long double value) { return value*PI/180; }

/** @copydoc operator "" _deg(long double) */
inline constexpr double operator "" _deg(unsigned long long value) { return value*PI/180; }

/**
 * @brief Angle in degrees, converted to float
 *
 * See also operator "" _deg().
 */
inline constexpr float operator "" _degf(long double value) { return value*PI/180; }

/** @copydoc operator "" _degf(long double) */
inline constexpr float operator "" _degf(unsigned long long value) { return value*PI/180; }

/**
 * @brief Angle in radians
 *
 * See also operator "" _deg().
 */
inline constexpr double operator "" _rad(long double value) { return value; }

/** @copydoc operator "" _rad(long double) */
inline constexpr double operator "" _rad(unsigned long long value) { return value; }

/**
 * @brief Angle in radians, converted to float
 *
 * See also operator "" _rad().
 */
inline constexpr float operator "" _radf(long double value) { return value; }

/** @copydoc operator "" _radf(long double) */
inline constexpr float operator "" _radf(unsigned long long value) { return value; }

}}

#endif
