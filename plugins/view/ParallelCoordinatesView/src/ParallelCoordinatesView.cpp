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

#include "ParallelCoordinatesView.h"
#include "ParallelCoordinatesViewQuickAccessbar.h"
#include "ParallelTools.h"
#include "ParallelCoordsDrawConfigWidget.h"
#include "QuantitativeParallelAxis.h"
#include "../utils/ViewGraphPropertiesSelectionWidget.h"

#include <talipot/GlGraph.h>
#include <talipot/TlpQtTools.h>

#include <QActionGroup>
#include <QKeyEvent>
#include <QMenu>

using namespace std;

const string propertiesTypes[] = {"double", "int", "string"};
const uint nbPropertiesTypes = sizeof(propertiesTypes) / sizeof(string);
const vector<string> propertiesTypesFilter(propertiesTypes, propertiesTypes + nbPropertiesTypes);

namespace tlp {

static void toggleGraphView(GlGraph *glGraph, bool displayNodes) {
  GlGraphRenderingParameters &param = glGraph->renderingParameters();
  param.setAntialiasing(true);
  param.setNodesStencil(2);
  param.setNodesLabelStencil(2);
  param.setSelectedNodesStencil(2);
  param.setDisplayEdges(false);
  param.setDisplayNodes(displayNodes);
  param.setFontsType(2);
}

PLUGIN(ParallelCoordinatesView)

ParallelCoordinatesView::ParallelCoordinatesView(const PluginContext *)
    : GlView(true), viewSetupMenu(nullptr), classicLayout(nullptr), circularLayout(nullptr),
      straightLinesType(nullptr), catmullRomSplineLinesType(nullptr),
      cubicBSplineInterpolationLinesType(nullptr), thickLines(nullptr), thinLines(nullptr),
      addRemoveDataFromSelection(nullptr), selectData(nullptr), deleteData(nullptr),
      showDataProperties(nullptr), axisMenuSeparator(nullptr), axisConfiguration(nullptr),
      removeAxisAction(nullptr), highlightMenuSeparator(nullptr),
      selectHighlightedElements(nullptr), addSelectHighlightedElements(nullptr),
      removeSelectHighlightedElements(nullptr), resetHightlightedElements(nullptr),
      mainLayer(nullptr), axisSelectionLayer(nullptr), glGraph(nullptr), axisPointsGraph(nullptr),
      graphProxy(nullptr), parallelCoordsDrawing(nullptr), dataConfigWidget(nullptr),
      drawConfigWidget(nullptr), firstSet(true), lastNbSelectedProperties(0), center(false),
      lastViewWindowWidth(0), lastViewWindowHeight(0), isConstruct(false),
      dontCenterViewAfterConfLoaded(false), needDraw(false) {}

ParallelCoordinatesView::~ParallelCoordinatesView() {
  for (auto *obs : triggers()) {
    removeRedrawTrigger(obs);
  }

  delete axisPointsGraph;
  delete axisSelectionLayer;
  delete graphProxy;
  graphProxy = nullptr;
  delete dataConfigWidget;
  delete drawConfigWidget;
  delete axisConfiguration;
  delete axisMenuSeparator;
  delete removeAxisAction;
  delete highlightMenuSeparator;
  delete selectHighlightedElements;
  delete addSelectHighlightedElements;
  delete removeSelectHighlightedElements;
  delete resetHightlightedElements;
  delete viewSetupMenu;
}

QuickAccessBar *ParallelCoordinatesView::getQuickAccessBarImpl() {
  auto *_bar = new ParallelCoordinatesViewQuickAccessBar(drawConfigWidget);
  connect(_bar, &ParallelCoordinatesViewQuickAccessBar::settingsChanged, this,
          &ParallelCoordinatesView::applySettings);
  return _bar;
}

void ParallelCoordinatesView::interactorsInstalled(const QList<tlp::Interactor *> &) {
  toggleInteractors(false);
}

void ParallelCoordinatesView::toggleInteractors(bool activate) {
  View::toggleInteractors(activate, {InteractorName::InteractorNavigation});
}

void ParallelCoordinatesView::initGlWidget() {
  GlScene *scene = glWidget()->scene();

  if (!mainLayer) {
    mainLayer = new GlLayer("Main");
    scene->addExistingLayer(mainLayer);
  }

  axisPointsGraph = tlp::newGraph();
  glGraph = new GlGraph(axisPointsGraph);
  mainLayer->addGlEntity(glGraph, "graph");
  axisSelectionLayer = new GlLayer("Axis selection layer");
  GlGraphRenderingParameters &param = scene->glGraph()->renderingParameters();
  param.setAntialiasing(true);
  param.setNodesStencil(2);
  param.setNodesLabelStencil(1);
  param.setSelectedNodesStencil(1);
  param.setDisplayEdges(false);
  param.setDisplayNodes(true);
  param.setViewNodeLabel(false);
  param.setFontsType(2);
  glWidget()->setMouseTracking(true);
}

QList<QWidget *> ParallelCoordinatesView::configurationWidgets() const {
  return QList<QWidget *>() << dataConfigWidget << drawConfigWidget;
}

void ParallelCoordinatesView::setState(const DataSet &dataSet) {

  if (!isConstruct) {
    initGlWidget();
    buildContextMenu();
    setOverviewVisible(true);
    glWidget()->installEventFilter(this);

    dataConfigWidget = new ViewGraphPropertiesSelectionWidget();
    drawConfigWidget = new ParallelCoordsDrawConfigWidget();

    isConstruct = true;
  }

  GlView::setState(dataSet);

  removeTriggers();

  vector<string> selectedPropertiesBak;

  bool sameGraphRoot = false;

  if (graph()) {
    if (graphProxy != nullptr && (graph()->getRoot() == graphProxy->getRoot())) {
      sameGraphRoot = true;
      selectedPropertiesBak = graphProxy->getSelectedProperties();
    }
  }

  if (parallelCoordsDrawing != nullptr && graphProxy->getGraph() != graph()) {
    mainLayer->deleteGlEntity(parallelCoordsDrawing);
    graphProxy->removeListener(parallelCoordsDrawing);
    delete parallelCoordsDrawing;
    parallelCoordsDrawing = nullptr;
  }

  if (graphProxy != nullptr && graphProxy->getGraph() != graph()) {
    delete graphProxy;
    graphProxy = nullptr;
  }

  if (graph() != nullptr) {
    if (graphProxy == nullptr) {
      graphProxy = new ParallelCoordinatesGraphProxy(graph());
    }

    if (sameGraphRoot) {
      graphProxy->setSelectedProperties(selectedPropertiesBak);
    }

    if (dataSet.exists("selectedProperties")) {
      vector<string> selectedProperties;
      DataSet items;
      dataSet.get("selectedProperties", items);
      int i = 0;
      stringstream ss;
      ss << i;

      while (items.exists(ss.str())) {
        string item;
        items.get(ss.str(), item);
        selectedProperties.push_back(item);
        ss.str("");
        ++i;
        ss << i;
      }

      graphProxy->setSelectedProperties(selectedProperties);
    }

    dataConfigWidget->setWidgetParameters(graph(), propertiesTypesFilter);
    dataConfigWidget->setSelectedProperties(graphProxy->getSelectedProperties());

    if (parallelCoordsDrawing == nullptr) {
      parallelCoordsDrawing = new ParallelCoordinatesDrawing(graphProxy, axisPointsGraph);
      graphProxy->addListener(parallelCoordsDrawing);
      mainLayer->addGlEntity(parallelCoordsDrawing, "Parallel Coordinates");
    }

    // overviewWidget->setObservedView(mainWidget, parallelCoordsDrawing);

    uint axisHeight = DEFAULT_AXIS_HEIGHT;
    uint linesColorAlphaValue = DEFAULT_LINES_COLOR_ALPHA_VALUE;

    if (dataSet.exists("dataLocation")) {
      int dataLocation = 0;
      dataSet.get("dataLocation", dataLocation);
      dataConfigWidget->setDataLocation(static_cast<ElementType>(dataLocation));
    }

    if (dataSet.exists("backgroundColor")) {
      Color backgroundColor;
      dataSet.get("backgroundColor", backgroundColor);
      drawConfigWidget->setBackgroundColor(backgroundColor);
    }

    if (dataSet.exists("axisPointMinSize")) {
      uint axisPointMinSize = 0;
      dataSet.get("axisPointMinSize", axisPointMinSize);
      drawConfigWidget->setAxisPointMinSize(axisPointMinSize);
    }

    if (dataSet.exists("axisPointMaxSize")) {
      uint axisPointMaxSize = 0;
      dataSet.get("axisPointMaxSize", axisPointMaxSize);
      drawConfigWidget->setAxisPointMaxSize(axisPointMaxSize);
    }

    if (dataSet.exists("drawPointsOnAxis")) {
      bool drawPointsOnAxis = true;
      dataSet.get("drawPointsOnAxis", drawPointsOnAxis);
      drawConfigWidget->setDrawPointOnAxis(drawPointsOnAxis);
    }

    if (dataSet.exists("linesTextureFileName")) {
      string linesTextureFileName;
      dataSet.get("linesTextureFileName", linesTextureFileName);
      drawConfigWidget->setLinesTextureFilename(linesTextureFileName);
    }

    if (dataSet.exists("axisHeight")) {
      dataSet.get("axisHeight", axisHeight);
    }

    if (dataSet.exists("linesColorAlphaValue")) {
      dataSet.get("linesColorAlphaValue", linesColorAlphaValue);
    }

    if (dataSet.exists("non highlighted alpha value")) {
      uint nonHighlightedAlphaValue = 0;
      dataSet.get("non highlighted alpha value", nonHighlightedAlphaValue);
      drawConfigWidget->setUnhighlightedEltsColorsAlphaValue(nonHighlightedAlphaValue);
    }

    if (dataSet.exists("linesType")) {
      int linesType = 0;
      dataSet.get("linesType", linesType);

      if (linesType == ParallelCoordinatesDrawing::STRAIGHT) {
        straightLinesType->setChecked(true);
        catmullRomSplineLinesType->setChecked(false);
        cubicBSplineInterpolationLinesType->setChecked(false);
      } else if (linesType == ParallelCoordinatesDrawing::CATMULL_ROM_SPLINE) {
        straightLinesType->setChecked(false);
        catmullRomSplineLinesType->setChecked(true);
        cubicBSplineInterpolationLinesType->setChecked(false);
      } else {
        straightLinesType->setChecked(false);
        catmullRomSplineLinesType->setChecked(false);
        cubicBSplineInterpolationLinesType->setChecked(true);
      }
    }

    if (dataSet.exists("layoutType")) {
      int layoutType = 0;
      dataSet.get("layoutType", layoutType);

      if (layoutType == ParallelCoordinatesDrawing::PARALLEL) {
        classicLayout->setChecked(true);
        circularLayout->setChecked(false);
      } else {
        classicLayout->setChecked(false);
        circularLayout->setChecked(true);
      }
    }

    drawConfigWidget->setAxisHeight(axisHeight);
    drawConfigWidget->setLinesColorAlphaValue(linesColorAlphaValue);

    dataSet.get("lastViewWindowWidth", lastViewWindowWidth);
    dataSet.get("lastViewWindowHeight", lastViewWindowHeight);

    if (dataSet.exists("scene")) {
      string sceneXML;
      dataSet.get("scene", sceneXML);
      glWidget()->scene()->setWithXML(sceneXML, nullptr);
      dontCenterViewAfterConfLoaded = true;
    }

  } else {
    dataConfigWidget->setWidgetParameters(nullptr, propertiesTypesFilter);
  }

  bool quickAccessBarVisible = false;

  if (dataSet.get<bool>("quickAccessBarVisible", quickAccessBarVisible)) {
    needQuickAccessBar = true;
    setQuickAccessBarVisible(quickAccessBarVisible);
  } else {
    setQuickAccessBarVisible(true);
  }

  setupAndDrawView();
}

DataSet ParallelCoordinatesView::state() const {

  DataSet dataSet = GlView::state();

  string sceneOut;
  glWidget()->scene()->getXMLOnlyForCameras(sceneOut);
  dataSet.set("scene", sceneOut);

  vector<string> selectedProperties = graphProxy->getSelectedProperties();
  DataSet selectedPropertiesData;
  int i = 0;

  for (const auto &p : selectedProperties) {
    std::stringstream s;
    s << i;
    selectedPropertiesData.set(s.str(), p);
    i++;
  }

  dataSet.set("selectedProperties", selectedPropertiesData);
  dataSet.set("dataLocation", int(graphProxy->getDataLocation()));
  dataSet.set("backgroundColor", drawConfigWidget->getBackgroundColor());
  dataSet.set("axisHeight", drawConfigWidget->getAxisHeight());
  uint axisPointMinSize = uint(drawConfigWidget->getAxisPointMinSize().getW());
  uint axisPointMaxSize = uint(drawConfigWidget->getAxisPointMaxSize().getW());
  dataSet.set("axisPointMinSize", axisPointMinSize);
  dataSet.set("axisPointMaxSize", axisPointMaxSize);
  dataSet.set("drawPointsOnAxis", drawConfigWidget->drawPointOnAxis());
  dataSet.set("linesTextureFileName", drawConfigWidget->getLinesTextureFilename());
  dataSet.set("linesColorAlphaValue", drawConfigWidget->getLinesColorAlphaValue());
  dataSet.set("non highlighted alpha value",
              drawConfigWidget->getUnhighlightedEltsColorsAlphaValue());
  dataSet.set("layoutType", int(getLayoutType()));
  dataSet.set("linesType", int(getLinesType()));
  dataSet.set("lastViewWindowWidth", glWidget()->width());
  dataSet.set("lastViewWindowHeight", glWidget()->height());

  if (needQuickAccessBar) {
    dataSet.set("quickAccessBarVisible", quickAccessBarVisible());
  }

  return dataSet;
}

void ParallelCoordinatesView::graphChanged(tlp::Graph *) {
  if (isConstruct) {
    setState(DataSet());
  } else if (quickAccessBarVisible()) {
    _quickAccessBar->setEnabled(false);
  }
}

void ParallelCoordinatesView::updateWithoutProgressBar() {
  if (parallelCoordsDrawing) {
    parallelCoordsDrawing->update(glWidget(), true);
  }
}

void ParallelCoordinatesView::updateWithProgressBar() {
  if (parallelCoordsDrawing) {
    setOverviewVisible(false);
    toggleGraphView(glGraph, false);
    parallelCoordsDrawing->update(glWidget());
    toggleGraphView(glGraph, true);
    centerView();
    glWidget()->draw();
    setOverviewVisible(true);
  }
}

void ParallelCoordinatesView::addEmptyViewLabel() {
  Color backgroundColor = drawConfigWidget->getBackgroundColor();
  glWidget()->scene()->setBackgroundColor(backgroundColor);

  Color foregroundColor;
  int bgV = backgroundColor.getV();

  if (bgV < 128) {
    foregroundColor = Color(255, 255, 255);
  } else {
    foregroundColor = Color(0, 0, 0);
  }

  auto *noDimsLabel = new GlLabel(Coord(0, 0, 0), Size(200, 200), foregroundColor);
  noDimsLabel->setText(ViewName::ParallelCoordinatesViewName);
  mainLayer->addGlEntity(noDimsLabel, "no dimensions label");
  auto *noDimsLabel1 =
      new GlLabel(Coord(0.0f, -50.0f, 0.0f), Size(400.0f, 200.0f), foregroundColor);
  noDimsLabel1->setText("No graph properties selected.");
  mainLayer->addGlEntity(noDimsLabel1, "no dimensions label 1");
  auto *noDimsLabel2 =
      new GlLabel(Coord(0.0f, -100.0f, 0.0f), Size(700.0f, 200.0f), foregroundColor);
  noDimsLabel2->setText("Go to the \"Properties\" tab in top right corner.");
  mainLayer->addGlEntity(noDimsLabel2, "no dimensions label 2");
  mainLayer->deleteGlEntity(parallelCoordsDrawing);
  mainLayer->deleteGlEntity(glGraph);
}

void ParallelCoordinatesView::removeEmptyViewLabel() {
  GlEntity *noDimsLabel = mainLayer->findGlEntity("no dimensions label");
  GlEntity *noDimsLabel1 = mainLayer->findGlEntity("no dimensions label 1");
  GlEntity *noDimsLabel2 = mainLayer->findGlEntity("no dimensions label 2");

  if (noDimsLabel != nullptr) {
    mainLayer->deleteGlEntity(noDimsLabel);
    delete noDimsLabel;
    mainLayer->deleteGlEntity(noDimsLabel1);
    delete noDimsLabel1;
    mainLayer->deleteGlEntity(noDimsLabel2);
    delete noDimsLabel2;

    if (parallelCoordsDrawing) {
      mainLayer->addGlEntity(parallelCoordsDrawing, "Parallel Coordinates");
    }

    mainLayer->addGlEntity(glGraph, "graph");
  }
}

void ParallelCoordinatesView::draw() {
  if (graph()) {
    if (graphProxy->selectedPropertiesisEmpty()) {
      removeEmptyViewLabel();
      addEmptyViewLabel();
      toggleInteractors(false);
      if (quickAccessBarVisible()) {
        _quickAccessBar->setEnabled(false);
      }
      glWidget()->scene()->centerScene();
      glWidget()->draw();
      return;
    } else {
      removeEmptyViewLabel();
      if (quickAccessBarVisible()) {
        _quickAccessBar->setEnabled(true);
      }
      toggleInteractors(true);
      if (graphProxy->getDataCount() > PROGRESS_BAR_DISPLAY_NB_DATA_THRESHOLD) {
        updateWithProgressBar();
      } else {
        updateWithoutProgressBar();
      }
    }

    if (lastNbSelectedProperties != graphProxy->getNumberOfSelectedProperties() || center) {
      if (!dontCenterViewAfterConfLoaded) {
        centerView();
      } else {
        dontCenterViewAfterConfLoaded = false;
      }

      center = false;
    } else {
      glWidget()->draw();
    }

    lastNbSelectedProperties = graphProxy->getNumberOfSelectedProperties();
  } else {
    glWidget()->draw();
  }

  needDraw = false;
}

void ParallelCoordinatesView::refresh() {
  if (!needDraw) {
    glWidget()->redraw();
  } else {
    draw();
  }
}

void ParallelCoordinatesView::init() {
  emit drawNeeded();
}

bool ParallelCoordinatesView::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto *keyEvent = dynamic_cast<QKeyEvent *>(event);

