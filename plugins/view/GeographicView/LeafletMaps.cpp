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

#include <talipot/TlpQtTools.h>

#include "GeographicView.h"
#include "LeafletMaps.h"

#include <QDesktopServices>
#include <QTimer>

#ifdef QT_HAS_WEBENGINE
#include <QWebChannel>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>
#endif

using namespace std;

namespace tlp {

static QString htmlMap() {
  return QString(R"(
<html>
<head>
)") +
#ifdef QT_HAS_WEBENGINE
         R"(<script type="text/javascript" src="qrc:///qtwebchannel/qwebchannel.js"></script>)" +
#endif
         QString(R"(
<link rel="stylesheet" href="qrc:///talipot/view/geographic/leaflet/leaflet.css" />
<script src="qrc:///talipot/view/geographic/leaflet/leaflet.js"></script>
<script type="text/javascript">
var map;
var mapBounds;
var layers = {};
var currentLayer;
var esriBaseUrl = 'https://server.arcgisonline.com/ArcGIS/rest/services/';
function refreshMap() {
  leafletMapsQObject.refreshMap();
}
function refreshMapWithDelay() {
  setTimeout(function() {
    leafletMapsQObject.refreshMap();
  }, 500);
}
function addEventHandlersToLayer(layer) {
  layer.on('tileload', refreshMapWithDelay);
  layer.on('load', refreshMapWithDelay);
}
function init(lat, lng, zoom) {
  map = L.map('map_canvas', {
    zoomControl: false
  });
  var osm = L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">' +
                 'OpenStreetMap</a> contributors'
  });
  addEventHandlersToLayer(osm);
  osm.addTo(map);
  layers['%1'] = osm;
  var esriSatellite = L.tileLayer(esriBaseUrl + 'World_Imagery/MapServer/tile/{z}/{y}/{x}', {
    attribution: 'Tiles &copy; Esri &mdash; Source: Esri, i-cubed, USDA, USGS, AEX, GeoEye, ' +
                  'Getmapping, Aerogrid, IGN, IGP, UPR-EGP, and the GIS User Community'
  });
  addEventHandlersToLayer(esriSatellite);
  layers['%2'] = esriSatellite;
  var esriTerrain = L.tileLayer(esriBaseUrl + 'World_Topo_Map/MapServer/tile/{z}/{y}/{x}', {
    attribution: 'Tiles &copy; Esri &mdash; Esri, DeLorme, NAVTEQ, TomTom, Intermap, iPC, ' +
                  'USGS, FAO, NPS, NRCAN, GeoBase, Kadaster NL, Ordnance Survey, Esri Japan, ' +
                  'METI, Esri China (Hong Kong), and the GIS User Community'
  });
  addEventHandlersToLayer(esriTerrain);
  layers['%3'] = esriTerrain;
  var esriGrayCanvas = L.tileLayer(esriBaseUrl + 'Canvas/World_Light_Gray_Base/MapServer/tile/{z}/{y}/{x}', {
    attribution: 'Tiles &copy; Esri &mdash; Esri, DeLorme, NAVTEQ',
    maxZoom: 16
  });
  addEventHandlersToLayer(esriGrayCanvas);
  layers['%4'] = esriGrayCanvas;
  var geoportailFrancePlan = L.tileLayer('https://wxs.ign.fr/{apikey}/geoportail/wmts?REQUEST=GetTile&SERVICE=WMTS' +
    '&VERSION=1.0.0&STYLE={style}&TILEMATRIXSET=PM&FORMAT={format}&LAYER=GEOGRAPHICALGRIDSYSTEMS.PLANIGNV2&' +
    'TILEMATRIX={z}&TILEROW={y}&TILECOL={x}', {
      attribution: '<a target="_blank" href="https://www.geoportail.gouv.fr/">Géoportail France</a>',
      bounds: [[-75, -180], [81, 180]],
      minZoom: 2,
      maxZoom: 18,
      apikey: 'choisirgeoportail',
      format: 'image/png',
      style: 'normal'
  });
  addEventHandlersToLayer(geoportailFrancePlan);
  layers['%5'] = geoportailFrancePlan;
  var geoportailFranceSatellite = L.tileLayer('https://wxs.ign.fr/{apikey}/geoportail/wmts?REQUEST=GetTile&SERVICE=WMTS' +
    '&VERSION=1.0.0&STYLE={style}&TILEMATRIXSET=PM&FORMAT={format}&LAYER=ORTHOIMAGERY.ORTHOPHOTOS' +
    '&TILEMATRIX={z}&TILEROW={y}&TILECOL={x}', {
      attribution: '<a target="_blank" href="https://www.geoportail.gouv.fr/">Géoportail France</a>',
      bounds: [[-75, -180], [81, 180]],
      minZoom: 2,
      maxZoom: 19,
      apikey: 'choisirgeoportail',
      format: 'image/jpeg',
      style: 'normal'
  });
  addEventHandlersToLayer(geoportailFranceSatellite);
  layers['%6'] = geoportailFranceSatellite;
  var geoportailFranceIgn = L.tileLayer('https://wxs.ign.fr/{apikey}/geoportail/wmts?REQUEST=GetTile&SERVICE=WMTS' +
    '&VERSION=1.0.0&STYLE={style}&TILEMATRIXSET=PM&FORMAT={format}&LAYER=GEOGRAPHICALGRIDSYSTEMS.MAPS' +
    '&TILEMATRIX={z}&TILEROW={y}&TILECOL={x}', {
      attribution: '<a target="_blank" href="https://www.geoportail.gouv.fr/">Géoportail France</a>',
      bounds: [[-75, -180], [81, 180]],
      minZoom: 2,
      maxZoom: 19,
      apikey: 'choisirgeoportail',
      format: 'image/jpeg',
      style: 'normal'
  });
  addEventHandlersToLayer(geoportailFranceIgn);
  layers['%7'] = geoportailFranceIgn;
  currentLayer = osm;
  map.setView(L.latLng(lat, lng), zoom);
  map.on('zoomstart', refreshMap);
  map.on('zoom', refreshMap);
  map.on('zoomend', refreshMap);
  map.on('movestart', refreshMap);
  map.on('move', refreshMap);
  map.on('moveend', refreshMap);
}
function setMapBounds(latLngArray) {
  var latLngBounds = L.latLngBounds();
  for (var i = 0 ; i < latLngArray.length ; ++i) {
    latLngBounds.extend(latLngArray[i]);
  }
  map.flyToBounds(latLngBounds);
}
function switchToTileLayerName(layerName) {
  switchToTileLayer(layers[layerName]);
}
function switchToTileLayer(layer) {
  map.removeLayer(currentLayer);
  map.addLayer(layer);
  currentLayer = layer;
  refreshMap();
}
function switchToCustomTileLayer(customTileLayerUrl) {
  var customTileLayer = L.tileLayer(customTileLayerUrl, {
      attribution: customTileLayerUrl,
      errorTileUrl: 'qrc:///talipot/view/geographic/leaflet/no-tile.png'
  });
  addEventHandlersToLayer(customTileLayer);
  switchToTileLayer(customTileLayer);
}
)")
             .arg(GeographicView::getViewNameFromType(GeographicView::OpenStreetMap),
                  GeographicView::getViewNameFromType(GeographicView::EsriSatellite),
                  GeographicView::getViewNameFromType(GeographicView::EsriTerrain),
                  GeographicView::getViewNameFromType(GeographicView::EsriGrayCanvas),
                  GeographicView::getViewNameFromType(GeographicView::GeoportailPlan),
                  GeographicView::getViewNameFromType(GeographicView::GeoportailSatellite),
                  GeographicView::getViewNameFromType(GeographicView::GeoportailIgn)) +
