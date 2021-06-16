/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef SPIRAL_LAYOUT_H
#define SPIRAL_LAYOUT_H

#include <string>
#include "LayoutFunction.h"

class SpiralLayout : public LayoutFunction {
public:
  tlp::Vec2i project(const uint id) const override;
  uint unproject(const tlp::Vec2i &) const override;
};

#endif // SPIRAL_LAYOUT_H
