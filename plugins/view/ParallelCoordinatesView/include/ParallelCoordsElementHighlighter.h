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

#ifndef PARALLEL_COORDS_ELEMENT_HIGHLIGHTER_H
#define PARALLEL_COORDS_ELEMENT_HIGHLIGHTER_H

#include "ParallelCoordsGlEntitiesSelector.h"

namespace tlp {

class ParallelCoordsElementHighlighter : public ParallelCoordsGlEntitiesSelector {
  public:
    ParallelCoordsElementHighlighter(Qt::MouseButton button = Qt::LeftButton,
                                     Qt::KeyboardModifier modifier = Qt::NoModifier)
        : ParallelCoordsGlEntitiesSelector(button, modifier) {}
    ~ParallelCoordsElementHighlighter() override;
    void unselectAllEntitiesHandler(ParallelCoordinatesView *parallelView) override;
    void selectedEntitiesHandler(ParallelCoordinatesView *parallelView, int x, int y,
                                 bool addToSelection) override;
    void selectedEntitiesHandler(ParallelCoordinatesView *parallelView, int x, int y, int w, int h,
                                 bool addToSelection) override;
};
}

#endif // PARALLEL_COORDS_ELEMENT_HIGHLIGHTER_H
