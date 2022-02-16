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

#include <QApplication>

#include <talipot/GlPolyQuad.h>
#include <talipot/GlCatmullRomCurve.h>
#include <talipot/GlCubicBSplineInterpolation.h>
#include <talipot/GlLine.h>
#include <talipot/GlProgressBar.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlWidget.h>
#include <talipot/ViewSettings.h>
#include <talipot/TlpQtTools.h>

#include "ParallelCoordinatesDrawing.h"
#include "NominalParallelAxis.h"
#include "QuantitativeParallelAxis.h"
#include "ParallelTools.h"
#include "ParallelCoordinatesGraphProxy.h"

using namespace std;

namespace tlp {

ParallelCoordinatesDrawing::ParallelCoordinatesDrawing(ParallelCoordinatesGraphProxy *graph,
                                                       Graph *axisPointsGraph)
    : nbAxis(0), firstAxisPos(Coord(0.0f, 0.0f, 0.0f)), width(0), height(DEFAULT_AXIS_HEIGHT),
      spaceBetweenAxis(height / 2), linesColorAlphaValue(DEFAULT_LINES_COLOR_ALPHA_VALUE),
      drawPointsOnAxis(true), graphProxy(graph), backgroundColor(Color(255, 255, 255)),
      createAxisFlag(true), axisPointsGraph(axisPointsGraph), layoutType(PARALLEL),
      linesType(STRAIGHT), linesThickness(THICK), resetAxisLayout(false) {
  axisPointsGraphLayout = axisPointsGraph->getLayoutProperty("viewLayout");
  axisPointsGraphSize = axisPointsGraph->getSizeProperty("viewSize");
  axisPointsGraphShape = axisPointsGraph->getIntegerProperty("viewShape");
  axisPointsGraphLabels = axisPointsGraph->getStringProperty("viewLabel");
  axisPointsGraphColors = axisPointsGraph->getColorProperty("viewColor");
  axisPointsGraphSelection = axisPointsGraph->getBooleanProperty("viewSelection");

  dataPlotComposite = new GlComposite();
  axisPlotComposite = new GlComposite();
  addGlEntity(dataPlotComposite, "data plot composite");
  addGlEntity(axisPlotComposite, "axis plot composite");
}

ParallelCoordinatesDrawing::~ParallelCoordinatesDrawing() = default;

void ParallelCoordinatesDrawing::createAxis(GlWidget *glWidget, GlProgressBar *progressBar) {

  glWidget->makeCurrent();

  uint pos = 0;
  vector<string> selectedProperties(graphProxy->getSelectedProperties());
  GlAxis::CaptionLabelPosition captionPosition;

  static LayoutType lastLayouType = PARALLEL;

  if (axisOrder.size() != selectedProperties.size() || lastLayouType != layoutType) {
    resetAxisLayout = true;
  }

  static vector<Coord> lastAxisCoord;

  if (progressBar) {
    progressBar->setComment("Creating parallel axes ...");
    progressBar->progress(0, selectedProperties.size());
    glWidget->scene()->centerScene();
    float glWidth = glWidget->scene()->getBoundingBox().width();
    glWidget->scene()->zoomFactor((glWidth - 50) / glWidth);
    glWidget->draw();
    // needed to display progressBar
    QApplication::processEvents();
  }

  if (layoutType == PARALLEL) {
    lastAxisCoord.clear();
    vector<ParallelAxis *> lastAxisOrder(getAllAxis());

    for (auto *axis : lastAxisOrder) {
      lastAxisCoord.push_back(axis->getBaseCoord());
    }
  }

  static vector<float> lastAxisRotAngle;

  if (layoutType == CIRCULAR) {
    lastAxisRotAngle.clear();
    vector<ParallelAxis *> lastAxisOrder(getAllAxis());

    for (auto *axis : lastAxisOrder) {
      lastAxisRotAngle.push_back(axis->getRotationAngle());
    }
  }

  for (const auto &it2 : parallelAxis) {
    it2.second->setHidden(true);
  }

  nbAxis = graphProxy->getNumberOfSelectedProperties();
  axisOrder.clear();

  if (nbAxis > 1) {
    width = spaceBetweenAxis * (nbAxis - 1);
  } else {
    width = spaceBetweenAxis;
  }

  float circleLayoutYOffset = (nbAxis * 100.0f) / 4.0f;

  Color axisColor;
  int bgV = backgroundColor.getV();

  if (bgV < 128) {
    axisColor = Color(255, 255, 255);
  } else {
    axisColor = Color(0, 0, 0);
  }

  float maxCaptionWidth = (8.0f / 10.0f) * spaceBetweenAxis;

  if (selectedProperties.size() < 3) {
    layoutType = PARALLEL;
  }

  float rotationAngleBase = 0.0f;

  if (layoutType == CIRCULAR) {
    rotationAngleBase = -(2.0f * M_PI) / selectedProperties.size();
    captionPosition = GlAxis::ABOVE;
  } else {
    captionPosition = GlAxis::BELOW;
  }

  uint cpt = 0;

  for (const string &selectedProp : selectedProperties) {

    ParallelAxis *axis = nullptr;
    float rotationAngle = (cpt++ * rotationAngleBase) * (180.0f / M_PI);
    Coord coord;

    if (layoutType == PARALLEL) {
      if (nbAxis != 1) {
        coord = Coord(firstAxisPos.getX() + pos * (width / (nbAxis - 1)), firstAxisPos.getY());
      } else {
        coord = Coord(firstAxisPos.getX() + (width / 2.0f), firstAxisPos.getY());
      }
    } else {
      coord = Coord(0, circleLayoutYOffset, 0.0f);
    }

    if (parallelAxis.find(selectedProp) != parallelAxis.end()) {
      axis = (parallelAxis.find(selectedProp))->second;

      if (layoutType == PARALLEL) {
        axis->setRotationAngle(0.0f);

        if (!resetAxisLayout && pos < lastAxisCoord.size()) {
          axis->translate(lastAxisCoord[pos] - axis->getBaseCoord());
        } else {
          axis->translate(coord - axis->getBaseCoord());
        }
      } else {
        axis->setBaseCoord(coord);

        if (!resetAxisLayout) {
          axis->setRotationAngle(lastAxisRotAngle[pos]);
        } else {
          axis->setRotationAngle(rotationAngle);
        }
      }

      axis->setCaptionPosition(captionPosition);
      axis->setAxisHeight(height);
      axis->setMaxCaptionWidth(maxCaptionWidth);
      axis->setAxisColor(axisColor);
      axis->redraw();
      axis->setHidden(false);
    } else {
      string typeName((graphProxy->getProperty(selectedProp))->getTypename());

      if (typeName == "string") {
        axis = new NominalParallelAxis(coord, height, maxCaptionWidth, graphProxy, selectedProp,
                                       axisColor, rotationAngle, captionPosition);
      } else if (typeName == "int" || typeName == "double") {
        axis =
            new QuantitativeParallelAxis(coord, height, maxCaptionWidth, graphProxy, selectedProp,
                                         true, axisColor, rotationAngle, captionPosition);
      }
    }

    if (axis != nullptr) {
      axisPlotComposite->addGlEntity(axis, selectedProp);
      axisOrder.push_back(selectedProp);
      parallelAxis[selectedProp] = axis;
      ++pos;
    }

    if (progressBar) {
      progressBar->progress(pos, selectedProperties.size());
      glWidget->draw();
      // needed to display progressBar
      QApplication::processEvents();
    }
  }

  resetAxisLayout = false;
  lastLayouType = layoutType;
}

void ParallelCoordinatesDrawing::destroyAxisIfNeeded() {
  for (const auto &it : parallelAxis) {
    if (!graphProxy->existProperty(it.first)) {
      delete it.second;
      parallelAxis.erase(it.first);
    }
  }
}

void ParallelCoordinatesDrawing::plotAllData(GlWidget *glWidget, GlProgressBar *progressBar) {
  Color color;
  computeResizeFactor();

  int currentStep = 0;
  int maxStep = graphProxy->getDataCount();
  int drawStep = maxStep / 100;

  if (progressBar) {
    progressBar->setComment("Updating parallel coordinates ...");
    progressBar->progress(0, maxStep);
    glWidget->draw();
    // needed to display progressBar
    QApplication::processEvents();
  }

  for (uint dataId : graphProxy->getDataIterator()) {

    if (!graphProxy->isDataSelected(dataId)) {
      color = graphProxy->getDataColor(dataId);

      if (linesColorAlphaValue <= 255 &&
          ((graphProxy->highlightedEltsSet() && graphProxy->isDataHighlighted(dataId)) ||
           (!graphProxy->highlightedEltsSet()))) {
        color.setA(linesColorAlphaValue);
      }
    } else {
      color = glWidget->renderingParameters().getSelectionColor();
    }

    plotData(dataId, color);

    if (progressBar && (++currentStep % drawStep == 0)) {
      progressBar->progress(currentStep, maxStep);
      glWidget->draw();
      // needed to display progressBar
      QApplication::processEvents();
    }
  }

  lastHighlightedElements = graphProxy->getHighlightedElts();
}

void ParallelCoordinatesDrawing::plotData(const uint dataId, const Color &color) {

  Size eltMinSize = graphProxy->getSizeProperty("viewSize")->getMin();
  Size dataViewSize = graphProxy->getDataViewSize(dataId);
  Size adjustedViewSize = axisPointMinSize + resizeFactor * (dataViewSize - eltMinSize);
  float pointRadius =
      ((adjustedViewSize[0] + adjustedViewSize[1] + adjustedViewSize[2]) / 3.0f) / 2.0f;
  float lineHalfWidth = pointRadius - (1.0f / 10.0f) * pointRadius;

  vector<Coord> polylineCoords;
  vector<Coord> splineCurvePassPoints;

  for (const auto &axis : axisOrder) {

    Coord pointCoord = parallelAxis[axis]->getPointCoordOnAxisForData(dataId);
    float axisRotAngle = parallelAxis[axis]->getRotationAngle();
    ostringstream oss;
    oss << "data " << dataId << " var " << axis;

    if (drawPointsOnAxis) {

      if (!graphProxy->highlightedEltsSet() || graphProxy->isDataSelected(dataId)) {
        node n = axisPointsGraph->addNode();
        axisPointsDataMap[n] = dataId;
        axisPointsGraphLayout->setNodeValue(n, pointCoord);
        axisPointsGraphSize->setNodeValue(n, adjustedViewSize);

        if (graphProxy->getDataLocation() == ElementType::NODE) {
          axisPointsGraphShape->setNodeValue(
              n, graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>("viewShape",
                                                                                   dataId));
        } else {
          axisPointsGraphShape->setNodeValue(n, NodeShape::Circle);
        }

        axisPointsGraphLabels->setNodeValue(
            n,
            graphProxy->getPropertyValueForData<StringProperty, StringType>("viewLabel", dataId));
        axisPointsGraphColors->setNodeValue(
            n, graphProxy->getPropertyValueForData<ColorProperty, ColorType>("viewColor", dataId));

        if (graphProxy->isDataSelected(dataId)) {
          axisPointsGraphSelection->setNodeValue(n, true);
        }
      }
    }

    if (linesType == STRAIGHT) {
      if (linesThickness == THICK) {
        Coord vec1 = {0.0f, -lineHalfWidth};
        Coord vec2 = {0.0f, lineHalfWidth};

        if (axisRotAngle != 0.0f) {
          rotateVector(vec1, axisRotAngle, Z_ROT);
          rotateVector(vec2, axisRotAngle, Z_ROT);
        }

        polylineCoords.push_back(pointCoord + vec1);
        polylineCoords.push_back(pointCoord + vec2);
      } else {
        polylineCoords.push_back(pointCoord);
      }
    } else {
      splineCurvePassPoints.push_back(pointCoord);
    }
  }

  if (axisOrder.size() < 2) {
    return;
  }

  GlEntity *line = nullptr;
  bool closeSpline = (layoutType == CIRCULAR);

  if (linesType == STRAIGHT) {
    if (layoutType == CIRCULAR) {
      polylineCoords.push_back(polylineCoords[0]);

      if (linesThickness == THICK) {
        polylineCoords.push_back(polylineCoords[1]);
      }
    }

    if (linesThickness == THICK) {
      auto *polyquad = new GlPolyQuad(polylineCoords, color, lineTextureFilename, true, 1, color);
      polyquad->setOutlined(false);
      line = polyquad;
    } else {
      vector<Color> lineColor;
      lineColor.push_back(color);
      line = new GlLine(polylineCoords, lineColor);
    }
  } else if (linesType == CATMULL_ROM_SPLINE) {
    float size = 1;
    string textureName;

    if (linesThickness == THICK) {
      size = pointRadius;
      textureName = lineTextureFilename;
    }

    auto *catmull = new GlCatmullRomCurve(splineCurvePassPoints, color, color, size, size,
                                          closeSpline, 20 * splineCurvePassPoints.size() - 1);

    if (textureName == DEFAULT_TEXTURE_FILE) {
      catmull->setOutlined(true);
      catmull->setOutlineColor(Color(0, 0, 0, 0));
    }

    catmull->setTexture(textureName);
    line = catmull;
  } else {
    float size = 1;
    string textureName;

    if (linesThickness == THICK) {
      size = pointRadius;
      textureName = lineTextureFilename;
    }

    if (layoutType == CIRCULAR) {
      splineCurvePassPoints.push_back(splineCurvePassPoints[0]);
    }

    auto *cubicInterpolation = new GlCubicBSplineInterpolation(
        splineCurvePassPoints, color, color, size, size, 20 * splineCurvePassPoints.size() - 1);

    if (textureName == DEFAULT_TEXTURE_FILE) {
      cubicInterpolation->setOutlined(true);
      cubicInterpolation->setOutlineColor(Color(0, 0, 0, 0));
    }

    cubicInterpolation->setTexture(textureName);
    line = cubicInterpolation;
  }

  if (graphProxy->isDataHighlighted(dataId)) {
    line->setStencil(4);
  }

  if (graphProxy->isDataSelected(dataId)) {
    line->setStencil(3);
  }

  ostringstream oss2;
  oss2 << "data " << dataId << " line ";
  dataPlotComposite->addGlEntity(line, oss2.str());
  glEntitiesDataMap[line] = dataId;
}

uint ParallelCoordinatesDrawing::nbParallelAxis() const {
  return nbAxis;
}

void ParallelCoordinatesDrawing::swapAxis(ParallelAxis *axis1, ParallelAxis *axis2) {
  int pi = 0, pj = 0;
  int pos = 0;

  for (const auto &axisName : axisOrder) {
    if (axisName == axis1->getAxisName()) {
      pi = pos;
    }

    if (axisName == axis2->getAxisName()) {
      pj = pos;
    }
    ++pos;
  }

  string tmp(axisOrder[pi]);
  axisOrder[pi] = axisOrder[pj];
  axisOrder[pj] = tmp;

  if (layoutType == PARALLEL) {

    Coord ci = parallelAxis[axis1->getAxisName()]->getBaseCoord();
    Coord cj = parallelAxis[axis2->getAxisName()]->getBaseCoord();

    parallelAxis[axis1->getAxisName()]->translate(cj - ci);
    parallelAxis[axis2->getAxisName()]->translate(ci - cj);

  } else {

    float axis1RotAngle = parallelAxis[axis1->getAxisName()]->getRotationAngle();
    parallelAxis[axis1->getAxisName()]->setRotationAngle(
        parallelAxis[axis2->getAxisName()]->getRotationAngle());
    parallelAxis[axis2->getAxisName()]->setRotationAngle(axis1RotAngle);
  }

  graphProxy->setSelectedProperties(axisOrder);

  createAxisFlag = false;
}

bool ParallelCoordinatesDrawing::getDataIdFromGlEntity(GlEntity *glEntity, uint &dataId) {

  bool dataMatch = glEntitiesDataMap.find(glEntity) != glEntitiesDataMap.end();

  if (dataMatch) {
    dataId = glEntitiesDataMap[glEntity];
  }

  return dataMatch;
}

bool ParallelCoordinatesDrawing::getDataIdFromAxisPoint(node axisPoint, uint &dataId) {

  bool dataMatch = axisPointsDataMap.find(axisPoint) != axisPointsDataMap.end();

  if (dataMatch) {
    dataId = axisPointsDataMap[axisPoint];
  }

  return dataMatch;
}

void ParallelCoordinatesDrawing::update(GlWidget *glWidget, bool updateWithoutProgressBar) {

  deleteGlEntity(axisPlotComposite);
  deleteGlEntity(dataPlotComposite);

  destroyAxisIfNeeded();

  // needed to have some feedback
  GlProgressBar *progressBar = nullptr;

  if (!updateWithoutProgressBar) {
    // disable user input
    // before allowing progressBar display
    tlp::disableQtUserInput();

    progressBar = new GlProgressBar(Coord(0.0f, 0.0f, 0.0f), 600, 100,
                                    // use same green color as the highlighting one
                                    // in workspace panel
                                    Color(0xCB, 0xDE, 0x5D));
    progressBar->setComment("Updating parallel coordinates ...");
    progressBar->progress(0, graphProxy->numberOfNodes());
    addGlEntity(progressBar, "progress bar");
    glWidget->draw();
    // needed to display progressBar
    QApplication::processEvents();
  }

  if (createAxisFlag) {
    axisPlotComposite->reset(false);
    createAxis(glWidget, progressBar);
  }

  eraseDataPlot();
  plotAllData(glWidget, progressBar);

  if (progressBar != nullptr) {
    deleteGlEntity(progressBar);
    delete progressBar;
    // re-enable user input
    tlp::enableQtUserInput();
  }

  createAxisFlag = true;

  addGlEntity(dataPlotComposite, "data plot composite");
  addGlEntity(axisPlotComposite, "axis plot composite");
}

void ParallelCoordinatesDrawing::eraseDataPlot() {
  dataPlotComposite->reset(true);
  axisPointsGraph->clear();
  glEntitiesDataMap.clear();
  axisPointsDataMap.clear();
}

void ParallelCoordinatesDrawing::eraseAxisPlot() {
  axisPlotComposite->reset(true);
  parallelAxis.clear();
}

void ParallelCoordinatesDrawing::erase() {
  eraseDataPlot();
  eraseAxisPlot();
}

void ParallelCoordinatesDrawing::removeAxis(ParallelAxis *axis) {
  if (!axisPlotComposite->findKey(axis).empty()) {
    axis->setHidden(true);
    axisPlotComposite->deleteGlEntity(axis);
  }
}

void ParallelCoordinatesDrawing::addAxis(ParallelAxis *axis) {
  if (axisPlotComposite->findKey(axis).empty()) {
    axis->setHidden(false);
    axisPlotComposite->addGlEntity(axis, axis->getAxisName());
  }
}

const vector<string> &ParallelCoordinatesDrawing::getAxisNames() const {
  return axisOrder;
}

void ParallelCoordinatesDrawing::computeResizeFactor() {
  Size eltMinSize = graphProxy->getSizeProperty("viewSize")->getMin();
  Size eltMaxSize = graphProxy->getSizeProperty("viewSize")->getMax();

  Size deltaSize = eltMaxSize - eltMinSize;

  for (uint i = 0; i < 3; ++i) {
    if (deltaSize[i] != 0.0f) {
      resizeFactor[i] = (axisPointMaxSize[i] - axisPointMinSize[i]) / deltaSize[i];
    } else {
      resizeFactor[i] = 0.0f;
    }
  }
}

vector<ParallelAxis *> ParallelCoordinatesDrawing::getAllAxis() {
  vector<ParallelAxis *> axis;

  for (const auto &ax : axisOrder) {
    ParallelAxis *pa = parallelAxis[ax];

    if (pa == nullptr) {
      // on the fly clean up for deleted properties
      parallelAxis.erase(ax);
      continue;
    }

    if (!pa->isHidden()) {
      axis.push_back(pa);
    }
  }

  return axis;
}

void ParallelCoordinatesDrawing::updateWithAxisSlidersRange(
    ParallelAxis *axis, HighlightedEltsSetOp highlightedEltsSetOp) {
  set<uint> dataSubset;

  if (highlightedEltsSetOp == INTERSECTION) {
    const set<uint> &eltsInSlidersRange(axis->getDataInSlidersRange());
    const set<uint> &currentHighlightedElts(graphProxy->getHighlightedElts());
    uint size = eltsInSlidersRange.size() > currentHighlightedElts.size()
                    ? eltsInSlidersRange.size()
                    : currentHighlightedElts.size();
    vector<uint> intersection(size);
    auto intersectionEnd = std::set_intersection(
        eltsInSlidersRange.begin(), eltsInSlidersRange.end(), currentHighlightedElts.begin(),
        currentHighlightedElts.end(), intersection.begin());
    dataSubset = set<uint>(intersection.begin(), intersectionEnd);
  } else if (highlightedEltsSetOp == UNION) {
    const set<uint> &eltsInSlidersRange(axis->getDataInSlidersRange());
    const set<uint> &currentHighlightedElts(graphProxy->getHighlightedElts());

    vector<uint> unionSet(eltsInSlidersRange.size() + currentHighlightedElts.size());
    auto unionEnd = std::set_union(eltsInSlidersRange.begin(), eltsInSlidersRange.end(),
                                   currentHighlightedElts.begin(), currentHighlightedElts.end(),
                                   unionSet.begin());
    dataSubset = set<uint>(unionSet.begin(), unionEnd);
  } else {
    dataSubset = axis->getDataInSlidersRange();
  }

  if (!dataSubset.empty()) {
    graphProxy->unsetHighlightedElts();

    for (auto d : dataSubset) {
      graphProxy->addOrRemoveEltToHighlight(d);
    }

    for (const auto &it2 : parallelAxis) {
      if (it2.second != axis) {
        it2.second->updateSlidersWithDataSubset(dataSubset);
      }
    }

    createAxisFlag = false;
  }
}

void ParallelCoordinatesDrawing::resetAxisSlidersPosition() {
  vector<ParallelAxis *> axis = getAllAxis();

  for (auto *ax : axis) {
    ax->resetSlidersPosition();
  }
}

void ParallelCoordinatesDrawing::delNode(Graph *, const node n) {
  if (graphProxy->getDataLocation() == ElementType::NODE) {
    removeHighlightedElt(n.id);
  }
}

void ParallelCoordinatesDrawing::delEdge(Graph *, const edge e) {
  if (graphProxy->getDataLocation() == ElementType::EDGE) {
    removeHighlightedElt(e.id);
  }
}

void ParallelCoordinatesDrawing::treatEvent(const tlp::Event &evt) {
  const auto *gEvt = dynamic_cast<const GraphEvent *>(&evt);

  if (gEvt) {
    Graph *graph = gEvt->getGraph();

    switch (gEvt->getType()) {
    case GraphEventType::TLP_DEL_NODE:
      delNode(graph, gEvt->getNode());
      break;

    case GraphEventType::TLP_DEL_EDGE:
      delEdge(graph, gEvt->getEdge());
      break;

    default:
      break;
    }
  }
}

void ParallelCoordinatesDrawing::removeHighlightedElt(const uint dataId) {
  if (lastHighlightedElements.erase(dataId)) {
    graphProxy->removeHighlightedElement(dataId);

    if (!graphProxy->highlightedEltsSet()) {
      graphProxy->colorDataAccordingToHighlightedElts();
    }
  }
}
}
