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

#ifndef LOGARITHMIC_MAPPING_COLOR_H
#define LOGARITHMIC_MAPPING_COLOR_H

#include <talipot/Color.h>

#include "LinearMappingColor.h"

class LogarithmicMappingColor : public LinearMappingColor {
  public:
    LogarithmicMappingColor(double min, double max);
    tlp::Color getColor(double value, uint) const override;
};

#endif // LOGARITHMIC_MAPPING_COLOR_H
