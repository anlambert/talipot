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

#ifndef ORIENTABLE_CONSTANTS_H
#define ORIENTABLE_CONSTANTS_H

enum orientationType {
    ORI_DEFAULT = 0,
    ORI_INVERSION_HORIZONTAL = 1,
    ORI_INVERSION_VERTICAL = 2,
    ORI_INVERSION_Z = 4,
    ORI_ROTATION_XY = 8
};

enum { UP_DOWN = 0, DOWN_UP = 1, RIGHT_LEFT = 2, LEFT_RIGHT = 3 };

orientationType maskFromPanel(bool reverseOrientation, bool reverseDirection);

#endif // ORIENTABLE_CONSTANTS_H
