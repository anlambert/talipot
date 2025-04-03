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

#include "HSIColorSpace.h"

using namespace std;

/***************************** Constructor **********************************/
HSI::HSI(double h, double s, double i) : hue(h < 6 ? h : h - 6), saturation(s), intensity(i) {}

/************************** Conversion from RGB to HSI ***********************/
HSI::HSI(const tlp::Color &c) {
    double red = c[0], green = c[1], blue = c[2];
    double mid = (red + green + blue) / 3.0;
    double mr = red - mid;
    double mg = green - mid;
    double mb = blue - mid;
    double cos_hue = (2.0 * mr - mg - mb) / sqrt((mr * mr + mg * mg + mb * mb) * 6.0);
    intensity = mid + sqrt(2.0 * (mr * mr + mg * mg + mb * mb) / 3.0);
    saturation = 2.0 * (intensity - mid) / intensity;
    hue = acos(cos_hue) * 3.0 / M_PI;

    if (blue > green) {
        hue = 6.0 - hue;
    }
}

/************************** Conversion from HSI to RGB ***********************/
tlp::Color HSI::getColor() const {
    tlp::Color ret;

    if (saturation == 0.0) {
        ret[0] = uchar(intensity * 255);
        ret[1] = uchar(intensity * 255);
        ret[2] = uchar(intensity * 255);
        ret[3] = uchar(255);
    } else {
        ret[0] = uchar(value(hue + 0.0) * 255); // 0 Grad = 0 pi
        ret[1] = uchar(value(hue + 4.0) * 255); // 240 Grad = 4/3 pi
        ret[2] = uchar(value(hue + 2.0) * 255); // 120 Grad = 2/3 pi
        ret[3] = uchar(255);
    }

    return ret;
}

double HSI::value(double hue_phase) const {
    double pure = 0.5 * (1 + cos(hue_phase * M_PI / 3.0));
    return (intensity * (1.0 - saturation * (1.0 - pure)));
}

/************************* Generation of HSIColorScales ************************/
HSIColorScale::HSIColorScale(const HSI &from, const HSI &to)
    : foot(from), hue_range(to.hue - from.hue), sat_range(to.saturation - from.saturation),
      int_range(to.intensity - from.intensity) {
    if (hue_range < 0.0) {
        hue_range += 6.0;
    }
}

HSI HSIColorScale::operator[](double f) const {
    return HSI(hue_range * f + foot.hue, sat_range * f + foot.saturation,
               int_range * f + foot.intensity);
}
