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

#ifndef PARALLEL_TOOLS_H
#define PARALLEL_TOOLS_H

#include <talipot/Coord.h>

#include <string>
#include <sstream>
#include <iostream>

namespace tlp {

const std::string DEFAULT_TEXTURE_FILE = ":/parallel_texture.png";

const std::string SLIDER_TEXTURE_NAME = ":/parallel_sliders_texture.png";

// data count threshold from which a progress bar is displayed on screen
// when creating or refreshing the view
const uint PROGRESS_BAR_DISPLAY_NB_DATA_THRESHOLD = 5000;

const uint DEFAULT_LINES_COLOR_ALPHA_VALUE = 200;

const float DEFAULT_AXIS_HEIGHT = 400;

const float DEFAULT_GRAD_WIDTH = 6;

const float DEFAULT_GRAD_HEIGHT = 2;

const float DEFAULT_LABELS_HEIGHT = 15;

const float DEFAULT_BITMAPS_ZOOM_FACTOR = 5;

const float DEFAULT_CAPTION_HEIGHT = 30;

const float DEFAULT_CAPTION_VERTICAl_OFFSET = 20;

template <typename T>
std::string getStringFromNumber(T number) {
    std::ostringstream oss;
    oss.precision(5);
    oss << number;
    return oss.str();
}

float computeABACAngleWithAlKashi(const Coord &A, const Coord &B, const Coord &C);

const int X_ROT = 0;
const int Y_ROT = 1;
const int Z_ROT = 2;
void rotateVector(Coord &vec, float alpha, int rot);

Coord *computeStraightLineIntersection(const Coord line1[2], const Coord line2[2]);
}

#endif // PARALLEL_TOOLS_H
