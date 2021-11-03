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

#include <talipot/GlVertexArrayManager.h>
#include <talipot/GlComplexPolygon.h>
#include <talipot/TlpQtTools.h>
#include <talipot/NodeLinkDiagramView.h>

#include <QTimer>

#include "GeographicView.h"
#include "GeographicViewGraphicsView.h"
#include "GeographicViewConfigWidget.h"
#include "GeolocationConfigWidget.h"

using namespace std;
using namespace tlp;

const QMap<GeographicView::ViewType, QString> GeographicView::viewTypeToName = {
    {GeographicView::OpenStreetMap, "OpenStreetMap (Leaflet)"},
    {GeographicView::EsriSatellite, "Esri Satellite (Leaflet)"},
    {GeographicView::EsriTerrain, "Esri Terrain (Leaflet)"},
    {GeographicView::EsriGrayCanvas, "Esri Gray Canvas (Leaflet)"},
    {GeographicView::Wikimedia, "Wikimedia (Leaflet)"},
    {GeographicView::Polygon, "Polygon"},
    {GeographicView::Globe, "Globe"},
    {GeographicView::LeafletCustomTileLayer, "Custom Tile Layer (Leaflet)"}};

GeographicView::GeographicView(PluginContext *)
    : geoViewGraphicsView(nullptr), geoViewConfigWidget(nullptr), geolocationConfigWidget(nullptr),
      sceneConfigurationWidget(nullptr), sceneLayersConfigurationWidget(nullptr),
      centerViewAction(nullptr), showConfPanelAction(nullptr), useSharedLayoutProperty(true),
      useSharedSizeProperty(true), useSharedShapeProperty(true), mapCenterLatitudeInit(0),
      mapCenterLongitudeInit(0), mapZoomInit(0), _viewActionsManager(nullptr) {
  _viewType = OpenStreetMap;
}

GeographicView::~GeographicView() {
  delete geolocationConfigWidget;
  delete geoViewConfigWidget;
  delete sceneConfigurationWidget;
  delete sceneLayersConfigurationWidget;
  delete _viewActionsManager;
}

void GeographicView::setupUi() {
  geoViewGraphicsView = new GeographicViewGraphicsView(this, new QGraphicsScene());

  geoViewConfigWidget = new GeographicViewConfigWidget();
  connect(geoViewConfigWidget, &GeographicViewConfigWidget::mapToPolygonSignal, this,
          &GeographicView::mapToPolygon);

  geolocationConfigWidget = new GeolocationConfigWidget();
  connect(geolocationConfigWidget, &GeolocationConfigWidget::computeGeoLayout, this,
          &GeographicView::computeGeoLayout);

  sceneConfigurationWidget = new SceneConfigWidget();
  sceneConfigurationWidget->setGlWidget(geoViewGraphicsView->getGlWidget());

  sceneLayersConfigurationWidget = new SceneLayersConfigWidget();
  sceneLayersConfigurationWidget->setGlWidget(geoViewGraphicsView->getGlWidget());

  centerViewAction = new QAction("Center view", this);
  connect(centerViewAction, &QAction::triggered, [this] { centerView(); });

  activateTooltipAndUrlManager(geoViewGraphicsView->getGlWidget());
  _viewActionsManager = new ViewActionsManager(this, geoViewGraphicsView->getGlWidget(), true);
}

void GeographicView::viewTypeChanged(int idx) {
  viewTypeChanged(geoViewGraphicsView->getViewTypeComboBox()->itemText(idx));
}

void GeographicView::viewTypeChanged(const QString &viewTypeName) {
  _viewType = getViewTypeFromName(viewTypeName);
  geoViewGraphicsView->switchViewType();
}

void GeographicView::fillContextMenu(QMenu *menu, const QPointF &pf) {
  _viewActionsManager->fillContextMenu(menu);
  QAction *action = menu->addAction("Zoom +");
  action->setToolTip("Increase zoom level");
  connect(action, &QAction::triggered, this, &GeographicView::zoomIn);
  action = menu->addAction("Zoom -");
  action->setToolTip("Increase zoom level");
  connect(action, &QAction::triggered, this, &GeographicView::zoomOut);
  menu->addSeparator();
  menu->addAction("Augmented display")->setEnabled(false);
  menu->addSeparator();
  View::fillContextMenu(menu, pf);
}

