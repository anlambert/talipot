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

#include "PathFinderComponent.h"

#include <talipot/GlGraph.h>
#include <talipot/GlView.h>

#include "PathFinder.h"

#include <QApplication>
#include <QMessageBox>
#include <QMouseEvent>

using namespace std;
using namespace tlp;

PathFinderComponent::PathFinderComponent(PathFinder *parent) : parent(parent) {}

PathFinderComponent::~PathFinderComponent() = default;

bool PathFinderComponent::eventFilter(QObject *obj, QEvent *event) {
  auto *qMouseEv = static_cast<QMouseEvent *>(event);
  auto *glw = dynamic_cast<GlWidget *>(obj);

  if (glw == nullptr) {
    return false;
  }

  if (event->type() == QEvent::MouseMove) {
    SelectedEntity entity;
    if (glw->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), entity) &&
        entity.getEntityType() == SelectedEntity::NODE_SELECTED) {
      tmp.id = entity.getGraphElementId();
      glw->setCursor(Qt::CrossCursor);
      return true;
    } else {
      tmp = node();
      glw->setCursor(Qt::ArrowCursor);
    }
  } else if (event->type() == QEvent::MouseButtonDblClick && qMouseEv->button() == Qt::LeftButton) {
    // double click will deselect all
    Observable::holdObservers();

    BooleanProperty *selectionProperty = glw->inputData()->selection();
    selectionProperty->setAllNodeValue(false);
    selectionProperty->setAllEdgeValue(false);

    if (tmp.isValid()) {
      // double click on a node
      // user is selecting a new path source
      src = tmp;
      // select it
      selectionProperty->setNodeValue(src, true);
    } else {
      src = node();
    }

    // invalidate the path target
    tgt = node();

    Observable::unholdObservers();

    return true;
  } else if (event->type() == QEvent::MouseButtonPress && qMouseEv->button() == Qt::LeftButton &&
             tmp.isValid()) {
    if (!src.isValid()) {
      // user can select the path source with a simple click
      // if no source already exists
      Observable::holdObservers();

      BooleanProperty *selectionProperty = glw->inputData()->selection();
      selectionProperty->setAllNodeValue(false);
      selectionProperty->setAllEdgeValue(false);
      // select it
      selectionProperty->setNodeValue(src = tmp, true);

      Observable::unholdObservers();
    } else {
      // as the path source already exists
      // we assume that the user is selecting a new path target
      tgt = tmp;
      // but we wait a bit to ensure the current event does not belong to a
      // QEvent::MouseButtonDblClick event
      obj->startTimer(QApplication::doubleClickInterval() + 5);
    }
    return true;
  } else if (event->type() == QEvent::Timer) {
    obj->killTimer(static_cast<QTimerEvent *>(event)->timerId());
    // tgt could have been invalidate by a QEvent::MouseButtonDblClick event
    if (tgt.isValid()) {
      Observable::holdObservers();

      BooleanProperty *selectionProperty = glw->inputData()->selection();
      selectionProperty->setAllNodeValue(false);
      selectionProperty->setAllEdgeValue(false);
      selectPath(glw, glw->scene()->glGraph()->graph());

      Observable::unholdObservers();

      glw->redraw();
    }
    return true;
  }

  return false;
}

void PathFinderComponent::selectPath(GlWidget *glWidget, Graph *graph) {
  GlGraphInputData *inputData = glWidget->inputData();

  BooleanProperty *selection = inputData->selection();

  if (src.isValid() && tgt.isValid()) { // We only select a path if source and target are valid
    Observable::holdObservers();
    DoubleProperty *weights = nullptr;
    string weightsMetricName = parent->getWeightMetricName();

    if (weightsMetricName != NO_METRIC && graph->existProperty(weightsMetricName)) {
      PropertyInterface *prop = graph->getProperty(weightsMetricName);

      if (prop && prop->getTypename() == "double") {
        weights = graph->getDoubleProperty(weightsMetricName);
      }
    }

    bool pathFound = PathAlgorithm::computePath(
        graph, parent->getPathsType(), parent->getEdgeOrientation(), src, tgt, selection, weights);
    Observable::unholdObservers();

    if (!pathFound) {
      selection->setAllNodeValue(false);
      selection->setAllEdgeValue(false);
      selection->setNodeValue(src, true);
      QMessageBox::warning(nullptr, "Path finder",
                           "A path between the selected nodes cannot be found.");
    }
  } else if (src.isValid()) {
    selection->setNodeValue(src, true);
  }
}

void PathFinderComponent::clear() {
  auto *glView = static_cast<GlView *>(view());
  glView->glWidget()->setCursor(QCursor());
}
