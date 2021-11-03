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

#include "HistogramViewNavigator.h"
#include "HistogramView.h"

#include <QMouseEvent>

using namespace std;

namespace tlp {

HistogramViewNavigator::HistogramViewNavigator()
    : histoView(nullptr), selectedHistoOverview(nullptr) {}

HistogramViewNavigator::~HistogramViewNavigator() = default;

void HistogramViewNavigator::viewChanged(View *view) {
  histoView = static_cast<HistogramView *>(view);
}

bool HistogramViewNavigator::eventFilter(QObject *widget, QEvent *e) {

  auto *glWidget = static_cast<GlWidget *>(widget);

  if (!glWidget->hasMouseTracking()) {
    glWidget->setMouseTracking(true);
  }

  if (!histoView->smallMultiplesViewSet() && !histoView->interactorsEnabled()) {
    histoView->toggleInteractors(true);
  }

  if (histoView->getHistograms().size() == 1) {
    return false;
  }

  if (e->type() == QEvent::MouseMove && histoView->smallMultiplesViewSet()) {
    auto *me = static_cast<QMouseEvent *>(e);
    float x = glWidget->width() - me->pos().x();
    float y = me->pos().y();
    Coord screenCoords = {x, y};
    Coord sceneCoords = glWidget->scene()->getGraphCamera().viewportTo3DWorld(
        glWidget->screenToViewport(screenCoords));
    selectedHistoOverview = getOverviewUnderPointer(sceneCoords);
    return true;
  } else if (e->type() == QEvent::MouseButtonDblClick) {
    if (selectedHistoOverview != nullptr && histoView->smallMultiplesViewSet()) {
      QtGlSceneZoomAndPanAnimator zoomAndPanAnimator(glWidget,
                                                     selectedHistoOverview->getBoundingBox());
      zoomAndPanAnimator.animateZoomAndPan();
      histoView->switchFromSmallMultiplesToDetailedView(selectedHistoOverview);
      selectedHistoOverview = nullptr;
    } else if (!histoView->smallMultiplesViewSet()) {
      histoView->switchFromDetailedViewToSmallMultiples();
      QtGlSceneZoomAndPanAnimator zoomAndPanAnimator(glWidget,
                                                     histoView->getSmallMultiplesBoundingBox());
      zoomAndPanAnimator.animateZoomAndPan();
    }

    return true;
  }

  return false;
}

Histogram *HistogramViewNavigator::getOverviewUnderPointer(const Coord &sceneCoords) const {
  Histogram *ret = nullptr;
  vector<Histogram *> overviews = histoView->getHistograms();

  for (auto *h : overviews) {
    BoundingBox overviewBB = h->getBoundingBox();

    if (sceneCoords.getX() >= overviewBB[0][0] && sceneCoords.getX() <= overviewBB[1][0] &&
        sceneCoords.getY() >= overviewBB[0][1] && sceneCoords.getY() <= overviewBB[1][1]) {
      ret = h;
      break;
    }
  }

  return ret;
}
}
