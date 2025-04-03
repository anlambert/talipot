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

#include "LinearMappingColor.h"

LinearMappingColor::LinearMappingColor(double min, double max) : _min(min), _max(max) {
    startColor[0] = 255;
    startColor[1] = 255;
    startColor[2] = 0;

    endColor[0] = 0;
    endColor[1] = 0;
    endColor[2] = 255;
}

tlp::Color LinearMappingColor::getColor(double value, uint) const {
    double ratio = (value - _min) / (_max - _min);
    tlp::Color color;

    for (uint i = 0; i < 3; ++i) {
        color[i] =
            uchar(double(startColor[i]) + (double(endColor[i]) - double(startColor[i])) * ratio);
    }

    color[3] = 255;
    return color;
}
