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

#include "ParallelCoordsElementHighlighter.h"
#include "ParallelCoordinatesView.h"

using namespace std;

namespace tlp {

void ParallelCoordsElementHighlighter::unselectAllEntitiesHandler(
    ParallelCoordinatesView *parallelView) {
    parallelView->resetHighlightedElements();
}

void ParallelCoordsElementHighlighter::selectedEntitiesHandler(
    ParallelCoordinatesView *parallelView, int x, int y, bool addToSelection) {
    parallelView->highlightDataUnderPointer(x, y, addToSelection);
}

void ParallelCoordsElementHighlighter::selectedEntitiesHandler(
    ParallelCoordinatesView *parallelView, int x, int y, int w, int h, bool addToSelection) {
    parallelView->highlightDataInRegion(x, y, w, h, addToSelection);
}

ParallelCoordsElementHighlighter::~ParallelCoordsElementHighlighter() {
    auto *parallelView = static_cast<ParallelCoordinatesView *>(view());

    if (parallelView) {
        parallelView->resetHighlightedElements();
    }
}
}