    if ((keyEvent->key() == Qt::Key_R) && (keyEvent->modifiers() & Qt::ControlModifier) != 0 &&
        (keyEvent->modifiers() & Qt::ShiftModifier) != 0) {
      emit drawNeeded();
    }

    if ((keyEvent->key() == Qt::Key_C) && (keyEvent->modifiers() & Qt::ControlModifier) != 0 &&
        (keyEvent->modifiers() & Qt::ShiftModifier) != 0) {
      centerView();
    }
  }

  if (graphProxy != nullptr && graphProxy->graphColorsModified()) {
    Observable::holdObservers();
    graphProxy->colorDataAccordingToHighlightedElts();
    Observable::unholdObservers();
  }

  return GlView::eventFilter(obj, event);
}

bool ParallelCoordinatesView::getNodeOrEdgeAtViewportPos(int x, int y, node &n, edge &e) const {
  set<uint> data;

  if (mapGlEntitiesInRegionToData(data, x, y)) {
    if (graphProxy->getDataLocation() == NODE) {
      n = node(*(data.begin()));
      return true;
    } else {
      e = edge(*(data.begin()));
      return true;
    }
  }
  return false;
}

void ParallelCoordinatesView::buildContextMenu() {

  viewSetupMenu = new QMenu("View setup");
  viewSetupMenu->addAction("Layout type")->setEnabled(false);
  auto *layoutActionGroup = new QActionGroup(this);
  classicLayout = viewSetupMenu->addAction("Classic layout", this,
                                           &ParallelCoordinatesView::centerSetupAndDrawView);
  classicLayout->setToolTip("Use parallel axis layout");
  classicLayout->setCheckable(true);
  classicLayout->setChecked(true);
  layoutActionGroup->addAction(classicLayout);
  circularLayout = viewSetupMenu->addAction("Circular layout", this,
                                            &ParallelCoordinatesView::centerSetupAndDrawView);
  circularLayout->setToolTip(
      "In the circular layout, the axis are laid out regularly as the radius of a circle");
  circularLayout->setCheckable(true);
  layoutActionGroup->addAction(circularLayout);
  viewSetupMenu->addSeparator();

  viewSetupMenu->addAction("Lines type")->setEnabled(false);
  auto *lineTypeActionGroup = new QActionGroup(this);
  straightLinesType =
      viewSetupMenu->addAction("Polyline", this, &ParallelCoordinatesView::setupAndDrawView);
  straightLinesType->setToolTip(
      "Draw a polyline joining the consecutive coordinates belonging to the same graph element");
  straightLinesType->setCheckable(true);
  straightLinesType->setChecked(true);
  lineTypeActionGroup->addAction(straightLinesType);
  catmullRomSplineLinesType = viewSetupMenu->addAction("Catmull-Rom spline", this,
                                                       &ParallelCoordinatesView::setupAndDrawView);
  catmullRomSplineLinesType->setToolTip("Draw a Catmull-Rom spline joining the consecutive "
                                        "coordinates belonging to the same graph element");
  catmullRomSplineLinesType->setCheckable(true);
  lineTypeActionGroup->addAction(catmullRomSplineLinesType);
  cubicBSplineInterpolationLinesType = viewSetupMenu->addAction(
      "Cubic B-spline interpolation", this, &ParallelCoordinatesView::setupAndDrawView);
  cubicBSplineInterpolationLinesType->setToolTip(
      "Draw a cubic B-spline joining the consecutive coordinates belonging to the same "
      "graph element");
  cubicBSplineInterpolationLinesType->setCheckable(true);
  lineTypeActionGroup->addAction(catmullRomSplineLinesType);
  viewSetupMenu->addSeparator();

  viewSetupMenu->addAction("Lines thickness")->setEnabled(false);
  auto *lineActionGroup = new QActionGroup(this);
  thickLines =
      viewSetupMenu->addAction("Map to viewSize", this, &ParallelCoordinatesView::setupAndDrawView);
  thickLines->setToolTip("The lines thickness is computed according the viewSize property values");
  thickLines->setCheckable(true);
  thickLines->setChecked(true);
  lineActionGroup->addAction(thickLines);
  thinLines =
      viewSetupMenu->addAction("Thin lines", this, &ParallelCoordinatesView::setupAndDrawView);
  thinLines->setToolTip(
      "The thickness is thin and the same for all the  curves representing the graph elements");
  thinLines->setCheckable(true);
  lineActionGroup->addAction(thinLines);
  axisMenuSeparator = new QAction(nullptr);
  axisMenuSeparator->setSeparator(true);
  axisConfiguration = new QAction("Axis configuration", nullptr);
  connect(axisConfiguration, &QAction::triggered, this,
          &ParallelCoordinatesView::axisConfigurationSlot);
  removeAxisAction = new QAction("Remove axis", nullptr);
  connect(removeAxisAction, &QAction::triggered, this, &ParallelCoordinatesView::removeAxisSlot);
  highlightMenuSeparator = new QAction(nullptr);
  highlightMenuSeparator->setSeparator(true);
  selectHighlightedElements = new QAction("Select highlighted elements", nullptr);
  selectHighlightedElements->setToolTip(
      "Select the graph elements corresponding to the currently highlighted curves");
  connect(selectHighlightedElements, &QAction::triggered, this,
          &ParallelCoordinatesView::selectHighlightedElementsSlot);
  addSelectHighlightedElements = new QAction("Add highlighted elements to selection", nullptr);
  addSelectHighlightedElements->setToolTip(
      "Add the graph elements corresponding to the currently highlighted curves to the "
      "current selection");
  connect(addSelectHighlightedElements, &QAction::triggered, this,
          &ParallelCoordinatesView::addSelectHighlightedElementsSlot);
  removeSelectHighlightedElements =
      new QAction("Remove highlighted elements to selection", nullptr);
  removeSelectHighlightedElements->setToolTip(
      "Remove the graph elements corresponding to the currently highlighted curves from "
      "the current selection");
  connect(removeSelectHighlightedElements, &QAction::triggered, this,
          &ParallelCoordinatesView::removeSelectHighlightedElementsSlot);
  resetHightlightedElements = new QAction("Reset highlighting of elements", nullptr);
  resetHightlightedElements->setToolTip("Unhighlight all the elements");
  connect(resetHightlightedElements, &QAction::triggered, this,
          &ParallelCoordinatesView::resetHighlightedElementsSlot);
}

