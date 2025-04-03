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

#ifndef PARALLEL_COORDS_ELEMENT_DELETER_H
#define PARALLEL_COORDS_ELEMENT_DELETER_H

#include <talipot/GLInteractor.h>

namespace tlp {

class ParallelCoordsElementDeleter : public GLInteractorComponent {
  public:
    ParallelCoordsElementDeleter() = default;
    ~ParallelCoordsElementDeleter() override = default;
    bool eventFilter(QObject *, QEvent *) override;
};
}

#endif // PARALLEL_COORDS_ELEMENT_DELETER_H
