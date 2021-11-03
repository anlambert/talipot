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

#include <talipot/GlGraph.h>
#include <talipot/GlProgressBar.h>
#include <talipot/TlpQtTools.h>

#include "PixelOrientedView.h"
#include "PixelOrientedViewQuickAccessBar.h"

#include "../utils/ViewGraphPropertiesSelectionWidget.h"

#include <QApplication>
#include <QGraphicsView>

using namespace std;

static void setGraphView(tlp::GlGraph *glGraph, bool displayNodes) {
  tlp::GlGraphRenderingParameters &param = glGraph->getRenderingParameters();
  param.setAntialiasing(true);
  param.setViewNodeLabel(true);
  param.setFontsType(2);
  param.setSelectedNodesStencil(1);
  param.setNodesStencil(0xFFFF);
  param.setNodesLabelStencil(0xFFFF);
  param.setDisplayEdges(false);
  param.setDisplayNodes(displayNodes);
  param.setDisplayMetaNodes(displayNodes);
}

const string propertiesTypes[] = {"double", "int"};
const uint nbPropertiesTypes = sizeof(propertiesTypes) / sizeof(string);
const vector<string> propertiesTypesFilter(propertiesTypes, propertiesTypes + nbPropertiesTypes);

namespace tlp {

PLUGIN(PixelOrientedView)

PixelOrientedView::PixelOrientedView(const PluginContext *)
    : GlView(true), pixelOrientedGraph(nullptr), glGraph(nullptr), mainLayer(nullptr),
      overviewsComposite(nullptr), optionsWidget(nullptr), propertiesSelectionWidget(nullptr),
      pixelOrientedMediator(nullptr), lastNbNodes(0), overviewWidth(0), overviewHeight(0),
      minWidth(0), refSize(0), hilbertLayout(nullptr), squareLayout(nullptr),
      spiralLayout(new SpiralLayout()), zorderLayout(nullptr), talipotNodeColorMapping(nullptr),
      smallMultiplesView(true), sceneRadiusBak(0.0), zoomFactorBak(0.0), detailViewLabel(nullptr),
      detailOverview(nullptr), newGraphSet(false), smallMultiplesNeedUpdate(false),
      lastViewWindowWidth(0), lastViewWindowHeight(0), center(false), isConstruct(false) {}

PixelOrientedView::~PixelOrientedView() {
  if (isConstruct) {
    destroyData();
  }

  delete spiralLayout;
  delete hilbertLayout;
  delete squareLayout;
  delete zorderLayout;
  delete talipotNodeColorMapping;
  delete pixelOrientedMediator;
  delete propertiesSelectionWidget;
  delete optionsWidget;
  delete glGraph;
}

QList<QWidget *> PixelOrientedView::configurationWidgets() const {
  return QList<QWidget *>() << propertiesSelectionWidget << optionsWidget;
}

void PixelOrientedView::initGlWidget() {
  mainLayer = glWidget()->scene()->getLayer("Main");

  if (mainLayer == nullptr) {
    mainLayer = new GlLayer("Main");
    glWidget()->scene()->addExistingLayer(mainLayer);
  }

  if (mainLayer->findGlEntity("graph")) {
    GlGraph *lastGlGraph = static_cast<GlGraph *>(mainLayer->findGlEntity("graph"));
    Graph *theGraph = lastGlGraph->getInputData()->graph();

    if (theGraph) {
      theGraph->removeListener(lastGlGraph);
    }
  }

  if (overviewsComposite != nullptr) {
    overviewsComposite->reset(true);
    detailOverview = nullptr;
  }

  mainLayer->getComposite()->reset(true);

  overviewsComposite = new GlComposite();
  mainLayer->addGlEntity(overviewsComposite, "overview composite");

  detailViewLabel = nullptr;

  glGraph = new GlGraph(pixelOrientedGraph);
  mainLayer->addGlEntity(glGraph, "graph");
  setGraphView(glGraph, false);
}

void PixelOrientedView::setColorFunction(ColorFunction *colorFunction) {
  pixelOrientedMediator->setColorFunction(colorFunction);
}

void PixelOrientedView::setLayoutFunction(LayoutFunction *layoutFunction) {
  pixelOrientedMediator->setLayoutFunction(layoutFunction);
}

void PixelOrientedView::setState(const DataSet &dataSet) {

  if (!isConstruct) {
    isConstruct = true;
    propertiesSelectionWidget = new ViewGraphPropertiesSelectionWidget();
    propertiesSelectionWidget->enableEdgesButton(false);
    pixelOrientedMediator = new PixelOrientedMediator(spiralLayout, nullptr);
    optionsWidget = new PixelOrientedOptionsWidget();
    layoutFunctionsMap["Spiral"] = spiralLayout;
    setOverviewVisible(true);
  }

  GlView::setState(dataSet);

  Graph *lastGraph = this->pixelOrientedGraph;

  if (graph() == nullptr) {
    this->pixelOrientedGraph = graph();
    destroyData();
    initGlWidget();
    lastNbNodes = 0;

    if (lastGraph != nullptr && lastGraph != pixelOrientedGraph) {
      overviewsComposite->reset(true);
      overviewsMap.clear();
      detailOverview = nullptr;
      newGraphSet = true;
    }

    for (auto *obs : triggers()) {
      removeRedrawTrigger(obs);
    }

    propertiesSelectionWidget->setWidgetParameters(nullptr, propertiesTypesFilter);

    center = true;
    draw();
    center = false;

    return;
  }

  if (!smallMultiplesView) {
    switchFromDetailViewToSmallMultiples();
  }

  if (talipotNodeColorMapping != nullptr && this->pixelOrientedGraph != graph()) {
    delete talipotNodeColorMapping;
    talipotNodeColorMapping = nullptr;
  }

  if (talipotNodeColorMapping == nullptr) {
    talipotNodeColorMapping = new NodeColorMapping(pixelOrientedGraph);
    pixelOrientedMediator->setColorFunction(talipotNodeColorMapping);
  }

  if (lastGraph == nullptr || lastGraph != graph()) {
    this->pixelOrientedGraph = graph();
    initGlWidget();
    destroyData();
    lastNbNodes = pixelOrientedGraph->numberOfNodes();
    initLayoutFunctions();

    if (lastGraph != nullptr && lastGraph != pixelOrientedGraph) {
      overviewsComposite->reset(true);
      overviewsMap.clear();
      detailOverview = nullptr;
      newGraphSet = true;
    }
  }

  if (lastGraph != nullptr && lastGraph->getRoot() != pixelOrientedGraph->getRoot()) {
    overviewGenMap.clear();
    detailOverviewPropertyName = "";
  }

  dataSet.get("lastViewWindowWidth", lastViewWindowWidth);
  dataSet.get("lastViewWindowHeight", lastViewWindowHeight);

  propertiesSelectionWidget->setWidgetParameters(pixelOrientedGraph, propertiesTypesFilter);

  Color backgroundColor = Color::White;
  dataSet.get("background color", backgroundColor);
  optionsWidget->setBackgroundColor(backgroundColor);

  bool haveSelectedGraphProperties = false;

  DataSet selectedGraphPropertiesDataSet;

  if (dataSet.get("selected graph properties", selectedGraphPropertiesDataSet)) {
    haveSelectedGraphProperties = true;
    int i = 0;
    ostringstream oss;
    oss << i;

    while (selectedGraphPropertiesDataSet.exists(oss.str())) {
      string propertyName;
      selectedGraphPropertiesDataSet.get(oss.str(), propertyName);
      selectedGraphProperties.push_back(propertyName);
      oss.str("");
      oss << ++i;
    }

    propertiesSelectionWidget->setSelectedProperties(selectedGraphProperties);

    for (const auto &selectedGraphProperty : selectedGraphProperties) {
      bool overviewGenerated = false;
      dataSet.get(selectedGraphProperty, overviewGenerated);
      overviewGenMap[selectedGraphProperty] = overviewGenerated;
    }

    string layoutName;
    dataSet.get("layout", layoutName);

    if (!layoutName.empty()) {
      optionsWidget->setLayoutType(layoutName);
    }
  }

  string detailOverviewName;
  dataSet.get("detail overview  name", detailOverviewName);

  center = true;

  if (haveSelectedGraphProperties) {
    updateOverviews(true);
    glWidget()->centerScene();
  }

  bool quickAccessBarVisible = false;

  if (dataSet.get<bool>("quickAccessBarVisible", quickAccessBarVisible)) {
    needQuickAccessBar = true;
    setQuickAccessBarVisible(quickAccessBarVisible);
  } else { // display quickaccessbar
    setQuickAccessBarVisible(true);
  }

  draw();
  center = false;

  if (!detailOverviewName.empty()) {
    switchFromSmallMultiplesToDetailView(overviewsMap[detailOverviewName]);
  }

  drawOverview(true);
  registerTriggers();
}

DataSet PixelOrientedView::state() const {
  DataSet dataSet = GlView::state();
  DataSet selectedGraphPropertiesDataSet;

  for (size_t i = 0; i < selectedGraphProperties.size(); ++i) {
    ostringstream oss;
    oss << i;
    selectedGraphPropertiesDataSet.set(oss.str(), selectedGraphProperties[i]);
  }

  dataSet.set("selected graph properties", selectedGraphPropertiesDataSet);
  dataSet.set("layout", optionsWidget->getLayoutType());

  map<string, bool> tmpOverviewGenMap = overviewGenMap;

  for (const auto &selectedGraphProperty : selectedGraphProperties) {
    dataSet.set(selectedGraphProperty, tmpOverviewGenMap[selectedGraphProperty]);
  }

  dataSet.set("lastViewWindowWidth", glWidget()->width());
  dataSet.set("lastViewWindowHeight", glWidget()->height());
  dataSet.set("detail overview  name", detailOverviewPropertyName);
  dataSet.set("background color", optionsWidget->getBackgroundColor());

  if (needQuickAccessBar) {
    dataSet.set("quickAccessBarVisible", quickAccessBarVisible());
  }

  return dataSet;
}

void PixelOrientedView::graphChanged(Graph *) {
  setState(DataSet());
}

void PixelOrientedView::initLayoutFunctions() {
  delete hilbertLayout;
  delete squareLayout;
  delete zorderLayout;

  hilbertLayout = new HilbertLayout(int(ceil(log(pixelOrientedGraph->numberOfNodes()) / log(4))));
  squareLayout = new SquareLayout(int(ceil(sqrt(pixelOrientedGraph->numberOfNodes()))));
  zorderLayout = new ZorderLayout(int(ceil(log(pixelOrientedGraph->numberOfNodes()) / log(4))));
  layoutFunctionsMap["Zorder"] = zorderLayout;
  layoutFunctionsMap["Peano"] = hilbertLayout;
  layoutFunctionsMap["Square"] = squareLayout;
}

void PixelOrientedView::initPixelView() {

  if (lastNbNodes != pixelOrientedGraph->numberOfNodes()) {
    initLayoutFunctions();
  }

  Color backgroundColor = optionsWidget->getBackgroundColor();
  Color textColor = getTextColor();

  setLayoutFunction(layoutFunctionsMap[optionsWidget->getLayoutType()]);

  overviewWidth = MIN_IMAGE_WIDTH;
  overviewHeight = MIN_IMAGE_HEIGHT;

  minWidth = uint(floor(sqrt(double(pixelOrientedGraph->numberOfNodes())))) + 1;

  while (minWidth > overviewWidth) {
    overviewWidth *= 2;
    overviewHeight *= 2;
  }

  uint labelHeight = overviewHeight / 4;
  float spaceBetweenOverviews = overviewWidth / 10;

  pixelOrientedMediator->setImageSize(overviewWidth, overviewHeight);
  pixelOrientedMediator->setScreenFunctionsParameters(1, 0, 0, 0);

  overviewsComposite->reset(false);

  selectedGraphProperties = propertiesSelectionWidget->getSelectedGraphProperties();

  if (selectedGraphProperties.empty()) {
    return;
  }

  float squareRoot = sqrt(float(selectedGraphProperties.size()));
  const uint N =
      uint(squareRoot) + (fmod(float(selectedGraphProperties.size()), squareRoot) == 0.f ? 0u : 1u);

  for (size_t i = 0; i < selectedGraphProperties.size(); ++i) {

    if (dataMap.find(selectedGraphProperties[i]) == dataMap.end()) {
      dataMap[selectedGraphProperties[i]] =
          new GraphDimension(pixelOrientedGraph, selectedGraphProperties[i]);
    } else {
      dataMap[selectedGraphProperties[i]]->updateNodesRank();
    }

    uint row = i / N;
    uint col = i % N;

    Coord overviewBLCorner =
        Coord(col * (overviewWidth + spaceBetweenOverviews),
              -(labelHeight + row * (labelHeight + overviewHeight + spaceBetweenOverviews)), 0);
    ostringstream oss;
    oss << "pixel oriented overview for dimension " << selectedGraphProperties[i];

    if (overviewsMap.find(selectedGraphProperties[i]) == overviewsMap.end()) {
      auto *pixelOrientedOverview = new PixelOrientedOverview(
          dataMap[selectedGraphProperties[i]], pixelOrientedMediator, overviewBLCorner,
          selectedGraphProperties[i], backgroundColor, textColor);
      pixelOrientedOverview->computePixelView();
      overviewsMap[selectedGraphProperties[i]] = pixelOrientedOverview;

      if (overviewGenMap.find(selectedGraphProperties[i]) == overviewGenMap.end()) {
        overviewGenMap[selectedGraphProperties[i]] = false;
      }
    } else {
      overviewsMap[selectedGraphProperties[i]]->setBLCorner(overviewBLCorner);
      overviewsMap[selectedGraphProperties[i]]->setBackgroundColor(backgroundColor);
      overviewsMap[selectedGraphProperties[i]]->setTextColor(textColor);
    }

    overviewsComposite->addGlEntity(overviewsMap[selectedGraphProperties[i]], oss.str());
  }

  if (!detailOverviewPropertyName.empty()) {
    detailOverview = overviewsMap[detailOverviewPropertyName];
  }

  lastNbNodes = pixelOrientedGraph->numberOfNodes();
}

void PixelOrientedView::destroyOverviewsIfNeeded() {
  vector<string> propertiesToRemove;

  for (const auto &selectedGraphProperty : selectedGraphProperties) {
    if (!pixelOrientedGraph->existProperty(selectedGraphProperty)) {
      if (overviewsMap[selectedGraphProperty] == detailOverview) {
        detailOverview = nullptr;
        detailOverviewPropertyName = "";
      }

      delete overviewsMap[selectedGraphProperty];
      overviewsMap.erase(selectedGraphProperty);
      delete dataMap[selectedGraphProperty];
      dataMap.erase(selectedGraphProperty);
      propertiesToRemove.push_back(selectedGraphProperty);
    }
  }

  for (const auto &prop : propertiesToRemove) {
    selectedGraphProperties.erase(
        remove(selectedGraphProperties.begin(), selectedGraphProperties.end(), prop),
        selectedGraphProperties.end());
  }
}

void PixelOrientedView::destroyData() {
  for (const auto &it : dataMap) {
    delete it.second;
  }

  dataMap.clear();
}

void PixelOrientedView::addEmptyViewLabel() {
  Color backgroundColor = {optionsWidget->getBackgroundColor()};
  glWidget()->scene()->setBackgroundColor(backgroundColor);

  Color textColor = getTextColor();

  auto *noDimsLabel = new GlLabel(Coord(0.0f, 0.0f, 0.0f), Size(200.0f, 200.0f), textColor);
  noDimsLabel->setText(ViewName::PixelOrientedViewName);
  mainLayer->addGlEntity(noDimsLabel, "no dimensions label");
  auto *noDimsLabel1 = new GlLabel(Coord(0.0f, -50.0f, 0.0f), Size(400.0f, 200.0f), textColor);
  noDimsLabel1->setText("No graph properties selected.");
  mainLayer->addGlEntity(noDimsLabel1, "no dimensions label 1");
  auto *noDimsLabel2 = new GlLabel(Coord(0.0f, -100.0f, 0.0f), Size(700.0f, 200.0f), textColor);
  noDimsLabel2->setText("Go to the \"Properties\" tab in top right corner.");
  mainLayer->addGlEntity(noDimsLabel2, "no dimensions label 2");

  mainLayer->deleteGlEntity(glGraph);
}

void PixelOrientedView::removeEmptyViewLabel() {
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

    mainLayer->addGlEntity(glGraph, "graph");
  }
}

