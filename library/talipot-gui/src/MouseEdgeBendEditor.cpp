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
#include <talipot/MouseEdgeBendEditor.h>
#include <talipot/GlGraph.h>
#include <talipot/GlyphManager.h>

using namespace tlp;
using namespace std;

//========================================================================================
MouseEdgeBendEditor::MouseEdgeBendEditor()
    : glWidget(nullptr), layer(nullptr), edgeEntity(nullptr),
      targetTriangle(Coord(0, 0, 0), Size(1, 1, 1)), circlesComposite(nullptr),
      mouseButtonPressOnEdge(false) {
  _operation = NONE_OP;

  basicCircle.resizePoints(30); // circle
  basicCircle.setFillMode(true);
  basicCircle.setOutlineMode(true);
  basicCircle.setFillColor(Color(255, 102, 255, 200));
  basicCircle.setOutlineColor(Color(128, 20, 20, 200));
  basicCircle.setStencil(0);
}
//========================================================================================
MouseEdgeBendEditor::~MouseEdgeBendEditor() = default;
//========================================================================================
void MouseEdgeBendEditor::clear() {
  if (glWidget != nullptr) {
    glWidget->scene()->removeLayer(layer, false);

    delete layer;
    layer = nullptr;
    circlesComposite = nullptr;

    glWidget->scene()->graphLayer()->deleteGlEntity("edgeEntity");
    delete edgeEntity;
    edgeEntity = nullptr;

    glWidget->setCursor(QCursor());
  }
}
//========================================================================================

void MouseEdgeBendEditor::stopEdition() {
  _operation = NONE_OP;
}

