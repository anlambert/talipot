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

#ifndef COLOR_FUNCTION_H
#define COLOR_FUNCTION_H

#include <talipot/Color.h>

class ColorFunction {
  public:
    virtual ~ColorFunction() = default;
    virtual tlp::Color getColor(double value, uint itemId) const = 0;
};

#endif // COLOR_FUNCTION_H
