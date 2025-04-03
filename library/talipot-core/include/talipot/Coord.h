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

#ifndef TALIPOT_COORD_H
#define TALIPOT_COORD_H

#include <talipot/config.h>
#include <talipot/Vector.h>

namespace tlp {

class TLP_SCOPE Coord : public Vec3f {

  public:
    Coord(float v = 0) : Vec3f(v) {}

    Coord(float x, float y, float z = 0) : Vec3f(x, y, z) {}

    Coord(const Vec3f &v) : Vec3f(v) {}

    Coord(const Vec2f &v, float z = 0) : Vec3f(v, z) {}

    // convenient accessor for coordinates
    float x() const {
        return (*this)[0];
    }

    float y() const {
        return (*this)[1];
    }

    float z() const {
        return (*this)[2];
    }

    float w() const {
        return (*this)[3];
    }

    float &x() {
        return (*this)[0];
    }

    float &y() {
        return (*this)[1];
    }

    float &z() {
        return (*this)[2];
    }

    float &w() {
        return (*this)[3];
    }

    void setX(float xx) {
        x() = xx;
    }
    void setY(float yy) {
        y() = yy;
    }
    void setZ(float zz) {
        z() = zz;
    }

    float getX() const {
        return x();
    }
    float getY() const {
        return y();
    }
    float getZ() const {
        return z();
    }
};

}

namespace std {

template <>
struct hash<tlp::Coord> {
    size_t operator()(const tlp::Coord &c) const {
        return hash_vector(c);
    }
};

}

#endif // TALIPOT_COORD_H
