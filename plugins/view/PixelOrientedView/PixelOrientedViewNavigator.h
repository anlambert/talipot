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

#ifndef PIXEL_ORIENTED_VIEW_NAVIGATOR_H
#define PIXEL_ORIENTED_VIEW_NAVIGATOR_H

#include <talipot/GLInteractor.h>

#include "PixelOrientedOverview.h"
#include "PixelOrientedView.h"

namespace tlp {

class PixelOrientedViewNavigator : public GLInteractorComponent {

  public:
    PixelOrientedViewNavigator();
    ~PixelOrientedViewNavigator() override;

    bool eventFilter(QObject *, QEvent *) override;
    void viewChanged(View *view) override;

  private:
    PixelOrientedOverview *getOverviewUnderPointer(Coord &sceneCoord);

    PixelOrientedView *pixelView;
    PixelOrientedOverview *selectedOverview;
};
}

#endif // PIXEL_ORIENTED_VIEW_NAVIGATOR_H
