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

#include "ZOrderLayout.h"

using namespace std;

//===============================================================
namespace {
inline tlp::Vec2i zorderPoint(const uint key, const unsigned char order) {
    tlp::Vec2i point;
    point.fill(0);

    for (char i = order - 1; i >= 0; --i) {
        // read two bits;
        unsigned char bits = (key >> (i << 1)) & 3;
        point[1] += (bits & 1) << i;
        point[0] += ((bits >> 1)) << i;
    }

    return point;
}

inline uint zorderKey(const tlp::Vec2i &p, const unsigned char order) {
    uint key = 0;

    for (char i = order - 1; i >= 0; --i) {
        // read two bits;
        unsigned char bits = (p[1] >> i) & 1;
        bits += ((p[0] >> i) & 1) << 1;
        key += bits << (i << 1);
    }

    return key;
}
}
//===============================================================
ZorderLayout::ZorderLayout(unsigned char order) : order(order) {
    shift = int(rint(sqrt(pow(4., order)) / 2.));
}
//==============================================================
uint ZorderLayout::unproject(const tlp::Vec2i &point) const {
    tlp::Vec2i p;

    if (point[0] <= -shift || point[0] >= shift) {
        return UINT_MAX;
    }

    if (point[1] <= -shift || point[1] >= shift) {
        return UINT_MAX;
    }

    p[0] = point[0] + shift;
    p[1] = point[1] + shift;
    return zorderKey(p, order);
}
//==============================================================
tlp::Vec2i ZorderLayout::project(const uint id) const {
    return zorderPoint(id, order) -= shift;
}
