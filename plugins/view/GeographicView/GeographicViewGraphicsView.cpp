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

#include "GeographicViewGraphicsView.h"
#include "GeographicViewConfigWidget.h"
#include "GeographicView.h"
#include "NominatimGeocoder.h"

#include <talipot/GlCPULODCalculator.h>
#include <talipot/GlComplexPolygon.h>
#include <talipot/GlSphere.h>
#include <talipot/GlSceneZoomAndPan.h>
#include <talipot/GlTextureManager.h>
#include <talipot/ViewSettings.h>
#include <talipot/TlpQtTools.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/Gl2DRect.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QNetworkDiskCache>
#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>
#include <QEvent>
#include <QGraphicsProxyWidget>
#include <QStandardPaths>

#include <QGeoView/QGVMap.h>
#include <QGeoView/QGVMapQGView.h>
#include <QGeoView/QGVLayerOSM.h>
#include <QGeoView/QGVProjection.h>
#include <QGeoView/QGVWidgetText.h>

#include "AddressSelectionDialog.h"
#include "ProgressWidgetGraphicsProxy.h"
#include "QGVLayerEsri.h"
#include "QGVLayerGeoPortail.h"
#include "QGVCustomTilesLayer.h"

using namespace std;

namespace tlp {

const string planisphereTextureId = ":/talipot/view/geographic/planisphere.jpg";

void setupCachedNetworkAccessManager(QObject *parent) {
  QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
  cacheDir.mkdir("geotiles");
  cacheDir.cd("geotiles");
  cacheDir.removeRecursively();
  auto cache = new QNetworkDiskCache(parent);
  cache->setCacheDirectory(cacheDir.absolutePath());
  auto manager = new QNetworkAccessManager(parent);
  manager->setCache(cache);
  QGV::setNetworkManager(manager);
}

class CustomGlWidget : public GlWidget {
public:
  CustomGlWidget(QWidget *parent = nullptr, View *view = nullptr) : GlWidget(parent, view) {}
  void zoomAndPanAnimation(const tlp::BoundingBox &, const double,
                           AdditionalGlSceneAnimation *) override {}
};

GlComposite *readPolyFile(QString fileName) {
  auto *composite = new GlComposite;

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    return nullptr;
  }

  string polygonName;
  vector<vector<Coord>> data;
  vector<Coord> currentVector;
  bool ok;
  QString line;

  while (!file.atEnd()) {
    line = file.readLine();

    if (line.isEmpty() || line == "\n") {
      continue;
    }

    line.toUInt(&ok);

    if (ok) {
      if (!currentVector.empty()) {
        data.push_back(currentVector);
      }

      currentVector = vector<Coord>();
      continue;
    }

    if (line == "END\n") {
      continue;
    }

    QStringList strList = line.split(" ");

    bool findLng = false;
    bool findLat = false;
    float lng = 0;
    float lat = 0;

    for (const auto &s : strList) {
      s.toDouble(&ok);

      if (ok) {
        if (!findLng) {
          findLng = true;
          lng = s.toDouble();
        } else {
          findLat = true;
          lat = s.toDouble();
        }
      }
    }

    if (!findLat) {

      if (!polygonName.empty()) {

        if (!currentVector.empty()) {
          data.push_back(currentVector);
        }

        if (!data.empty()) {

          composite->addGlEntity(
              new GlComplexPolygon(data, Color(0, 0, 0, 50), Color(0, 0, 0, 255)), polygonName);
          data.clear();
          currentVector.clear();
        }
      }

      polygonName = QStringToTlpString(line);
      continue;
    }

    if (lat == 90.) {
      lat = 89.999f;
    }

    double mercatorLatitude = lat * 2. / 360. * M_PI;
    mercatorLatitude = sin(abs(mercatorLatitude));
    mercatorLatitude = log((1. + mercatorLatitude) / (1. - mercatorLatitude)) / 2.;

    if (lat < 0) {
      mercatorLatitude = 0. - mercatorLatitude;
    }

    if (mercatorLatitude * 360. / M_PI < -360) {
      mercatorLatitude = -M_PI;
    }

    currentVector.push_back(Coord(lng * 2., mercatorLatitude * 360. / M_PI, 0));
  }

  if (!polygonName.empty()) {
    if (!currentVector.empty()) {
      data.push_back(currentVector);
    }

    composite->addGlEntity(new GlComplexPolygon(data, Color(0, 0, 0, 50), Color(0, 0, 0, 255)),
                           polygonName);
  }

  return composite;
}