#ifdef QT_HAS_WEBENGINE
         R"(
document.addEventListener("DOMContentLoaded", function () {
  new QWebChannel(qt.webChannelTransport, function (channel) {
    leafletMapsQObject = channel.objects.leafletMapsQObject;
    refreshMap();
  });
});
)" +
#endif
         R"(
</script>
</head>
<body style="margin:0px; padding:0px;" >
<div id="map_canvas" style="width:100%; height:100%"></div>
</body>
</html>
)";
}

#ifdef QT_HAS_WEBENGINE
// https://stackoverflow.com/questions/66925445/qt-webengine-not-loading-openstreetmap-tiles
class OpenStreetMapSetAcceptLanguageHeader : public QWebEngineUrlRequestInterceptor {
public:
  OpenStreetMapSetAcceptLanguageHeader(LeafletMaps *leafletMaps) : _leafletMaps(leafletMaps) {}

  void interceptRequest(QWebEngineUrlRequestInfo &info) override {
    if (_leafletMaps->getCurrentLayerName() ==
        GeographicView::getViewNameFromType(GeographicView::OpenStreetMap)) {
      info.setHttpHeader("Accept-Language", "en-US,en;q=0.9,fr;q=0.8,de;q=0.7");
    }
  }

private:
  LeafletMaps *_leafletMaps;
};
#endif