void ParallelCoordinatesView::fillContextMenu(QMenu *menu, const QPointF &point) {
  GlView::fillContextMenu(menu, point);
  menu->addAction(viewSetupMenu->menuAction());

  axisUnderPointer = getAxisUnderPointer(point.x(), point.y());

  if (axisUnderPointer != nullptr) {
    menu->addAction(axisMenuSeparator);
    menu->addAction(axisConfiguration);
    axisConfiguration->setToolTip("Configure the axis '" +
                                  tlpStringToQString(axisUnderPointer->getAxisName()) + "'");
    menu->addAction(removeAxisAction);
    removeAxisAction->setToolTip(
        "Remove the axis '" + tlpStringToQString(axisUnderPointer->getAxisName()) +
        "': the property is then deselected in the Properties configuration panel");
  }

  if (graphProxy->highlightedEltsSet()) {
    menu->addAction(highlightMenuSeparator);
    menu->addAction(selectHighlightedElements);
    menu->addAction(addSelectHighlightedElements);
    menu->addAction(removeSelectHighlightedElements);
    menu->addAction(resetHightlightedElements);
  }
}

void ParallelCoordinatesView::axisConfigurationSlot() {
  axisUnderPointer->showConfigDialog();
  emit drawNeeded();
}

void ParallelCoordinatesView::removeAxisSlot() {
  graphProxy->removePropertyFromSelection(axisUnderPointer->getAxisName());
  dataConfigWidget->setSelectedProperties(graphProxy->getSelectedProperties());
  emit drawNeeded();
}

