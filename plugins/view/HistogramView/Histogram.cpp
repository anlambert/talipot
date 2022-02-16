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

#include <talipot/GlPolyQuad.h>
#include <talipot/Gl2DRect.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/GlTextureManager.h>

#include "Histogram.h"

using namespace std;

const float DEFAULT_AXIS_LENGTH = 1000.;

const std::string XAXIS_ID = "x axis";
const std::string YAXIS_ID = "y axis";

template <typename T>
std::string getStringFromNumber(T number, uint precision = 5) {
  std::ostringstream oss;
  oss.precision(precision);
  oss << number;
  return oss.str();
}

namespace tlp {

static void setGraphView(GlGraph *glGraph, bool displayEdges) {
  GlGraphRenderingParameters &param = glGraph->renderingParameters();
  param.setAntialiasing(true);
  param.setViewNodeLabel(true);
  param.setFontsType(2);
  param.setSelectedNodesStencil(1);
  param.setNodesStencil(1);
  param.setNodesLabelStencil(1);
  param.setDisplayEdges(displayEdges);
  param.setEdgesStencil(2);
  param.setSelectedEdgesStencil(1);
  param.setDisplayNodes(true);
  param.setDisplayMetaNodes(true);
}

int Histogram::overviewCpt(0);

Histogram::Histogram(Graph *graph, Graph *edgeGraph, std::unordered_map<edge, node> &edgeMap,
                     const std::string &propertyName, const ElementType &dataLocation,
                     const Coord &blCorner, uint size, const Color &backgroundColor,
                     const Color &textColor)
    : graph(graph), propertyName(propertyName), blCorner(blCorner), size(size),
      nbHistogramBins(100), xAxis(nullptr), yAxis(nullptr), xAxisLogScale(false),
      yAxisLogScale(false), nbXGraduations(15), yAxisIncrementStep(0),
      histogramLayout(new LayoutProperty(graph)), histogramEdgeLayout(new LayoutProperty(graph)),
      histogramSize(new SizeProperty(graph)), histoBinsComposite(new GlComposite()),
      uniformQuantification(false), cumulativeFreqHisto(false), lastCumulHisto(false),
      edgeAsNodeGraph(edgeGraph), edgeToNode(edgeMap), backgroundColor(backgroundColor),
      textColor(textColor), integerScale(false), dataLocation(dataLocation), displayEdges(false),
      layoutUpdateNeeded(true), sizesUpdateNeeded(true), updateNeeded(true),
      xAxisScaleDefined(false), yAxisScaleDefined(false), xAxisScale(make_pair(0, 0)),
      yAxisScale(make_pair(0, 0)), initXAxisScale(make_pair(0, 0)),
      initYAxisScale(make_pair(0, 0)) {

  if (dataLocation == ElementType::NODE) {
    _glGraph = new GlGraph(graph);
    GlGraphInputData *glGraphInputData = _glGraph->inputData();
    glGraphInputData->setLayout(histogramLayout);
    glGraphInputData->setSizes(histogramSize);
  } else {
    _glGraph = new GlGraph(edgeAsNodeGraph);
    GlGraphInputData *glGraphInputData = _glGraph->inputData();
    glGraphInputData->setLayout(histogramEdgeLayout);
  }

  setGraphView(_glGraph, (dataLocation == ElementType::NODE) ? displayEdges : false);
  overviewId = overviewCpt++;
  textureName = propertyName + " histo texture " + getStringFromNumber(overviewId);
  update();
}

Histogram::~Histogram() {
  GlTextureManager::deleteTexture(textureName);
  delete histogramLayout;
  delete histogramEdgeLayout;
  delete histogramSize;
  delete histoBinsComposite;
  delete _glGraph;
  delete xAxis;
  delete yAxis;
}

void Histogram::setDataLocation(const ElementType &dataLocation) {
  if (dataLocation != this->dataLocation) {
    delete _glGraph;
    xAxisScaleDefined = false;
    yAxisScaleDefined = false;

    if (dataLocation == ElementType::NODE) {
      _glGraph = new GlGraph(graph);
      GlGraphInputData *glGraphInputData = _glGraph->inputData();
      glGraphInputData->setLayout(histogramLayout);
      glGraphInputData->setSizes(histogramSize);
    } else {
      _glGraph = new GlGraph(edgeAsNodeGraph);
      GlGraphInputData *glGraphInputData = _glGraph->inputData();
      glGraphInputData->setLayout(histogramEdgeLayout);
    }
  }

  this->dataLocation = dataLocation;
}

static uint clamp(uint ui, uint minVal, uint maxVal) {
  return min(max(ui, minVal), maxVal);
}

void Histogram::computeHistogram() {

  histogramBins.clear();
  binMinMaxMap.clear();

  histogramBins.resize(nbHistogramBins);

  if (graph->getProperty(propertyName)->getTypename() == "double") {
    if (dataLocation == ElementType::NODE) {
      min = graph->getDoubleProperty(propertyName)->getNodeMin(graph);
      max = graph->getDoubleProperty(propertyName)->getNodeMax(graph);
    } else {
      min = graph->getDoubleProperty(propertyName)->getEdgeMin(graph);
      max = graph->getDoubleProperty(propertyName)->getEdgeMax(graph);
    }
  } else {
    if (dataLocation == ElementType::NODE) {
      min = graph->getIntegerProperty(propertyName)->getNodeMin(graph);
      max = graph->getIntegerProperty(propertyName)->getNodeMax(graph);
    } else {
      min = graph->getIntegerProperty(propertyName)->getEdgeMin(graph);
      max = graph->getIntegerProperty(propertyName)->getEdgeMax(graph);
    }
  }

  initXAxisScale = make_pair(min, max);

  if (xAxisScaleDefined) {
    if (min > xAxisScale.first) {
      min = xAxisScale.first;
    }

    if (max < xAxisScale.second) {
      max = xAxisScale.second;
    }
  }

  xAxisScale.first = min;
  xAxisScale.second = max;

  if (min == max) {
    max += 1;
  }

  binWidth = (max - min) / nbHistogramBins;

  maxBinSize = 0;

  integerScale = true;

  if (uniformQuantification) {
    DoubleProperty propertyCopy(graph);

    if (graph->getProperty(propertyName)->getTypename() == "double") {
      propertyCopy = *(graph->getDoubleProperty(propertyName));
    } else {
      if (dataLocation == ElementType::NODE) {
        for (auto n : graph->nodes()) {
          propertyCopy.setNodeValue(n, graph->getIntegerProperty(propertyName)->getNodeValue(n));
        }
      } else {
        for (auto e : graph->edges()) {
          propertyCopy.setEdgeValue(e, graph->getIntegerProperty(propertyName)->getEdgeValue(e));
        }
      }
    }

    propertyCopy.uniformQuantification(nbHistogramBins);

    for (uint i = 0; i < nbHistogramBins; ++i) {
      binMinMaxMap[i].first = DBL_MAX;
      binMinMaxMap[i].second = 0;
    }

    if (dataLocation == ElementType::NODE) {

      for (auto n : graph->nodes()) {
        auto binId = uint(propertyCopy.getNodeValue(n));
        histogramBins[binId].push_back(n.id);

        if (histogramBins[binId].size() > maxBinSize) {
          maxBinSize = histogramBins[binId].size();
        }

        double val;

        if (graph->getProperty(propertyName)->getTypename() == "double") {
          val = graph->getDoubleProperty(propertyName)->getNodeValue(n);
        } else {
          val = graph->getIntegerProperty(propertyName)->getNodeValue(n);
        }

        if (val < binMinMaxMap[binId].first) {
          binMinMaxMap[binId].first = val;
        }

        if (val > binMinMaxMap[binId].second) {
          binMinMaxMap[binId].second = val;
        }
      }

    } else {

      for (auto e : graph->edges()) {
        auto binId = uint(propertyCopy.getEdgeValue(e));
        histogramBins[binId].push_back(e.id);

        if (histogramBins[binId].size() > maxBinSize) {
          maxBinSize = histogramBins[binId].size();
        }

        double val;

        if (graph->getProperty(propertyName)->getTypename() == "double") {
          val = graph->getDoubleProperty(propertyName)->getEdgeValue(e);
        } else {
          val = graph->getIntegerProperty(propertyName)->getEdgeValue(e);
        }

        if (val < binMinMaxMap[binId].first) {
          binMinMaxMap[binId].first = val;
        }

        if (val > binMinMaxMap[binId].second) {
          binMinMaxMap[binId].second = val;
        }
      }
    }

    uniformQuantificationAxisLabels.clear();

    for (uint i = 0; i < nbHistogramBins; ++i) {
      if (histogramBins[i].size() > 0) {
        uniformQuantificationAxisLabels.push_back(getStringFromNumber(binMinMaxMap[i].first));
      } else {
        if (histogramBins[i - 1].size() > 0) {
          uniformQuantificationAxisLabels.push_back(
              getStringFromNumber(binMinMaxMap[i - 1].second));
        } else {
          uniformQuantificationAxisLabels.push_back(" ");
        }
      }
    }

    uniformQuantificationAxisLabels.push_back(
        getStringFromNumber(binMinMaxMap[nbHistogramBins - 1].second));
  } else {
    if (dataLocation == ElementType::NODE) {

      for (auto n : graph->nodes()) {
        double value;

        if (graph->getProperty(propertyName)->getTypename() == "double") {
          value = graph->getDoubleProperty(propertyName)->getNodeValue(n);
          double intpart, fracpart;
          fracpart = modf(value, &intpart);
          integerScale = integerScale && (fracpart == 0);
        } else {
          value = graph->getIntegerProperty(propertyName)->getNodeValue(n);
        }

        if (value != max) {
          uint nodeHistoBin = clamp(uint(floor((value - min) / binWidth)), 0, nbHistogramBins - 1);
          histogramBins[nodeHistoBin].push_back(n.id);

          if (histogramBins[nodeHistoBin].size() > maxBinSize) {
            maxBinSize = histogramBins[nodeHistoBin].size();
          }
        } else {
          histogramBins[nbHistogramBins - 1].push_back(n.id);

          if (histogramBins[nbHistogramBins - 1].size() > maxBinSize) {
            maxBinSize = histogramBins[nbHistogramBins - 1].size();
          }
        }
      }

    } else {

      for (auto e : graph->edges()) {
        double value;

        if (graph->getProperty(propertyName)->getTypename() == "double") {
          value = graph->getDoubleProperty(propertyName)->getEdgeValue(e);
          double intpart, fracpart;
          fracpart = modf(value, &intpart);
          integerScale = integerScale && (fracpart == 0);
        } else {
          value = graph->getIntegerProperty(propertyName)->getEdgeValue(e);
        }

        if (value != max) {
          auto nodeHistoBin = uint(floor((value - min) / binWidth));
          histogramBins[nodeHistoBin].push_back(e.id);

          if (histogramBins[nodeHistoBin].size() > maxBinSize) {
            maxBinSize = histogramBins[nodeHistoBin].size();
          }
        } else {
          histogramBins[nbHistogramBins - 1].push_back(e.id);

          if (histogramBins[nbHistogramBins - 1].size() > maxBinSize) {
            maxBinSize = histogramBins[nbHistogramBins - 1].size();
          }
        }
      }
    }
  }
}

void Histogram::createAxis() {

  delete xAxis;
  delete yAxis;

  const float axisLength = DEFAULT_AXIS_LENGTH;

  uint maxAxisValue, minAxisValue = 0;

  if (cumulativeFreqHisto) {
    if (dataLocation == ElementType::NODE) {
      maxAxisValue = graph->numberOfNodes();
    } else {
      maxAxisValue = graph->numberOfEdges();
    }
  } else {
    maxAxisValue = maxBinSize;
  }

  initYAxisScale = make_pair(minAxisValue, maxAxisValue);

  if (yAxisScaleDefined) {
    if (yAxisScale.first < double(minAxisValue)) {
      minAxisValue = uint(yAxisScale.first);
    }

    if (yAxisScale.second > double(maxAxisValue)) {
      maxAxisValue = uint(yAxisScale.second);
    }
  }

  yAxisScale.first = minAxisValue;
  yAxisScale.second = maxAxisValue;

  yAxisIncrementStep = maxAxisValue / 10;

  if (yAxisIncrementStep < 1) {
    yAxisIncrementStep = 1;
  }

  if (lastCumulHisto != cumulativeFreqHisto) {
    uint n;

    if (!lastCumulHisto) {
      n = maxBinSize;
    } else {
      if (dataLocation == ElementType::NODE) {
        n = graph->numberOfNodes();
      } else {
        n = graph->numberOfEdges();
      }
    }

    yAxisIncrementStep = (yAxisIncrementStep * maxAxisValue) / n;

    if (lastCumulHisto) {
      ++yAxisIncrementStep;
    }
  }

  yAxis = new GlQuantitativeAxis(
      (dataLocation == ElementType::NODE ? "number of nodes" : "number of edges"), Coord(0, 0, 0),
      axisLength, GlAxis::VERTICAL_AXIS, textColor);
  yAxis->setAxisParameters(int(minAxisValue), int(maxAxisValue), yAxisIncrementStep,
                           GlAxis::LEFT_OR_BELOW, true);
  yAxis->setLogScale(yAxisLogScale);
  float yAxisGradsWidth = axisLength / 20;
  yAxis->setAxisGradsWidth(yAxisGradsWidth);
  yAxis->updateAxis();
  yAxis->addCaption(GlAxis::BELOW, axisLength / 10, false, (axisLength / 2.), 0, " ");

  xAxis = new GlQuantitativeAxis(propertyName, Coord(0, 0, 0), axisLength, GlAxis::HORIZONTAL_AXIS,
                                 textColor, true);
  float xAxisGradWidth = axisLength / 20;

  if (uniformQuantification) {
    xAxisGradWidth /= 2.;
  }

  xAxis->setAxisGradsWidth(xAxisGradWidth);

  if (uniformQuantification) {
    xAxis->setAxisGraduations(uniformQuantificationAxisLabels, GlAxis::LEFT_OR_BELOW);
  } else {
    if (integerScale) {
      auto axisMin = static_cast<long long>(min);
      auto axisMax = static_cast<long long>(max);

      // check the [min, max] double values range
      // fit in a long long range
      if (axisMax != LLONG_MIN) {
        auto incrementStep = static_cast<long long>((max - min) / nbXGraduations);

        if (incrementStep < 1) {
          incrementStep = 1;
        }

        xAxis->setAxisParameters(axisMin, axisMax, incrementStep, GlAxis::LEFT_OR_BELOW, true);
      } else {
        xAxis->setAxisParameters(min, max, nbXGraduations, GlAxis::LEFT_OR_BELOW, true);
      }
    } else {
      xAxis->setAxisParameters(min, max, nbXGraduations, GlAxis::LEFT_OR_BELOW, true);
    }
  }

  xAxis->setLogScale(xAxisLogScale);
  xAxis->addCaption(GlAxis::BELOW, 100, false, 300, 100, " ");
  xAxis->setAxisGraduationsMaxLabelWidth(300);
  xAxis->updateAxis();

  refSizeX = axisLength / nbHistogramBins;

  if (cumulativeFreqHisto) {
    if (dataLocation == ElementType::NODE) {
      refSizeY =
          yAxis->getAxisPointCoordForValue(graph->numberOfNodes()).getY() / graph->numberOfNodes();
    } else {
      refSizeY =
          yAxis->getAxisPointCoordForValue(graph->numberOfEdges()).getY() / graph->numberOfEdges();
    }
  } else {
    refSizeY = yAxis->getAxisPointCoordForValue(maxBinSize).getY() / maxBinSize;
  }

  refSize = std::min(refSizeX, refSizeY);

  edgeAsNodeGraph->getSizeProperty("viewSize")->setAllNodeValue(Size(refSize, refSize, 0));
}

void Histogram::updateLayout() {
  computeHistogram();
  createAxis();

  uint cumulativeSize = 0;

  for (uint i = 0; i < nbHistogramBins; ++i) {
    uint binSize = histogramBins[i].size();
    cumulativeSize += binSize;
    float binXCoord, binXCoordEnd;

    if (!uniformQuantification) {
      binXCoord = xAxis->getAxisPointCoordForValue(min + i * binWidth).getX();
      binXCoordEnd = xAxis->getAxisPointCoordForValue(min + (i + 1) * binWidth).getX();
    } else {
      binXCoord = i * refSizeX;
      binXCoordEnd = (i + 1) * refSizeX;
    }

    float nodeXCoord = (binXCoord + binXCoordEnd) / 2.f;

    for (uint j = 0; j < binSize; ++j) {
      float nodeYCoord;

      if (!cumulativeFreqHisto) {
        nodeYCoord = (yAxis->getAxisPointCoordForValue(j).getY() +
                      yAxis->getAxisPointCoordForValue(j + 1).getY()) /
                     2.f;

      } else {
        nodeYCoord = (yAxis->getAxisPointCoordForValue(cumulativeSize - j - 1).getY() +
                      yAxis->getAxisPointCoordForValue(cumulativeSize - j).getY()) /
                     2.;
      }

      Coord nodeCoord = {nodeXCoord, nodeYCoord};

      if (dataLocation == ElementType::NODE) {
        histogramLayout->setNodeValue(node(histogramBins[i][j]), nodeCoord);
      } else {
        node n = edgeToNode[edge(histogramBins[i][j])];
        histogramEdgeLayout->setNodeValue(n, nodeCoord);

        vector<Coord> edgeHistoPointCoord;
        edgeHistoPointCoord.push_back(nodeCoord);
        histogramLayout->setEdgeValue(edge(histogramBins[i][j]), edgeHistoPointCoord);
      }
    }
  }

  layoutUpdateNeeded = false;
}

void Histogram::updateSizes() {
  SizeProperty *viewSize = graph->getSizeProperty("viewSize");

  Size resizeFactor;
  Size eltMinSize = viewSize->getMin();
  Size eltMaxSize = viewSize->getMax();
  float minSize = (refSize / 10.);
  Size deltaSize = eltMaxSize - eltMinSize;

  for (uint i = 0; i < 2; ++i) {
    if (deltaSize[i] != 0) {
      resizeFactor[i] = (refSize - minSize) / deltaSize[i];
    } else {
      resizeFactor[i] = 0;
    }
  }

  bool resize = true;

  if (eltMinSize[0] == eltMaxSize[0] && eltMinSize[1] == eltMaxSize[1]) {
    resize = false;
  }

  for (uint i = 0; i < nbHistogramBins; ++i) {

    uint binSize = histogramBins[i].size();

    float binXCoord, binXCoordEnd;

    if (!uniformQuantification) {
      binXCoord = xAxis->getAxisPointCoordForValue(min + i * binWidth).getX();
      binXCoordEnd = xAxis->getAxisPointCoordForValue(min + (i + 1) * binWidth).getX();
    } else {
      binXCoord = i * refSizeX;
      binXCoordEnd = (i + 1) * refSizeX;
    }

    for (uint j = 0; j < binSize; ++j) {
      if (dataLocation == ElementType::NODE) {
        const Size &currentNodeSize = viewSize->getNodeValue(node(histogramBins[i][j]));
        Size newNodeSize;

        if (resize) {
          newNodeSize[0] = minSize + resizeFactor[0] * (currentNodeSize[0] - eltMinSize[0]);
          newNodeSize[1] = minSize + resizeFactor[1] * (currentNodeSize[1] - eltMinSize[1]);
        } else {
          newNodeSize[0] = refSize;
          newNodeSize[1] = refSize;
        }

        if (newNodeSize[0] > (binXCoordEnd - binXCoord)) {
          newNodeSize[0] = (binXCoordEnd - binXCoord);
        }

        newNodeSize[2] = currentNodeSize[2];
        histogramSize->setNodeValue(node(histogramBins[i][j]), newNodeSize);
      }
    }
  }

  sizesUpdateNeeded = false;
}

void Histogram::update() {

  if (!updateNeeded) {
    return;
  }

  if (layoutUpdateNeeded) {
    updateLayout();
  }

  if (sizesUpdateNeeded) {
    updateSizes();
  }

  ostringstream oss;
  edgeAsNodeGraph->getSizeProperty("viewSize")->setAllNodeValue(Size(refSize, refSize, refSize));
  ColorProperty *viewColor = graph->getColorProperty("viewColor");

  reset(true);
  histoBinsComposite->reset(true);

  uint cumulativeSize = 0;
  uint rectCpt = 0;
  auto *cumulativeHistogram = new GlPolyQuad();

  for (uint i = 0; i < nbHistogramBins; ++i) {
    Vec4i quadColorCumul;
    quadColorCumul.fill(0);
    Color quadColor;
    uint binSize = histogramBins[i].size();
    cumulativeSize += binSize;

    for (uint j = 0; j < binSize; ++j) {
      if (dataLocation == ElementType::NODE) {
        for (uint k = 0; k < 4; ++k) {
          quadColorCumul[k] += uint(viewColor->getNodeValue(node(histogramBins[i][j]))[k]);
        }
      }
    }

    float binXCoord, binXCoordEnd;

    if (!uniformQuantification) {
      binXCoord = xAxis->getAxisPointCoordForValue(min + i * binWidth).getX();
      binXCoordEnd = xAxis->getAxisPointCoordForValue(min + (i + 1) * binWidth).getX();
    } else {
      binXCoord = i * refSizeX;
      binXCoordEnd = (i + 1) * refSizeX;
    }

    if (binSize > 0) {
      float binYTop, binYBottom;

      if (!cumulativeFreqHisto) {
        binYTop = yAxis->getAxisPointCoordForValue(binSize).getY();
        binYBottom = yAxis->getAxisPointCoordForValue(0).getY();
      } else {
        binYTop = yAxis->getAxisPointCoordForValue(cumulativeSize).getY();
        binYBottom = yAxis->getAxisPointCoordForValue(cumulativeSize - binSize).getY();
      }

      Coord startLeftVertex = {binXCoord, binYBottom};
      Coord endLeftVertex = {binXCoord, binYTop};
      Coord startRightVertex = {binXCoordEnd, binYBottom};
      Coord endRightVertex = {binXCoordEnd, binYTop};
      vector<Coord> polyQuadCoords;
      polyQuadCoords.push_back(startLeftVertex);
      polyQuadCoords.push_back(startRightVertex);
      polyQuadCoords.push_back(endLeftVertex);
      polyQuadCoords.push_back(endRightVertex);

      if (dataLocation == ElementType::NODE) {
        quadColorCumul /= binSize;
        quadColor =
            Color(quadColorCumul[0], quadColorCumul[1], quadColorCumul[2], quadColorCumul[3]);
      } else {
        quadColor = Color(255, 0, 0);
      }

      auto *binRect =
          new GlPolyQuad(polyQuadCoords, quadColor, BIN_RECT_TEXTURE, true, 1, quadColor);
      binRect->setStencil(1);
      oss.str("");
      oss << "bin rect " << ++rectCpt;
      histoBinsComposite->addGlEntity(binRect, oss.str());
    }

    if (cumulativeFreqHisto) {
      float quadEdgeYTopCoord = yAxis->getAxisPointCoordForValue(cumulativeSize).getY();

      if ((i == 0 || i == (nbHistogramBins - 1)) || binSize > 0) {
        cumulativeHistogram->addQuadEdge(Coord(binXCoord, xAxis->getAxisBaseCoord().getY()),
                                         Coord(binXCoord, quadEdgeYTopCoord, 0), quadColor);

        if (i == (nbHistogramBins - 1)) {
          cumulativeHistogram->addQuadEdge(Coord(binXCoordEnd, xAxis->getAxisBaseCoord().getY()),
                                           Coord(binXCoordEnd, quadEdgeYTopCoord, 0), quadColor);
        }
      }
    }
  }

  if (cumulativeFreqHisto) {
    histoBinsComposite->addGlEntity(cumulativeHistogram, "cumulative histogram");
  } else {
    delete cumulativeHistogram;
  }

  setGraphView(_glGraph, (dataLocation == ElementType::NODE) ? displayEdges : false);

  GlOffscreenRenderer &glOffscreenRenderer = GlOffscreenRenderer::instance();
  glOffscreenRenderer.setViewPortSize(size, size);
  glOffscreenRenderer.clearScene();
  glOffscreenRenderer.setSceneBackgroundColor(backgroundColor);
  glOffscreenRenderer.addGlEntityToScene(xAxis);
  glOffscreenRenderer.addGlEntityToScene(yAxis);
  glOffscreenRenderer.addGlEntityToScene(histoBinsComposite);
  glOffscreenRenderer.addGlGraphToScene(_glGraph);
  glOffscreenRenderer.renderScene(true);
  GLuint textureId = glOffscreenRenderer.getGLTexture(true);
  GlTextureManager::deleteTexture(textureName);
  GlTextureManager::registerExternalTexture(textureName, textureId);
  glOffscreenRenderer.clearScene();

  auto *rectTextured = new Gl2DRect(blCorner.getY() + size, blCorner.getY(), blCorner.getX(),
                                    blCorner.getX() + size, textureName);
  rectTextured->setFillColor(Color(255, 255, 255));
  addGlEntity(rectTextured, textureName + " overview");
  computeBoundingBox();

  lastCumulHisto = cumulativeFreqHisto;

  updateNeeded = false;
}

void Histogram::setBLCorner(const Coord &blCorner) {
  GlComposite::translate(blCorner - this->blCorner);
  this->blCorner = blCorner;
  computeBoundingBox();
}

void Histogram::setTextColor(const Color &color) {
  if (xAxis != nullptr) {
    xAxis->setAxisColor(color);
  }

  if (yAxis != nullptr) {
    yAxis->setAxisColor(color);
  }

  textColor = color;
}

void Histogram::setLayoutUpdateNeeded() {
  layoutUpdateNeeded = true;
  updateNeeded = true;
}

void Histogram::setSizesUpdateNeeded() {
  sizesUpdateNeeded = true;
  updateNeeded = true;
}

void Histogram::setUpdateNeeded() {
  updateNeeded = true;
}
}