class WebPage :
#ifdef QT_HAS_WEBENGINE
    public QWebEnginePage {
protected:
  bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type,
                               bool isMainFrame) override {
    if (type == QWebEnginePage::NavigationTypeLinkClicked) {
      QDesktopServices::openUrl(url);
      return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
  }

  void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel,
                                const QString &message, int, const QString &) override {
    tlp::warning() << "[JavaScript output] " << QStringToTlpString(message) << std::endl;
  }
#else
    public QWebPage {
protected:
  bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request,
                               NavigationType type) override {
    if (type == QWebPage::NavigationTypeLinkClicked) {
      QDesktopServices::openUrl(request.url());
      return false;
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
  }

  void javaScriptConsoleMessage(const QString &message, int, const QString &) override {
    tlp::warning() << "[JavaScript output] " << QStringToTlpString(message) << std::endl;
  }
#endif
};

#ifdef QT_HAS_WEBENGINE
void MapRefresher::refreshMap() {
  emit refreshMapSignal();
}

JsCallback *JsCallback::_lastCreatedInstance = nullptr;
#endif

#ifdef QT_HAS_WEBKIT
LeafletMaps::LeafletMaps(QWidget *parent) : QWebView(parent), init(false) {
#else
LeafletMaps::LeafletMaps(QWidget *parent) : QWebEngineView(parent), init(false) {
#endif
  setPage(new WebPage);
#ifdef QT_HAS_WEBKIT
  frame = page()->mainFrame();
  frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
  frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
#else
  frame = page();
  osmSetAccessLanguageHeader = new OpenStreetMapSetAcceptLanguageHeader(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
  frame->profile()->setUrlRequestInterceptor(osmSetAccessLanguageHeader);
#else
  frame->profile()->setRequestInterceptor(osmSetAccessLanguageHeader);
#endif
  mapRefresher = new MapRefresher;
  connect(mapRefresher, &MapRefresher::refreshMapSignal, this, &LeafletMaps::refreshMap);
  QWebChannel *channel = new QWebChannel(frame);
  frame->setWebChannel(channel);
  channel->registerObject(QStringLiteral("leafletMapsQObject"), mapRefresher);
#endif
  frame->setHtml(htmlMap());
  QTimer::singleShot(500, this, &LeafletMaps::triggerLoading);
}

LeafletMaps::~LeafletMaps() {
#ifdef QT_HAS_WEBENGINE
  delete mapRefresher;
  delete osmSetAccessLanguageHeader;
#endif
}

QVariant LeafletMaps::executeJavascript(const QString &jsCode) {
#ifdef QT_HAS_WEBKIT
  return frame->evaluateJavaScript(jsCode);
#else
  QVariant ret;
  JsCallback jscb(&ret);
  frame->runJavaScript(jsCode, jscb);
  JsCallback::waitForCallback();
  return ret;
#endif
}

bool LeafletMaps::pageLoaded() {
  QString code = "typeof init !== \"undefined\"";
  QVariant ret = executeJavascript(code);
  return ret.toBool();
}

bool LeafletMaps::mapLoaded() {
  QString code = "typeof map !== \"undefined\"";
  QVariant ret = executeJavascript(code);
  return ret.toBool();
}

void LeafletMaps::triggerLoading() {
  if (!pageLoaded()) {
    QTimer::singleShot(500, this, &LeafletMaps::triggerLoading);
    return;
  }
#ifdef QT_HAS_WEBKIT
  frame->addToJavaScriptWindowObject("leafletMapsQObject", this);
#endif
  // map is first centered in the Atlantic Ocean
  // in order to emphasize the need to configure geolocation
  static const QString code = "init(44.8084, -40, 3)";
  executeJavascript(code);
  init = true;
}

void LeafletMaps::switchToTileLayer(const QString &layerName) {
  static const QString code = "switchToTileLayerName('%1')";
  currentLayerName = layerName;
  executeJavascript(code.arg(layerName));
}

void LeafletMaps::switchToCustomTileLayer(const QString &customTileLayerUrl) {
  static const QString code = "switchToCustomTileLayer('%1')";
  executeJavascript(code.arg(customTileLayerUrl));
}

void LeafletMaps::setMapCenter(double latitude, double longitude) {
  static const QString code = "map.setView(L.latLng(%1, %2), map.getZoom());";
  executeJavascript(code.arg(latitude).arg(longitude));
}

Coord LeafletMaps::getPixelPosOnScreenForLatLng(double lat, double lng) {
  static const QString code = "map.latLngToContainerPoint(L.latLng(%1, %2)).toString();";
  QVariant ret = executeJavascript(code.arg(lat).arg(lng));

  QString pointStr = ret.toString();
  int pos = pointStr.indexOf('(') + 1;
  QString xStr = pointStr.mid(pos, pointStr.lastIndexOf(',') - pos);
  pos = pointStr.lastIndexOf(',') + 1;
  QString yStr = pointStr.mid(pos, pointStr.lastIndexOf(')') - pos);

  bool ok;
  return Coord(xStr.toDouble(&ok), yStr.toDouble(&ok), 0);
}

std::pair<double, double> LeafletMaps::getLatLngForPixelPosOnScreen(int x, int y) {
  static const QString code = "map.containerPointToLatLng(L.point(%1, %2)).toString();";
  QVariant ret = executeJavascript(code.arg(x).arg(y));

  QString latLngStr = ret.toString();
  int pos = latLngStr.indexOf('(') + 1;
  QString latStr = latLngStr.mid(pos, latLngStr.lastIndexOf(',') - pos);
  pos = latLngStr.lastIndexOf(',') + 1;
  QString lngStr = latLngStr.mid(pos, latLngStr.lastIndexOf(')') - pos);
  return make_pair(latStr.toDouble(), lngStr.toDouble());
}

int LeafletMaps::getCurrentMapZoom() {
  static const QString code = "map.getZoom();";
  QVariant ret = executeJavascript(code);
  return ret.toInt();
}

static int clamp(int i, int minVal, int maxVal) {
  return min(max(i, minVal), maxVal);
}

void LeafletMaps::setCurrentZoom(int zoom) {
  static const QString code = "map.setZoom(%1);";
  executeJavascript(code.arg(clamp(zoom, 0, 20)));
  emit currentZoomChanged();
}

pair<double, double> LeafletMaps::getCurrentMapCenter() {
  static const QString code = "map.getCenter().toString();";
  QVariant ret = executeJavascript(code);

  pair<double, double> latLng;

  if (!ret.isNull()) {
    QString pointStr = ret.toString();
    int pos = pointStr.indexOf('(') + 1;
    QString xStr = pointStr.mid(pos, pointStr.lastIndexOf(',') - pos);
    pos = pointStr.lastIndexOf(',') + 1;
    QString yStr = pointStr.mid(pos, pointStr.lastIndexOf(')') - pos);
    latLng = make_pair(xStr.toDouble(), yStr.toDouble());
  }

  return latLng;
}

void LeafletMaps::setMapBounds(Graph *graph,
                               const flat_hash_map<node, pair<double, double>> &nodesLatLngs) {

  if (!nodesLatLngs.empty()) {

    auto minLatLng = make_pair(90.0, 180.0);
    auto maxLatLng = make_pair(-90.0, -180.0);

    for (const auto &it : nodesLatLngs) {
      if (graph->isElement(it.first)) {
        minLatLng.first = std::min(minLatLng.first, it.second.first);
        minLatLng.second = std::min(minLatLng.second, it.second.second);
        maxLatLng.first = std::max(maxLatLng.first, it.second.first);
        maxLatLng.second = std::max(maxLatLng.second, it.second.second);
      }
    }

    zoomOnBounds(minLatLng, maxLatLng);
  }
}

void LeafletMaps::zoomOnBounds(const std::pair<double, double> &minLatLng,
                               const std::pair<double, double> &maxLatLng) {
  auto code = QString("setMapBounds([L.latLng(%1, %2), L.latLng(%3, %4)])")
                  .arg(minLatLng.first)
                  .arg(minLatLng.second)
                  .arg(maxLatLng.first)
                  .arg(maxLatLng.second);
  executeJavascript(code);
}
}
