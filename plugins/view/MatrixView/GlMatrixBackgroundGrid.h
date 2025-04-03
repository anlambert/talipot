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

#ifndef GL_MATRIX_BACKGROUND_GRID_H
#define GL_MATRIX_BACKGROUND_GRID_H

#include <talipot/GlEntity.h>
namespace tlp {
class MatrixView;

class GlMatrixBackgroundGrid : public tlp::GlEntity {
  public:
    GlMatrixBackgroundGrid(MatrixView *view);
    tlp::BoundingBox getBoundingBox() override;
    void draw(float lod, tlp::Camera *camera) override;

    void getXML(std::string &) override {}
    void setWithXML(const std::string &, uint &) override {}

  private:
    MatrixView *_view;
};
}
#endif // GL_MATRIX_BACKGROUND_GRID_H