void GeographicView::setState(const DataSet &dataSet) {
  geolocationConfigWidget->setGraph(graph());
  geoViewGraphicsView->setGraph(graph());

  updatePoly(true);

  if (dataSet.exists("configurationWidget")) {
    DataSet conf;
    dataSet.get("configurationWidget", conf);
    geoViewConfigWidget->setState(conf);
    updatePoly();
    updateSharedProperties();
  }

  loadStoredPolyInformation(dataSet);

  if (dataSet.exists("viewType")) {
    int viewType = 0;
    dataSet.get("viewType", viewType);
    _viewType = static_cast<ViewType>(viewType);
  }

  string viewTypeName = QStringToTlpString(getViewNameFromType(_viewType));

  viewTypeChanged(viewTypeName.c_str());

  sceneLayersConfigurationWidget->setGlWidget(geoViewGraphicsView->getGlWidget());
  sceneConfigurationWidget->setGlWidget(geoViewGraphicsView->getGlWidget());

  registerTriggers();

  string latitudePropName = "latitude";
  string longitudePropName = "longitude";

  dataSet.get("latitudePropertyName", latitudePropName);
  dataSet.get("longitudePropertyName", longitudePropName);

  if (graph()->existProperty(latitudePropName) && graph()->existProperty(longitudePropName)) {
    geolocationConfigWidget->setLatLngGeoLocMethod(latitudePropName, longitudePropName);

    string edgesPathsPropName;
    dataSet.get("edgesPathsPropertyName", edgesPathsPropName);
    geolocationConfigWidget->setEdgesPathsPropertyName(edgesPathsPropName);

    computeGeoLayout();
  }

  GlGraph *glGraph = geoViewGraphicsView->getGlWidget()->getScene()->getGlGraph();
  GlGraphRenderingParameters &rp = glGraph->getRenderingParameters();

  if (dataSet.exists("renderingParameters")) {
    DataSet renderingParameters;
    dataSet.get("renderingParameters", renderingParameters);
    rp.setParameters(renderingParameters);
    string s;

    if (renderingParameters.get("elementsOrderingPropertyName", s) && !s.empty()) {
      rp.setElementOrderingProperty(dynamic_cast<tlp::NumericProperty *>(graph()->getProperty(s)));
    }
  } else {
    // same default initialization as NodeLinkDiagramView
    NodeLinkDiagramView::initRenderingParameters(rp);
  }

  sceneConfigurationWidget->resetChanges();

  View::setState(dataSet);

  if (dataSet.exists("mapCenterLatitude")) {

    dataSet.get("mapCenterLatitude", mapCenterLatitudeInit);
    dataSet.get("mapCenterLongitude", mapCenterLongitudeInit);
    dataSet.get("mapZoom", mapZoomInit);

    QTimer::singleShot(1500, this, &GeographicView::initMap);
  }
}

void GeographicView::initMap() {
  geoViewGraphicsView->getLeafletMapsPage()->setMapCenter(mapCenterLatitudeInit,
                                                          mapCenterLongitudeInit);
  geoViewGraphicsView->getLeafletMapsPage()->setCurrentZoom(mapZoomInit);
}

DataSet GeographicView::state() const {
  DataSet dataSet = View::state();
  DataSet configurationWidget = geoViewConfigWidget->state();
  dataSet.set("configurationWidget", configurationWidget);
  dataSet.set("viewType", int(_viewType));
  pair<double, double> mapCenter = geoViewGraphicsView->getLeafletMapsPage()->getCurrentMapCenter();
  dataSet.set("mapCenterLatitude", mapCenter.first);
  dataSet.set("mapCenterLongitude", mapCenter.second);
  dataSet.set("mapZoom", geoViewGraphicsView->getLeafletMapsPage()->getCurrentMapZoom());
  dataSet.set("renderingParameters",
              geoViewGraphicsView->getGlWidget()->getGlGraphRenderingParameters().getParameters());

  saveStoredPolyInformation(dataSet);

  std::string latitudePropName = geolocationConfigWidget->getLatitudeGraphPropertyName();
  std::string longitudePropName = geolocationConfigWidget->getLongitudeGraphPropertyName();

  if (latitudePropName != longitudePropName && graph()->existProperty(latitudePropName) &&
      graph()->existProperty(longitudePropName)) {
    dataSet.set("latitudePropertyName", latitudePropName);
    dataSet.set("longitudePropertyName", longitudePropName);
  }

  std::string edgesPathsPropName = geolocationConfigWidget->getEdgesPathsPropertyName();

  if (graph()->existProperty(edgesPathsPropName)) {
    dataSet.set("edgesPathsPropertyName", edgesPathsPropName);
  }

  return dataSet;
}

