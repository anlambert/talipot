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

#include "Orientation.h"

//====================================================================
orientationType maskFromPanel(bool reverseOrientation, bool reverseDirection) {
    if (reverseOrientation) {
        if (reverseDirection) {
            return orientationType(ORI_ROTATION_XY | ORI_INVERSION_HORIZONTAL);
        } else {
            return ORI_ROTATION_XY;
        }
    } else if (reverseDirection) {
        return ORI_INVERSION_VERTICAL;
    } else {
        return ORI_DEFAULT;
    }
}
