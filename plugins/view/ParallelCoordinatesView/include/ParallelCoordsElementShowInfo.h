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

#ifndef PARALLEL_COORDS_ELEMENT_SHOW_INFO_H
#define PARALLEL_COORDS_ELEMENT_SHOW_INFO_H

#include <talipot/MouseShowElementInfo.h>

namespace tlp {

class ParallelCoordsElementShowInfo : public MouseShowElementInfo {
  public:
    bool pick(int x, int y, SelectedEntity &selectedEntity) override;
};
}
#endif // PARALLEL_COORDS_ELEMENT_SHOW_INFO_H
