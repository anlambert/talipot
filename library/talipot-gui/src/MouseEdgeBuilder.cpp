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

#include <QMouseEvent>

#include <talipot/GlWidget.h>
#include <talipot/GlLine.h>
#include <talipot/MouseEdgeBuilder.h>

using namespace std;
using namespace tlp;

MouseEdgeBuilder::MouseEdgeBuilder()
    : _source(node()), _started(false), _graph(nullptr), _layoutProperty(nullptr),
      glWidget(nullptr) {}

bool MouseEdgeBuilder::eventFilter(QObject *widget, QEvent *e) {

  if (glWidget == nullptr) {
    glWidget = dynamic_cast<GlWidget *>(widget);
  }

  assert(glWidget);

  if (e->type() == QEvent::MouseButtonPress) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    SelectedEntity selectedEntity;
    GlGraphInputData *inputData = glWidget->inputData();
    Graph *_graph = inputData->graph();

    LayoutProperty *mLayout = inputData->layout();

    if (qMouseEv->buttons() == Qt::LeftButton) {
      if (!_started) {
        bool result =
            glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity);

        if (result && (selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED)) {
          _started = true;
          initObserver(_graph);
          _source = node(selectedEntity.getComplexEntityId());
          _curPos = _startPos = mLayout->getNodeValue(_source);
          return true;
        }

        return false;
      } else {
        bool result =
            glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity);

        if (result && (selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED)) {
          Observable::holdObservers();
          clearObserver();
          // allow to undo
          _graph->push();
          addLink(_source, node(selectedEntity.getComplexEntityId()));
          // edge finished. clear _source and _started
          _source = node();
          _started = false;
          Observable::unholdObservers();

        } else {
          Coord point = {glWidget->width() - float(qMouseEv->pos().x()),
                         float(qMouseEv->pos().y())};
          _bends.push_back(glWidget->scene()->getGraphCamera().viewportTo3DWorld(
              glWidget->screenToViewport(point)));
          glWidget->redraw();
        }
      }

      return true;
    }

    if (qMouseEv->buttons() == Qt::MiddleButton) {
      _bends.clear();
      _started = false;
      _source = node();
      clearObserver();
      glWidget->draw();
      return true;
    }
  }

  if (e->type() == QEvent::MouseMove) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (!_started) {
      SelectedEntity selectedEntity;
      bool hoveringOverNode =
          glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
          selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED;

      if (!hoveringOverNode) {
        glWidget->setCursor(QCursor(Qt::ArrowCursor));
        return false;
      } else {
        glWidget->setCursor(QCursor(Qt::CrossCursor));
      }
    } else {
      SelectedEntity selectedEntity;

      if (glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
          selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
        glWidget->setCursor(QCursor(Qt::CrossCursor));
      } else {
        glWidget->setCursor(QCursor(Qt::ArrowCursor));
      }

      Coord point = Coord(glWidget->width() - qMouseEv->pos().x(), qMouseEv->pos().y());
      point =
          glWidget->scene()->getGraphCamera().viewportTo3DWorld(glWidget->screenToViewport(point));
      _curPos.set(point[0], point[1], point[2]);
      glWidget->redraw();
    }

    return true;
  }

  return false;
}

bool MouseEdgeBuilder::draw(GlWidget *glWidget) {
  if (!_started) {
    return false;
  }

  glDisable(GL_STENCIL_TEST);
  glWidget->scene()->getGraphCamera().initGl();
  vector<Coord> lineVertices;
  lineVertices.push_back(_startPos);
  lineVertices.insert(lineVertices.end(), _bends.begin(), _bends.end());
  lineVertices.push_back(_curPos);
  vector<Color> lineColors;
  lineColors.resize(lineVertices.size(), Color(255, 0, 0, 255));
  GlLine editedEdge(lineVertices, lineColors);
  editedEdge.draw(0, nullptr);
  return true;
}

void MouseEdgeBuilder::initObserver(Graph *newGraph) {
  newGraph->addListener(this);
  _graph = newGraph;
  _layoutProperty = newGraph->getLayoutProperty("viewLayout");
  _layoutProperty->addListener(this);
}

void MouseEdgeBuilder::clearObserver() {
  if (_graph) {
    _graph->removeListener(this);
  }

  _graph = nullptr;

  if (_layoutProperty) {
    _layoutProperty->removeListener(this);
  }

  _layoutProperty = nullptr;
}

void MouseEdgeBuilder::treatEvent(const Event &evt) {
  if (typeid(evt) == typeid(GraphEvent)) {
    const auto *graphEvent = dynamic_cast<const GraphEvent *>(&evt);

    if (graphEvent && graphEvent->getType() == GraphEvent::TLP_DEL_NODE &&
        graphEvent->getNode() == _source) {
      _bends.clear();
      _started = false;
      clearObserver();
    }
  } else {
    const auto *propertyEvent = dynamic_cast<const PropertyEvent *>(&evt);

    if (propertyEvent && propertyEvent->getType() == PropertyEvent::TLP_AFTER_SET_NODE_VALUE &&
        propertyEvent->getNode() == _source && propertyEvent->getProperty() == _layoutProperty) {
      _startPos = _layoutProperty->getNodeValue(_source);
    }
  }
}

void MouseEdgeBuilder::clear() {
  if (glWidget) {
    glWidget->setCursor(QCursor());
  }
}

void MouseEdgeBuilder::addLink(const node &source, const node &target) {
  assert(glWidget);
  Graph *g = glWidget->inputData()->graph();

  LayoutProperty *mLayout = glWidget->inputData()->layout();
  edge newEdge = g->addEdge(source, target);
  mLayout->setEdgeValue(newEdge, bends());
  _bends.clear();
}