GlComposite *readCsvFile(QString fileName) {

  auto *composite = new GlComposite;

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    return nullptr;
  }

  vector<vector<Coord>> data;
  vector<Coord> currentVector;
  int lastIndex = 0;

  while (!file.atEnd()) {
    QString line(file.readLine());
    QStringList strList = line.split("\t");

    if (strList.size() != 3) {
      if (!currentVector.empty()) {
        data.push_back(currentVector);
      }

      currentVector.clear();
      continue;
    }

    if (strList[0].toInt() != lastIndex) {
      if (!currentVector.empty()) {
        data.push_back(currentVector);
      }

      lastIndex = strList[0].toInt();
      currentVector.clear();
    }

    double mercatorLatitude = strList[1].toDouble();
    mercatorLatitude = sin(abs(mercatorLatitude));
    mercatorLatitude = log((1. + mercatorLatitude) / (1. - mercatorLatitude)) / 2.;

    if (strList[1].toDouble() < 0) {
      mercatorLatitude = 0. - mercatorLatitude;
    }

    currentVector.push_back(
        Coord((strList[2].toDouble()) * 360. / M_PI, mercatorLatitude * 360. / M_PI, 0));
  }

  if (data.empty()) {
    return nullptr;
  }

  composite->addGlEntity(new GlComplexPolygon(data, Color(0, 0, 0, 50), Color(0, 0, 0, 255)),
                         "polygon");

  return composite;
}

static inline double toRadian(double val) {
  return val * M_PI / 360.;
}

static inline double toDegree(double val) {
  return val * 360. / M_PI;
}

static inline double latitudeToMercator(double latitude) {
  double mercatorLatitude = toRadian(latitude * 2);
  mercatorLatitude = sin(abs(mercatorLatitude));
  mercatorLatitude = log((1. + mercatorLatitude) / (1. - mercatorLatitude)) / 2.;

  if (latitude < 0) {
    return toDegree(-mercatorLatitude);
  }

  return toDegree(mercatorLatitude);
}

static double mercatorToLatitude(double mercator) {
  return (atan(sinh(mercator / 360. * M_PI)) / M_PI * 360.) / 2;
}

static inline Coord latLngToPolar(const pair<double, double> &latLng) {
  return Coord(toRadian(latLng.first * 2), toRadian(latLng.second * 2));
}

static inline Coord polarToSpherical(const Coord &polar, float radius) {
  float lambda = polar[1];
  float theta = lambda;
  if (lambda > M_PI) {
    theta = lambda + 2. * M_PI;
  }
  float phi = M_PI / 2. - polar[0];
  return {radius * sin(phi) * cos(theta), radius * sin(phi) * sin(theta), radius * cos(phi)};
}

static inline Coord projectLatLngToSphere(const pair<double, double> &latLng, float radius) {
  return polarToSpherical(latLngToPolar(latLng), radius);
}

QGraphicsProxyWidget *proxyGM = nullptr;

static flat_hash_map<GeographicView::ViewType, unique_ptr<QGVLayerTilesOnline>> initTilesLayers() {
  flat_hash_map<GeographicView::ViewType, unique_ptr<QGVLayerTilesOnline>> tilesLayers;
  tilesLayers.insert({GeographicView::OpenStreetMap, make_unique<QGVLayerOSM>()});
  tilesLayers.insert(
      {GeographicView::EsriSatellite, make_unique<QGVLayerEsri>(EsriMapType::Satellite)});
  tilesLayers.insert(
      {GeographicView::EsriTerrain, make_unique<QGVLayerEsri>(EsriMapType::Terrain)});
  tilesLayers.insert(
      {GeographicView::EsriGrayCanvas, make_unique<QGVLayerEsri>(EsriMapType::GrayCanvas)});
  tilesLayers.insert(
      {GeographicView::GeoportailPlan, make_unique<QGVLayerGeoPortail>(GeoPortailMapType::Plan)});
  tilesLayers.insert({GeographicView::GeoportailSatellite,
                      make_unique<QGVLayerGeoPortail>(GeoPortailMapType::Satellite)});
  tilesLayers.insert({GeographicView::CustomTilesLayer, make_unique<QGVCustomTilesLayer>()});

  tilesLayers[GeographicView::OpenStreetMap]->setDescription(
      "© <a href=\"https://www.openstreetmap.org/copyright\">OpenStreetMap contributors</a> ♥ <a "
      "href=\"https://supporting.openstreetmap.org\">Make a Donation</a>. <a "
      "href=\"https://wiki.osmfoundation.org/wiki/Terms_of_Use\">Website and API terms</a>");
  return tilesLayers;
}

