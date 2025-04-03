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

#include "SquareLayout.h"

using namespace std;
using namespace tlp;

//==============================================================
SquareLayout::SquareLayout(uint width) : _width(width) {}
//==============================================================
uint SquareLayout::unproject(const Vec2i &point) const {

    int x = point[0] + _width / 2;
    int y = point[1] + _width / 2;

    if (x > int(_width)) {
        return UINT_MAX;
    }

    if (y > int(_width)) {
        return UINT_MAX;
    }

    return uint(y) * _width + uint(x);
}
//==============================================================
Vec2i SquareLayout::project(const uint id) const {
    Vec2i point;
    point[0] = id % _width - _width / 2;
    point[1] = id / _width - _width / 2;
    return point;
}
