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

#ifndef LAYOUT_FUNCTION_H
#define LAYOUT_FUNCTION_H

#include <talipot/Vector.h>

class LayoutFunction {
  public:
    virtual ~LayoutFunction() = default;
    virtual tlp::Vec2i project(const uint id) const = 0;
    virtual uint unproject(const tlp::Vec2i &) const = 0;
};

#endif // LAYOUT_FUNCTION_H