void GeographicView::draw() {
  geoViewGraphicsView->draw();
}

void GeographicView::refresh() {
  geoViewGraphicsView->draw();
}

void GeographicView::computeGeoLayout() {
  if (geolocationConfigWidget->geolocateByAddress()) {
    geoViewGraphicsView->createLayoutWithAddresses(
        geolocationConfigWidget->getAddressGraphPropertyName(),
        geolocationConfigWidget->createLatAndLngProperties(),
        geolocationConfigWidget->resetLatAndLngValues());

    if (geolocationConfigWidget->createLatAndLngProperties()) {
      geolocationConfigWidget->setGraph(graph());
      geolocationConfigWidget->setLatLngGeoLocMethod("latitude", "longitude");
    }
  } else {
    string latProp = geolocationConfigWidget->getLatitudeGraphPropertyName();
    string lngProp = geolocationConfigWidget->getLongitudeGraphPropertyName();
    string edgesPathsProp;

    if (geolocationConfigWidget->useEdgesPaths()) {
      edgesPathsProp = geolocationConfigWidget->getEdgesPathsPropertyName();
    }

    if (latProp != lngProp) {
      geoViewGraphicsView->createLayoutWithLatLngs(latProp, lngProp, edgesPathsProp);
    }
  }

  geoViewGraphicsView->centerView();
  // check for shared properties
  // before computing view layout
  updateSharedProperties();
  geoViewGraphicsView->setGeoLayoutComputed();
  // compute view layout
  geoViewGraphicsView->switchViewType();
}

void GeographicView::centerView() {
  geoViewGraphicsView->centerView();
}

void GeographicView::zoomIn() {
  geoViewGraphicsView->zoomIn();
}

void GeographicView::zoomOut() {
  geoViewGraphicsView->zoomOut();
}

void GeographicView::currentZoomChanged() {
  geoViewGraphicsView->currentZoomChanged();
}

QList<QWidget *> GeographicView::configurationWidgets() const {
  return {geolocationConfigWidget, geoViewConfigWidget, sceneConfigurationWidget,
          sceneLayersConfigurationWidget};
}

void GeographicView::applySettings() {
  updateSharedProperties();
  updatePoly();
}

void GeographicView::updateSharedProperties() {
  GlGraphInputData *inputData = geoViewGraphicsView->getGlWidget()->getGlGraphInputData();

  if (useSharedLayoutProperty != geoViewConfigWidget->useSharedLayoutProperty()) {
    useSharedLayoutProperty = geoViewConfigWidget->useSharedLayoutProperty();

    if (useSharedLayoutProperty) {
      geoViewGraphicsView->setGeoLayout(graph()->getLayoutProperty("viewLayout"));
    } else {
      geoViewGraphicsView->setGeoLayout(new LayoutProperty(graph()));
    }
  }

  if (useSharedShapeProperty != geoViewConfigWidget->useSharedShapeProperty()) {
    useSharedShapeProperty = geoViewConfigWidget->useSharedShapeProperty();

    if (useSharedShapeProperty) {
      geoViewGraphicsView->setGeoShape(graph()->getIntegerProperty("viewShape"));
    } else {
      geoViewGraphicsView->setGeoShape(new IntegerProperty(graph()));
    }
  }

  if (useSharedSizeProperty != geoViewConfigWidget->useSharedSizeProperty()) {
    useSharedSizeProperty = geoViewConfigWidget->useSharedSizeProperty();

    if (useSharedSizeProperty) {
      geoViewGraphicsView->setGeoSizes(graph()->getSizeProperty("viewSize"));
    } else {
      geoViewGraphicsView->setGeoSizes(new SizeProperty(graph()));
    }
  }

  inputData->glVertexArrayManager()->setHaveToComputeAll(true);
}

void GeographicView::updatePoly(bool force) {
  if (geoViewConfigWidget->polyOptionsChanged() || force) {
    switch (geoViewConfigWidget->polyFileType()) {
    case GeographicViewConfigWidget::CsvFile: {
      geoViewGraphicsView->loadCsvFile(geoViewConfigWidget->getCsvFile());
      break;
    }

    case GeographicViewConfigWidget::PolyFile: {
      geoViewGraphicsView->loadPolyFile(geoViewConfigWidget->getPolyFile());
      break;
    }

    default: {
      geoViewGraphicsView->loadDefaultMap();
      break;
    }
    }
  }
}