GeographicViewGraphicsView::GeographicViewGraphicsView(GeographicView *geoView,
                                                       QGraphicsScene *graphicsScene,
                                                       QWidget *parent)
    : QGraphicsView(graphicsScene, parent), _geoView(geoView), graph(nullptr),
      globeCameraBackup(nullptr, true), mapCameraBackup(nullptr, true), geoLayout(nullptr),
      geoViewSize(nullptr), geoViewShape(nullptr), geoLayoutBackup(nullptr), geocodingActive(false),
      cancelGeocoding(false), polygonEntity(nullptr), planisphereEntity(nullptr),
      noLayoutMsgBox(nullptr), firstGlobeSwitch(true), geoLayoutComputed(false),
      latitudeProperty(nullptr), longitudeProperty(nullptr), qgvMap(nullptr),
      currentMapLayer(nullptr), mapAttributionWidget(nullptr), tilesLayers(initTilesLayers()) {
  setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing |
                 QPainter::TextAntialiasing);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setFrameStyle(QFrame::NoFrame);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMouseTracking(false);
  setBackgroundBrush(Qt::white);

  qgvMap = new QGVMap();
  initTilesLayers();
  setupCachedNetworkAccessManager(this);

  progressWidget = new ProgressWidgetGraphicsProxy();
  progressWidget->hide();
  progressWidget->setZValue(2);
  addressSelectionDialog = new AddressSelectionDialog(qgvMap->geoView());
  scene()->addItem(progressWidget);
  addressSelectionProxy = scene()->addWidget(addressSelectionDialog, Qt::Dialog);
  addressSelectionProxy->hide();
  addressSelectionProxy->setZValue(3);

  _placeholderItem = new QGraphicsRectItem(0, 0, 1, 1);
  _placeholderItem->setBrush(Qt::transparent);
  _placeholderItem->setPen(QPen(Qt::transparent));
  scene()->addItem(_placeholderItem);

  qgvMap->geoView()->setParent(nullptr);
  qgvMap->setVisible(false);
  QGraphicsProxyWidget *proxyGM = scene()->addWidget(qgvMap->geoView());
  proxyGM->setPos(0, 0);
  proxyGM->setParentItem(_placeholderItem);

  _glWidget = new CustomGlWidget(nullptr, geoView);
  delete _glWidget->scene()->getCalculator();
  _glWidget->scene()->setCalculator(new GlCPULODCalculator());
  _glWidget->scene()->setBackgroundColor(Color(0, 0, 0, 0));

  glWidgetItem = new GlWidgetGraphicsItem(_glWidget, 512, 512);
  glWidgetItem->setPos(0, 0);

  scene()->addItem(glWidgetItem);
  glWidgetItem->setParentItem(_placeholderItem);

  // combo box to choose the map type
  viewTypeComboBox = new QComboBox;
  QStringList items;
  for (auto viewType : GeographicView::getViewTypes()) {
    items.append(GeographicView::getViewNameFromType(viewType));
  }
  viewTypeComboBox->addItems(items);

  QGraphicsProxyWidget *comboBoxProxy = scene()->addWidget(viewTypeComboBox);
  comboBoxProxy->setParentItem(_placeholderItem);
  comboBoxProxy->setPos(20, 20);
  comboBoxProxy->setZValue(1);

  connect(viewTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), _geoView,
          QOverload<int>::of(&GeographicView::viewTypeChanged));

  // 2 push buttons
  // zoom +
  zoomInButton = new QPushButton(QIcon(":/talipot/view/geographic/zoom+.png"), "");
  zoomInButton->setFixedSize(29, 27);
  zoomInButton->setContentsMargins(0, 0, 0, 0);
  connect(zoomInButton, &QAbstractButton::pressed, _geoView, &GeographicView::zoomIn);
  QGraphicsProxyWidget *buttonProxy = scene()->addWidget(zoomInButton);
  buttonProxy->setParentItem(_placeholderItem);
  buttonProxy->setPos(20, 50);

  // zoom -
  zoomOutButton = new QPushButton(QIcon(":/talipot/view/geographic/zoom-.png"), "");
  zoomOutButton->setFixedSize(29, 27);
  zoomOutButton->setContentsMargins(0, 0, 0, 0);
  connect(zoomOutButton, &QAbstractButton::pressed, _geoView, &GeographicView::zoomOut);
  buttonProxy = scene()->addWidget(zoomOutButton);
  buttonProxy->setParentItem(_placeholderItem);
  buttonProxy->setPos(20, 76);

  auto *msgBox = new QMessageBox(QMessageBox::Warning, "Geolocated layout not initialized",
                                 "Warning : the geolocated layout\n"
                                 "has not been initialized yet.\n"
                                 "The graph will not be displayed until\n"
                                 "that operation has been performed.\n\n"
                                 "Open the Geolocation configuration tab\n"
                                 "to proceed.");
  msgBox->setModal(false);
  noLayoutMsgBox = scene()->addWidget(msgBox);
  noLayoutMsgBox->setParentItem(_placeholderItem);

  setAcceptDrops(false);

  connect(qgvMap, &QGVMap::scaleChanged, this, &GeographicViewGraphicsView::refreshMap);
  connect(qgvMap, &QGVMap::azimuthChanged, this, &GeographicViewGraphicsView::refreshMap);
  connect(qgvMap, &QGVMap::areaChanged, this, &GeographicViewGraphicsView::refreshMap);
  connect(qgvMap, &QGVMap::stateChanged, this, &GeographicViewGraphicsView::refreshMap);

  mapAttributionWidget = new QGVWidgetText();
  mapAttributionWidget->label()->setStyleSheet(
      "background-color: white; color: black; padding: 2px;");
  mapAttributionWidget->label()->setTextFormat(Qt::RichText);
  mapAttributionWidget->label()->setTextInteractionFlags(Qt::TextBrowserInteraction);
  mapAttributionWidget->label()->setOpenExternalLinks(true);
  mapAttributionWidget->label()->setScaledContents(true);
  mapAttributionWidget->setAnchor(QPoint(0, 1), {Qt::RightEdge, Qt::BottomEdge});
  qgvMap->addWidget(mapAttributionWidget);

  QGraphicsProxyWidget *textProxy = scene()->addWidget(mapAttributionWidget);
  textProxy->setParentItem(_placeholderItem);
  textProxy->setOpacity(0.7);

  QTimer::singleShot(100, this, [this]() {
    QGV::GeoRect target = QGV::GeoRect(63.1199, -74.292, -19.2807, 63.5284);
    qgvMap->cameraTo(QGVCameraActions(qgvMap).scaleTo(target));
  });
}

