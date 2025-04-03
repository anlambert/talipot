/**
 *
 * Copyright (C) 2020-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Matrix.h>

#define INSTANTIATE_OPERATORS(MATRIX, TYPE, SIZE)                                             \
    template TLP_TEMPLATE_DEFINE_SCOPE MATRIX tlp::operator+(const MATRIX &, const MATRIX &); \
    template TLP_TEMPLATE_DEFINE_SCOPE MATRIX tlp::operator-(const MATRIX &, const MATRIX &); \
    template TLP_TEMPLATE_DEFINE_SCOPE MATRIX tlp::operator*(const MATRIX &, const TYPE &);   \
    template TLP_TEMPLATE_DEFINE_SCOPE MATRIX tlp::operator*(const MATRIX &, const MATRIX &); \
    template TLP_TEMPLATE_DEFINE_SCOPE tlp::Vector<TYPE, SIZE> tlp::operator*(                \
        const tlp::Vector<TYPE, SIZE> &vec, const MATRIX &);                                  \
    template TLP_TEMPLATE_DEFINE_SCOPE tlp::Vector<TYPE, SIZE> tlp::operator*(                \
        const MATRIX &, const tlp::Vector<TYPE, SIZE> &vec);

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Matrix<float, 3>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Matrix<float, 3>), float, 3)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Matrix<float, 4>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Matrix<float, 4>), float, 4)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Matrix<double, 3>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Matrix<double, 3>), double, 3)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Matrix<double, 4>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Matrix<double, 4>), double, 4)