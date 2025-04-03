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

#ifndef LINEAR_MAPPING_COLOR_H
#define LINEAR_MAPPING_COLOR_H

#include <talipot/Color.h>
#include "ColorFunction.h"

class LinearMappingColor : public ColorFunction {
  public:
    LinearMappingColor(double min, double max);
    tlp::Color getColor(double value, uint) const override;

  private:
    double _min;
    double _max;
    tlp::Color startColor;
    tlp::Color endColor;
};
#endif // LINEAR_MAPPING_COLOR_H