GeographicViewGraphicsView::~GeographicViewGraphicsView() {
  if (geocodingActive) {
    if (addressSelectionDialog->isVisible()) {
      addressSelectionDialog->accept();
    }

    cancelGeocoding = true;

    // disable user input
    // before allowing some display feedback
    tlp::disableQtUserInput();

    while (geocodingActive) {
      QApplication::processEvents();
    }

    // re-enable user input
    tlp::enableQtUserInput();
  }

  cleanup();
  if (currentMapLayer) {
    qgvMap->removeItem(currentMapLayer);
  }
  tilesLayers.clear();
  // delete the graphics scene and all the items it contains
  delete scene();
}

void GeographicViewGraphicsView::cleanup() {
  if (graph) {

    GlScene *scene = _glWidget->scene();
    scene->clearLayersList();

    if (geoLayout != graph->getLayoutProperty("viewLayout")) {
      delete geoLayout;
    }

    if (geoViewSize != graph->getSizeProperty("viewSize")) {
      delete geoViewSize;
    }

    if (geoViewShape != graph->getIntegerProperty("viewShape")) {
      delete geoViewShape;
    }

    // those entities have been deleted by the prior call to GlScene::clearLayersList,
    // so reset the pointers to nullptr value
    polygonEntity = nullptr;
    planisphereEntity = nullptr;
  }
}

void GeographicViewGraphicsView::setGraph(Graph *graph) {
  if (this->graph != graph) {

    GlGraphRenderingParameters rp;

    if (this->graph) {
      rp = _glWidget->renderingParameters();
    } else {
      rp.setNodesLabelStencil(1);
      rp.setLabelsAreBillboarded(true);
    }

    cleanup();
    this->graph = graph;

    GlScene *scene = _glWidget->scene();
    auto *glGraph = new GlGraph(graph);
    glGraph->setVisible(false);
    glGraph->setRenderingParameters(rp);
    GlLayer *layer = scene->createLayer("Main");

    layer->addGlEntity(glGraph, "graph");

    if (geoLayout) {
      geoLayout->removeListener(this);
    }

    geoLayout = graph->getLayoutProperty("viewLayout");
    geoViewSize = graph->getSizeProperty("viewSize");
    geoViewShape = graph->getIntegerProperty("viewShape");
    polygonEntity = nullptr;

    geoLayout->addListener(this);

    draw();
  }
}

static string removeQuotesIfAny(const string &s) {
  if (s[0] == '"' && s[s.length() - 1] == '"') {
    return s.substr(1, s.length() - 2);
  } else {
    return s;
  }
}

void GeographicViewGraphicsView::loadDefaultMap() {
  bool oldPolyVisible = false;

  if (polygonEntity != nullptr) {
    oldPolyVisible = polygonEntity->isVisible();
    delete polygonEntity;
  }

  polygonEntity = readCsvFile(":/talipot/view/geographic/MAPAGR4.txt");
  polygonEntity->setVisible(oldPolyVisible);

  GlScene *scene = _glWidget->scene();
  GlLayer *layer = scene->getLayer("Main");
  layer->addGlEntity(polygonEntity, "polygonMap");
}

void GeographicViewGraphicsView::loadCsvFile(QString fileName) {
  bool oldPolyVisible = false;

  if (polygonEntity != nullptr) {
    oldPolyVisible = polygonEntity->isVisible();
    delete polygonEntity;
  }

  polygonEntity = readCsvFile(fileName);

  if (!polygonEntity) {
    QMessageBox::critical(nullptr, "Can't read .poly file",
                          "We can't read csv file : " + fileName + "\nVerify the file.");
    return;
  }

  polygonEntity->setVisible(oldPolyVisible);

  GlScene *scene = _glWidget->scene();
  GlLayer *layer = scene->getLayer("Main");
  layer->addGlEntity(polygonEntity, "polygonMap");
}

void GeographicViewGraphicsView::loadPolyFile(QString fileName) {
  bool oldPolyVisible = false;

  if (polygonEntity != nullptr) {
    oldPolyVisible = polygonEntity->isVisible();
    delete polygonEntity;
  }

  polygonEntity = readPolyFile(fileName);

  if (!polygonEntity) {
    QMessageBox::critical(nullptr, "Can't read .poly file",
                          "We can't read .poly file : " + fileName + "\nVerify the file.");
    return;
  }

  polygonEntity->setVisible(oldPolyVisible);

  GlScene *scene = _glWidget->scene();
  GlLayer *layer = scene->getLayer("Main");
  layer->addGlEntity(polygonEntity, "polygonMap");
}

