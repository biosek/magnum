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

#include "MeshBuilderTest.h"

#include <QtTest/QTest>

#include "MeshBuilder.h"

QTEST_APPLESS_MAIN(Magnum::Test::MeshBuilderTest)

using namespace std;

namespace Magnum { namespace Test {

void MeshBuilderTest::setData() {
    MeshBuilder<unsigned int> builder;

    unsigned int vertexData[] = { 1, 2, 3, 4 };
    GLubyte indexData[] = { 0, 1, 2, 1, 2, 3 };
    builder.setData(vertexData, indexData, 4, 6);

    QVERIFY((builder.vertices() == vector<unsigned int>{1, 2, 3, 4}));
    QVERIFY((builder.indices() == vector<unsigned int>{0, 1, 2, 1, 2, 3}));
}

void MeshBuilderTest::addFace() {
    MeshBuilder<unsigned int> builder;
    builder.addVertex(1);
    builder.addVertex(2);
    builder.addVertex(3);
    builder.addVertex(4);
    builder.addFace(0, 1, 2);
    builder.addFace(1, 2, 3);

    QVERIFY((builder.vertices() == vector<unsigned int>{1, 2, 3, 4}));
    QVERIFY((builder.indices() == vector<unsigned int>{0, 1, 2, 1, 2, 3}));
}

}}
