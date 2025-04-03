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

#include <talipot/Vector.h>

#define INSTANTIATE_OPERATORS(VECTOR, TYPE)                                                   \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator*(const VECTOR &, const VECTOR &); \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator*(const TYPE, const VECTOR &);     \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator*(const VECTOR &, const TYPE);     \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator+(const VECTOR &, const VECTOR &); \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator+(const VECTOR &, const TYPE);     \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator-(const VECTOR &, const VECTOR &); \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator-(const VECTOR &, const TYPE);     \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator/(const VECTOR &, const VECTOR &); \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator/(const VECTOR &, const TYPE);     \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator^(const VECTOR &, const VECTOR &); \
    template TLP_TEMPLATE_DEFINE_SCOPE VECTOR tlp::operator-(const VECTOR &);

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<unsigned char, 4>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<unsigned char, 4>), unsigned char)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<uint, 2>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<uint, 2>), uint)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<uint, 3>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<uint, 3>), uint)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<uint, 4>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<uint, 4>), uint)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<int, 2>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<int, 2>), int)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<int, 3>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<int, 3>), int)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<int, 4>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<int, 4>), int)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<float, 1, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<float, 2, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<float, 2, double>), float)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<float, 3, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<float, 3, double>), float)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<float, 4, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<float, 4, double>), float)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<float, 5, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<float, 5, double>), float)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 2, long double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 2, long double>), double)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 1, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 2, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 2, double>), double)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 3, long double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 3, long double>), double)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 3, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 3, double>), double)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 4, long double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 4, long double>), double)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 4, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 4, double>), double)

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 5, long double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 5, long double>), double)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::Vector<double, 5, double>), TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_OPERATORS(SINGLE_ARG(tlp::Vector<double, 5, double>), double)