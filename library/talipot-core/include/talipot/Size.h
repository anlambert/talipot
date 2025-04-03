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

#ifndef TALIPOT_SIZE_H
#define TALIPOT_SIZE_H

#include <talipot/config.h>
#include <talipot/Vector.h>

namespace tlp {
class TLP_SCOPE Size : public Vec3f {

  public:
    Size(float s = 1) : Vec3f(s) {}

    Size(float w, float h, float d = 1) : Vec3f(w, h, d) {}

    Size(const Vec3f &v) : Vec3f(v) {}

    Size(const Vec2f &v, float d = 1) : Vec3f(v, d) {}

    float width() const {
        return (*this)[0];
    }

    float height() const {
        return (*this)[1];
    }

    float depth() const {
        return (*this)[2];
    }

    float &width() {
        return (*this)[0];
    }

    float &height() {
        return (*this)[1];
    }

    float &depth() {
        return (*this)[2];
    }

    void setW(const float width) {
        (*this)[0] = width;
    }

    void setH(const float height) {
        (*this)[1] = height;
    }

    void setD(const float depth) {
        (*this)[2] = depth;
    }

    float getW() const {
        return (*this)[0];
    }

    float getH() const {
        return (*this)[1];
    }

    float getD() const {
        return (*this)[2];
    }
};
}

namespace std {

template <>
struct hash<tlp::Size> {
    size_t operator()(const tlp::Size &s) const {
        return hash_vector(s);
    }
};

}

#endif // TALIPOT_SIZE_H