void PixelOrientedView::generatePixelOverview(PixelOrientedOverview *pixelOverview,
                                              GlWidget *glWidget) {
  pixelOverview->computePixelView(glWidget);
  overviewGenMap[pixelOverview->getDimensionName()] = true;
}

void PixelOrientedView::draw() {
  GlWidget *glw = glWidget();
  GlScene *scene = glw->scene();

  if (pixelOrientedGraph != nullptr) {
    scene->setBackgroundColor(optionsWidget->getBackgroundColor());
    uint lastNbDimensionsSelected = selectedGraphProperties.size();
    destroyOverviewsIfNeeded();

    if (pixelOrientedGraph->numberOfNodes() >= 2) { // We stop the creation of pixel overviews if we
      // have less than 2 nodes as we need at least two
      // to compute sizes for overview
      initPixelView();
    } else {
      selectedGraphProperties.clear();
    }

    if (selectedGraphProperties.empty()) {
      if (!smallMultiplesView) {
        switchFromDetailViewToSmallMultiples();
      }

      removeEmptyViewLabel();
      addEmptyViewLabel();
      if (quickAccessBarVisible()) {
        _quickAccessBar->setEnabled(false);
      }
      scene->centerScene();
      glw->draw();
      return;
    } else {
      removeEmptyViewLabel();
      if (quickAccessBarVisible()) {
        _quickAccessBar->setEnabled(true);
      }
    }

    if (lastNbDimensionsSelected != selectedGraphProperties.size()) {
      center = true;
    }

    if (!smallMultiplesView && lastNbDimensionsSelected == 1 &&
        selectedGraphProperties.size() > 1) {
      switchFromDetailViewToSmallMultiples();
    } else if (selectedGraphProperties.size() == 1) {
      glw->makeCurrent();
      overviewsMap[selectedGraphProperties[0]]->computePixelView();
      overviewGenMap[selectedGraphProperties[0]] = true;
      switchFromSmallMultiplesToDetailView(overviewsMap[selectedGraphProperties[0]]);
    } else if (!smallMultiplesView && detailOverview != nullptr) {
      detailOverview->computePixelView();
      smallMultiplesNeedUpdate = true;

      if (newGraphSet) {
        switchFromSmallMultiplesToDetailView(detailOverview);
        newGraphSet = false;
      }
    } else if (!smallMultiplesView && detailOverview == nullptr) {
      switchFromDetailViewToSmallMultiples();
    } else {
      glw->makeCurrent();
      updateOverviews(true);
    }

    if (!smallMultiplesView && detailViewLabel != nullptr) {
      detailViewLabel->setColor(getTextColor());
    }

    if (center) {
      centerView();
      center = false;
    } else {
      glw->draw();
    }
  } else {
    glw->draw();
  }
}

