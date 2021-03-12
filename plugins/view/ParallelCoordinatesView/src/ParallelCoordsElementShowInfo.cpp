/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ParallelCoordsElementShowInfo.h"
#include "ParallelCoordinatesView.h"

using namespace std;

namespace tlp {

bool ParallelCoordsElementShowInfo::pick(int x, int y, SelectedEntity &selectedEntity) {
  auto *parallelView = static_cast<ParallelCoordinatesView *>(view());
  return parallelView->getDataUnderPointerProperties(x, y, selectedEntity);
}
}