void GeographicView::loadStoredPolyInformation(const DataSet &dataset) {
  if (dataset.exists("polygons")) {
    DataSet polyConf;
    dataset.get("polygons", polyConf);
    GlComposite *composite = geoViewGraphicsView->getPolygon();
    const map<string, GlEntity *> &entities = composite->getGlEntities();

    for (const auto &it : entities) {
      DataSet entityData;

      if (polyConf.exists(it.first)) {
        polyConf.get(it.first, entityData);
        Color color;
        entityData.get("color", color);
        static_cast<GlComplexPolygon *>(it.second)->setFillColor(color);
        entityData.get("outlineColor", color);
        static_cast<GlComplexPolygon *>(it.second)->setOutlineColor(color);
      }
    }
  }
}

void GeographicView::saveStoredPolyInformation(DataSet &dataset) const {
  GlComposite *composite = geoViewGraphicsView->getPolygon();
  DataSet polyConf;
  const map<string, GlEntity *> &entities = composite->getGlEntities();

  for (const auto &it : entities) {
    DataSet entityData;
    entityData.set("color", static_cast<GlComplexPolygon *>(it.second)->getFillColor());
    entityData.set("outlineColor", static_cast<GlComplexPolygon *>(it.second)->getOutlineColor());
    polyConf.set(it.first, entityData);
  }

  dataset.set("polygons", polyConf);
}

QGraphicsItem *GeographicView::centralItem() const {
  return geoViewGraphicsView->getPlaceHolderItem();
}

void GeographicView::registerTriggers() {
  clearRedrawTriggers();

  if (graph() == nullptr) {
    return;
  }

  addRedrawTrigger(geoViewGraphicsView->getGlWidget()->getScene()->getGlGraph()->getGraph());
  auto properties = geoViewGraphicsView->getGlWidget()->getGlGraphInputData()->properties();

  for (auto *p : properties) {
    addRedrawTrigger(p);
  }
}

QPixmap GeographicView::snapshot(const QSize &size) const {

  // hide the graphics widget used to configure the view
  // before taking a snapshot
  QList<QGraphicsProxyWidget *> gWidgetsToRestore;
  QList<QGraphicsItem *> sceneItems = geoViewGraphicsView->scene()->items();

  for (auto *sceneItem : sceneItems) {
    auto *gWidget = dynamic_cast<QGraphicsProxyWidget *>(sceneItem);

    if (gWidget && gWidget->isVisible()) {
      gWidget->hide();
      gWidgetsToRestore.push_back(gWidget);
    }
  }

  int width = geoViewGraphicsView->width();
  int height = geoViewGraphicsView->height();

  QImage snapshotImage(width, height, QImage::Format_RGB32);
  QPainter painter(&snapshotImage);
  geoViewGraphicsView->scene()->render(&painter);
  painter.end();

  // restore the graphics widgets previously hidden
  for (auto *w : gWidgetsToRestore) {
    w->show();
  }

  return QPixmap::fromImage(snapshotImage)
      .scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

GeographicView::ViewType GeographicView::getViewTypeFromName(const QString &name) {
  for (auto vt : viewTypeToName.keys()) {
    if (viewTypeToName[vt] == name) {
      return vt;
    }
  }
  return GeographicView::OpenStreetMap;
}

QString GeographicView::getViewNameFromType(GeographicView::ViewType viewType) {
  if (viewTypeToName.contains(viewType)) {
    return viewTypeToName[viewType];
  }
  return viewTypeToName[GeographicView::OpenStreetMap];
}

QList<GeographicView::ViewType> GeographicView::getViewTypes() {
  return viewTypeToName.keys();
}

void GeographicView::graphChanged(Graph *graph) {
  setState(DataSet());
  if (graph->isEmpty()) {
    computeGeoLayout();
  }
}

LeafletMaps *GeographicView::getLeafletMap() {
  return geoViewGraphicsView->getLeafletMapsPage();
}

bool GeographicView::getNodeOrEdgeAtViewportPos(int x, int y, node &n, edge &e) const {
  return GlView::getNodeOrEdgeAtViewportPos(geoViewGraphicsView->getGlWidget(), x, y, n, e);
}

void GeographicView::currentInteractorChanged(tlp::Interactor *i) {
  i->install(geoViewGraphicsView->getGlWidget());
}

void GeographicView::mapToPolygon() {
  geoViewGraphicsView->mapToPolygon();
}

QGraphicsView *GeographicView::graphicsView() const {
  return geoViewGraphicsView;
}

PLUGIN(GeographicView)