void GeographicViewGraphicsView::mapToPolygon() {
  GlComposite *composite = polygonEntity;

  if (!composite) {
    return;
  }

  const map<string, GlEntity *> entities = composite->getGlEntities();

  for (auto n : graph->nodes()) {

    Coord nodePos = geoLayout->getNodeValue(n);

    for (const auto &entity : entities) {
      if (entity.second->getBoundingBox().contains(nodePos)) {
        auto *polygon = static_cast<GlComplexPolygon *>(entity.second);

        const auto &polygonSides = polygon->getPolygonSides();

        for (const auto &polygonSide : polygonSides) {
          bool oddNodes = false;
          Coord lastCoord = polygonSide[0];

          for (auto it = (++polygonSide.begin()); it != polygonSide.end(); ++it) {
            if ((((*it)[1] < nodePos[1] && lastCoord[1] >= nodePos[1]) ||
                 (lastCoord[1] < nodePos[1] && (*it)[1] >= nodePos[1])) &&
                ((*it)[0] <= nodePos[0] || lastCoord[0] <= nodePos[0])) {
              oddNodes ^= ((*it)[0] + (nodePos[1] - (*it)[1]) / (lastCoord[1] - (*it)[1]) *
                                          (lastCoord[0] - (*it)[0]) <
                           nodePos[0]);
            }

            lastCoord = (*it);
          }

          if (oddNodes) {

            BoundingBox bb;

            for (const auto &c : polygonSides[0]) {
              bb.expand(c);
            }

            geoLayout->setNodeValue(n, bb.center());
            polygon->setFillColor(graph->getColorProperty("viewColor")->getNodeValue(n));
            polygon->setOutlineColor(graph->getColorProperty("viewBorderColor")->getNodeValue(n));
            break;
          }
        }
      }
    }
  }
}

static const double zoomExponentDown = std::pow(2, 1.0 / 5.0);
static const double zoomExponentUp = 1.0 / std::pow(2, 1.0 / 5.0);

void GeographicViewGraphicsView::zoomIn() {
  qgvMap->cameraTo(QGVCameraActions(qgvMap).scaleBy(zoomExponentDown));
}

void GeographicViewGraphicsView::zoomOut() {
  qgvMap->cameraTo(QGVCameraActions(qgvMap).scaleBy(zoomExponentUp));
}

GlGraph *GeographicViewGraphicsView::glGraph() const {
  return _glWidget->scene()->glGraph();
}

void GeographicViewGraphicsView::createLayoutWithAddresses(const string &addressPropertyName,
                                                           bool createLatAndLngProps,
                                                           bool resetLatAndLngValues) {
  geocodingActive = true;
  nodeLatLng.clear();
  addressSelectionDialog->setPickFirstResult(false);
  Observable::holdObservers();

  if (graph->existProperty(addressPropertyName)) {
    StringProperty *addressProperty = graph->getStringProperty(addressPropertyName);

    if (createLatAndLngProps) {
      latitudeProperty = graph->getDoubleProperty("latitude");
      longitudeProperty = graph->getDoubleProperty("longitude");
    }

    int nbNodes = graph->numberOfNodes();
    int nbNodesProcessed = 0;
    progressWidget->setFrameColor(Qt::green);
    progressWidget->setProgress(nbNodesProcessed, nbNodes);
    progressWidget->setPos(width() / 2 - progressWidget->sceneBoundingRect().width() / 2,
                           height() / 2 - progressWidget->sceneBoundingRect().height() / 2);
    progressWidget->show();

    pair<double, double> latLng;
    flat_hash_map<string, pair<double, double>> addressesLatLngMap;

    NominatimGeocoder nominatimGeocoder;

    Iterator<node> *nodesIt = graph->getNodes();
    node n;

    while (nodesIt->hasNext() && !progressWidget->cancelRequested() && !cancelGeocoding) {

      n = nodesIt->next();
      progressWidget->setProgress(++nbNodesProcessed, nbNodes);

      string addr = removeQuotesIfAny(addressProperty->getNodeValue(n));

      if (addr.empty()) {
        continue;
      }

      progressWidget->setComment("Retrieving latitude and longitude for address : \n" +
                                 tlpStringToQString(addr));

      if (!nodeLatLng.contains(n)) {

        if (addressesLatLngMap.contains(addr)) {
          nodeLatLng[n] = addressesLatLngMap[addr];

          if (createLatAndLngProps) {
            latitudeProperty->setNodeValue(n, nodeLatLng[n].first);
            longitudeProperty->setNodeValue(n, nodeLatLng[n].second);
          }
        } else {
          if (!resetLatAndLngValues) {
            // check if latitude/longitude are already set
            latLng.first = latitudeProperty->getNodeValue(n);
            latLng.second = longitudeProperty->getNodeValue(n);
            if (latLng.first != 0 || latLng.second != 0) {
              nodeLatLng[n] = addressesLatLngMap[addr] = latLng;
              continue;
            }
          }

          uint idx = 0;
          vector<NominatimGeocoderResult> geocodingResults =
              nominatimGeocoder.getLatLngForAddress(addr);

          if (geocodingResults.size() > 1) {
            bool showProgressWidget = false;

            if (progressWidget->isVisible()) {
              progressWidget->hide();
              showProgressWidget = true;
            }

            if (!addressSelectionDialog->pickFirstResult()) {
              addressSelectionDialog->clearList();
              addressSelectionDialog->setBaseAddress(tlpStringToQString(addr));

              for (auto &geocodingResult : geocodingResults) {
                addressSelectionDialog->addResultToList(
                    tlpStringToQString(geocodingResult.address));
              }

              addressSelectionProxy->setPos(
                  width() / 2 - addressSelectionProxy->sceneBoundingRect().width() / 2,
                  height() / 2 - addressSelectionProxy->sceneBoundingRect().height() / 2);

              addressSelectionDialog->show();
              addressSelectionDialog->exec();
              idx = addressSelectionDialog->getPickedResultIdx();
              addressSelectionDialog->hide();

            } else {
              idx = 0;
            }

            if (showProgressWidget) {
              progressWidget->show();
            }
          } else if (geocodingResults.empty()) {
            progressWidget->hide();
            QMessageBox::warning(nullptr, "Geolocation failed",
                                 "No results were found for address : \n" +
                                     tlpStringToQString(addr));
            progressWidget->show();
          }

          if (geocodingResults.size() > 0) {
            const pair<double, double> &latLng = geocodingResults[idx].latLng;
            nodeLatLng[n] = latLng;
            addressesLatLngMap[addr] = latLng;

            if (createLatAndLngProps) {
              latitudeProperty->setNodeValue(n, latLng.first);
              longitudeProperty->setNodeValue(n, latLng.second);
            }
          }
        }

        QApplication::processEvents();
      }
    }

    delete nodesIt;
    progressWidget->hide();
  }

  Observable::unholdObservers();
  geocodingActive = false;
}

