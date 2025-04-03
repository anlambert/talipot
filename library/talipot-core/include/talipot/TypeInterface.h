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

#ifndef TALIPOT_TYPE_INTERFACE_H
#define TALIPOT_TYPE_INTERFACE_H

#include <iostream>

namespace tlp {
template <typename T>
class TypeInterface {
  public:
    typedef T RealType;
    static RealType undefinedValue() {
        return T();
    }
    static RealType defaultValue() {
        return T();
    }

    static uint valueSize() {
        return sizeof(T);
    }

    static void write(std::ostream &, const RealType &) {}
    static void writeb(std::ostream &oss, const RealType &v) {
        oss.write(reinterpret_cast<const char *>(&v), sizeof(v));
    }

    static bool read(std::istream &, RealType &) {
        return false;
    }
    static bool readb(std::istream &iss, RealType &v) {
        return bool(iss.read(reinterpret_cast<char *>(&v), sizeof(v)));
    }

    static std::string toString(const RealType &) {
        return "";
    }
    static bool fromString(RealType &, const std::string &) {
        return false;
    }
};
}

#define REAL_TYPE(TYPE) typename TYPE::RealType

#endif // TALIPOT_TYPE_INTERFACE_H
