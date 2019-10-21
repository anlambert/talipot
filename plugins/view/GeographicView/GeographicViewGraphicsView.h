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

#ifndef GEOGRAPHIC_VIEW_GRAPHICS_VIEW_H
#define GEOGRAPHIC_VIEW_GRAPHICS_VIEW_H

#include <unordered_map>

#include "LeafletMaps.h"

#include <talipot/GlGraphComposite.h>
#include <talipot/GlMainWidget.h>
#include <talipot/GlMainWidgetGraphicsItem.h>
#include <talipot/Camera.h>

#include <QGraphicsView>
#include <QComboBox>

class QOpenGLFramebufferObject;

namespace tlp {

class GeographicView;

class GeographicViewGraphicsView : public QGraphicsView, public Observable {

  Q_OBJECT

public:
  GeographicViewGraphicsView(GeographicView *_geoView, QGraphicsScene *graphicsScene,
                             QWidget *parent = nullptr);
  ~GeographicViewGraphicsView() override;

  void setGraph(Graph *graph);
  void createLayoutWithAddresses(const std::string &addressPropertyName, bool createLatAndLngProps,
                                 bool resetLatAndLngValues);
  void createLayoutWithLatLngs(const std::string &latitudePropertyName,
                               const std::string &longitudePropertyName,
                               const std::string &edgesPathsPropertyName);

  GlGraphComposite *getGlGraphComposite() const;

  std::pair<double, double> getLatLngForNode(node n) {
    return nodeLatLng[n];
  }

  void setNodeLatLng(node n, const std::pair<double, double> &latLng) {
    nodeLatLng[n] = latLng;
  }

  void setEdgeBendsLatLng(edge e, const std::vector<std::pair<double, double>> &bendsLatLng) {
    edgeBendsLatLng[e] = bendsLatLng;
  }

  void draw() {
    glWidgetItem->setRedrawNeeded(true);
    scene()->update();
  }

  void setMapTranslationBlocked(const bool mapTranslationBlocked);

  void centerView();

  void centerMapOnNode(const node n);

  GlMainWidget *getGlMainWidget() {
    return glMainWidget;
  }

  LeafletMaps *getLeafletMapsPage() const {
    return leafletMaps;
  }

  LayoutProperty *getGeoLayout() const {
    return geoLayout;
  }

  SizeProperty *getGeoSizes() const {
    return geoViewSize;
  }

  void setGeoLayout(LayoutProperty *);

  void setGeoSizes(SizeProperty *);

  void setGeoShape(IntegerProperty *);

  void treatEvent(const Event &ev) override;

  void afterSetNodeValue(PropertyInterface *, const node);

  void afterSetAllNodeValue(PropertyInterface *);

  void forceRedraw() {
    currentMapZoom = 0;
  }

  GlMainWidgetGraphicsItem *getGlMainWidgetItem() {
    return glWidgetItem;
  }

  QGraphicsRectItem *getPlaceHolderItem() const {
    return _placeholderItem;
  }

  void switchViewType();

  void loadDefaultMap();
  void loadCsvFile(QString fileName);
  void loadPolyFile(QString fileName);

  QComboBox *getViewTypeComboBox() {
    return viewTypeComboBox;
  }

  GlComposite *getPolygon() {
    return polygonEntity;
  }

  void setGeoLayoutComputed();

public slots:

  void mapToPolygon();
  void zoomIn();
  void zoomOut();
  void currentZoomChanged();
#ifdef QT_HAS_WEBENGINE
  void queueMapRefresh();
#endif
  void refreshMap();

protected:
  void cleanup();
  void resizeEvent(QResizeEvent *event) override;
#ifdef QT_HAS_WEBENGINE
  int tId;
  void timerEvent(QTimerEvent *event) override;
#endif
  void paintEvent(QPaintEvent *event) override;
  void updateMapTexture();

private:
  GeographicView *_geoView;
  Graph *graph;
  LeafletMaps *leafletMaps;
  std::unordered_map<node, std::pair<double, double>> nodeLatLng;
  std::unordered_map<node, std::pair<double, double>> nodeLatLngFOR;
  std::unordered_map<edge, std::vector<std::pair<double, double>>> edgeBendsLatLng;
  std::pair<double, double> currentMapCenter;
  int currentMapZoom;
  Camera globeCameraBackup;
  Camera mapCameraBackup;

  LayoutProperty *geoLayout;
  SizeProperty *geoViewSize;
  IntegerProperty *geoViewShape;
  LayoutProperty *geoLayoutBackup;

  bool mapTranslationBlocked;

  bool geocodingActive;
  bool cancelGeocoding;

  GlMainWidget *glMainWidget;
  GlMainWidgetGraphicsItem *glWidgetItem;
  QComboBox *viewTypeComboBox;
  QPushButton *zoomOutButton;
  QPushButton *zoomInButton;

  GlComposite *polygonEntity;
  GlSimpleEntity *planisphereEntity;

  AddressSelectionDialog *addressSelectionDialog;
  QGraphicsProxyWidget *addressSelectionProxy;
  ProgressWidgetGraphicsProxy *progressWidget;
  QGraphicsProxyWidget *noLayoutMsgBox;

  bool firstGlobeSwitch;

  QGraphicsRectItem *_placeholderItem;

  bool geoLayoutComputed;

  QOpenGLFramebufferObject *renderFbo;
  GlLayer *backgroundLayer;
};
}

#endif // GEOGRAPHIC_VIEW_GRAPHICS_VIEW_H
