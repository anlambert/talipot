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

// This implementation of HSI Color Space is from the following paper : "Issues in Visualizing Large
// Databases"
// D. A. Keim, H.-P. Kriegel

#ifndef HSI_COLOR_SPACE_H
#define HSI_COLOR_SPACE_H

#include <talipot/Color.h>

class HSI {

  public:
    HSI(double hue, double saturation, double intensity);
    HSI(const tlp::Color &c);
    double value(double hue_phase) const;
    tlp::Color getColor() const;

    double hue, saturation, intensity;
};

class HSIColorScale {

  public:
    HSIColorScale(const HSI &from, const HSI &to);
    HSI operator[](double f) const;

  private:
    HSI foot;
    double hue_range, sat_range, int_range;
};

#endif // HSI_COLOR_SPACE_H
