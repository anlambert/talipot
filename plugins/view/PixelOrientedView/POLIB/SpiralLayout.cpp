/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "SpiralLayout.h"

using namespace std;
using namespace tlp;

namespace {
inline double eq2D2(const double c) {
  return (4. - sqrt(16. - 16. * c)) / -8.;
}
}

//==============================================================
uint SpiralLayout::unproject(const Vec2i &point) const {
  int x = point[0];
  int y = point[1];

  if (x == 0 && y == 0) {
    return 0;
  }

  int c = std::max(x, y);
  c = std::max(c, std::max(-x, -y));
  uint t1 = (c - 1), t2 = t1 << 2;
  uint p = t2 * t1 + t2 + 1; // p equal the number of if id in the disk

  if (x == c && y == c) {    // last point
    p += 8 * c - 1;
  } else if (x == c) {       // right
    p += 1 * c - y - 1;
  } else if (y == -c) {      // bottom
    p += 3 * c - x - 1;
  } else if (x == -c) {      // left
    p += 5 * c + y - 1;
  } else if (y == c) {       // top
    p += 7 * c + x - 1;
  }

  return p;
}
//==============================================================
Vec2i SpiralLayout::project(const uint _id) const {
  Vec2i point;

  if (_id == 0) {
    point[0] = 0;
    point[1] = 0;
  } else {
    int c = 0;

    if (_id == 1) {
      c = 1;
    } else {
      // c = (int)ceil(eq2D(4., 4., 1. - float(_id)));
      c = int(ceil(eq2D2(1. - double(_id))));
    }

    uint t1 = (c - 1), t2 = t1 << 2;
    int p = _id - (t1 * t2 + t2 + 1);
    unsigned char k = p / (2 * c);

    switch (k) {
    case 0:
      point[0] = c;
      point[1] = c - 1 - p;
      break;

    case 1:
      point[0] = c - (p - 2 * c) - 1;
      point[1] = -c;
      break;

    case 2:
      point[0] = -c;
      point[1] = -c + (p - 4 * c) + 1;
      break;

    case 3:
    case 4:
      point[0] = -c + (p - 6 * c) + 1;
      point[1] = c;
      break;

    default:
      cerr << "[error] : " << int(k) << endl;
    }
  }

  return point;
}