void ParallelCoordinatesView::selectHighlightedElementsSlot() {
  Observable::holdObservers();
  graphProxy->selectHighlightedElements();
  Observable::unholdObservers();
}

void ParallelCoordinatesView::addSelectHighlightedElementsSlot() {
  Observable::holdObservers();
  graphProxy->setSelectHighlightedElements(true);
  Observable::unholdObservers();
}

void ParallelCoordinatesView::removeSelectHighlightedElementsSlot() {
  Observable::holdObservers();
  graphProxy->setSelectHighlightedElements(false);
  Observable::unholdObservers();
}

void ParallelCoordinatesView::resetHighlightedElementsSlot() {
  Observable::holdObservers();
  graphProxy->unsetHighlightedElts();
  parallelCoordsDrawing->resetAxisSlidersPosition();
  graphProxy->colorDataAccordingToHighlightedElts();
  Observable::unholdObservers();
}

void ParallelCoordinatesView::centerSetupAndDrawView() {
  center = true;
  setupAndDrawView();
}

void ParallelCoordinatesView::setupAndDrawView() {
  if (!graphProxy) {
    return;
  }

  if (graph()) {
    GlScene *scene = glWidget()->scene();
    graphProxy->setSelectedProperties(dataConfigWidget->getSelectedGraphProperties());
    graphProxy->setDataLocation(dataConfigWidget->getDataLocation());
    scene->setBackgroundColor(drawConfigWidget->getBackgroundColor());
    parallelCoordsDrawing->setAxisHeight(drawConfigWidget->getAxisHeight());
    parallelCoordsDrawing->setAxisPointMinSize(drawConfigWidget->getAxisPointMinSize());
    parallelCoordsDrawing->setAxisPointMaxSize(drawConfigWidget->getAxisPointMaxSize());
    parallelCoordsDrawing->setBackgroundColor(drawConfigWidget->getBackgroundColor());
    parallelCoordsDrawing->setDrawPointsOnAxis(drawConfigWidget->drawPointOnAxis());
    parallelCoordsDrawing->setLineTextureFilename(drawConfigWidget->getLinesTextureFilename());
    parallelCoordsDrawing->setLinesColorAlphaValue(drawConfigWidget->getLinesColorAlphaValue());
    parallelCoordsDrawing->setLayoutType(getLayoutType());
    parallelCoordsDrawing->setLinesType(getLinesType());
    parallelCoordsDrawing->setLinesThickness(getLinesThickness());
    scene->glGraph()->renderingParameters().setViewNodeLabel(drawConfigWidget->displayNodeLabels());

    if (graphProxy->getUnhighlightedEltsColorAlphaValue() !=
        drawConfigWidget->getUnhighlightedEltsColorsAlphaValue()) {
      graphProxy->setUnhighlightedEltsColorAlphaValue(
          drawConfigWidget->getUnhighlightedEltsColorsAlphaValue());
      Observable::holdObservers();
      graphProxy->colorDataAccordingToHighlightedElts();
      Observable::unholdObservers();
    }
  }

  registerTriggers();
  needDraw = true;
  draw();
  drawOverview(true);
}

