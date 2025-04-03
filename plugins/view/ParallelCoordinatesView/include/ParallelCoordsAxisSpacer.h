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

#ifndef PARALLEL_COORDS_AXIS_SPACER_H
#define PARALLEL_COORDS_AXIS_SPACER_H

#include <talipot/GLInteractor.h>

namespace tlp {

class ParallelCoordinatesView;
class ParallelAxis;

class ParallelCoordsAxisSpacer : public GLInteractorComponent {

  public:
    ParallelCoordsAxisSpacer();
    bool eventFilter(QObject *, QEvent *) override;
    bool draw(GlWidget *glWidget) override;
    void viewChanged(View *view) override;

  private:
    void determineAxisRegionAtPos(const Coord &pos);
    void drawCurrentEditedRegionMarker();

    ParallelCoordinatesView *parallelView;
    ParallelAxis *selectedAxis;
    std::pair<ParallelAxis *, ParallelAxis *> neighborsAxis;
    int x, y;
    bool dragStarted;
};
}

#endif // PARALLEL_COORDS_AXIS_SPACER_H