void GeographicViewGraphicsView::createLayoutWithLatLngs(const std::string &latitudePropertyName,
                                                         const std::string &longitudePropertyName,
                                                         const string &edgesPathsPropertyName) {
  nodeLatLng.clear();
  pair<double, double> latLng;

  if (graph->existProperty(latitudePropertyName) && graph->existProperty(longitudePropertyName)) {
    latitudeProperty = graph->getDoubleProperty(latitudePropertyName);
    longitudeProperty = graph->getDoubleProperty(longitudePropertyName);
    for (auto n : graph->nodes()) {
      latLng.first = latitudeProperty->getNodeValue(n);
      latLng.second = longitudeProperty->getNodeValue(n);
      nodeLatLng[n] = latLng;
    }
  }

  if (graph->existProperty(edgesPathsPropertyName)) {
    DoubleVectorProperty *edgesPathsProperty =
        graph->getDoubleVectorProperty(edgesPathsPropertyName);
    for (auto e : graph->edges()) {
      const std::vector<double> &edgePath = edgesPathsProperty->getEdgeValue(e);
      std::vector<std::pair<double, double>> latLngs;

      for (size_t i = 0; i < edgePath.size(); i += 2) {
        latLngs.push_back(make_pair(edgePath[i], edgePath[i + 1]));
      }

      edgeBendsLatLng[e] = latLngs;
    }
  }
}

void GeographicViewGraphicsView::resizeEvent(QResizeEvent *event) {
  QGraphicsView::resizeEvent(event);
  scene()->setSceneRect(QRect(QPoint(0, 0), size()));
  qgvMap->geoView()->resize(width(), height());
  glWidgetItem->resize(width(), height());
  if (progressWidget->isVisible()) {
    progressWidget->setPos(width() / 2 - progressWidget->sceneBoundingRect().width() / 2,
                           height() / 2 - progressWidget->sceneBoundingRect().height() / 2);
  }

  if (noLayoutMsgBox && noLayoutMsgBox->isVisible()) {
    noLayoutMsgBox->setPos(width() / 2 - noLayoutMsgBox->sceneBoundingRect().width() / 2,
                           height() / 2 - noLayoutMsgBox->sceneBoundingRect().height() / 2);
  }

  if (addressSelectionProxy->isVisible()) {
    addressSelectionProxy->setPos(
        width() / 2 - addressSelectionProxy->sceneBoundingRect().width() / 2,
        height() / 2 - addressSelectionProxy->sceneBoundingRect().height() / 2);
  }
  if (mapAttributionWidget->width() > width()) {
    mapAttributionWidget->setAnchor(QPoint(0, 1), {Qt::LeftEdge, Qt::BottomEdge});
  } else {
    mapAttributionWidget->setAnchor(QPoint(0, 1), {Qt::RightEdge, Qt::BottomEdge});
  }
  if (qgvMap->geoView()->isVisible()) {
    refreshMap();
  } else if (scene()) {
    scene()->update();
  }
}

void GeographicViewGraphicsView::refreshMap() {
  if (!qgvMap->geoView()->isVisible()) {
    return;
  }

  QGVProjection *projection = qgvMap->getProjection();

  QGV::GeoRect geoRect = projection->projToGeo(qgvMap->getCamera().projRect());
  QGV::GeoPos southWest = geoRect.bottomRight();
  QGV::GeoPos northEast = geoRect.topLeft();

  if (southWest.longitude() != northEast.longitude()) {
    BoundingBox bb;
    bb.expand(Coord(northEast.longitude() * 2, latitudeToMercator(northEast.latitude())));
    bb.expand(Coord(southWest.longitude() * 2, latitudeToMercator(southWest.latitude())));
    GlSceneZoomAndPan sceneZoomAndPan(_glWidget->scene(), bb, "Main", 1);
    sceneZoomAndPan.zoomAndPanAnimationStep(1);
  }

  qgvMap->setVisible(false);
  glWidgetItem->setRedrawNeeded(true);

  scene()->update();
}