//========================================================================================
bool MouseEdgeBendEditor::eventFilter(QObject *widget, QEvent *e) {

  auto *qMouseEv = dynamic_cast<QMouseEvent *>(e);

  if (qMouseEv == nullptr) {
    return false;
  }

  // Double click to create a new control point
  if (e->type() == QEvent::MouseButtonDblClick && qMouseEv->button() == Qt::LeftButton &&
      haveSelection(glWidget)) {
    _operation = NEW_OP;
    mMouseCreate(qMouseEv->pos().x(), qMouseEv->pos().y(), glWidget);
    return true;
  }

  if (e->type() == QEvent::MouseButtonPress) {
    if (!glWidget) {
      glWidget = static_cast<GlWidget *>(widget);
    }

    initProxies(glWidget);
    bool hasSelection = haveSelection(glWidget);

    editPosition[0] = qMouseEv->pos().x();
    editPosition[1] = qMouseEv->pos().y();
    editPosition[2] = 0;

    switch (qMouseEv->buttons()) {
    case Qt::LeftButton: {

      if (!hasSelection) {
        // event occurs outside the selection rectangle
        // so from now we delegate the job to a MouseEdgeSelector object
        // which should intercept the event
        _operation = NONE_OP;
      } else {

        std::vector<SelectedEntity> selectedEntities;
        bool entityIsSelected = glWidget->pickGlEntities(
            int(editPosition[0]) - 3, int(editPosition[1]) - 3, 6, 6, selectedEntities, layer);

        if (!entityIsSelected) {
          // We have click outside an entity
          _operation = NONE_OP;
        } else {
          for (const auto &selection : selectedEntities) {
            selectedEntity = circlesComposite->findKey(selection.getEntity());
            if (!selectedEntity.empty()) {
              break;
            }
          }

          if (qMouseEv->modifiers() &
#if defined(__APPLE__)
              Qt::AltModifier
#else
              Qt::ControlModifier
#endif
          ) {
            _operation = DELETE_OP;
            mMouseDelete();
          } else {
            _graph->push();
            _operation = TRANSLATE_OP;
            glWidget->setCursor(QCursor(Qt::SizeAllCursor));
            mode = COORD;
          }

          return true;
        }
      }

      break;
    }

    default: {
      return false;
    }
    }

    glWidget->redraw();
  }

  if (e->type() == QEvent::MouseButtonRelease && qMouseEv->button() == Qt::LeftButton &&
      _operation != NONE_OP) {
    auto *glWidget = static_cast<GlWidget *>(widget);

    Camera &camera = glWidget->scene()->graphCamera();

    if (selectedEntity == "targetTriangle") {
      SelectedEntity selectedEntity;

      if (glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
          selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
        const auto &[src, tgt] = glWidget->scene()->glGraph()->graph()->ends(mEdge);
        _graph->setEnds(mEdge, src, node(selectedEntity.getGraphElementId()));
        glWidget->setCursor(QCursor(Qt::PointingHandCursor));
        glWidget->redraw();
      } else {
        Coord tmp = camera.worldTo2DViewport(end);
        targetTriangle.translate(Coord(-glWidget->screenToViewport(editPosition[0] - tmp[0]),
                                       glWidget->screenToViewport(editPosition[1] - tmp[1]), 0));
        glWidget->draw(false);
      }
    } else if (selectedEntity == "sourceCircle") {
      SelectedEntity selectedEntity;

      if (glWidget->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
          selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
        const auto &[src, tgt] = glWidget->scene()->glGraph()->graph()->ends(mEdge);
        _graph->setEnds(mEdge, node(selectedEntity.getGraphElementId()), tgt);
        glWidget->setCursor(QCursor(Qt::PointingHandCursor));
        glWidget->redraw();
      } else {
        Coord tmp = camera.worldTo2DViewport(start);
        sourceCircle.translate(Coord(-glWidget->screenToViewport(editPosition[0] - tmp[0]),
                                     glWidget->screenToViewport(editPosition[1] - tmp[1]), 0));
        glWidget->draw(false);
      }
    }

    _graph->popIfNoUpdates();
    selectedEntity = "";

    _operation = NONE_OP;
    return true;
  }

  if (e->type() == QEvent::MouseButtonPress) {
    vector<SelectedEntity> selectedEntities;
    glWidget->pickGlEntities(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntities);

    if (!selectedEntities.empty()) {
      if (selectedEntities[0].getEntityType() == SelectedEntity::SIMPLE_ENTITY_SELECTED) {
        if (selectedEntities[0].getEntity() == edgeEntity) {
          mouseButtonPressOnEdge = true;
          return true;
        }
      }
    }
  }

  if (e->type() == QEvent::MouseButtonRelease) {
    vector<SelectedEntity> selectedEntities;
    glWidget->pickGlEntities(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntities);

    if (!selectedEntities.empty()) {
      if (selectedEntities[0].getEntityType() == SelectedEntity::SIMPLE_ENTITY_SELECTED) {
        if (selectedEntities[0].getEntity() == edgeEntity && mouseButtonPressOnEdge) {
          mouseButtonPressOnEdge = false;
          return true;
        }
      }

      mouseButtonPressOnEdge = false;
    }
  }

  if (e->type() == QEvent::MouseMove) {
    if (qMouseEv->buttons() == Qt::LeftButton && _operation != NONE_OP) {
      auto *glWidget = static_cast<GlWidget *>(widget);

      switch (_operation) {
      case TRANSLATE_OP:
        mMouseTranslate(qMouseEv->pos().x(), qMouseEv->pos().y(), glWidget);
        return true;

      default:
        return false;
      }
    } else if (qMouseEv->buttons() == Qt::NoButton) {
      SelectedEntity selectedEntity;
      vector<SelectedEntity> selectedEntities;

      auto *g = static_cast<GlWidget *>(widget);

      if (g->pickNodesEdges(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
          selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
        g->setCursor(Qt::CrossCursor);
      } else if (g->pickGlEntities(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntities)) {
        for (const auto &selection : selectedEntities) {
          if (!circlesComposite->findKey(selection.getEntity()).empty()) {
            g->setCursor(Qt::CrossCursor);
            break;
          }
        }
      } else {
        g->setCursor(Qt::ArrowCursor);
      }

      return false;
    }
  }

  return false;
}
//========================================================================================
bool MouseEdgeBendEditor::compute(GlWidget *glWidget) {
  if (computeBendsCircles(glWidget)) {
    if (_operation == NONE_OP) {
      glWidget->setCursor(QCursor(Qt::PointingHandCursor));
    }

    if (!layer) {
      layer = new GlLayer("edgeBendEditorLayer", true);
      layer->setCamera(new Camera(glWidget->scene(), false));

      if (!circlesComposite) {
        circlesComposite = new GlComposite(false);
      }

      layer->addGlEntity(circlesComposite, "selectionComposite");
    }

    bool layerInScene = false;
    const auto &layersList = glWidget->scene()->getLayersList();

    for (const auto &it : layersList) {
      if (it.second == layer) {
        layerInScene = true;
        break;
      }
    }

    if (!layerInScene) {
      glWidget->scene()->addExistingLayerAfter(layer, "Main");
    }

    this->glWidget = glWidget;
    return true;
  } else {
    glWidget->setCursor(QCursor(Qt::CrossCursor));
  }

  return false;
}
//========================================================================================
bool MouseEdgeBendEditor::draw(GlWidget *) {
  return true;
}
//========================================================================================
void MouseEdgeBendEditor::initProxies(GlWidget *glWidget) {
  GlGraphInputData *inputData = glWidget->inputData();
  _graph = inputData->graph();
  _layout = inputData->layout();
  _selection = inputData->selection();
  _rotation = inputData->rotations();
  _sizes = inputData->sizes();
  _shape = inputData->shapes();

  if (_graph->existProperty("viewPolygonCoords")) {
    _coordsVectorProperty = _graph->getCoordVectorProperty("viewPolygonCoords");
  } else {
    _coordsVectorProperty = nullptr;
  }
}
//========================================================================================
// Does the point p belong to the segment [u,v]?
bool MouseEdgeBendEditor::belong(const Coord &u, const Coord &v, const Coord &p,
                                 GlWidget *glWidget) {
  float w = glWidget->screenToViewport(glWidget->width());
  float h = glWidget->screenToViewport(glWidget->height());
  Coord m = {w, h}, n = {w, h};
  m -= glWidget->scene()->graphCamera().worldTo2DViewport(u);
  n -= glWidget->scene()->graphCamera().worldTo2DViewport(v);
  double mnDist = m.dist(n);
  double mpDist = m.dist(p);
  double pnDist = p.dist(n);
  return ((mpDist + pnDist) - mnDist) / mnDist < 1E-3;
}
//========================================================================================
void MouseEdgeBendEditor::mMouseTranslate(int newX, int newY, GlWidget *glWidget) {
  initProxies(glWidget);

  Camera &camera = glWidget->scene()->graphCamera();

  int i;
  if (selectedEntity == "targetTriangle") {
    targetTriangle.translate(Coord(-glWidget->screenToViewport(editPosition[0] - newX),
                                   glWidget->screenToViewport(editPosition[1] - newY), 0));
    glWidget->draw(false);
  } else if (selectedEntity == "sourceCircle") {
    sourceCircle.translate(Coord(-glWidget->screenToViewport(editPosition[0] - newX),
                                 glWidget->screenToViewport(editPosition[1] - newY), 0));
    glWidget->draw(false);
  } else if (IntegerType::fromString(i, selectedEntity)) {
    Coord v0;
    Coord v1 = {editPosition[0] - newX, -(editPosition[1] - newY), 0};
    v0 = camera.viewportTo3DWorld(glWidget->screenToViewport(v0));
    v1 = camera.viewportTo3DWorld(glWidget->screenToViewport(v1));
    coordinates[i] += v1 - v0;

    Observable::holdObservers();
    if (edgeSelected) {
      _layout->setEdgeValue(mEdge, coordinates);
    } else {
      _coordsVectorProperty->setNodeValue(mNode, coordinates);
    }
    Observable::unholdObservers();
  }

  editPosition[0] = newX;
  editPosition[1] = newY;
}
//========================================================================================
void MouseEdgeBendEditor::mMouseDelete() {
  int i;
  if (selectedEntity != "targetTriangle" && selectedEntity != "sourceCircle" &&
      IntegerType::fromString(i, selectedEntity)) {

    auto coordIt = coordinates.begin();
    auto circleIt = circles.begin();
    int tmp = 0;

    while (tmp != i) {
      ++tmp;
      ++coordIt;
      ++circleIt;
    }

    if (!edgeSelected && coordinates.size() <= 3) {
      return;
    }

    coordinates.erase(coordIt);
    circles.erase(circleIt);
    edgeEntity->setCoordinates(start, end, coordinates);
    Observable::holdObservers();
    // allow to undo
    _graph->push();

    if (edgeSelected) {
      _layout->setEdgeValue(mEdge, coordinates);
    } else {
      _coordsVectorProperty->setNodeValue(mNode, coordinates);
    }

    Observable::unholdObservers();
  }
}
//========================================================================================
void MouseEdgeBendEditor::mMouseCreate(int x, int y, GlWidget *glWidget) {
  Coord viewportClick =
      Coord(glWidget->screenToViewport(glWidget->width() - x), glWidget->screenToViewport(y));

  Coord worldLocation = glWidget->scene()->graphCamera().viewportTo3DWorld(viewportClick);

  if (coordinates.empty()) {
    coordinates.push_back(worldLocation);
  } else {
    Coord first = coordinates[0];
    Coord last = coordinates[coordinates.size() - 1];
    bool firstSeg = belong(start, first, viewportClick, glWidget);
    bool lastSeg = belong(end, last, viewportClick, glWidget);
    bool firstLastSeg = false;

    if (!edgeSelected) {
      firstLastSeg = belong(first, last, viewportClick, glWidget);
    }

    if (firstSeg) {
      coordinates.insert(coordinates.begin(), worldLocation);
    }

    if (lastSeg || firstLastSeg) {
      coordinates.push_back(worldLocation);
    }

    if (!firstSeg && !lastSeg && !firstLastSeg) {
      auto coordIt = coordinates.begin();
      last = *coordIt;
      ++coordIt;

      while (coordIt != coordinates.end()) {
        first = last;
        last = *coordIt;
        bool midSeg = belong(first, last, viewportClick, glWidget);

        if (midSeg) {
          coordinates.insert(coordIt, worldLocation);
          break;
        }

        ++coordIt;
      }
    }
  }

  Observable::holdObservers();
  // allow to undo
  _graph->push();

  if (edgeSelected) {
    _layout->setEdgeValue(mEdge, coordinates);
  } else {
    if (_coordsVectorProperty) {
      _coordsVectorProperty->setNodeValue(mNode, coordinates);
    }
  }

  Observable::unholdObservers();
}
//========================================================================================
bool MouseEdgeBendEditor::haveSelection(GlWidget *glWidget) {
  initProxies(glWidget);

  bool hasSelection = false;
  bool multipleSelection = false;

  for (auto e : _graph->edges()) {
    if (_selection->getEdgeValue(e)) {
      if (hasSelection) {
        hasSelection = false;
        multipleSelection = true;
        break;
      } else {
        mEdge = e;
        edgeSelected = true;
        hasSelection = true;
      }
    }
  }

  if (!multipleSelection) {
    for (auto n : _graph->nodes()) {
      if (_selection->getNodeValue(n)) {
        if (hasSelection) {
          hasSelection = false;
          break;
        } else {
          mNode = n;
          edgeSelected = false;
          hasSelection = true;
        }
      }
    }
  }

  return hasSelection;
}
//========================================================================================
void MouseEdgeBendEditor::computeSrcTgtEntities(GlWidget *glWidget) {
  float endAngle = 0.;

  if (coordinates.empty()) {
    endAngle = atan((start[1] - end[1]) / (start[0] - end[0]));

    if (start[0] - end[0] > 0) {
      endAngle += float(M_PI);
    }
  } else {
    endAngle = atan((end[1] - coordinates[coordinates.size() - 1][1]) /
                    (end[0] - coordinates[coordinates.size() - 1][0]));

    if (end[0] - coordinates[coordinates.size() - 1][0] > 0) {
      endAngle += float(M_PI);
    }
  }

  Camera &camera = glWidget->scene()->graphCamera();

  if (selectedEntity != "targetTriangle") {
    Coord tmp = camera.worldTo2DViewport(end);
    targetTriangle =
        GlTriangle(tmp, Size(7, 7, 0), Color(255, 102, 255, 200), Color(128, 20, 20, 200));
    targetTriangle.setStartAngle(M_PI + endAngle);
    targetTriangle.setStencil(0);
  }

  if (selectedEntity != "sourceCircle") {
    Coord tmp = camera.worldTo2DViewport(start);
    sourceCircle = GlCircle(tmp, 6, Color(128, 20, 20, 200), Color(255, 102, 255, 200), true, true);
    sourceCircle.setStencil(0);
  }
}
//========================================================================================
bool MouseEdgeBendEditor::computeBendsCircles(GlWidget *glWidget) {
  initProxies(glWidget);

  Coord tmp;
  coordinates.clear();
  circles.clear();

  if (circlesComposite) {
    circlesComposite->reset(false);
  } else {
    circlesComposite = new GlComposite(false);
  }

  bool hasSelection = haveSelection(glWidget);

  if (!hasSelection) {
    glWidget->scene()->graphLayer()->deleteGlEntity("edgeEntity");
    return false;
  }

  if (edgeSelected) {
    coordinates = _layout->getEdgeValue(mEdge);
    start = _layout->getNodeValue(_graph->source(mEdge));
    end = _layout->getNodeValue(_graph->target(mEdge));

    computeSrcTgtEntities(glWidget);
    circlesComposite->addGlEntity(&targetTriangle, "targetTriangle");
    circlesComposite->addGlEntity(&sourceCircle, "sourceCircle");

    // Bends circles
    for (const auto &coord : coordinates) {
      basicCircle.set(glWidget->scene()->graphCamera().worldTo2DViewport(coord), 5, 0.);
      circles.push_back(basicCircle);
    }

    if (!edgeEntity) {
      edgeEntity = new EdgeEntity;
    }

    edgeEntity->setCoordinates(start, end, coordinates);
    glWidget->scene()->graphLayer()->addGlEntity(edgeEntity, "edgeEntity");
  } else {
    int complexPolygonGlyphId = GlyphManager::glyphId("2D - Complex Polygon", false);

    if (complexPolygonGlyphId && (_shape->getNodeValue(mNode) == complexPolygonGlyphId)) {
      if (_coordsVectorProperty) {
        const vector<Coord> &baseCoordinates = _coordsVectorProperty->getNodeValue(mNode);
        vector<Coord> coordinatesWithRotation;

        Coord min, max;
        min = baseCoordinates[0];
        max = baseCoordinates[0];

        for (const auto &coord : baseCoordinates) {
          if (coord[0] < min[0]) {
            min[0] = coord[0];
          }

          if (coord[0] > max[0]) {
            max[0] = coord[0];
          }

          if (coord[1] < min[1]) {
            min[1] = coord[1];
          }

          if (coord[1] > max[1]) {
            max[1] = coord[1];
          }
        }

        Size nodeSize = _sizes->getNodeValue(mNode);
        double rotation = _rotation->getNodeValue(mNode) * M_PI / 180;

        for (const auto &coord : baseCoordinates) {
          Coord tmp = {((coord[0] - min[0]) / (max[0] - min[0])) * nodeSize[0] - nodeSize[0] / 2.f,
                       ((coord[1] - min[1]) / (max[1] - min[1])) * nodeSize[1] - nodeSize[1] / 2.f};
          coordinatesWithRotation.push_back(_layout->getNodeValue(mNode) +
                                            Coord(tmp[0] * cos(rotation) - tmp[1] * sin(rotation),
                                                  tmp[0] * sin(rotation) + tmp[1] * cos(rotation)));
          coordinates.push_back(_layout->getNodeValue(mNode) + tmp);
        }

        for (const auto &coord : coordinatesWithRotation) {
          basicCircle.set(glWidget->scene()->graphCamera().worldTo2DViewport(coord), 5, 0.);
          circles.push_back(basicCircle);
        }
      }
    }
  }

  for (uint i = 0; i < circles.size(); ++i) {
    circlesComposite->addGlEntity(&circles[i], IntegerType::toString(i));
  }

  return hasSelection;
}
//========================================================================================
