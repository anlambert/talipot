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

#include <talipot/MouseSelectionEditor.h>
#include <talipot/GlWidget.h>
#include <talipot/DrawingTools.h>
#include <talipot/TlpQtTools.h>

#define EPSILON 1.0
#define EPSILON_SCREEN 50
#define EPSILON_STRETCH_MIN 1 - 1.0e-01
#define EPSILON_STRETCH_MAX 1 + 1.0e-01

using namespace tlp;
using namespace std;

const vector<Coord> arrowWithLine = {Coord(0, 3, 0),  Coord(-5, -5, 0), Coord(5, -5, 0),
                                     Coord(0, 3, 0),  Coord(5, 3, 0),   Coord(5, 5, 0),
                                     Coord(-5, 5, 0), Coord(-5, 3, 0)};

const vector<Coord> twoArrowWithLine = {
    Coord(0, 0, 0), Coord(5, -5, 0), Coord(-5, -5, 0), Coord(0, 0, 0),  Coord(-5, 0, 0),
    Coord(5, 0, 0), Coord(0, 0, 0),  Coord(5, 5, 0),   Coord(-5, 5, 0), Coord(0, 0, 0)};

//========================================================================================
MouseSelectionEditor::MouseSelectionEditor()
    : glWidget(nullptr), layer(nullptr), composite(nullptr) {
  operation = NONE;

  _controls[0].resizePoints(3); // triangle
  _controls[0].setStencil(0);
  _controls[1].resizePoints(4); // square
  _controls[1].setStencil(0);
  _controls[2].resizePoints(3); // triangle
  _controls[2].setStencil(0);
  _controls[3].resizePoints(30); // circle
  _controls[3].setStencil(0);
  _controls[4].resizePoints(3); // triangle
  _controls[4].setStencil(0);
  _controls[5].resizePoints(4); // square
  _controls[5].setStencil(0);
  _controls[6].resizePoints(3); // triangle
  _controls[6].setStencil(0);
  _controls[7].resizePoints(30); // circle
  _controls[7].setStencil(0);

  centerRect.setStencil(0);
  advRect.setStencil(0);

  Color hudColor = {128, 128, 128, 128};
  centerRect.setFillMode(true);
  centerRect.setOutlineMode(true);
  centerRect.setFillColor(hudColor);
  hudColor = Color(128, 128, 128, 64);
  advRect.setFillMode(true);
  advRect.setOutlineMode(false);
  advRect.setFillColor(hudColor);

  for (auto &control : _controls) {
    control.setFillMode(true);
    control.setOutlineMode(true);
    control.setFillColor(Color(255, 40, 40, 200));
    control.setOutlineColor(Color(128, 20, 20, 200));
  }
}
//========================================================================================
MouseSelectionEditor::~MouseSelectionEditor() = default;
//========================================================================================
void MouseSelectionEditor::clear() {
  if (glWidget != nullptr) {
    glWidget->getScene()->removeLayer(layer, false);
    delete layer;
    layer = nullptr;

    glWidget->setCursor(QCursor());
  }
}
//========================================================================================
void MouseSelectionEditor::getOperation(GlEntity *select) {
  // left <-> right anchor
  // stretch_x
  if (select == &_controls[0] || select == &_controls[4]) {
    operation = STRETCH_X;
    glWidget->setCursor(QCursor(Qt::SizeHorCursor));
    return;
  }

  // top <-> bottom anchor
  // stretch_y
  if (select == &_controls[2] || select == &_controls[6]) {
    operation = STRETCH_Y;
    glWidget->setCursor(QCursor(Qt::SizeVerCursor));
    return;
  }

  // Corner anchor bottom-right top-left
  // rotate
  if (select == &_controls[3] || select == &_controls[7]) {
    glWidget->setCursor(QCursor(Qt::PointingHandCursor));
    operation = ROTATE_Z;
    return;
  }

  // Corner anchor top-right bottom-left
  // stretch_xy
  if (select == &_controls[1] || select == &_controls[5]) {
    operation = STRETCH_XY;
    glWidget->setCursor(QCursor(Qt::SizeFDiagCursor));
    return;
  }

  if (select == &_advControls[0]) {
    operation = ALIGN_TOP;
    return;
  }

  if (select == &_advControls[1]) {
    operation = ALIGN_BOTTOM;
    return;
  }

  if (select == &_advControls[2]) {
    operation = ALIGN_LEFT;
    return;
  }

  if (select == &_advControls[3]) {
    operation = ALIGN_RIGHT;
    return;
  }

  if (select == &_advControls[4]) {
    operation = ALIGN_HORIZONTALLY;
    return;
  }

  if (select == &_advControls[5]) {
    operation = ALIGN_VERTICALLY;
    return;
  }
}
//========================================================================================
bool MouseSelectionEditor::eventFilter(QObject *widget, QEvent *e) {
  auto *qMouseEv = static_cast<QMouseEvent *>(e);
  auto *glWidget = static_cast<GlWidget *>(widget);

  if (e->type() == QEvent::MouseButtonPress) {

    initProxies(glWidget);
    computeFFD(glWidget);

    int H;
    H = glWidget->height();
    editCenter = centerRect.getCenter();
    editCenter[2] = 0;
    editCenter[1] = glWidget->screenToViewport(H) - editCenter[1];
    // editCenter[0] = W - editCenter[0];
    editPosition[0] = qMouseEv->pos().x();
    editPosition[1] = qMouseEv->pos().y();
    editPosition[2] = 0;
    editLayoutCenter = _layoutCenter;

    vector<SelectedEntity> select;

    switch (qMouseEv->buttons()) {
    case Qt::LeftButton: {
      // first ensure that something is selected
      bool hasSelection = _selection->hasNonDefaultValuatedNodes(_graph) ||
                          _selection->hasNonDefaultValuatedEdges(_graph);

      if (!hasSelection ||
          (!glWidget->pickGlEntities(int(editPosition[0]) - 3, int(editPosition[1]) - 3, 6, 6,
                                     select, layer))) {
        // event occurs outside the selection rectangle
        // so from now we delegate the job to a MouseSelector object
        // which should intercept the event
        operation = NONE;
        glWidget->setCursor(QCursor(Qt::CrossCursor));

        return false;
      }

      glWidget->setCursor(QCursor(Qt::PointingHandCursor));

      int shapeId = -1;
      bool advShape = false;

      for (uint i = 0; (i < select.size()) && (shapeId == -1); ++i) {
        for (const auto &control : _controls) {
          if (select[i].getEntity() == &control) {
            shapeId = i;
          }
        }

        for (const auto &advControl : _advControls) {
          if (select[i].getEntity() == &advControl) {
            advShape = true;
            shapeId = i;
          }
        }
      }

      if (shapeId != -1) {
        if (!advShape) {
          static_cast<GlCircle *>(select[shapeId].getEntity())
              ->setFillColor(Color(40, 255, 40, 200));
          static_cast<GlCircle *>(select[shapeId].getEntity())
              ->setOutlineColor(Color(20, 128, 20, 200));
        }

        getOperation(select[shapeId].getEntity());

        switch (operation) {
        case ALIGN_TOP:
        case ALIGN_BOTTOM:
        case ALIGN_LEFT:
        case ALIGN_RIGHT:
        case ALIGN_VERTICALLY:
        case ALIGN_HORIZONTALLY:
          mAlign(operation, glWidget);
          return true;

        case ROTATE_Z:
        case ROTATE_XY:
        case NONE:
        case STRETCH_X:
        case STRETCH_Y:
        case STRETCH_XY:
        case TRANSLATE:
        default:
          break;
        }
      } else {
        if (qMouseEv->modifiers() &
#if defined(__APPLE__)
            Qt::AltModifier
#else
            Qt::ControlModifier
#endif
        ) {
          operation = ROTATE_XY;
          glWidget->setCursor(QCursor(Qt::PointingHandCursor));
        } else {
          operation = TRANSLATE;
          glWidget->setCursor(QCursor(Qt::SizeAllCursor));
        }
      }

      mode = COORD_AND_SIZE;

      if (qMouseEv->modifiers() & Qt::ShiftModifier) {
        mode = COORD;
      }

      if (qMouseEv->modifiers() &
#if defined(__APPLE__)
          Qt::AltModifier
#else
          Qt::ControlModifier
#endif
      )
        mode = SIZE;

      initEdition();
      break;
    }

    case Qt::MiddleButton:
      undoEdition();
      glWidget->setCursor(QCursor(Qt::ArrowCursor));
      break;

    default:
      return false;
    }

    glWidget->redraw();
    return true;
  }

  if (e->type() == QEvent::MouseButtonRelease && qMouseEv->button() == Qt::LeftButton &&
      operation != NONE) {
    stopEdition();

    // restore colors
    for (auto &control : _controls) {
      control.setFillColor(Color(255, 40, 40, 200));
      control.setOutlineColor(Color(128, 20, 20, 200));
    }

    glWidget->setCursor(QCursor(Qt::ArrowCursor));
    glWidget->draw();
    return true;
  }

  if (e->type() == QEvent::KeyPress) {
    // first ensure that something is selected
    bool hasSelection = _selection->hasNonDefaultValuatedNodes(_graph) ||
                        _selection->hasNonDefaultValuatedEdges(_graph);

    if (hasSelection) {
      switch (static_cast<QKeyEvent *>(e)->key()) {
      case Qt::Key_Left:
        mMouseTranslate(editPosition[0] - 1, editPosition[1], glWidget);
        break;

      case Qt::Key_Right:
        mMouseTranslate(editPosition[0] + 1, editPosition[1], glWidget);
        break;

      case Qt::Key_Up:
        mMouseTranslate(editPosition[0], editPosition[1] - 1, glWidget);
        break;

      case Qt::Key_Down:
        mMouseTranslate(editPosition[0], editPosition[1] + 1, glWidget);
        break;
      }
    }
  }

  if (e->type() == QEvent::MouseMove && qMouseEv->buttons() & Qt::LeftButton && operation != NONE) {
    int newX = qMouseEv->pos().x();
    int newY = qMouseEv->pos().y();

    switch (operation) {
    case STRETCH_X:
    case STRETCH_Y:
    case STRETCH_XY:
      mMouseStretchAxis(newX, newY, glWidget);
      return true;

    case ROTATE_Z:
    case ROTATE_XY:
      mMouseRotate(newX, newY, glWidget);
      return true;

    case TRANSLATE:
      mMouseTranslate(newX, newY, glWidget);
      return true;

    case NONE:
    case ALIGN_TOP:
    case ALIGN_BOTTOM:
    case ALIGN_LEFT:
    case ALIGN_RIGHT:
    case ALIGN_VERTICALLY:
    case ALIGN_HORIZONTALLY:
    default:
      qWarning() << "[Error] : " << __FUNCTION__ << " should not have been called\n";
      break;
    }
  }

  return false;
}
//========================================================================================
bool MouseSelectionEditor::compute(GlWidget *glWidget) {
  if (computeFFD(glWidget)) {
    if (!layer) {
      layer = new GlLayer("selectionEditorLayer", true);
      layer->setCamera(new Camera(glWidget->getScene(), false));
      composite = new GlComposite(false);
      layer->addGlEntity(composite, "selectionComposite");
    }

    bool layerInScene = false;
    const auto &layersList = glWidget->getScene()->getLayersList();

    for (const auto &it : layersList) {
      if (it.second == layer) {
        layerInScene = true;
        break;
      }
    }

    if (!layerInScene) {
      glWidget->getScene()->addExistingLayerAfter(layer, "Main");
    }

    composite->addGlEntity(&centerRect, "CenterRectangle");
    composite->addGlEntity(&_controls[0], "left");
    composite->addGlEntity(&_controls[1], "top-left");
    composite->addGlEntity(&_controls[2], "top");
    composite->addGlEntity(&_controls[3], "top-right");
    composite->addGlEntity(&_controls[4], "right");
    composite->addGlEntity(&_controls[5], "bottom-right");
    composite->addGlEntity(&_controls[6], "bottom");
    composite->addGlEntity(&_controls[7], "bottom-left");

    bool moreThanOneNode = iteratorCountCheck(_selection->getNodesEqualTo(true, _graph), 2);

    if (moreThanOneNode) {
      composite->addGlEntity(&advRect, "AdvRectangle");

      composite->addGlEntity(&_advControls[0], "center-top");
      composite->addGlEntity(&_advControls[1], "center-bottom");
      composite->addGlEntity(&_advControls[2], "center-right");
      composite->addGlEntity(&_advControls[3], "center-left");
      composite->addGlEntity(&_advControls[4], "center-horizontally");
      composite->addGlEntity(&_advControls[5], "center-vertically");
    } else {
      composite->deleteGlEntity("AdvRectangle");

      composite->deleteGlEntity("center-top");
      composite->deleteGlEntity("center-bottom");
      composite->deleteGlEntity("center-right");
      composite->deleteGlEntity("center-left");
      composite->deleteGlEntity("center-horizontally");
      composite->deleteGlEntity("center-vertically");
    }

    this->glWidget = glWidget;
    return true;
  } else {
    if (layer) {
      glWidget->getScene()->removeLayer(layer, true);
      layer = nullptr;
    }

    return false;
  }
}
//========================================================================================
bool MouseSelectionEditor::draw(GlWidget *) {
  // return compute(glWidget);
  return true;
}
//========================================================================================
void MouseSelectionEditor::initEdition() {
  _graph->push();
}
//========================================================================================
void MouseSelectionEditor::undoEdition() {
  if (operation == NONE) {
    return;
  }

  _graph->pop();
  operation = NONE;
}
//========================================================================================
void MouseSelectionEditor::stopEdition() {
  if (layer) {
    glWidget->getScene()->removeLayer(layer, true);
    layer = nullptr;
  }

  operation = NONE;
}
//========================================================================================
void MouseSelectionEditor::initProxies(GlWidget *glWidget) {
  GlGraphInputData *inputData = glWidget->getGlGraphInputData();
  _graph = inputData->graph();
  _layout = inputData->layout();
  _selection = inputData->selection();
  _rotation = inputData->rotations();
  _sizes = inputData->sizes();
}
//========================================================================================
void MouseSelectionEditor::mMouseTranslate(double newX, double newY, GlWidget *glWidget) {
  Observable::holdObservers();
  initProxies(glWidget);
  Coord v0;
  Coord v1 = Coord(editPosition[0] - newX, -(editPosition[1] - newY));
  v0 = glWidget->getScene()->getGraphCamera().viewportTo3DWorld(v0);
  v1 = glWidget->getScene()->getGraphCamera().viewportTo3DWorld(glWidget->screenToViewport(v1));
  v1 -= v0;
  _layout->translate(v1, _selection->getNodesEqualTo(true, _graph),
                     _selection->getEdgesEqualTo(true, _graph));
  editPosition[0] = newX;
  editPosition[1] = newY;
  Observable::unholdObservers();
}
//========================================================================================
void MouseSelectionEditor::mMouseStretchAxis(double newX, double newY, GlWidget *glWidget) {
  Coord curPos = Coord(glWidget->screenToViewport(newX), glWidget->screenToViewport(newY));
  Coord stretch = {1};

  if (operation == STRETCH_X || operation == STRETCH_XY) {
    stretch[0] =
        (curPos[0] - editCenter[0]) / (glWidget->screenToViewport(editPosition[0]) - editCenter[0]);
  }

  if (operation == STRETCH_Y || operation == STRETCH_XY) {
    stretch[1] =
        (curPos[1] - editCenter[1]) / (glWidget->screenToViewport(editPosition[1]) - editCenter[1]);
  }

  Observable::holdObservers();
  _graph->pop();
  _graph->push();

  // stretch layout
  if (mode == COORD_AND_SIZE || mode == COORD) {
    Coord center = editLayoutCenter;
    center *= -1.;
    // move the center to the origin in order to be able to scale
    _layout->translate(center, _selection->getNodesEqualTo(true, _graph),
                       _selection->getEdgesEqualTo(true, _graph));
    // scale the drawing
    _layout->scale(stretch, _selection->getNodesEqualTo(true, _graph),
                   _selection->getEdgesEqualTo(true, _graph));
    // replace the center of the graph at its originale position
    center *= -1.;
    _layout->translate(center, _selection->getNodesEqualTo(true, _graph),
                       _selection->getEdgesEqualTo(true, _graph));
  }

  // stretch size
  if (mode == COORD_AND_SIZE || mode == SIZE) {
    stretch[0] = abs(stretch[0]);
    stretch[1] = abs(stretch[1]);
    stretch[2] = abs(stretch[1]);
    _sizes->scale(stretch, _selection->getNodesEqualTo(true, _graph),
                  _selection->getEdgesEqualTo(true, _graph));
  }

  Observable::unholdObservers();
}
//========================================================================================
void MouseSelectionEditor::mMouseRotate(double newX, double newY, GlWidget *glWidget) {
  if (operation == ROTATE_Z) {
    Coord curPos = Coord(newX, newY);

    Coord vCS = glWidget->screenToViewport(editPosition) - editCenter;
    vCS /= vCS.norm();
    Coord vCP = glWidget->screenToViewport(curPos) - editCenter;
    vCP /= vCP.norm();

    float sign = (vCS ^ vCP)[2];
    sign /= fabs(sign);
    double cosalpha = vCS.dotProduct(vCP);
    double deltaAngle = sign * acos(cosalpha);

    Observable::holdObservers();

    initProxies(glWidget);
    _graph->pop();
    _graph->push();

    double degAngle = (deltaAngle * 180.0 / M_PI);

    // rotate layout
    if (mode == COORD_AND_SIZE || mode == COORD) {
      Coord center = editLayoutCenter;
      center *= -1.;
      _layout->translate(center, _selection->getNodesEqualTo(true, _graph),
                         _selection->getEdgesEqualTo(true, _graph));
      _layout->rotateZ(-degAngle, _selection->getNodesEqualTo(true, _graph),
                       _selection->getEdgesEqualTo(true, _graph));
      center *= -1.;
      _layout->translate(center, _selection->getNodesEqualTo(true, _graph),
                         _selection->getEdgesEqualTo(true, _graph));
    }

    if (mode == COORD_AND_SIZE || mode == SIZE) {
      for (auto n : _selection->getNodesEqualTo(true, _graph)) {
        double rotation = _rotation->getNodeValue(n);
        _rotation->setNodeValue(n, rotation - degAngle);
      }
    }

    Observable::unholdObservers();
  } else {
    double initDelta, delta, cosa;
    double xAngle = 0, yAngle = 0;
    double nbPI = 0;

    delta = abs(glWidget->screenToViewport(newX - editPosition[0]));

    if (delta > abs(glWidget->screenToViewport(newY - editPosition[1]))) {
      initDelta = abs(editCenter[0] - glWidget->screenToViewport(editPosition[0]));
      nbPI = floor(delta / (2. * initDelta));
      delta -= nbPI * 2. * initDelta;
      cosa = (initDelta - delta) / initDelta;

      yAngle = (acos(cosa) + (nbPI * M_PI)) * 180.0 / M_PI;
    } else {
      delta = abs(glWidget->screenToViewport(newY - editPosition[1]));
      initDelta = abs(editCenter[1] - glWidget->screenToViewport(editPosition[1]));
      nbPI = floor(delta / (2. * initDelta));
      delta -= nbPI * 2. * initDelta;
      cosa = (initDelta - delta) / initDelta;

      xAngle = (acos(cosa) + (nbPI * M_PI)) * 180.0 / M_PI;
    }

    Observable::holdObservers();

    initProxies(glWidget);
    _graph->pop();
    _graph->push();

    Coord center = editLayoutCenter;
    center *= -1.;
    _layout->translate(center, _selection->getNodesEqualTo(true, _graph),
                       _selection->getEdgesEqualTo(true, _graph));

    if (yAngle > xAngle) {
      _layout->rotateY(yAngle, _selection->getNodesEqualTo(true, _graph),
                       _selection->getEdgesEqualTo(true, _graph));
    } else {
      _layout->rotateX(xAngle, _selection->getNodesEqualTo(true, _graph),
                       _selection->getEdgesEqualTo(true, _graph));
    }

    center *= -1.;
    _layout->translate(center, _selection->getNodesEqualTo(true, _graph),
                       _selection->getEdgesEqualTo(true, _graph));

    Observable::unholdObservers();
  }
}
//========================================================================================
void MouseSelectionEditor::mAlign(EditOperation operation, GlWidget *) {
  Observable::holdObservers();
  _graph->push();

  bool init = false;
  float min = -FLT_MAX, max = FLT_MAX;

  for (auto n : _selection->getNodesEqualTo(true, _graph)) {

    float valueMin = -FLT_MAX, valueMax = FLT_MAX;

    switch (operation) {
    case ALIGN_TOP:
      valueMax = _layout->getNodeValue(n)[1] + _sizes->getNodeValue(n)[1] / 2.;
      break;

    case ALIGN_BOTTOM:
      valueMin = _layout->getNodeValue(n)[1] - _sizes->getNodeValue(n)[1] / 2.;
      break;

    case ALIGN_HORIZONTALLY:
      valueMax = _layout->getNodeValue(n)[1] + _sizes->getNodeValue(n)[1] / 2.;
      valueMin = _layout->getNodeValue(n)[1] - _sizes->getNodeValue(n)[1] / 2.;
      break;

    case ALIGN_LEFT:
      valueMin = _layout->getNodeValue(n)[0] - _sizes->getNodeValue(n)[0] / 2.;
      break;

    case ALIGN_RIGHT:
      valueMax = _layout->getNodeValue(n)[0] + _sizes->getNodeValue(n)[0] / 2.;
      break;

    case ALIGN_VERTICALLY:
      valueMax = _layout->getNodeValue(n)[0] - _sizes->getNodeValue(n)[0] / 2.;
      valueMin = _layout->getNodeValue(n)[0] + _sizes->getNodeValue(n)[0] / 2.;
      break;

    case STRETCH_X:
    case STRETCH_Y:
    case STRETCH_XY:
    case ROTATE_Z:
    case ROTATE_XY:
    case TRANSLATE:
    case NONE:
    default:
      break;
    }

    if (!init) {
      max = valueMax;
      min = valueMin;
      init = true;
    } else {
      switch (operation) {
      case ALIGN_TOP:
      case ALIGN_RIGHT:

        if (valueMax > max) {
          max = valueMax;
        }

        break;

      case ALIGN_BOTTOM:
      case ALIGN_LEFT:

        if (valueMin < min) {
          min = valueMin;
        }

        break;

      case ALIGN_HORIZONTALLY:
      case ALIGN_VERTICALLY:

        if (valueMax > max) {
          max = valueMax;
        }

        if (valueMin < min) {
          min = valueMin;
        }

        break;

      case STRETCH_X:
      case STRETCH_Y:
      case STRETCH_XY:
      case ROTATE_Z:
      case ROTATE_XY:
      case TRANSLATE:
      case NONE:
      default:
        break;
      }
    }
  }

  for (auto n : _selection->getNodesEqualTo(true, _graph)) {
    Coord old = _layout->getNodeValue(n);

    switch (operation) {
    case ALIGN_TOP:
      old[1] = max - _sizes->getNodeValue(n)[1] / 2.;
      break;

    case ALIGN_BOTTOM:
      old[1] = min + _sizes->getNodeValue(n)[1] / 2.;
      break;

    case ALIGN_HORIZONTALLY:
      old[1] = (max + min) / 2;
      break;

    case ALIGN_LEFT:
      old[0] = min + _sizes->getNodeValue(n)[0] / 2.;
      break;

    case ALIGN_RIGHT:
      old[0] = max - _sizes->getNodeValue(n)[0] / 2.;
      break;

    case ALIGN_VERTICALLY:
      old[0] = (max + min) / 2;
      break;

    case STRETCH_X:
    case STRETCH_Y:
    case STRETCH_XY:
    case ROTATE_Z:
    case ROTATE_XY:
    case TRANSLATE:
    case NONE:
    default:
      break;
    }

    _layout->setNodeValue(n, old);
  }

  Observable::unholdObservers();
}
//========================================================================================
bool MouseSelectionEditor::computeFFD(GlWidget *glWidget) {
  if (!glWidget->getScene()->getGlGraph() || !glWidget->getGlGraphInputData()->graph()) {
    return false;
  }

  // We calculate the bounding box for the selection :
  initProxies(glWidget);
  BoundingBox boundingBox = tlp::computeBoundingBox(_graph, _layout, _sizes, _rotation, _selection);

  if (!boundingBox.isValid()) {
    return false;
  }

  if (operation == NONE) {
    glWidget->setCursor(QCursor(Qt::PointingHandCursor));
  }

  Coord min2D, max2D;
  _layoutCenter = boundingBox.center();

  // project the 8 points of the cube to obtain the bounding square on the 2D screen
  Coord bbsize = boundingBox[1] - boundingBox[0];
  // v1
  Coord tmp = boundingBox[0];
  tmp = glWidget->getScene()->getGraphCamera().worldTo2DViewport(tmp);
  min2D = tmp;
  max2D = tmp;

  // v2, v3, V4
  for (uint i = 0; i < 3; ++i) {
    tmp = boundingBox[0];
    tmp[i] += bbsize[i];
    tmp = glWidget->getScene()->getGraphCamera().worldTo2DViewport(tmp);
    min2D = minVector(tmp, min2D);
    max2D = maxVector(tmp, max2D);
  }

  // v4
  tmp = boundingBox[0];
  tmp[0] += bbsize[0];
  tmp[1] += bbsize[1];
  tmp = glWidget->getScene()->getGraphCamera().worldTo2DViewport(tmp);
  min2D = minVector(tmp, min2D);
  max2D = maxVector(tmp, max2D);
  // v6
  tmp = Coord(boundingBox[0]);
  tmp[0] += bbsize[0];
  tmp[2] += bbsize[2];
  tmp = glWidget->getScene()->getGraphCamera().worldTo2DViewport(tmp);
  min2D = minVector(tmp, min2D);
  max2D = maxVector(tmp, max2D);
  // v7
  tmp = boundingBox[0];
  tmp[1] += bbsize[1];
  tmp[2] += bbsize[2];
  tmp = glWidget->getScene()->getGraphCamera().worldTo2DViewport(tmp);
  min2D = minVector(tmp, min2D);
  max2D = maxVector(tmp, max2D);
  // v8
  tmp = boundingBox[0];
  tmp += bbsize;
  tmp = glWidget->getScene()->getGraphCamera().worldTo2DViewport(tmp);
  min2D = minVector(tmp, min2D);
  max2D = maxVector(tmp, max2D);

  ffdCenter = boundingBox.center();

  Coord tmpCenter = glWidget->getScene()->getGraphCamera().worldTo2DViewport(ffdCenter);

  int x = int(max2D[0] - min2D[0]) / 2 + 1; // (+1) because selection use glLineWidth=3 thus
  int y = int(max2D[1] - min2D[1]) / 2 + 1; // the rectangle can be too small.

  if (x < 20) {
    x = 18;
  }

  if (y < 20) {
    y = 18;
  }

  Coord positions[8];

  // we keep the z coordinate of the ffdCenter
  // to ensure a correct position of our controls (see GlHudPolygon.cpp)
  positions[0] = Coord(x, 0, ffdCenter[2]) + tmpCenter;   // left
  positions[1] = Coord(x, -y, ffdCenter[2]) + tmpCenter;  // Top left
  positions[2] = Coord(0, -y, ffdCenter[2]) + tmpCenter;  // Top
  positions[3] = Coord(-x, -y, ffdCenter[2]) + tmpCenter; // Top r
  positions[4] = Coord(-x, 0, ffdCenter[2]) + tmpCenter;  // r
  positions[5] = Coord(-x, y, ffdCenter[2]) + tmpCenter;  // Bottom r
  positions[6] = Coord(0, y, ffdCenter[2]) + tmpCenter;   // Bottom
  positions[7] = Coord(x, y, ffdCenter[2]) + tmpCenter;   // Bottom l

  for (auto &position : positions) {
    position[2] = 0;
  }

  // Parameters of the rectangle that shows the selected area.
  centerRect.setTopLeftPos(positions[1]);
  centerRect.setBottomRightPos(positions[5]);
  advRect.setTopLeftPos(positions[7] + Coord(-92, 16, 0));
  advRect.setBottomRightPos(positions[7]);

  _controls[0].set(positions[0], 7, 0.0);                // t
  _controls[1].set(positions[1], 6, float(M_PI) / 4.f);  // c
  _controls[2].set(positions[2], 7, float(-M_PI) / 2.f); // t
  _controls[3].set(positions[3], 5, 0.);                 // s
  _controls[4].set(positions[4], 7, float(M_PI));        // t
  _controls[5].set(positions[5], 6, float(M_PI) / 4.f);  // c
  _controls[6].set(positions[6], 7, float(M_PI) / 2.f);  //
  _controls[7].set(positions[7], 5, 0.);                 // s

  vector<Coord> advControlVect;

  for (const auto &p : arrowWithLine) {
    advControlVect.push_back(p + positions[7] + Coord(-11, 8, 0));
  }

  _advControls[0] =
      GlComplexPolygon(advControlVect, Color(255, 40, 40, 200), Color(128, 20, 20, 200));
  advControlVect.clear();

  for (const auto &p : arrowWithLine) {
    advControlVect.push_back(Coord(p[0], -p[1], 0) + positions[7] + Coord(-25, 8, 0));
  }

  _advControls[1] =
      GlComplexPolygon(advControlVect, Color(255, 40, 40, 200), Color(128, 20, 20, 200));
  advControlVect.clear();

  for (const auto &p : arrowWithLine) {
    advControlVect.push_back(Coord(-p[1], p[0], 0) + positions[7] + Coord(-39, 8, 0));
  }

  _advControls[2] =
      GlComplexPolygon(advControlVect, Color(255, 40, 40, 200), Color(128, 20, 20, 200));
  advControlVect.clear();

  for (const auto &p : arrowWithLine) {
    advControlVect.push_back(Coord(p[1], p[0], 0) + positions[7] + Coord(-53, 8, 0));
  }

  _advControls[3] =
      GlComplexPolygon(advControlVect, Color(255, 40, 40, 200), Color(128, 20, 20, 200));
  advControlVect.clear();

  for (const auto &p : twoArrowWithLine) {
    advControlVect.push_back(p + positions[7] + Coord(-67, 8, 0));
  }

  _advControls[4] =
      GlComplexPolygon(advControlVect, Color(255, 40, 40, 200), Color(128, 20, 20, 200));
  advControlVect.clear();

  for (const auto &p : twoArrowWithLine) {
    advControlVect.push_back(Coord(p[1], p[0], 0) + positions[7] + Coord(-81, 8, 0));
  }

  _advControls[5] =
      GlComplexPolygon(advControlVect, Color(255, 40, 40, 200), Color(128, 20, 20, 200));
  advControlVect.clear();

  for (auto &advControl : _advControls) {
    advControl.setStencil(0);
  }

  return true;
}
//========================================================================================
