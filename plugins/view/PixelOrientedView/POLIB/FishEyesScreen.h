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

#ifndef FISH_EYES_SCREEN_H
#define FISH_EYES_SCREEN_H

#include "ScreenFunction.h"

class FishEyesScreen : public ScreenFunction {

  public:
    FishEyesScreen();
    tlp::Vec2f project(const tlp::Vec2f &) const override;
    tlp::Vec2f unproject(const tlp::Vec2f &) const override;
    void setCenter(double x, double y);
    void setRadius(double r);
    double getRadius() const;
    void setHeight(double h);

  private:
    double R;
    double k;
    tlp::Vec2f fisheyesCenter;
};
#endif // FISH_EYES_SCREEN_H