ParallelCoordinatesDrawing::LayoutType ParallelCoordinatesView::getLayoutType() const {
  return (classicLayout->isChecked()) ? ParallelCoordinatesDrawing::PARALLEL
                                      : ParallelCoordinatesDrawing::CIRCULAR;
}

ParallelCoordinatesDrawing::LinesType ParallelCoordinatesView::getLinesType() const {
  if (straightLinesType->isChecked()) {
    return ParallelCoordinatesDrawing::STRAIGHT;
  } else if (catmullRomSplineLinesType->isChecked()) {
    return ParallelCoordinatesDrawing::CATMULL_ROM_SPLINE;
  } else {
    return ParallelCoordinatesDrawing::CUBIC_BSPLINE_INTERPOLATION;
  }
}

ParallelCoordinatesDrawing::LinesThickness ParallelCoordinatesView::getLinesThickness() const {
  return (thickLines->isChecked()) ? ParallelCoordinatesDrawing::THICK
                                   : ParallelCoordinatesDrawing::THIN;
}

bool ParallelCoordinatesView::mapGlEntitiesInRegionToData(std::set<uint> &mappedData, const int x,
                                                          const int y, const uint width,
                                                          const uint height) const {

  vector<SelectedEntity> selectedEntities;
  vector<SelectedEntity> selectedAxisPoints;
  vector<SelectedEntity> dummy;

  mappedData.clear();

  bool result = glWidget()->pickGlEntities(x, y, width, height, selectedEntities, mainLayer);

  if (result) {
    for (const auto &ite : selectedEntities) {
      GlEntity *entity = ite.getEntity();
      uint selectedEltId;

      if (parallelCoordsDrawing->getDataIdFromGlEntity(entity, selectedEltId)) {
        mappedData.insert(selectedEltId);
      }
    }
  }

  glWidget()->pickNodesEdges(x, y, width, height, selectedAxisPoints, dummy, mainLayer);

  for (const auto &entity : selectedAxisPoints) {
    node n(entity.getGraphElementId());
    uint selectedEltId;

    if (parallelCoordsDrawing->getDataIdFromAxisPoint(n, selectedEltId)) {
      mappedData.insert(selectedEltId);
    }
  }

  return !mappedData.empty();
}

