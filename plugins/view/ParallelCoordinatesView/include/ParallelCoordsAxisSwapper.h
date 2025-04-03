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

#ifndef PARALLEL_COORDS_AXIS_SWAPPER_H
#define PARALLEL_COORDS_AXIS_SWAPPER_H

#include <talipot/GLInteractor.h>
#include <talipot/Coord.h>

namespace tlp {

class ParallelAxis;
class ParallelCoordinatesView;

class ParallelCoordsAxisSwapper : public GLInteractorComponent {

  public:
    ParallelCoordsAxisSwapper();
    ~ParallelCoordsAxisSwapper() override;
    bool eventFilter(QObject *, QEvent *) override;
    bool draw(GlWidget *glWidget) override;
    void viewChanged(View *view) override;

  private:
    ParallelCoordinatesView *parallelView;
    ParallelAxis *selectedAxis;
    ParallelAxis *otherAxisToSwap;
    Coord initialSelectedAxisCoord;
    float initialSelectedAxisRotAngle;
    bool dragStarted;
    int x, y;
    bool mouseMove;
    bool axisSwapStarted;
};
}

#endif // PARALLEL_COORDS_AXIS_SWAPPER_H
