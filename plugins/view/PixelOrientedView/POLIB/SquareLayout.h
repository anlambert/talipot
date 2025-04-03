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

#ifndef SQUARE_LAYOUT_H
#define SQUARE_LAYOUT_H

#include "LayoutFunction.h"

class SquareLayout : public LayoutFunction {
  public:
    SquareLayout(uint width);
    tlp::Vec2i project(const uint id) const override;
    uint unproject(const tlp::Vec2i &) const override;

  private:
    uint _width;
};

#endif // SQUARE_LAYOUT_H
