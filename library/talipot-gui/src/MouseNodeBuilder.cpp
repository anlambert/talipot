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

#include "talipot/MouseNodeBuilder.h"

#include <QMouseEvent>

#include <talipot/GlWidget.h>

using namespace tlp;
using namespace std;

bool MouseNodeBuilder::eventFilter(QObject *widget, QEvent *e) {
  auto *qMouseEv = dynamic_cast<QMouseEvent *>(e);

  if (qMouseEv != nullptr) {
    SelectedEntity selectedEntity;

    if (glWidget == nullptr) {
      glWidget = static_cast<GlWidget *>(widget);
    }

    if (e->type() == QEvent::MouseMove) {
      if (glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
          selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
        glWidget->setCursor(Qt::ForbiddenCursor);
      } else {
        glWidget->setCursor(Qt::ArrowCursor);
      }

      return false;
    }

    if (e->type() == _eventType) {
      if (qMouseEv->button() == Qt::LeftButton) {
        if (glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
            selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
          return true;
        }

        GlGraphInputData *inputData = glWidget->inputData();
        Graph *_graph = inputData->graph();
        LayoutProperty *mLayout = inputData->layout();
        // allow to undo
        _graph->push();
        Observable::holdObservers();
        node newNode;
        newNode = _graph->addNode();
        Coord point =
            Coord(glWidget->width() - float(qMouseEv->pos().x()), float(qMouseEv->pos().y()));
        point =
            glWidget->scene()->graphCamera().viewportTo3DWorld(glWidget->screenToViewport(point));

        // This code is here to block z coordinate to 0 when we are in "2D mode"
        Coord cameraDirection = glWidget->scene()->graphCamera().getEyes() -
                                glWidget->scene()->graphCamera().getCenter();

        if (cameraDirection[0] == 0 && cameraDirection[1] == 0) {
          point[2] = 0;
        }

        (*mLayout)[newNode] = point;
        Observable::unholdObservers();

        return true;
      }
    }
  }

  return false;
}

void MouseNodeBuilder::clear() {
  if (glWidget) {
    glWidget->setCursor(QCursor());
  }
}
