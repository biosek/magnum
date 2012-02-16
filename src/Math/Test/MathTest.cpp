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

#include "MathTest.h"

#include <QtTest/QTest>

#include "Math.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::MathTest)

namespace Magnum { namespace Math { namespace Test {

void MathTest::degrad() {
    QCOMPARE(90.0_deg, PI/2);
    QCOMPARE(90_deg, PI/2);
    QCOMPARE(90.0_degf, static_cast<float>(PI/2));
    QCOMPARE(90_degf, static_cast<float>(PI/2));
    QCOMPARE(3_rad, 3.0);
    QCOMPARE(3_radf, 3.0f);
    QCOMPARE(3.14_rad, 3.14);
    QCOMPARE(3.14_radf, 3.14f);
}

void MathTest::pow() {
    QCOMPARE(Math::pow<10>(2), 1024ul);
    QCOMPARE(Math::pow<0>(3), 1ul);
}

void MathTest::log() {
    QCOMPARE(Math::log(2, 256), 8ul);
    QCOMPARE(Math::log(256, 2), 0ul);
}

}}}