void PixelOrientedView::refresh() {
  glWidget()->redraw();
}

void PixelOrientedView::init() {
  emit drawNeeded();
}

Color PixelOrientedView::getTextColor() const {
  Color backgroundColor = optionsWidget->getBackgroundColor();

  Color textColor;
  int bgV = backgroundColor.getV();

  if (bgV < 128) {
    textColor = Color(255, 255, 255);
  } else {
    textColor = Color(0, 0, 0);
  }

  return textColor;
}

void PixelOrientedView::centerView(bool) {
  if (!glWidget()->isVisible()) {
    if (lastViewWindowWidth != 0 && lastViewWindowHeight != 0) {
      glWidget()->scene()->adjustSceneToSize(lastViewWindowWidth, lastViewWindowHeight);
    } else {
      glWidget()->scene()->centerScene();
    }
  } else {
    glWidget()->scene()->adjustSceneToSize(glWidget()->width(), glWidget()->height());
  }

  // we apply a zoom factor to preserve a 50 px margin height
  // to ensure the scene will not be drawn under the configuration tabs title
  float glHeight = graphicsView()->width();
  glWidget()->scene()->zoomFactor((glHeight - 50) / glHeight);
  glWidget()->draw();
}

void PixelOrientedView::updateOverviews(const bool updateAll) {

  if (smallMultiplesView) {
    mainLayer->deleteGlEntity(overviewsComposite);
  } else {
    setGraphView(glGraph, false);
    mainLayer->deleteGlEntity(detailViewLabel);
  }

  uint nbOverviews = selectedGraphProperties.size();
  unsigned currentStep = 0;
  Camera &cam = glWidget()->scene()->getGraphCamera();
  double sceneRadiusBak = cam.getSceneRadius();
  double zoomFactorBak = cam.getZoomFactor();
  Coord eyesBak = cam.getEyes();
  Coord centerBak = cam.getCenter();
  Coord upBak = cam.getUp();
  float width = cam.getBoundingBox().width();

  auto *progressBar =
      new GlProgressBar(centerBak + Coord(0, width / 70, 0), width - width / 10, width / 12,
                        // use same green color as the highlighting one
                        // in workspace panel
                        Color(0xCB, 0xDE, 0x5D));
  progressBar->setComment("Updating pixel oriented view...");
  progressBar->progress(currentStep, nbOverviews);
  mainLayer->addGlEntity(progressBar, "progress bar");
  glWidget()->draw();

  // disable user input
  tlp::disableQtUserInput();
  // before allowing the progressBar display
  QApplication::processEvents();

  for (const auto &it : overviewsMap) {
    if (std::find(selectedGraphProperties.begin(), selectedGraphProperties.end(), it.first) !=
        selectedGraphProperties.end()) {
      if (updateAll || overviewGenMap[it.first]) {
        it.second->computePixelView();
        overviewGenMap[it.first] = true;
      }

      progressBar->progress(++currentStep, nbOverviews);
      glWidget()->draw();
      // needed to display progressBar
      QApplication::processEvents();
    }
  }

  // re-enable user input
  tlp::enableQtUserInput();

  mainLayer->deleteGlEntity(progressBar);
  delete progressBar;

  if (smallMultiplesView) {
    mainLayer->addGlEntity(overviewsComposite, "overviews composite");
  } else {
    setGraphView(glGraph, true);
    mainLayer->addGlEntity(detailViewLabel, "dimension label");
  }

  cam.setSceneRadius(sceneRadiusBak);
  cam.setZoomFactor(zoomFactorBak);
  cam.setEyes(eyesBak);
  cam.setCenter(centerBak);
  cam.setUp(upBak);

  glWidget()->draw();
}