void GeographicViewGraphicsView::centerView() {
  _glWidget->centerScene();
  if (qgvMap->geoView()->isVisible()) {
    if (!nodeLatLng.empty()) {
      auto minLatLng = make_pair(90.0, 180.0);
      auto maxLatLng = make_pair(-90.0, -180.0);
      for (auto [n, latLng] : nodeLatLng) {
        if (graph->isElement(n)) {
          minLatLng.first = std::min(minLatLng.first, latLng.first);
          minLatLng.second = std::min(minLatLng.second, latLng.second);
          maxLatLng.first = std::max(maxLatLng.first, latLng.first);
          maxLatLng.second = std::max(maxLatLng.second, latLng.second);
        }
      }
      QGV::GeoRect bounds(minLatLng.first, minLatLng.second, maxLatLng.first, maxLatLng.second);
      qgvMap->flyTo(QGVCameraActions(qgvMap).scaleTo(bounds));
    }
  }
}

void GeographicViewGraphicsView::setGeoLayout(LayoutProperty *property) {
  if (geoLayout) {
    geoLayout->removeListener(this);
    *property = *geoLayout;
  }
  geoLayout = property;
  geoLayout->addListener(this);
  _glWidget->inputData()->setLayout(geoLayout);
}

void GeographicViewGraphicsView::setGeoSizes(SizeProperty *property) {
  *property = *geoViewSize;
  geoViewSize = property;
  _glWidget->inputData()->setSizes(geoViewSize);
}

void GeographicViewGraphicsView::setGeoShape(IntegerProperty *property) {
  *property = *geoViewShape;
  geoViewShape = property;
  _glWidget->inputData()->setShapes(geoViewShape);
}

void GeographicViewGraphicsView::treatEvent(const Event &ev) {
  const auto *propEvt = dynamic_cast<const PropertyEvent *>(&ev);

  if (propEvt && propEvt->getType() == PropertyEventType::TLP_AFTER_SET_NODE_VALUE &&
      propEvt->getProperty() == geoLayout) {
    // compute new node latitude / longitude from updated coordinates
    node n = propEvt->getNode();
    const Coord &p = geoLayout->getNodeValue(n);
    pair<double, double> latLng = {mercatorToLatitude(p.y()), p.x() / 2};
    nodeLatLng[n] = latLng;
    if (latitudeProperty && longitudeProperty) {
      latitudeProperty->setNodeValue(n, latLng.first);
      longitudeProperty->setNodeValue(n, latLng.second);
    }
  }
}

