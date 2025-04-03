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

#ifndef PARALLEL_COORDS_GL_ENTITIES_SELECTOR_H
#define PARALLEL_COORDS_GL_ENTITIES_SELECTOR_H

#include <QMouseEvent>

#include <talipot/MouseSelector.h>

namespace tlp {

class ParallelCoordinatesView;

class ParallelCoordsGlEntitiesSelector : public MouseSelector {
  public:
    ParallelCoordsGlEntitiesSelector(Qt::MouseButton button = Qt::LeftButton,
                                     Qt::KeyboardModifier modifier = Qt::NoModifier)
        : MouseSelector(button, modifier) {}
    ~ParallelCoordsGlEntitiesSelector() override = default;
    bool eventFilter(QObject *, QEvent *) override;
    virtual void unselectAllEntitiesHandler(ParallelCoordinatesView *parallelView) = 0;
    virtual void selectedEntitiesHandler(ParallelCoordinatesView *parallelView, int x, int y,
                                         bool addToSelection) = 0;
    virtual void selectedEntitiesHandler(ParallelCoordinatesView *parallelView, int x, int y, int w,
                                         int h, bool addToSelection) = 0;
};
}

#endif // PARALLEL_COORDS_GL_ENTITIES_SELECTOR_H