void ParallelCoordinatesView::setDataUnderPointerSelectFlag(const int x, const int y,
                                                            const bool selectFlag) {
  set<uint> dataUnderPointer;
  mapGlEntitiesInRegionToData(dataUnderPointer, x, y);

  for (auto i : dataUnderPointer) {
    if (!graphProxy->highlightedEltsSet() || graphProxy->isDataHighlighted(i)) {
      graphProxy->setDataSelected(i, selectFlag);
    }
  }
}

void ParallelCoordinatesView::setDataInRegionSelectFlag(const int x, const int y, const uint width,
                                                        const uint height, const bool selectFlag) {
  set<uint> dataUnderPointer;
  mapGlEntitiesInRegionToData(dataUnderPointer, x, y, width, height);

  for (auto i : dataUnderPointer) {
    if (!graphProxy->highlightedEltsSet() || graphProxy->isDataHighlighted(i)) {
      graphProxy->setDataSelected(i, selectFlag);
    }
  }
}

void ParallelCoordinatesView::resetSelection() {
  graphProxy->resetSelection();
}

void ParallelCoordinatesView::deleteDataUnderPointer(const int x, const int y) {
  set<uint> dataUnderPointer;
  mapGlEntitiesInRegionToData(dataUnderPointer, x, y);

  for (auto i : dataUnderPointer) {
    if (!graphProxy->highlightedEltsSet() || graphProxy->isDataHighlighted(i)) {
      graphProxy->deleteData(i);
    }
  }
}