void GeographicViewGraphicsView::switchViewType() {
  GeographicView::ViewType viewType = _geoView->viewType();

  bool enableQGeoViewMap = false;
  bool enablePolygon = false;
  bool enablePlanisphere = false;

  switch (viewType) {

  case GeographicView::Polygon: {
    enablePolygon = true;
    glWidgetItem->setRedrawNeeded(true);
    break;
  }

  case GeographicView::Globe: {
    enablePlanisphere = true;
    break;
  }

  default: {
    enableQGeoViewMap = true;
    if (currentMapLayer) {
      qgvMap->removeItem(currentMapLayer);
    }
    currentMapLayer = tilesLayers[viewType].get();
    if (viewType == GeographicView::CustomTilesLayer) {
      static_cast<QGVCustomTilesLayer *>(currentMapLayer)
          ->setTilesUrl(_geoView->getConfigWidget()->getCustomTilesLayerUrl());
      currentMapLayer->setDescription(
          _geoView->getConfigWidget()->getCustomTilesLayerAttribution());
    }

    qgvMap->addItem(currentMapLayer);
    mapAttributionWidget->setText(currentMapLayer->getDescription());
    mapAttributionWidget->adjustSize();
    if (mapAttributionWidget->width() > width()) {
      mapAttributionWidget->setAnchor(QPoint(0, 1), {Qt::LeftEdge, Qt::BottomEdge});
    } else {
      mapAttributionWidget->setAnchor(QPoint(0, 1), {Qt::RightEdge, Qt::BottomEdge});
    }

    break;
  }
  }

  if (planisphereEntity && planisphereEntity->isVisible()) {
    globeCameraBackup = _glWidget->scene()->graphCamera();
  } else {
    mapCameraBackup = _glWidget->scene()->graphCamera();
  }

  if (geoLayout && geoLayoutBackup != nullptr && geoLayoutComputed) {
    *geoLayout = *geoLayoutBackup;
    delete geoLayoutBackup;
    geoLayoutBackup = nullptr;
  }

  GlLayer *layer = _glWidget->scene()->getLayer("Main");

  if (geoLayout == graph->getLayoutProperty("viewLayout") && geoLayoutComputed) {
    graph->push();
  }

  Observable::holdObservers();

  qgvMap->geoView()->setVisible(enableQGeoViewMap);

  if (polygonEntity) {
    polygonEntity->setVisible(enablePolygon);
  }

  layer->setCamera(new Camera(_glWidget->scene()));

  if (viewType != GeographicView::Globe && geoLayoutComputed) {
    geoLayout->removeListener(this);

    SizeProperty *viewSize = graph->getSizeProperty("viewSize");

    for (auto n : graph->nodes()) {
      if (viewSize != geoViewSize) {
        const Size &nodeSize = viewSize->getNodeValue(n);
        geoViewSize->setNodeValue(n, nodeSize);
      }

      if (nodeLatLng.contains(n)) {
        const auto &[lat, lng] = nodeLatLng[n];
        geoLayout->setNodeValue(n, Coord(lng * 2., latitudeToMercator(lat), 0));
      }
    }

    if (!edgeBendsLatLng.empty()) {
      for (auto e : graph->edges()) {
        vector<Coord> edgeBendsCoords;

        for (const auto &[lat, lng] : edgeBendsLatLng[e]) {
          edgeBendsCoords.push_back(Coord(lng * 2., latitudeToMercator(lat), 0));
        }

        geoLayout->setEdgeValue(e, edgeBendsCoords);
      }
    }

    geoLayout->addListener(this);

    Camera &camera = _glWidget->scene()->graphCamera();
    camera.setEyes(mapCameraBackup.getEyes());
    camera.setCenter(mapCameraBackup.getCenter());
    camera.setUp(mapCameraBackup.getUp());
    camera.setZoomFactor(mapCameraBackup.getZoomFactor());
    camera.setSceneRadius(mapCameraBackup.getSceneRadius());
  }

  else {

    if (!planisphereEntity) {
      GlOffscreenRenderer::instance().makeOpenGLContextCurrent();
      GlTextureManager::loadTexture(planisphereTextureId);
      planisphereEntity = new GlSphere(Coord(0., 0., 0.), 50., planisphereTextureId, 255, 0, 0, 90);
      _glWidget->scene()->getLayer("Main")->addGlEntity(planisphereEntity, "globeMap");
    }

    if (geoLayoutComputed) {

      geoLayout->removeListener(this);

      SizeProperty *viewSize = graph->getSizeProperty("viewSize");

      assert(geoLayoutBackup == nullptr);
      geoLayoutBackup = new LayoutProperty(graph);
      *geoLayoutBackup = *geoLayout;

      geoViewShape->setAllNodeValue(NodeShape::Sphere);
      geoViewShape->setAllEdgeValue(EdgeShape::CubicBSplineCurve);

      for (auto n : graph->nodes()) {
        if (viewSize != geoViewSize) {
          const Size &nodeSize = viewSize->getNodeValue(n);
          geoViewSize->setNodeValue(n, nodeSize);
        }

        if (nodeLatLng.contains(n)) {
          geoLayout->setNodeValue(n, projectLatLngToSphere(nodeLatLng[n], 50));
        }
      }

      for (auto e : graph->edges()) {
        const auto &[src, tgt] = graph->ends(e);
        uint bendsNumber = 2;
        vector<Coord> bends;

        Coord srcCoord = latLngToPolar(nodeLatLng[src]);
        Coord tgtCoord = latLngToPolar(nodeLatLng[tgt]);

        for (uint i = 0; i < bendsNumber; ++i) {
          Coord tmp = srcCoord + ((tgtCoord - srcCoord) / (bendsNumber + 1.f)) * (i + 1.f);
          bends.push_back(polarToSpherical(tmp, 75));
        }

        geoLayout->setEdgeValue(e, bends);
      }
      geoLayout->addListener(this);
    }

    if (firstGlobeSwitch) {
      firstGlobeSwitch = false;

      _glWidget->scene()->centerScene();
      Camera &camera = _glWidget->scene()->graphCamera();
      float centerEyeDistance = (camera.getEyes() - camera.getCenter()).norm();
      camera.setCenter(Coord(0, 0, 0));
      camera.setEyes(Coord(centerEyeDistance, 0, 0));
      camera.setUp(Coord(0, 0, 1));

      globeCameraBackup = camera;

    } else {
      Camera &camera = _glWidget->scene()->graphCamera();
      camera.setEyes(globeCameraBackup.getEyes());
      camera.setCenter(globeCameraBackup.getCenter());
      camera.setUp(globeCameraBackup.getUp());
      camera.setZoomFactor(globeCameraBackup.getZoomFactor());
      camera.setSceneRadius(globeCameraBackup.getSceneRadius());
    }
  }

  if (planisphereEntity) {
    planisphereEntity->setVisible(enablePlanisphere);
  }

  _glWidget->renderingParameters().setEdge3D(viewType == GeographicView::Globe);

  Observable::unholdObservers();

  graph->popIfNoUpdates();

  draw();
}

void GeographicViewGraphicsView::setGeoLayoutComputed() {
  geoLayoutComputed = true;
  noLayoutMsgBox->setVisible(false);
  _glWidget->scene()->glGraph()->setVisible(true);
}

}