vector<PixelOrientedOverview *> PixelOrientedView::getOverviews() {
  vector<PixelOrientedOverview *> ret;

  for (const auto &it : overviewsMap) {
    if (std::find(selectedGraphProperties.begin(), selectedGraphProperties.end(), it.first) !=
        selectedGraphProperties.end()) {
      ret.push_back(it.second);
    }
  }

  return ret;
}

QuickAccessBar *PixelOrientedView::getQuickAccessBarImpl() {
  auto *_bar = new PixelOrientedViewQuickAccessBar(optionsWidget);

  connect(_bar, &PixelOrientedViewQuickAccessBar::settingsChanged, this,
          &PixelOrientedView::applySettings);
  return _bar;
}

void PixelOrientedView::switchFromSmallMultiplesToDetailView(PixelOrientedOverview *pixelOverview) {

  if (smallMultiplesView) {
    sceneRadiusBak = glWidget()->scene()->getGraphCamera().getSceneRadius();
    zoomFactorBak = glWidget()->scene()->getGraphCamera().getZoomFactor();
    eyesBak = glWidget()->scene()->getGraphCamera().getEyes();
    centerBak = glWidget()->scene()->getGraphCamera().getCenter();
    upBak = glWidget()->scene()->getGraphCamera().getUp();
  }

  mainLayer->deleteGlEntity(overviewsComposite);
  GlGraphInputData *inputData = glGraph->getInputData();
  inputData->setLayout(pixelOverview->getPixelViewLayout());
  inputData->setSizes(pixelOverview->getPixelViewSize());

  setGraphView(glGraph, true);

  GlBoundingBoxSceneVisitor glBBSV(inputData);
  glGraph->acceptVisitor(&glBBSV);
  BoundingBox graphBoundingBox = glBBSV.getBoundingBox();

  delete detailViewLabel;
  detailViewLabel = new GlLabel(
      Coord((graphBoundingBox[0][0] + graphBoundingBox[1][0]) / 2.f,
            graphBoundingBox[0][1] - (graphBoundingBox[1][1] - graphBoundingBox[0][1]) / 8.f),
      Size((graphBoundingBox[1][0] - graphBoundingBox[0][0]),
           (graphBoundingBox[1][1] - graphBoundingBox[0][1]) / 4.f),
      getTextColor());
  detailViewLabel->setText(pixelOverview->getData()->getDimensionName());
  mainLayer->addGlEntity(detailViewLabel, "dimension label");
  smallMultiplesView = false;
  toggleInteractors(true);
  detailOverview = pixelOverview;
  detailOverviewPropertyName = pixelOverview->getDimensionName();

  if (selectedGraphProperties.size() > 1) {
    propertiesSelectionWidget->setEnabled(false);
  }

  centerView();
}