bool ParallelCoordinatesView::getDataUnderPointerProperties(const int x, const int y,
                                                            SelectedEntity &selectedEntity) {
  set<uint> dataUnderPointer;
  mapGlEntitiesInRegionToData(dataUnderPointer, x, y);

  if (!dataUnderPointer.empty()) {
    uint dataId;

    if (!graphProxy->highlightedEltsSet()) {
      dataId = *(dataUnderPointer.begin());
    } else {
      auto it = dataUnderPointer.begin();

      while (it != dataUnderPointer.end() && !graphProxy->isDataHighlighted(*it)) {
        ++it;
      }

      if (it == dataUnderPointer.end()) {
        return false;
      } else {
        dataId = *it;
      }
    }

    if (graphProxy->getDataLocation() == NODE) {
      selectedEntity = SelectedEntity(graph(), dataId, SelectedEntity::NODE_SELECTED);
    } else {
      selectedEntity = SelectedEntity(graph(), dataId, SelectedEntity::EDGE_SELECTED);
    }

    return true;
  } else {
    return false;
  }
}

void ParallelCoordinatesView::highlightDataUnderPointer(const int x, const int y,
                                                        const bool addEltToMagnifyFlag) {
  if (!addEltToMagnifyFlag) {
    graphProxy->unsetHighlightedElts();
  }

  set<uint> dataUnderPointer;
  mapGlEntitiesInRegionToData(dataUnderPointer, x, y);

  for (auto i : dataUnderPointer) {
    graphProxy->addOrRemoveEltToHighlight(i);
  }

  graphProxy->colorDataAccordingToHighlightedElts();
}

