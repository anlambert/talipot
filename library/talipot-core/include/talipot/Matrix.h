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

#ifndef TALIPOT_MATRIX_H
#define TALIPOT_MATRIX_H

#include <talipot/Vector.h>

#include <cassert>
#include <iostream>
#include <vector>

namespace tlp {

#define MATRIX tlp::Matrix<Obj, SIZE>

/**
 * @ingroup Structures
 * \brief class for mathematical square matrix
 *
 * Enables to create a Square Matrix of Obj with a
 * limited size and provides Mathematical operation. Mathematical
 * operators must be defined for Obj. Out of bound accesses
 * are only checked in debug mode.
 *
 * \author : David Auber auber@labri.fr
 *
 * \author Contributor : Maxime Delorme
 * \version 0.0.2 27/04/2005
 */
template <typename Obj, size_t SIZE>
class Matrix : public Array<Vector<Obj, SIZE>, SIZE> {
  public:
    Matrix() = default;
    Matrix(const Array<Vector<Obj, SIZE>, SIZE> &a) : Array<Vector<Obj, SIZE>, SIZE>(a) {}

    // Builds a correlation matrix from a covariance matrix !
    Matrix(const std::vector<std::vector<Obj>> &covarianceMatrix);

    /**
     * Fill the matrix with the value of obj
     */
    MATRIX &fill(Obj obj);
    /**
     * Compute the determinant of the matrix,
     */
    Obj determinant() const;
    /**
     * Transpose the matrix and return "&(*this)".
     */
    MATRIX &transpose();
    /**
     * Inverse the matrix and return "&(*this)"
     */
    MATRIX &inverse();
    /**
     * add another matrix to the current one and return "&(*this)"
     */
    MATRIX &operator+=(const MATRIX &mat);
    /**
     * subtract another matrix from the current and return "&(*this)"
     */
    MATRIX &operator-=(const MATRIX &mat);
    /**
     * Check equality of two Matrices
     */
    bool operator==(const MATRIX &) const;
    /**
     * Check non equality of two Matrices
     */
    bool operator!=(const MATRIX &) const;
    /**
     * Multiply the matrix by another matrix and return "&(*this)"
     */
    MATRIX &operator*=(const MATRIX &mat);
    /**
     * Multiply all elements of the matrix by obj, return "&(*this)"
     */
    MATRIX &operator*=(const Obj &obj);
    /**
     *  Divide the matrix by another one return "&(*this)"
     */
    MATRIX &operator/=(const MATRIX &mat);
    /**
     * Divide all elements of the matrix by obj, return "&(*this)"
     */
    MATRIX &operator/=(const Obj &obj);
    /**
     * Returns the cofactor Matrix of this
     */
    MATRIX cofactor() const;
    /**
     * Returns a new matrix equal to the division of the matrix by
     * another matrix"
     */
    MATRIX operator/(const MATRIX &mat2) const;

    /**
     * Returns a new matrix equal to the division of the matrix by
     * obj"
     */
    MATRIX operator/(const Obj &obj) const;

    /**
     * Returns a new vector equal to the most influent eigenvector of the
     * matrix
     */
    Vector<Obj, SIZE> powerIteration(const uint nIterations) const;

    /**
     * Simplifies a 3x3 matrix in 2x2 matrix to be used with computeEigenVector
     */
    bool simplify(Matrix<Obj, 2> &simplifiedMatrix) const;

    /**
     * Returns the EigenVector of the matrix corresponding to the EigenValue passed, with a base x
     *           /!\ This can only be used with a 2x2 matrix !!! /!\
     */
    bool computeEigenVector(const float x, Vector<Obj, 3> &eigenVector) const;
};

typedef Matrix<float, 3> Mat3f;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Matrix<float, 3>), TLP_TEMPLATE_DECLARE_SCOPE)

typedef Matrix<double, 3> Mat3d;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Matrix<double, 3>), TLP_TEMPLATE_DECLARE_SCOPE)

typedef Matrix<float, 4> Mat4f;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Matrix<float, 4>), TLP_TEMPLATE_DECLARE_SCOPE)

typedef Matrix<double, 4> Mat4d;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Matrix<double, 4>), TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * Returns a new matrix equal to the sum of 2 matrices
 */
template <typename Obj, size_t SIZE>
extern TLP_TEMPLATE_DECLARE_SCOPE MATRIX operator+(const MATRIX &mat1, const MATRIX &mat2);
/**
 * Returns a new matrix equal to the difference of 2 matrices
 */
template <typename Obj, size_t SIZE>
extern TLP_TEMPLATE_DECLARE_SCOPE MATRIX operator-(const MATRIX &mat1, const MATRIX &mat2);
/**
 * Returns a new matrix equal to the multiplication of the matrix by
 * obj
 */
template <typename Obj, size_t SIZE>
extern TLP_TEMPLATE_DECLARE_SCOPE MATRIX operator*(const MATRIX &mat, const Obj &obj);
/**
 * Returns a new matrix equal to the multiplication of the matrix by
 * another matrix
 */
template <typename Obj, size_t SIZE>
extern TLP_TEMPLATE_DECLARE_SCOPE MATRIX operator*(const MATRIX &mat1, const MATRIX &mat2);
/**
 * Returns a new vector equal to the multiplication of the vector by
 * a matrix,(the vector is automatically transposed to do the multiplication)
 */
template <typename Obj, size_t SIZE>
extern TLP_TEMPLATE_DECLARE_SCOPE Vector<Obj, SIZE> operator*(const Vector<Obj, SIZE> &vec,
                                                              const tlp::Matrix<Obj, SIZE> &);
/**
 * Returns a new vector equal to the multiplication of the matrix by
 * a vector
 */
template <typename Obj, size_t SIZE>
extern TLP_TEMPLATE_DECLARE_SCOPE Vector<Obj, SIZE> operator*(const Matrix<Obj, SIZE> &,
                                                              const Vector<Obj, SIZE> &vec);
}

#ifdef DLL_TALIPOT
#include "cxx/Matrix.cxx"
#endif

#endif // TALIPOT_MATRIX_H
