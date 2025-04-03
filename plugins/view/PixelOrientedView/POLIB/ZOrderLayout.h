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

#ifndef Z_ORDER_LAYOUT_H
#define Z_ORDER_LAYOUT_H

#include "LayoutFunction.h"

class ZorderLayout : public LayoutFunction {
  public:
    ZorderLayout(unsigned char order);
    tlp::Vec2i project(const uint id) const override;
    uint unproject(const tlp::Vec2i &) const override;

  private:
    unsigned char order;
    int shift;
};
#endif // Z_ORDER_LAYOUT_H