void PixelOrientedView::switchFromDetailViewToSmallMultiples() {
  if (smallMultiplesNeedUpdate) {
    updateOverviews(true);
    smallMultiplesNeedUpdate = false;
  }

  setGraphView(glGraph, false);
  mainLayer->deleteGlEntity(detailViewLabel);
  mainLayer->addGlEntity(overviewsComposite, "overviews composite");
  glWidget()->scene()->getGraphCamera().setSceneRadius(sceneRadiusBak);
  glWidget()->scene()->getGraphCamera().setZoomFactor(zoomFactorBak);
  glWidget()->scene()->getGraphCamera().setEyes(eyesBak);
  glWidget()->scene()->getGraphCamera().setCenter(centerBak);
  glWidget()->scene()->getGraphCamera().setUp(upBak);
  smallMultiplesView = true;
  toggleInteractors(false);
  detailOverview = nullptr;
  detailOverviewPropertyName = "";
  propertiesSelectionWidget->setEnabled(true);
  glWidget()->draw();
}

BoundingBox PixelOrientedView::getSmallMultiplesViewBoundingBox() {
  GlBoundingBoxSceneVisitor glBBSV(nullptr);
  overviewsComposite->acceptVisitor(&glBBSV);
  return glBBSV.getBoundingBox();
}

