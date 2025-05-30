/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Interactor.h>
#include <talipot/GlRect.h>
#include <talipot/TlpQtTools.h>
#include <talipot/QuickAccessBar.h>
#include <talipot/ViewSettings.h>

#include <QApplication>
#include <QHelpEvent>
#include <QToolTip>

#include "HistogramView.h"
#include "../utils/ViewGraphPropertiesSelectionWidget.h"
#include "HistoOptionsWidget.h"

using namespace std;

const uint OVERVIEW_SIZE = 512;

const string propertiesTypes[] = {"double", "int"};
const uint nbPropertiesTypes = sizeof(propertiesTypes) / sizeof(string);
const vector<string> propertiesTypesFilter(propertiesTypes, propertiesTypes + nbPropertiesTypes);

template <typename T>
std::string getStringFromNumber(T number, uint precision = 5) {
  std::ostringstream oss;
  oss.precision(precision);
  oss << number;
  return oss.str();
}

namespace tlp {

PLUGIN(HistogramView)

HistogramView::HistogramView(const PluginContext *)
    : GlView(true), propertiesSelectionWidget(nullptr), histoOptionsWidget(nullptr),
      xAxisDetail(nullptr), yAxisDetail(nullptr), _histoGraph(nullptr), emptyGraph(nullptr),
      emptyGlGraph(nullptr), histogramsComposite(nullptr), labelsComposite(nullptr),
      axisComposite(nullptr), smallMultiplesView(true), mainLayer(nullptr),
      detailedHistogram(nullptr), sceneRadiusBak(0), zoomFactorBak(0), noDimsLabel(nullptr),
      noDimsLabel1(nullptr), noDimsLabel2(nullptr), emptyRect(nullptr), emptyRect2(nullptr),
      isConstruct(false), lastNbHistograms(0), dataLocation(ElementType::NODE),
      needUpdateHistogram(false), edgeAsNodeGraph(nullptr) {}

HistogramView::~HistogramView() {
  if (isConstruct) {
    if (currentInteractor() != nullptr) {
      currentInteractor()->uninstall();
    }

    delete propertiesSelectionWidget;
    delete histoOptionsWidget;
    delete emptyGlGraph;
    delete histogramsComposite;
    delete labelsComposite;
    delete emptyGraph;
    delete axisComposite;
    delete edgeAsNodeGraph;
  }
}

QList<QWidget *> HistogramView::configurationWidgets() const {
  return QList<QWidget *>() << propertiesSelectionWidget << histoOptionsWidget;
}

void HistogramView::initGlWidget(Graph *) {
  GlLayer *layer = glWidget()->scene()->getLayer("Main");

  if (layer == nullptr) {
    layer = new GlLayer("Main");
    glWidget()->scene()->addExistingLayer(layer);
  }

  mainLayer = layer;

  cleanupGlScene();

  if (emptyGlGraph == nullptr) {
    emptyGraph = newGraph();
    emptyGlGraph = new GlGraph(emptyGraph);
  }

  mainLayer->addGlEntity(emptyGlGraph, "graph");

  if (histogramsComposite == nullptr) {
    histogramsComposite = new GlComposite();
    mainLayer->addGlEntity(histogramsComposite, "overviews composite");
  }

  if (labelsComposite == nullptr) {
    labelsComposite = new GlComposite();
    mainLayer->addGlEntity(labelsComposite, "labels composite");
  }

  if (axisComposite == nullptr) {
    axisComposite = new GlComposite();
  }
}

void HistogramView::cleanupGlScene() {
  if (!smallMultiplesView && detailedHistogram != nullptr) {
    mainLayer->deleteGlEntity(detailedHistogram->getBinsComposite());
  }

  if (axisComposite != nullptr) {
    axisComposite->reset(false);
  }

  if (labelsComposite != nullptr) {
    labelsComposite->reset(true);
  }

  if (histogramsComposite != nullptr) {
    histogramsComposite->reset(true);
    histogramsMap.clear();
  }
}

QuickAccessBar *HistogramView::getQuickAccessBarImpl() {
  auto *_bar = new QuickAccessBarImpl(
      nullptr, QuickAccessBarImpl::QuickAccessButtons(
                   QuickAccessBarImpl::SCREENSHOT | QuickAccessBarImpl::BACKGROUNDCOLOR |
                   QuickAccessBarImpl::SHOWLABELS | QuickAccessBarImpl::LABELSSCALED |
                   QuickAccessBarImpl::SHOWEDGES | QuickAccessBarImpl::NODECOLOR |
                   QuickAccessBarImpl::EDGECOLOR | QuickAccessBarImpl::NODEBORDERCOLOR |
                   QuickAccessBarImpl::LABELCOLOR));
  return _bar;
}

void HistogramView::setState(const DataSet &dataSet) {

  GlWidget *gl = glWidget();

  if (!isConstruct) {
    isConstruct = true;
    gl->installEventFilter(this);
    setOverviewVisible(true);

    propertiesSelectionWidget = new ViewGraphPropertiesSelectionWidget();
    histoOptionsWidget = new HistoOptionsWidget();
    propertiesSelectionWidget->setWidgetEnabled(true);
    histoOptionsWidget->setWidgetEnabled(false);
  }

  GlView::setState(dataSet);

  Graph *lastGraph = _histoGraph;
  _histoGraph = graph();
  destroyHistogramsIfNeeded();

  if (lastGraph == nullptr || lastGraph != _histoGraph) {
    if (lastGraph) {
      lastGraph->removeListener(this);
      lastGraph->getProperty("viewColor")->removeListener(this);
      lastGraph->getProperty("viewLabel")->removeListener(this);
      lastGraph->getProperty("viewSize")->removeListener(this);
      lastGraph->getProperty("viewShape")->removeListener(this);
      lastGraph->getProperty("viewSelection")->removeListener(this);
      lastGraph->getProperty("viewTexture")->removeListener(this);
    }

    initGlWidget(graph());
    detailedHistogram = nullptr;

    if (edgeAsNodeGraph) {
      delete edgeAsNodeGraph;
    }

    if (_histoGraph) {
      edgeAsNodeGraph = tlp::newGraph();
      edgeToNode.clear();
      nodeToEdge.clear();
      for (auto e : _histoGraph->edges()) {
        nodeToEdge[edgeToNode[e] = edgeAsNodeGraph->addNode()] = e;
        edgeAsNodeGraph->getColorProperty("viewColor")
            ->setNodeValue(edgeToNode[e],
                           _histoGraph->getColorProperty("viewColor")->getEdgeValue(e));
        edgeAsNodeGraph->getBooleanProperty("viewSelection")
            ->setNodeValue(edgeToNode[e],
                           _histoGraph->getBooleanProperty("viewSelection")->getEdgeValue(e));
        edgeAsNodeGraph->getStringProperty("viewLabel")
            ->setNodeValue(edgeToNode[e],
                           _histoGraph->getStringProperty("viewLabel")->getEdgeValue(e));
      }
      edgeAsNodeGraph->getIntegerProperty("viewShape")->setAllNodeValue(NodeShape::Circle);
      edgeAsNodeGraph->getBooleanProperty("viewSelection")->addListener(this);
      _histoGraph->addListener(this);
      _histoGraph->getProperty("viewColor")->addListener(this);
      _histoGraph->getProperty("viewLabel")->addListener(this);
      _histoGraph->getProperty("viewSize")->addListener(this);
      _histoGraph->getProperty("viewShape")->addListener(this);
      _histoGraph->getProperty("viewSelection")->addListener(this);
      _histoGraph->getProperty("viewTexture")->addListener(this);
    }
  }

  propertiesSelectionWidget->setWidgetParameters(graph(), propertiesTypesFilter);

  dataSet.get("histo detailed name", detailedHistogramPropertyName);
  Color backgroundColor;

  if (dataSet.get("backgroundColor", backgroundColor)) {
    histoOptionsWidget->setBackgroundColor(backgroundColor);
  }

  flat_hash_map<string, DataSet> histogramParametersMap;
  DataSet histogramParameters;
  int i = 0;
  stringstream ss;
  ss << i;

  while (dataSet.get("histo" + ss.str(), histogramParameters)) {
    string propertyName;
    histogramParameters.get("property name", propertyName);
    selectedProperties.push_back(propertyName);
    histogramParametersMap[propertyName] = histogramParameters;

    ss.str("");
    ss << ++i;
  }

  propertiesSelectionWidget->setSelectedProperties(selectedProperties);

  if (!selectedProperties.empty()) {
    buildHistograms();

    for (const auto &selectedProperty : selectedProperties) {
      uint nbHistogramBins = 0;

      Histogram *histo = histogramsMap[selectedProperty];

      if (histogramParametersMap[selectedProperty].get("nb histogram bins", nbHistogramBins)) {
        histo->setLayoutUpdateNeeded();
        histo->setNbHistogramBins(nbHistogramBins);
      }

      uint nbXGraduations = 0;

      if (histogramParametersMap[selectedProperty].get("x axis nb graduations", nbXGraduations)) {
        histo->setLayoutUpdateNeeded();
        histo->setNbXGraduations(nbXGraduations);
      }

      uint yAxisIncrementStep = 0;

      if (histogramParametersMap[selectedProperty].get("y axis increment step",
                                                       yAxisIncrementStep)) {
        histo->setLayoutUpdateNeeded();
        histo->setYAxisIncrementStep(yAxisIncrementStep);
      }

      bool cumulativeFrequenciesHisto = false;

      if (histogramParametersMap[selectedProperty].get("cumulative frequencies histogram",
                                                       cumulativeFrequenciesHisto)) {
        histo->setLayoutUpdateNeeded();
        histo->setCumulativeHistogram(cumulativeFrequenciesHisto);
        histo->setLastCumulativeHistogram(cumulativeFrequenciesHisto);
      }

      bool uniformQuantification = false;

      if (histogramParametersMap[selectedProperty].get("uniform quantification",
                                                       uniformQuantification)) {
        histo->setLayoutUpdateNeeded();
        histo->setUniformQuantification(uniformQuantification);
      }

      bool xAxisLogScale = false;

      if (histogramParametersMap[selectedProperty].get("x axis logscale", xAxisLogScale)) {
        histo->setLayoutUpdateNeeded();
        histo->setXAxisLogScale(xAxisLogScale);
      }

      bool yAxisLogScale = false;

      if (histogramParametersMap[selectedProperty].get("y axis logscale", yAxisLogScale)) {
        histo->setLayoutUpdateNeeded();
        histo->setYAxisLogScale(yAxisLogScale);
      }

      bool useCustomAxisScale = false;

      if (histogramParametersMap[selectedProperty].get("x axis custom scale", useCustomAxisScale)) {
        histo->setLayoutUpdateNeeded();
        histo->setXAxisScaleDefined(useCustomAxisScale);

        if (useCustomAxisScale) {
          std::pair<double, double> axisScale(0, 0);
          histogramParametersMap[selectedProperty].get("x axis scale min", axisScale.first);
          histogramParametersMap[selectedProperty].get("x axis scale max", axisScale.second);
          histo->setXAxisScale(axisScale);
        }
      }

      if (histogramParametersMap[selectedProperty].get("y axis custom scale", useCustomAxisScale)) {
        histo->setLayoutUpdateNeeded();
        histo->setYAxisScaleDefined(useCustomAxisScale);

        if (useCustomAxisScale) {
          std::pair<double, double> axisScale(0, 0);
          histogramParametersMap[selectedProperty].get("y axis scale min", axisScale.first);
          histogramParametersMap[selectedProperty].get("y axis scale max", axisScale.second);
          histo->setXAxisScale(axisScale);
        }
      }
    }
  }

  unsigned nodes = static_cast<unsigned>(ElementType::NODE);
  dataSet.get("Nodes/Edges", nodes);
  dataLocation = static_cast<ElementType>(nodes);
  propertiesSelectionWidget->setDataLocation(dataLocation);
  viewConfigurationChanged();

  registerTriggers();

  if (!detailedHistogramPropertyName.empty()) {
    Histogram *histo = histogramsMap[detailedHistogramPropertyName];
    histo->update();
    switchFromSmallMultiplesToDetailedView(histo);
  }

  bool quickAccessBarVisible = false;

  if (dataSet.get<bool>("quickAccessBarVisible", quickAccessBarVisible)) {
    needQuickAccessBar = true;
    setQuickAccessBarVisible(quickAccessBarVisible);
  } else {
    setQuickAccessBarVisible(true);
  }
}

DataSet HistogramView::state() const {
  vector<string> selectedPropertiesTmp = vector<string>(selectedProperties);
  map<string, Histogram *> histogramsMapTmp = map<string, Histogram *>(histogramsMap);

  DataSet dataSet = GlView::state();
  dataSet.set("Nodes/Edges", static_cast<unsigned>(dataLocation));

  for (size_t i = 0; i < selectedPropertiesTmp.size(); ++i) {
    std::stringstream ss;
    ss << i;
    DataSet histogramParameters;
    histogramParameters.set("property name", selectedPropertiesTmp[i]);
    histogramParameters.set("nb histogram bins",
                            histogramsMapTmp[selectedPropertiesTmp[i]]->getNbHistogramBins());
    histogramParameters.set("x axis nb graduations",
                            histogramsMapTmp[selectedPropertiesTmp[i]]->getNbXGraduations());
    histogramParameters.set("y axis increment step",
                            histogramsMapTmp[selectedPropertiesTmp[i]]->getYAxisIncrementStep());
    histogramParameters.set(
        "cumulative frequencies histogram",
        histogramsMapTmp[selectedPropertiesTmp[i]]->cumulativeFrequenciesHistogram());
    histogramParameters.set(
        "uniform quantification",
        histogramsMapTmp[selectedPropertiesTmp[i]]->uniformQuantificationHistogram());
    histogramParameters.set("x axis logscale",
                            histogramsMapTmp[selectedPropertiesTmp[i]]->xAxisLogScaleSet());
    histogramParameters.set("y axis logscale",
                            histogramsMapTmp[selectedPropertiesTmp[i]]->yAxisLogScaleSet());
    bool customScale = histogramsMapTmp[selectedPropertiesTmp[i]]->getXAxisScaleDefined();
    histogramParameters.set("x axis custom scale", customScale);

    if (customScale) {
      const std::pair<double, double> &scale =
          histogramsMapTmp[selectedPropertiesTmp[i]]->getXAxisScale();
      histogramParameters.set("x axis scale min", scale.first);
      histogramParameters.set("x axis scale max", scale.second);
    }

    customScale = histogramsMapTmp[selectedPropertiesTmp[i]]->getYAxisScaleDefined();
    histogramParameters.set("y axis custom scale", customScale);

    if (customScale) {
      const std::pair<double, double> &scale =
          histogramsMapTmp[selectedPropertiesTmp[i]]->getYAxisScale();
      histogramParameters.set("y axis scale min", scale.first);
      histogramParameters.set("y axis scale max", scale.second);
    }

    dataSet.set("histo" + ss.str(), histogramParameters);
  }

  dataSet.set("backgroundColor", glWidget()->scene()->getBackgroundColor());
  string histoDetailedNamed;

  if (detailedHistogram != nullptr) {
    histoDetailedNamed = detailedHistogram->getPropertyName();
  }

  dataSet.set("histo detailed name", histoDetailedNamed);

  if (needQuickAccessBar) {
    dataSet.set("quickAccessBarVisible", quickAccessBarVisible());
  }

  return dataSet;
}

bool HistogramView::eventFilter(QObject *object, QEvent *event) {
  if (xAxisDetail != nullptr && event->type() == QEvent::ToolTip &&
      !detailedHistogram->uniformQuantificationHistogram()) {
    GlWidget *glw = glWidget();
    auto *he = static_cast<QHelpEvent *>(event);
    float x = glw->width() - he->pos().x();
    float y = he->pos().y();
    Coord screenCoords = {x, y};
    Coord sceneCoords = glw->scene()->getLayer("Main")->getCamera().viewportTo3DWorld(
        glw->screenToViewport(screenCoords));
    BoundingBox xAxisBB = xAxisDetail->getBoundingBox();

    if (sceneCoords.getX() > xAxisBB[0][0] && sceneCoords.getX() < xAxisBB[1][0] &&
        sceneCoords.getY() > xAxisBB[0][1] && sceneCoords.getY() < xAxisBB[1][1]) {
      double val = xAxisDetail->getValueForAxisPoint(sceneCoords);
      string valStr(getStringFromNumber(val));
      QToolTip::showText(he->globalPos(), tlp::tlpStringToQString(valStr));
    }

    return true;
  }

  return GlView::eventFilter(object, event);
}

void HistogramView::addEmptyViewLabel() {

  Color backgroundColor = histoOptionsWidget->getBackgroundColor();
  glWidget()->scene()->setBackgroundColor(backgroundColor);

  Color foregroundColor;
  int bgV = backgroundColor.getV();

  if (bgV < 128) {
    foregroundColor = Color(255, 255, 255);
  } else {
    foregroundColor = Color(0, 0, 0);
  }

  if (noDimsLabel == nullptr) {
    noDimsLabel = new GlLabel(Coord(0, 0, 0), Size(200, 200), foregroundColor);
    noDimsLabel->setText(ViewName::HistogramViewName);
    noDimsLabel1 = new GlLabel(Coord(0, -50, 0), Size(400, 200), foregroundColor);
    noDimsLabel1->setText("No graph properties selected.");
    noDimsLabel2 = new GlLabel(Coord(0, -100, 0), Size(700, 200), foregroundColor);
    noDimsLabel2->setText("Go to the \"Properties\" tab in top right corner.");
  } else {
    noDimsLabel->setColor(foregroundColor);
    noDimsLabel1->setColor(foregroundColor);
    noDimsLabel2->setColor(foregroundColor);
  }

  mainLayer->addGlEntity(noDimsLabel, "no dimensions label");
  mainLayer->addGlEntity(noDimsLabel1, "no dimensions label 1");
  mainLayer->addGlEntity(noDimsLabel2, "no dimensions label 2");
}

void HistogramView::removeEmptyViewLabel() {
  if (noDimsLabel != nullptr) {
    mainLayer->deleteGlEntity(noDimsLabel);
    delete noDimsLabel;
    noDimsLabel = nullptr;
    mainLayer->deleteGlEntity(noDimsLabel1);
    delete noDimsLabel1;
    noDimsLabel1 = nullptr;
    mainLayer->deleteGlEntity(noDimsLabel2);
    delete noDimsLabel2;
    noDimsLabel2 = nullptr;
  }
}

void HistogramView::viewConfigurationChanged() {
  glWidget()->scene()->setBackgroundColor(histoOptionsWidget->getBackgroundColor());
  bool dataLocationChanged = propertiesSelectionWidget->getDataLocation() != dataLocation;

  if (dataLocationChanged) {
    histogramsComposite->reset(true);
    axisComposite->reset(false);
    histogramsMap.clear();
    detailedHistogram = nullptr;
  }

  buildHistograms();

  if (detailedHistogram != nullptr && lastNbHistograms != 0 && !dataLocationChanged) {

    detailedHistogram->setNbHistogramBins(histoOptionsWidget->getNbOfHistogramBins());
    detailedHistogram->setNbXGraduations(histoOptionsWidget->getNbXGraduations());
    detailedHistogram->setYAxisIncrementStep(histoOptionsWidget->getYAxisIncrementStep());
    detailedHistogram->setXAxisLogScale(histoOptionsWidget->xAxisLogScaleSet());
    detailedHistogram->setYAxisLogScale(histoOptionsWidget->yAxisLogScaleSet());
    detailedHistogram->setCumulativeHistogram(histoOptionsWidget->cumulativeFrequenciesHisto());
    detailedHistogram->setUniformQuantification(histoOptionsWidget->uniformQuantification());
    detailedHistogram->setDisplayGraphEdges(histoOptionsWidget->showGraphEdges());
    detailedHistogram->setXAxisScaleDefined(histoOptionsWidget->useCustomXAxisScale());
    detailedHistogram->setXAxisScale(histoOptionsWidget->getXAxisScale());
    detailedHistogram->setYAxisScaleDefined(histoOptionsWidget->useCustomYAxisScale());
    detailedHistogram->setYAxisScale(histoOptionsWidget->getYAxisScale());
    detailedHistogram->setLayoutUpdateNeeded();
    detailedHistogram->update();
    histoOptionsWidget->setBinWidth(detailedHistogram->getHistogramBinsWidth());
    histoOptionsWidget->setYAxisIncrementStep(detailedHistogram->getYAxisIncrementStep());
  }

  updateHistograms(detailedHistogram);
  draw();
  drawOverview(true);
}

void HistogramView::draw() {
  GlWidget *gl = glWidget();

  if (selectedProperties.empty()) {
    if (!interactors().empty()) {
      setCurrentInteractor(interactors().front());
    }

    if (!smallMultiplesView) {
      switchFromDetailedViewToSmallMultiples();
    }

    removeEmptyViewLabel();
    addEmptyViewLabel();
    gl->centerScene();

    if (quickAccessBarVisible()) {
      _quickAccessBar->setEnabled(false);
    }

    return;
  }

  if (quickAccessBarVisible()) {
    _quickAccessBar->setEnabled(true);
  }

  if (detailedHistogram != nullptr) {
    needUpdateHistogram = true;
    detailedHistogram->update();
    updateDetailedHistogramAxis();
  } else {
    updateHistograms();
  }

  if (!smallMultiplesView && detailedHistogram != nullptr) {
    switchFromSmallMultiplesToDetailedView(detailedHistogram);
  }

  if (!selectedProperties.empty()) {
    removeEmptyViewLabel();
  }

  if (!smallMultiplesView &&
      (detailedHistogram == nullptr || (selectedProperties.size() > 1 && lastNbHistograms == 1))) {
    switchFromDetailedViewToSmallMultiples();
  }

  if (selectedProperties.size() == 1) {
    switchFromSmallMultiplesToDetailedView(histogramsMap[selectedProperties[0]]);
    propertiesSelectionWidget->setWidgetEnabled(true);
  }

  if (lastNbHistograms != selectedProperties.size()) {
    centerView();
    lastNbHistograms = selectedProperties.size();
    return;
  }

  gl->draw();
  lastNbHistograms = selectedProperties.size();
}

void HistogramView::refresh() {
  glWidget()->redraw();
}

void HistogramView::graphChanged(Graph *) {
  // We copy the value of "Nodes/Edges"
  // in the new state in order to keep
  // the user choice when changing graph
  DataSet oldDs = state();
  unsigned nodes = static_cast<unsigned>(ElementType::NODE);
  oldDs.get("Nodes/Edges", nodes);
  DataSet newDs;
  newDs.set("Nodes/Edges", nodes);
  setState(newDs);
  drawOverview();
}

void HistogramView::buildHistograms() {

  glWidget()->makeCurrent();

  histogramsComposite->reset(false);
  labelsComposite->reset(true);

  selectedProperties = propertiesSelectionWidget->getSelectedGraphProperties();
  dataLocation = propertiesSelectionWidget->getDataLocation();

  if (selectedProperties.empty()) {
    return;
  }

  float spaceBetweenOverviews = OVERVIEW_SIZE / 10.f;
  float labelHeight = OVERVIEW_SIZE / 6.0f;

  float squareRoot = sqrt(float(selectedProperties.size()));
  const uint N =
      uint(squareRoot) + (fmod(float(selectedProperties.size()), squareRoot) == 0.f ? 0u : 1u);

  Color backgroundColor = histoOptionsWidget->getBackgroundColor();
  glWidget()->scene()->setBackgroundColor(backgroundColor);

  Color foregroundColor;
  int bgV = backgroundColor.getV();

  if (bgV < 128) {
    foregroundColor = Color::White;
  } else {
    foregroundColor = Color::Black;
  }

  vector<GlLabel *> propertiesLabels;
  float minSize = FLT_MIN;

  // disable user input
  // before allowing some display feedback
  tlp::disableQtUserInput();

  for (size_t i = 0; i < selectedProperties.size(); ++i) {

    uint row = i / N;
    uint col = i % N;

    Coord overviewBLCorner = {
        col * (OVERVIEW_SIZE + spaceBetweenOverviews),
        -(labelHeight + row * (OVERVIEW_SIZE + spaceBetweenOverviews + labelHeight))};
    ostringstream oss;
    oss << "histogram overview for property " << selectedProperties[i];

    if (!histogramsMap.contains(selectedProperties[i])) {
      auto *histoOverview = new Histogram(_histoGraph, edgeAsNodeGraph, edgeToNode,
                                          selectedProperties[i], dataLocation, overviewBLCorner,
                                          OVERVIEW_SIZE, backgroundColor, foregroundColor);
      histogramsMap[selectedProperties[i]] = histoOverview;
    } else {
      histogramsMap[selectedProperties[i]]->setDataLocation(dataLocation);
      histogramsMap[selectedProperties[i]]->setBLCorner(overviewBLCorner);
      histogramsMap[selectedProperties[i]]->setBackgroundColor(backgroundColor);
      histogramsMap[selectedProperties[i]]->setTextColor(foregroundColor);
    }

    histogramsComposite->addGlEntity(histogramsMap[selectedProperties[i]], oss.str());

    auto *propertyLabel =
        new GlLabel(Coord(overviewBLCorner.getX() + OVERVIEW_SIZE / 2,
                          overviewBLCorner.getY() - labelHeight / 2, 0),
                    Size((8.f / 10.f) * OVERVIEW_SIZE, labelHeight), foregroundColor);
    propertyLabel->setText(selectedProperties[i]);
    propertiesLabels.push_back(propertyLabel);

    if (i == 0) {
      minSize = propertyLabel->getHeightAfterScale();
    } else {
      if (minSize > propertyLabel->getHeightAfterScale()) {
        minSize = propertyLabel->getHeightAfterScale();
      }
    }

    labelsComposite->addGlEntity(propertyLabel, selectedProperties[i] + " label");

    if (selectedProperties.size() == 1 ||
        (detailedHistogramPropertyName == selectedProperties[i])) {
      detailedHistogram = histogramsMap[selectedProperties[i]];
    }

    // add some feedback
    if (i % 10 == 0) {
      QApplication::processEvents();
    }
  }

  // re-enable user input
  tlp::enableQtUserInput();

  for (auto *label : propertiesLabels) {
    label->setSize(Size(label->getSize()[0], minSize));
  }
}

void HistogramView::updateHistograms(Histogram *detailOverview) {
  needUpdateHistogram = false;
  glWidget()->makeCurrent();

  for (auto &prop : selectedProperties) {
    auto *histo = histogramsMap[prop];
    if (histo != detailOverview) {
      histo->setUpdateNeeded();
      histo->update();
    }
  }
}

vector<Histogram *> HistogramView::getHistograms() const {
  vector<Histogram *> ret;

  for (const auto &prop : selectedProperties) {
    ret.push_back(histogramsMap.find(prop)->second);
  }

  return ret;
}

void HistogramView::destroyHistogramsIfNeeded() {
  vector<string> propertiesToRemove;

  for (auto &prop : selectedProperties) {
    if (!_histoGraph || !_histoGraph->existProperty(prop)) {
      auto it = histogramsMap.find(prop);
      if (it->second == detailedHistogram) {
        if (!smallMultiplesView) {
          mainLayer->deleteGlEntity(detailedHistogram->getBinsComposite());
        }

        detailedHistogram = nullptr;
      }

      propertiesToRemove.push_back(prop);
      delete it->second;
      histogramsMap.erase(it);
    }
  }

  for (auto &prop : propertiesToRemove) {
    selectedProperties.erase(remove(selectedProperties.begin(), selectedProperties.end(), prop),
                             selectedProperties.end());
  }
}

void HistogramView::switchFromSmallMultiplesToDetailedView(Histogram *histogramToDetail) {
  if (!histogramToDetail) {
    return;
  }

  if (smallMultiplesView) {
    sceneRadiusBak = glWidget()->scene()->graphCamera().getSceneRadius();
    zoomFactorBak = glWidget()->scene()->graphCamera().getZoomFactor();
    eyesBak = glWidget()->scene()->graphCamera().getEyes();
    centerBak = glWidget()->scene()->graphCamera().getCenter();
    upBak = glWidget()->scene()->graphCamera().getUp();
  }

  mainLayer->deleteGlEntity(histogramsComposite);
  mainLayer->deleteGlEntity(labelsComposite);

  if (detailedHistogram) {
    _histoGraph->getProperty(detailedHistogram->getPropertyName())->removeListener(this);
  }

  detailedHistogram = histogramToDetail;
  detailedHistogramPropertyName = detailedHistogram->getPropertyName();
  _histoGraph->getProperty(detailedHistogramPropertyName)->addListener(this);

  updateDetailedHistogramAxis();

  mainLayer->addGlEntity(axisComposite, "axis composite");
  mainLayer->addGlEntity(histogramToDetail->getBinsComposite(), "bins composite");

  float offset = detailedHistogram->getYAxis()->getMaxLabelWidth() + 90;
  Coord brCoord = detailedHistogram->getYAxis()->getAxisBaseCoord() - Coord(offset, 0, 0);
  Coord tlCoord = detailedHistogram->getYAxis()->getAxisBaseCoord() - Coord(offset + 65, 0, 0) +
                  Coord(0, detailedHistogram->getYAxis()->getAxisLength());
  delete emptyRect;
  emptyRect = new GlRect(tlCoord, brCoord, Color(0, 0, 0, 0), Color(0, 0, 0, 0));

  float offset2 = (detailedHistogram->getXAxis()->getAxisGradsWidth() / 2.) +
                  detailedHistogram->getXAxis()->getLabelHeight();
  Coord tlCoord2 = detailedHistogram->getXAxis()->getAxisBaseCoord() - Coord(0, offset2, 0);
  Coord brCoord2 = detailedHistogram->getXAxis()->getAxisBaseCoord() +
                   Coord(detailedHistogram->getXAxis()->getAxisLength(), 0, 0) -
                   Coord(0, offset2 + 60, 0);
  delete emptyRect2;
  emptyRect2 = new GlRect(tlCoord2, brCoord2, Color(0, 0, 0, 0), Color(0, 0, 0, 0));

  mainLayer->addGlEntity(emptyRect, "emptyRect");
  mainLayer->addGlEntity(emptyRect2, "emptyRect2");

  mainLayer->addGlEntity(histogramToDetail->glGraph(), "graph");

  toggleInteractors(true);

  if (smallMultiplesView) {
    centerView();
  }

  smallMultiplesView = false;

  if (selectedProperties.size() > 1) {
    propertiesSelectionWidget->setWidgetEnabled(false);
  }

  histoOptionsWidget->setWidgetEnabled(true);

  histoOptionsWidget->enableShowGraphEdgesCB(dataLocation == ElementType::NODE);
  histoOptionsWidget->setUniformQuantification(detailedHistogram->uniformQuantificationHistogram());
  histoOptionsWidget->setNbOfHistogramBins(detailedHistogram->getNbHistogramBins());
  histoOptionsWidget->setBinWidth(detailedHistogram->getHistogramBinsWidth());
  histoOptionsWidget->setYAxisIncrementStep(detailedHistogram->getYAxisIncrementStep());
  histoOptionsWidget->setYAxisLogScale(detailedHistogram->yAxisLogScaleSet());
  histoOptionsWidget->setNbXGraduations(detailedHistogram->getNbXGraduations());
  histoOptionsWidget->setXAxisLogScale(detailedHistogram->xAxisLogScaleSet());
  histoOptionsWidget->setCumulativeFrequenciesHistogram(
      detailedHistogram->cumulativeFrequenciesHistogram());
  histoOptionsWidget->setShowGraphEdges(detailedHistogram->displayGraphEdges());
  histoOptionsWidget->useCustomXAxisScale(detailedHistogram->getXAxisScaleDefined());
  histoOptionsWidget->setXAxisScale(detailedHistogram->getXAxisScale());
  histoOptionsWidget->useCustomYAxisScale(detailedHistogram->getYAxisScaleDefined());
  histoOptionsWidget->setYAxisScale(detailedHistogram->getYAxisScale());
  histoOptionsWidget->setInitXAxisScale(detailedHistogram->getInitXAxisScale());
  histoOptionsWidget->setInitYAxisScale(detailedHistogram->getInitYAxisScale());

  glWidget()->draw();
}

void HistogramView::switchFromDetailedViewToSmallMultiples() {

  if (needUpdateHistogram) {
    updateHistograms();
  }

  mainLayer->addGlEntity(emptyGlGraph, "graph");

  mainLayer->deleteGlEntity(axisComposite);
  mainLayer->deleteGlEntity(emptyRect);
  mainLayer->deleteGlEntity(emptyRect2);
  delete emptyRect;
  delete emptyRect2;
  emptyRect = nullptr;
  emptyRect2 = nullptr;

  if (detailedHistogram != nullptr) {
    mainLayer->deleteGlEntity(detailedHistogram->getBinsComposite());
  }

  detailedHistogram = nullptr;
  detailedHistogramPropertyName = "";
  GlWidget *gl = glWidget();
  xAxisDetail = nullptr;
  yAxisDetail = nullptr;
  mainLayer->addGlEntity(histogramsComposite, "overviews composite");
  mainLayer->addGlEntity(labelsComposite, "labels composite");
  Camera &cam = gl->scene()->graphCamera();
  cam.setSceneRadius(sceneRadiusBak);
  cam.setZoomFactor(zoomFactorBak);
  cam.setEyes(eyesBak);
  cam.setCenter(centerBak);
  cam.setUp(upBak);

  smallMultiplesView = true;

  toggleInteractors(false);
  propertiesSelectionWidget->setWidgetEnabled(true);
  histoOptionsWidget->setWidgetEnabled(false);
  histoOptionsWidget->resetAxisScale();

  gl->draw();
}

void HistogramView::toggleInteractors(const bool activate) {
  View::toggleInteractors(activate, {InteractorName::HistogramInteractorNavigation});
}

void HistogramView::updateDetailedHistogramAxis() {
  GlQuantitativeAxis *xAxis = detailedHistogram->getXAxis();
  GlQuantitativeAxis *yAxis = detailedHistogram->getYAxis();
  xAxis->addCaption(GlAxis::BELOW, 100, false, 300, 155, detailedHistogram->getPropertyName());
  yAxis->addCaption(GlAxis::LEFT, 100, false, 300, 155,
                    (dataLocation == ElementType::NODE ? "number of nodes" : "number of edges"));

  if (xAxis->getCaptionHeight() > yAxis->getCaptionHeight()) {
    xAxis->setCaptionHeight(yAxis->getCaptionHeight(), false);
  } else {
    yAxis->setCaptionHeight(xAxis->getCaptionHeight(), false);
  }

  axisComposite->reset(false);
  axisComposite->addGlEntity(xAxis, "x axis");
  axisComposite->addGlEntity(yAxis, "y axis");

  if (xAxis->getSpaceBetweenAxisGrads() > yAxis->getSpaceBetweenAxisGrads()) {
    xAxis->setGradsLabelsHeight(yAxis->getSpaceBetweenAxisGrads() / 2.);
  } else {
    yAxis->setGradsLabelsHeight(xAxis->getSpaceBetweenAxisGrads() / 2.);
  }

  xAxisDetail = xAxis;
  yAxisDetail = yAxis;
}

BoundingBox HistogramView::getSmallMultiplesBoundingBox() const {
  GlBoundingBoxSceneVisitor glBBSV(nullptr);
  histogramsComposite->acceptVisitor(&glBBSV);
  labelsComposite->acceptVisitor(&glBBSV);
  return glBBSV.getBoundingBox();
}

void HistogramView::registerTriggers() {
  for (auto *obs : triggers()) {
    removeRedrawTrigger(obs);
  }

  if (graph()) {
    addRedrawTrigger(graph());
    for (auto *prop : graph()->getObjectProperties()) {
      addRedrawTrigger(prop);
    }
  }
}

void HistogramView::interactorsInstalled(const QList<tlp::Interactor *> &) {
  toggleInteractors(false);
}

void HistogramView::applySettings() {
  if (propertiesSelectionWidget->configurationChanged() ||
      histoOptionsWidget->configurationChanged()) {
    viewConfigurationChanged();
  }
}

void HistogramView::treatEvent(const Event &message) {
  if (typeid(message) == typeid(GraphEvent)) {
    const auto *graphEvent = dynamic_cast<const GraphEvent *>(&message);

    if (graphEvent) {
      if (graphEvent->getType() == GraphEventType::TLP_ADD_NODE) {
        addNode(graphEvent->getGraph(), graphEvent->getNode());
      }

      if (graphEvent->getType() == GraphEventType::TLP_ADD_EDGE) {
        addEdge(graphEvent->getGraph(), graphEvent->getEdge());
      }

      if (graphEvent->getType() == GraphEventType::TLP_DEL_NODE) {
        delNode(graphEvent->getGraph(), graphEvent->getNode());
      }

      if (graphEvent->getType() == GraphEventType::TLP_DEL_EDGE) {
        delEdge(graphEvent->getGraph(), graphEvent->getEdge());
      }
    }
  }

  if (typeid(message) == typeid(PropertyEvent)) {
    const auto *propertyEvent = dynamic_cast<const PropertyEvent *>(&message);

    if (propertyEvent) {
      if (propertyEvent->getType() == PropertyEventType::TLP_AFTER_SET_NODE_VALUE) {
        afterSetNodeValue(propertyEvent->getProperty(), propertyEvent->getNode());
      }

      if (propertyEvent->getType() == PropertyEventType::TLP_AFTER_SET_EDGE_VALUE) {
        afterSetEdgeValue(propertyEvent->getProperty(), propertyEvent->getEdge());
      }

      if (propertyEvent->getType() == PropertyEventType::TLP_AFTER_SET_ALL_NODE_VALUE) {
        afterSetAllNodeValue(propertyEvent->getProperty());
      }

      if (propertyEvent->getType() == PropertyEventType::TLP_AFTER_SET_ALL_EDGE_VALUE) {
        afterSetAllEdgeValue(propertyEvent->getProperty());
      }
    }
  }
}

void HistogramView::afterSetNodeValue(PropertyInterface *p, const node n) {
  if (p->getGraph() == edgeAsNodeGraph && p->getName() == "viewSelection") {
    auto *edgeAsNodeGraphSelection = static_cast<BooleanProperty *>(p);
    BooleanProperty *viewSelection = _histoGraph->getBooleanProperty("viewSelection");
    viewSelection->removeListener(this);
    viewSelection->setEdgeValue(nodeToEdge[n], edgeAsNodeGraphSelection->getNodeValue(n));
    viewSelection->addListener(this);
    setUpdateNeeded();
    return;
  }

  afterSetAllNodeValue(p);
}

void HistogramView::afterSetEdgeValue(PropertyInterface *p, const edge e) {
  if (!edgeToNode.contains(e)) {
    return;
  }

  if (p->getName() == "viewColor") {
    ColorProperty *edgeAsNodeGraphColors = edgeAsNodeGraph->getColorProperty("viewColor");
    auto *viewColor = static_cast<ColorProperty *>(p);
    edgeAsNodeGraphColors->setNodeValue(edgeToNode[e], viewColor->getEdgeValue(e));
    setUpdateNeeded();
  } else if (p->getName() == "viewLabel") {
    StringProperty *edgeAsNodeGraphLabels = edgeAsNodeGraph->getStringProperty("viewLabel");
    auto *viewLabel = static_cast<StringProperty *>(p);
    edgeAsNodeGraphLabels->setNodeValue(edgeToNode[e], viewLabel->getEdgeValue(e));
  } else if (p->getName() == "viewSelection") {
    BooleanProperty *edgeAsNodeGraphSelection =
        edgeAsNodeGraph->getBooleanProperty("viewSelection");
    auto *viewSelection = static_cast<BooleanProperty *>(p);
    edgeAsNodeGraphSelection->removeListener(this);

    if (edgeAsNodeGraphSelection->getNodeValue(edgeToNode[e]) != viewSelection->getEdgeValue(e)) {
      edgeAsNodeGraphSelection->setNodeValue(edgeToNode[e], viewSelection->getEdgeValue(e));
    }

    edgeAsNodeGraphSelection->addListener(this);
    setUpdateNeeded();
  }
}

void HistogramView::afterSetAllNodeValue(PropertyInterface *p) {
  if (detailedHistogram && p->getName() == detailedHistogram->getPropertyName()) {
    setLayoutUpdateNeeded();
  } else if (p->getName() == "viewSize") {
    setSizesUpdateNeeded();
  } else if (p->getName() == "viewSelection") {
    if (p->getGraph() == edgeAsNodeGraph) {
      auto *edgeAsNodeGraphSelection = static_cast<BooleanProperty *>(p);
      BooleanProperty *viewSelection = _histoGraph->getBooleanProperty("viewSelection");
      viewSelection->setAllEdgeValue(
          edgeAsNodeGraphSelection->getNodeValue(edgeAsNodeGraph->getOneNode()));
    }

    setUpdateNeeded();
  } else if (p->getName() == "viewColor" || p->getName() == "viewShape" ||
             p->getName() == "viewTexture") {
    setUpdateNeeded();
  }
}

void HistogramView::afterSetAllEdgeValue(PropertyInterface *p) {

  if (detailedHistogram && p->getName() == detailedHistogram->getPropertyName()) {
    setLayoutUpdateNeeded();
  }

  if (p->getName() == "viewColor") {
    ColorProperty *edgeAsNodeGraphColors = edgeAsNodeGraph->getColorProperty("viewColor");
    auto *viewColor = static_cast<ColorProperty *>(p);
    edgeAsNodeGraphColors->setAllNodeValue(viewColor->getEdgeDefaultValue());
    setUpdateNeeded();
  } else if (p->getName() == "viewLabel") {
    StringProperty *edgeAsNodeGraphLabels = edgeAsNodeGraph->getStringProperty("viewLabel");
    auto *viewLabel = static_cast<StringProperty *>(p);
    edgeAsNodeGraphLabels->setAllNodeValue(viewLabel->getEdgeDefaultValue());
  } else if (p->getName() == "viewSelection") {
    BooleanProperty *edgeAsNodeGraphSelection =
        edgeAsNodeGraph->getBooleanProperty("viewSelection");
    auto *viewSelection = static_cast<BooleanProperty *>(p);
    for (auto e : _histoGraph->edges()) {
      if (edgeAsNodeGraphSelection->getNodeValue(edgeToNode[e]) != viewSelection->getEdgeValue(e)) {
        edgeAsNodeGraphSelection->setNodeValue(edgeToNode[e], viewSelection->getEdgeValue(e));
      }
    }

    setUpdateNeeded();
  }
}

void HistogramView::addNode(Graph *, const node) {
  setLayoutUpdateNeeded();
  setSizesUpdateNeeded();
}

void HistogramView::addEdge(Graph *, const edge e) {
  edgeToNode[e] = edgeAsNodeGraph->addNode();
  setLayoutUpdateNeeded();
  setSizesUpdateNeeded();
}

void HistogramView::delNode(Graph *, const node) {
  setLayoutUpdateNeeded();
  setSizesUpdateNeeded();
}

void HistogramView::delEdge(Graph *, const edge e) {
  edgeAsNodeGraph->delNode(edgeToNode[e]);
  edgeToNode.erase(e);
  setLayoutUpdateNeeded();
  setSizesUpdateNeeded();
}

uint HistogramView::getMappedId(uint id) {
  if (dataLocation == ElementType::EDGE) {
    return nodeToEdge[node(id)].id;
  }

  return id;
}
}
