/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef LEAFLET_MAPS_H
#define LEAFLET_MAPS_H

#ifdef QT_HAS_WEBKIT
#include <QWebView>
#include <QWebFrame>
#else
#include <QWebEngineView>
#include <QEventLoop>
#endif
#include <talipot/Coord.h>
#include <talipot/Graph.h>

#include "ProgressWidgetGraphicsProxy.h"
#include "AddressSelectionDialog.h"

namespace tlp {

#ifdef QT_HAS_WEBENGINE

class MapRefresher : public QObject {

  Q_OBJECT

public slots:

  void refreshMap();

signals:

  void refreshMapSignal();
};

#endif

#ifdef QT_HAS_WEBKIT
class LeafletMaps : public QWebView {
#else
class LeafletMaps : public QWebEngineView {
#endif

  Q_OBJECT

public:
  LeafletMaps(QWidget *parent = nullptr);

  ~LeafletMaps();

  bool pageLoaded();

  bool mapLoaded();

  void setMapCenter(double latitude, double longitude);

  int getCurrentMapZoom();

  void setCurrentZoom(int zoom);

  std::pair<double, double> getCurrentMapCenter();

  Coord getPixelPosOnScreenForLatLng(double lat, double lng);

  Coord mercatorProjection(const Coord &swPixel, const Coord &nePixel, const double latitude,
                           const double longitude);

  std::pair<double, double> getLatLngForPixelPosOnScreen(int x, int y);

  bool pageInit() const {
    return init;
  }

  void setMapBounds(Graph *graph,
                    const std::unordered_map<node, std::pair<double, double>> &nodesLatLngs);

  void setMapBounds(Coord nw, Coord se);

  void panMap(int dx, int dy);

  std::pair<double, double> getMapCurrentSouthWestLatLng();

  std::pair<double, double> getMapCurrentNorthEastLatLng();

  int getWorldWidth();

  void switchToOpenStreetMap();

  void switchToEsriSatellite();

  void switchToEsriTerrain();

  void switchToEsriGrayCanvas();

  void switchToCustomTileLayer(const QString &customTileLayerUrl);

  void setProgressWidget(ProgressWidgetGraphicsProxy *progressWidget) {
    this->progressWidget = progressWidget;
  }

  void setAdresseSelectionDialog(AddressSelectionDialog *addressSelectionDialog,
                                 QGraphicsProxyWidget *addresseSelectionProxy) {
    this->addressSelectionDialog = addressSelectionDialog;
    this->addressSelectionProxy = addresseSelectionProxy;
  }

signals:

  void currentZoomChanged();
  void refreshMap();

private slots:

  void triggerLoading();

private:
  QVariant executeJavascript(const QString &jsCode);

  bool init;
#ifdef QT_HAS_WEBKIT
  QWebFrame *frame;
#else
  QWebEnginePage *frame;
#endif
  int x, y;

  AddressSelectionDialog *addressSelectionDialog;
  QGraphicsProxyWidget *addressSelectionProxy;
  ProgressWidgetGraphicsProxy *progressWidget;

#ifdef QT_HAS_WEBENGINE
  MapRefresher *mapRefresher;
#endif
};

#ifdef QT_HAS_WEBENGINE

// Use this class as a hack to simulate synchronous javascript callbacks
// as QtWebEngine is asynchronous while QtWebKit was not
class JsCallback : public QObject {

  Q_OBJECT

public:
  JsCallback(QVariant *ret) : _ret(ret) {
    _lastCreatedInstance = this;
  }

  JsCallback(const JsCallback &jscb) : QObject() {
    _ret = jscb._ret;
    _lastCreatedInstance = this;
  }

  void operator()(const QVariant &result) {
    *_ret = result;
    emit called();
  }

  static void waitForCallback() {
    QEventLoop loop;
    connect(_lastCreatedInstance, SIGNAL(called()), &loop, SLOT(quit()));
    loop.exec();
  }

signals:

  void called();

private:
  QVariant *_ret;

  static JsCallback *_lastCreatedInstance;
};

#endif
} // namespace tlp

#endif // LEAFLET_MAPS_H
