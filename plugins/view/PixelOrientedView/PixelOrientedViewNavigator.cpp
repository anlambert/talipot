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

#include <talipot/QtGlSceneZoomAndPanAnimator.h>

#include "PixelOrientedViewNavigator.h"

#include <QMouseEvent>

using namespace std;

namespace tlp {

PixelOrientedViewNavigator::PixelOrientedViewNavigator()
    : pixelView(nullptr), selectedOverview(nullptr) {}

PixelOrientedViewNavigator::~PixelOrientedViewNavigator() = default;

void PixelOrientedViewNavigator::viewChanged(View *view) {
  pixelView = static_cast<PixelOrientedView *>(view);
}

bool PixelOrientedViewNavigator::eventFilter(QObject *widget, QEvent *e) {

  if (e->type() != QEvent::MouseButtonDblClick && e->type() != QEvent::MouseMove) {
    return false;
  }

  auto *glWidget = static_cast<GlWidget *>(widget);

  if (!glWidget->hasMouseTracking()) {
    glWidget->setMouseTracking(true);
  }

  if (!pixelView->smallMultiplesViewSet() && !pixelView->interactorsEnabled()) {
    pixelView->toggleInteractors(true);
  }

  if (pixelView->getOverviews().empty()) {
    return false;
  }

  if (e->type() == QEvent::MouseMove && pixelView->smallMultiplesViewSet()) {
    auto *me = static_cast<QMouseEvent *>(e);
    float x = glWidget->width() - me->pos().x();
    float y = me->pos().y();
    Coord screenCoords = {x, y};
    Coord sceneCoords = glWidget->scene()->getGraphCamera().viewportTo3DWorld(
        glWidget->screenToViewport(screenCoords));
    PixelOrientedOverview *overviewUnderPointer = getOverviewUnderPointer(sceneCoords);

    if (overviewUnderPointer != nullptr && overviewUnderPointer != selectedOverview) {
      selectedOverview = overviewUnderPointer;
    }

    return true;
  } else if (e->type() == QEvent::MouseButtonDblClick) {
    if (selectedOverview != nullptr && !selectedOverview->overviewGenerated()) {
      pixelView->generatePixelOverview(selectedOverview, glWidget);
      glWidget->draw();
    } else if (selectedOverview != nullptr && pixelView->smallMultiplesViewSet()) {
      QtGlSceneZoomAndPanAnimator zoomAndPanAnimator(glWidget, selectedOverview->getBoundingBox());
      zoomAndPanAnimator.animateZoomAndPan();
      pixelView->switchFromSmallMultiplesToDetailView(selectedOverview);
      selectedOverview = nullptr;
    } else if (!pixelView->smallMultiplesViewSet() && pixelView->getOverviews().size() > 1) {
      pixelView->switchFromDetailViewToSmallMultiples();
      QtGlSceneZoomAndPanAnimator zoomAndPanAnimator(glWidget,
                                                     pixelView->getSmallMultiplesViewBoundingBox());
      zoomAndPanAnimator.animateZoomAndPan();
      pixelView->centerView();
    }

    return true;
  }

  return false;
}

PixelOrientedOverview *PixelOrientedViewNavigator::getOverviewUnderPointer(Coord &sceneCoords) {
  PixelOrientedOverview *ret = nullptr;
  vector<PixelOrientedOverview *> overviews = pixelView->getOverviews();

  for (auto *poo : overviews) {
    BoundingBox overviewBB = poo->getBoundingBox();

    if (sceneCoords.getX() >= overviewBB[0][0] && sceneCoords.getX() <= overviewBB[1][0] &&
        sceneCoords.getY() >= overviewBB[0][1] && sceneCoords.getY() <= overviewBB[1][1]) {
      ret = poo;
      break;
    }
  }

  return ret;
}
}
