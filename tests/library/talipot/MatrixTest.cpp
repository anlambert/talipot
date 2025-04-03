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

#include <iomanip>

#include "MatrixTest.h"

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(MatrixTest);

template <typename Obj, uint SIZE>
void display(const Matrix<Obj, SIZE> &mat) {
    for (uint row = 0; row < SIZE; ++row) {
        cout << "| ";

        for (uint col = 0; col < SIZE; ++col) {
            cout << setw(4) << mat[row][col] << setw(1) << " ";
        }

        cout << "|" << endl;
    }
}

//==========================================================
void MatrixTest::testExternalOperation() {
    const uint SIZE = 4;
    Matrix<double, SIZE> mat1, matid, matnull;
    matnull.fill(0);
    matid.fill(0);
    Vector<double, SIZE> vec;

    for (uint i = 0; i < SIZE; ++i) {
        vec[i] = i;

        for (uint j = 0; j < SIZE; ++j) {
            mat1[i][j] = double((i + 1) * (j + SIZE));

            if (i == j) {
                matid[i][j] = 1.0;
            }

            if (i == j) {
                mat1[i][j] = 0.0; // ensure be inversible
            }
        }
    }

    const Matrix<double, SIZE> mat2(mat1);

    Matrix<double, SIZE> result;

    result = mat2;

    result = mat2 + result;

    result = result / 2.0;

    CPPUNIT_ASSERT_EQUAL(mat2, result);

    result = mat2 * 2.0;

    result = result - mat2;

    CPPUNIT_ASSERT_EQUAL(mat2, result);

    result = result - mat2;

    CPPUNIT_ASSERT_EQUAL(matnull, result);

    result = mat2;

    CPPUNIT_ASSERT_EQUAL(result, mat2);

    Matrix<double, SIZE> matinv(mat2);

    matinv.inverse();

    result = mat2 * matinv;

    for (uint i = 0; i < SIZE; ++i) {
        for (uint j = 0; j < SIZE; ++j) {
            double res = fabs(matid[i][j] - result[i][j]);
            bool ok = false;

            if (fabs(res) < 1.E-5) {
                ok = true;
            }

            CPPUNIT_ASSERT(ok);
        }
    }

    Vector<double, SIZE> vec2, vec3;
    vec2 = matid * vec;

    CPPUNIT_ASSERT_EQUAL(vec, vec2);

    vec2 = mat2 * vec;
    vec2 = matinv * vec2;

    for (uint j = 0; j < SIZE; ++j) {
        double res = fabs(vec2[j] - vec[j]);
        bool ok = false;

        if (res < 1.E-5) {
            ok = true;
        }

        CPPUNIT_ASSERT(ok);
    }

    vec2 = mat2 * vec;
    vec3 = vec * mat2;
    bool ok = false;

    for (uint j = 0; j < SIZE; ++j) {
        double res = fabs(vec2[j] - vec3[j]);

        if (res > 1.E-5) {
            ok = true;
        }
    }

    CPPUNIT_ASSERT(ok);

    vec2 = vec * mat2;
    vec3 = vec2 * matinv;

    for (uint j = 0; j < SIZE; ++j) {
        bool ok = false;
        double res = fabs(vec[j] - vec3[j]);

        if (res < 1.E-5) {
            ok = true;
        }

        CPPUNIT_ASSERT(ok);
    }
}
//==========================================================
void MatrixTest::testInternalOperation() {
    const uint SIZE = 4;
    Matrix<double, SIZE> mat1, matnull, matid;
    matnull.fill(0);
    matid.fill(0);

    for (uint i = 0; i < SIZE; ++i) {
        for (uint j = 0; j < SIZE; ++j) {
            mat1[i][j] = double((i + 1) * (j + SIZE));

            if (i == j) {
                matid[i][j] = 1.0;
            }

            CPPUNIT_ASSERT_EQUAL(0.0, matnull[i][j]);
            CPPUNIT_ASSERT_EQUAL(double(((i + 1) * (j + SIZE))), mat1[i][j]);

            if (i == j) {
                mat1[i][j] = 0.0;
            }
        }
    }

    Matrix<double, SIZE> mat2(mat1);
    CPPUNIT_ASSERT_EQUAL(mat2, mat1);
    mat1[1] += 3;
    CPPUNIT_ASSERT(mat1 != mat2);
    mat1[1] -= 3;
    CPPUNIT_ASSERT_EQUAL(mat2, mat1);
    mat1 += mat2;
    mat1 /= 2.0;
    CPPUNIT_ASSERT_EQUAL(mat2, mat1);
    mat1 *= 2.0;
    mat1 -= mat2;
    CPPUNIT_ASSERT_EQUAL(mat2, mat1);
    mat1 -= mat2;
    CPPUNIT_ASSERT_EQUAL(matnull, mat1);
    mat1 = mat2;
    CPPUNIT_ASSERT_EQUAL(mat2, mat1);
    Matrix<double, SIZE> matinv(mat1);
    matinv.inverse();
    mat1 *= matinv;

    for (uint i = 0; i < SIZE; ++i) {
        for (uint j = 0; j < SIZE; ++j) {
            double res = fabs(matid[i][j] - mat1[i][j]);
            bool ok = false;

            if (res < 1.E-5) {
                ok = true;
            }

            CPPUNIT_ASSERT(ok);
        }
    }

    mat1.fill(1.0);
    mat2 = mat1;
    mat1 *= mat2;
    mat2 *= SIZE;
    CPPUNIT_ASSERT_EQUAL(mat2, mat1);
    mat1.fill(1.0);
    mat1 *= mat1;
    CPPUNIT_ASSERT_EQUAL(mat2, mat1);
}
