/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <climits>

#include "SquareLayout.h"

using namespace std;
using namespace tlp;

namespace pocore {
//==============================================================
SquareLayout::SquareLayout(unsigned int width) : _width(width) {}
//==============================================================
unsigned int SquareLayout::unproject(const Vector<int, 2> &point) const {

  int x = point[0] + _width / 2;
  int y = point[1] + _width / 2;

  if (x > int(_width))
    return UINT_MAX;

  if (y > int(_width))
    return UINT_MAX;

  return uint(y) * _width + uint(x);
}
//==============================================================
Vector<int, 2> SquareLayout::project(const unsigned int id) const {
  Vector<int, 2> point;
  point[0] = id % _width - _width / 2;
  point[1] = id / _width - _width / 2;
  return point;
}
} // namespace pocore
