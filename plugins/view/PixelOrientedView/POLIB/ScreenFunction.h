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

#ifndef SCREEN_FUNCTION_H
#define SCREEN_FUNCTION_H

#include <talipot/Vector.h>

class ScreenFunction {

  public:
    virtual ~ScreenFunction() = default;
    virtual tlp::Vec2f project(const tlp::Vec2f &) const = 0;
    virtual tlp::Vec2f unproject(const tlp::Vec2f &) const = 0;
};

#endif // SCREEN_FUNCTION_H
