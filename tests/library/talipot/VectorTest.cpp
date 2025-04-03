/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "VectorTest.h"

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(VectorTest);

//==========================================================
void VectorTest::testExternalOperation() {}
//==========================================================
void VectorTest::testInternalOperation() {
    const size_t SIZE = 4;
    Vec4d vect1 = {0, 1, 2, 3};
    Vec4d vectnull = {0};

    for (size_t i = 0; i < SIZE; ++i) {
        CPPUNIT_ASSERT_EQUAL(0.0, vectnull[i]);
        CPPUNIT_ASSERT_EQUAL(double(i), vect1[i]);
    }

    Vec4d vect2(vect1);
    CPPUNIT_ASSERT_EQUAL(vect2, vect1);
    vect1 += 3;
    CPPUNIT_ASSERT(vect1 != vect2);

    for (size_t i = 0; i < SIZE; ++i) {
        CPPUNIT_ASSERT_EQUAL(double(i + 3), vect1[i]);
    }

    vect1 -= 3;
    CPPUNIT_ASSERT_EQUAL(vect2, vect1);
    vect1 += vect2;
    vect1 /= 2.0;
    CPPUNIT_ASSERT_EQUAL(vect2, vect1);
    vect1 -= vect2;
    CPPUNIT_ASSERT_EQUAL(vectnull, vect1);
    vect1 = vect2;
    CPPUNIT_ASSERT_EQUAL(vect2, vect1);

    for (size_t i = 0; i < SIZE; ++i) {
        vect1[i] = i;
        vect2[i] = i;
    }

    CPPUNIT_ASSERT_EQUAL(false, vect1 < vect2);
    CPPUNIT_ASSERT_EQUAL(false, vect1 > vect2);
    CPPUNIT_ASSERT_EQUAL(vect2, vect1);
    vect2[SIZE - 1]++;
    CPPUNIT_ASSERT(vect1 < vect2);
    CPPUNIT_ASSERT_EQUAL(false, vect1 > vect2);
    CPPUNIT_ASSERT(vect1 != vect2);
    vect1[0]++;
    CPPUNIT_ASSERT_EQUAL(false, vect1 < vect2);
    CPPUNIT_ASSERT(vect1 > vect2);
    CPPUNIT_ASSERT(vect1 != vect2);
}
