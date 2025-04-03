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

#include <talipot/config.h>

//=================================================================
template <typename T, size_t N>
std::ostream &tlp::operator<<(std::ostream &os, const tlp::Array<T, N> &a) {
    os << "(";

    for (uint i = 0; i < N; ++i) {
        if (i > 0) {
            os << ",";
        }

        os << a[i];
    }

    os << ")";
    return os;
}

template <typename T, size_t N>
std::istream &tlp::operator>>(std::istream &is, tlp::Array<T, N> &outA) {
    char c;
    int pos = is.tellg();
    is.clear();

    // skip spaces
    while (bool(is >> c) && isspace(c)) {
    }

    if (c != '(') {
        is.seekg(pos);
        is.setstate(std::ios::failbit);
        return is;
    }

    for (uint i = 0; i < N; ++i) {
        bool ok;

        if (i > 0) {
            // skip spaces
            while ((ok = bool(is >> c)) && isspace(c)) {
            }

            if (!ok || c != ',') {
                is.seekg(pos);
                is.setstate(std::ios::failbit);
                return is;
            }
        }

        // skip spaces
        while ((ok = bool(is >> c)) && isspace(c)) {
        }

        is.unget();
        bool done = bool(is >> outA[i]);

        if (!done) {
            is.seekg(pos);
            is.setstate(std::ios::failbit);
            return is;
        }
    }

    // skip spaces
    while (bool(is >> c) && isspace(c)) {
    }

    if (c != ')') {
        is.seekg(pos);
        is.setstate(std::ios::failbit);
        return is;
    }

    return is;
}
//=================================================================
