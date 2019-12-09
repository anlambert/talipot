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

#ifndef HILBERT_LAYOUT_H
#define HILBERT_LAYOUT_H

#include <string>

#include "LayoutFunction.h"

class HilbertLayout : public LayoutFunction {
public:
  HilbertLayout(unsigned char order);
  tlp::Vec2i project(const unsigned int id) const override;
  unsigned int unproject(const tlp::Vec2i &) const override;

private:
  unsigned char order;
  int shift;
};

#endif // HILBERT_LAYOUT_H
