/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ScatterPlot2DView.h"
#include "ScatterPlot2DViewNavigator.h"
#include "ScatterPlot2D.h"

#include <QMouseEvent>

using namespace std;

namespace tlp {

ScatterPlot2DViewNavigator::ScatterPlot2DViewNavigator()
    : scatterPlot2dView(nullptr), selectedScatterPlotOverview(nullptr), glWidget(nullptr) {}

ScatterPlot2DViewNavigator::~ScatterPlot2DViewNavigator() = default;

void ScatterPlot2DViewNavigator::viewChanged(View *view) {
  scatterPlot2dView = static_cast<ScatterPlot2DView *>(view);
}

bool ScatterPlot2DViewNavigator::eventFilter(QObject *widget, QEvent *e) {

  if (glWidget == nullptr) {
    glWidget = static_cast<GlWidget *>(widget);
  }

  if (glWidget != nullptr) {
    if (!glWidget->hasMouseTracking()) {
      glWidget->setMouseTracking(true);
    }

    if (!scatterPlot2dView->matrixViewSet() && !scatterPlot2dView->interactorsEnabled()) {
      scatterPlot2dView->toggleInteractors(true);
    }

    if (e->type() == QEvent::MouseMove && scatterPlot2dView->matrixViewSet()) {
      auto *me = static_cast<QMouseEvent *>(e);
      float x = glWidget->width() - me->pos().x();
      float y = me->pos().y();
      Coord screenCoords = {x, y};
      Coord sceneCoords = glWidget->scene()->graphCamera().viewportTo3DWorld(
          glWidget->screenToViewport(screenCoords));
      selectedScatterPlotOverview = getOverviewUnderPointer(sceneCoords);
      return true;
    } else if (e->type() == QEvent::MouseButtonDblClick) {
      if (selectedScatterPlotOverview != nullptr &&
          !selectedScatterPlotOverview->overviewGenerated()) {
        scatterPlot2dView->generateScatterPlot(selectedScatterPlotOverview, glWidget);
        glWidget->draw();
      } else if (selectedScatterPlotOverview != nullptr && scatterPlot2dView->matrixViewSet()) {
        glWidget->zoomAndPanAnimation(selectedScatterPlotOverview->getBoundingBox());
        scatterPlot2dView->switchFromMatrixToDetailView(selectedScatterPlotOverview, true);
        selectedScatterPlotOverview = nullptr;
      } else if (!scatterPlot2dView->matrixViewSet()) {
        scatterPlot2dView->switchFromDetailViewToMatrixView();
        glWidget->zoomAndPanAnimation(scatterPlot2dView->getMatrixBoundingBox());
      }

      return true;
    }
  }

  return false;
}

ScatterPlot2D *ScatterPlot2DViewNavigator::getOverviewUnderPointer(const Coord &sceneCoords) {
  ScatterPlot2D *ret = nullptr;
  vector<ScatterPlot2D *> overviews = scatterPlot2dView->getSelectedScatterPlots();

  for (auto *overview : overviews) {
    if (!overview) {
      continue;
    }

    BoundingBox overviewBB = overview->getBoundingBox();

    if (sceneCoords.getX() >= overviewBB[0][0] && sceneCoords.getX() <= overviewBB[1][0] &&
        sceneCoords.getY() >= overviewBB[0][1] && sceneCoords.getY() <= overviewBB[1][1]) {
      ret = overview;
      break;
    }
  }

  return ret;
}
}