void ParallelCoordinatesView::highlightDataInRegion(const int x, const int y, const int width,
                                                    const int height,
                                                    const bool addEltToMagnifyFlag) {
  if (!addEltToMagnifyFlag) {
    graphProxy->unsetHighlightedElts();
  }

  set<uint> dataUnderPointer;
  mapGlEntitiesInRegionToData(dataUnderPointer, x, y, width, height);

  for (auto i : dataUnderPointer) {
    graphProxy->addOrRemoveEltToHighlight(i);
  }

  graphProxy->colorDataAccordingToHighlightedElts();
}

void ParallelCoordinatesView::resetHighlightedElements() {
  graphProxy->unsetHighlightedElts();
  graphProxy->colorDataAccordingToHighlightedElts();
}

ParallelAxis *ParallelCoordinatesView::getAxisUnderPointer(const int x, const int y) const {
  vector<ParallelAxis *> allAxis = parallelCoordsDrawing->getAllAxis();
  axisSelectionLayer->setSharedCamera(&glWidget()->scene()->getLayer("Main")->getCamera());
  axisSelectionLayer->getComposite()->reset(false);

  for (auto *axis : allAxis) {
    axisSelectionLayer->addGlEntity(axis, getStringFromNumber(axis));
  }

  vector<SelectedEntity> pickedEntities;

  if (glWidget()->pickGlEntities(x, y, pickedEntities, axisSelectionLayer)) {
    return dynamic_cast<ParallelAxis *>(pickedEntities[0].getEntity());
  }

  axisSelectionLayer->getComposite()->reset(false);
  return nullptr;
}

void ParallelCoordinatesView::swapAxis(ParallelAxis *firstAxis, ParallelAxis *secondAxis) {
  parallelCoordsDrawing->swapAxis(firstAxis, secondAxis);
  dataConfigWidget->setSelectedProperties(graphProxy->getSelectedProperties());
}

void ParallelCoordinatesView::removeAxis(ParallelAxis *axis) {
  parallelCoordsDrawing->removeAxis(axis);
}

void ParallelCoordinatesView::addAxis(ParallelAxis *axis) {
  parallelCoordsDrawing->addAxis(axis);
}

vector<ParallelAxis *> ParallelCoordinatesView::getAllAxis() {
  return parallelCoordsDrawing->getAllAxis();
}

void ParallelCoordinatesView::updateAxisSlidersPosition() {
  if (!graphProxy->highlightedEltsSet()) {
    parallelCoordsDrawing->resetAxisSlidersPosition();
  } else {
    const set<uint> &highlightedElts(graphProxy->getHighlightedElts());
    vector<ParallelAxis *> axis(getAllAxis());

    for (auto *ax : axis) {
      ax->updateSlidersWithDataSubset(highlightedElts);
    }
  }
}

void ParallelCoordinatesView::updateWithAxisSlidersRange(
    ParallelAxis *axis, ParallelCoordinatesDrawing::HighlightedEltsSetOp highlightedEltsSetOp) {
  parallelCoordsDrawing->updateWithAxisSlidersRange(axis, highlightedEltsSetOp);
  graphProxy->colorDataAccordingToHighlightedElts();
}

bool ParallelCoordinatesView::highlightedElementsSet() const {
  return graphProxy->highlightedEltsSet();
}

void ParallelCoordinatesView::highlightDataInAxisBoxPlotRange(QuantitativeParallelAxis *axis) {
  const set<uint> &eltToHighlight(axis->getDataBetweenBoxPlotBounds());

  if (!eltToHighlight.empty()) {
    graphProxy->resetHighlightedElts(eltToHighlight);
    graphProxy->colorDataAccordingToHighlightedElts();
    updateAxisSlidersPosition();
  }
}

void ParallelCoordinatesView::removeTriggers() {
  for (auto *obs : triggers()) {
    removeRedrawTrigger(obs);
  }
}

void ParallelCoordinatesView::registerTriggers() {
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

void ParallelCoordinatesView::applySettings() {
  if (dataConfigWidget->configurationChanged() || drawConfigWidget->configurationChanged()) {
    setupAndDrawView();
  }
}
}