void PixelOrientedView::interactorsInstalled(const QList<tlp::Interactor *> &) {
  toggleInteractors(false);
}

void PixelOrientedView::toggleInteractors(const bool activate) {
  View::toggleInteractors(activate, {InteractorName::PixelOrientedInteractorNavigation});
}

void PixelOrientedView::registerTriggers() {
  for (auto *obs : triggers()) {
    removeRedrawTrigger(obs);
  }

  addRedrawTrigger(graph());

  for (auto *prop : graph()->getObjectProperties()) {
    addRedrawTrigger(prop);
  }
}

void PixelOrientedView::applySettings() {
  if (propertiesSelectionWidget->configurationChanged() || optionsWidget->configurationChanged()) {
    updateOverviews(true);
    draw();

    if (!smallMultiplesView) {
      mainLayer->deleteGlEntity(detailViewLabel);
      GlGraphInputData *inputData = glGraph->getInputData();
      inputData->setLayout(detailOverview->getPixelViewLayout());
      inputData->setSizes(detailOverview->getPixelViewSize());
      GlBoundingBoxSceneVisitor glBBSV(inputData);
      glGraph->acceptVisitor(&glBBSV);
      BoundingBox graphBoundingBox = glBBSV.getBoundingBox();

      detailViewLabel->setPosition(
          Coord((graphBoundingBox[0][0] + graphBoundingBox[1][0]) / 2.f,
                graphBoundingBox[0][1] - (graphBoundingBox[1][1] - graphBoundingBox[0][1]) / 8.f));

      detailViewLabel->setSize(Size((graphBoundingBox[1][0] - graphBoundingBox[0][0]),
                                    (graphBoundingBox[1][1] - graphBoundingBox[0][1]) / 4.f));

      mainLayer->addGlEntity(detailViewLabel, "dimension label");
      centerView();
    }
  }
}
}
