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

#ifndef UNIFORM_DEFORMATION_SCREEN_H
#define UNIFORM_DEFORMATION_SCREEN_H

#include <talipot/Matrix.h>
#include "ScreenFunction.h"

class UniformDeformationScreen : public ScreenFunction {
  public:
    tlp::Vec2f project(const tlp::Vec2f &) const override;
    tlp::Vec2f unproject(const tlp::Vec2f &) const override;
    void setTranslation(double x, double y);
    void getTranslation(double &x, double &y) const;
    void setZoom(double zoom);
    double getZoom() const;
    UniformDeformationScreen();

  private:
    void updateMatrix();

  private:
    double _tx;
    double _ty;
    double _zoom;
    tlp::Matrix<double, 3> _mat;
    tlp::Matrix<double, 3> _invMat;
};

#endif // UNIFORM_DEFORMATION_SCREEN_H
