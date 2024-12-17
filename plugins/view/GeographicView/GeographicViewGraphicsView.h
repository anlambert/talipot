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

#ifndef GEOGRAPHIC_VIEW_GRAPHICS_VIEW_H
#define GEOGRAPHIC_VIEW_GRAPHICS_VIEW_H

#include <talipot/hash.h>

#include <talipot/GlGraph.h>
#include <talipot/GlWidget.h>
#include <talipot/GlWidgetGraphicsItem.h>
#include <talipot/Camera.h>

#include <QGraphicsView>
#include <QComboBox>
#include <QPushButton>

#include "GeographicView.h"

class QGVMap;
class QGVLayerTilesOnline;
class QGVWidgetText;
class QGVWidgetScale;

namespace tlp {

class AddressSelectionDialog;
class GeographicView;
class ProgressWidgetGraphicsProxy;

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

  GlGraph *glGraph() const;

  void draw() {
    glWidgetItem->setRedrawNeeded(true);
    scene()->update();
  }

  void centerView();

  QGVMap *getQGVMap() {
    return qgvMap;
  }

  GlWidget *glWidget() {
    return _glWidget;
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
  void refreshMap();
  void setMapScaleVisible(bool visible);

protected:
  void cleanup();
  void resizeEvent(QResizeEvent *event) override;

private:
  GeographicView *_geoView;
  Graph *graph;
  flat_hash_map<node, std::pair<double, double>> nodeLatLng;
  flat_hash_map<edge, std::vector<std::pair<double, double>>> edgeBendsLatLng;

  Camera globeCameraBackup;
  Camera mapCameraBackup;

  LayoutProperty *geoLayout;
  SizeProperty *geoViewSize;
  IntegerProperty *geoViewShape;
  LayoutProperty *geoLayoutBackup;

  bool geocodingActive;
  bool cancelGeocoding;

  GlWidget *_glWidget;
  GlWidgetGraphicsItem *glWidgetItem;
  QComboBox *viewTypeComboBox;
  QPushButton *zoomOutButton;
  QPushButton *zoomInButton;

  GlComposite *polygonEntity;
  GlEntity *planisphereEntity;

  AddressSelectionDialog *addressSelectionDialog;
  QGraphicsProxyWidget *addressSelectionProxy;
  ProgressWidgetGraphicsProxy *progressWidget;
  QGraphicsProxyWidget *noLayoutMsgBox;

  bool firstGlobeSwitch;

  QGraphicsRectItem *_placeholderItem;

  bool geoLayoutComputed;

  DoubleProperty *latitudeProperty;
  DoubleProperty *longitudeProperty;

  QGVMap *qgvMap;
  QGVLayerTilesOnline *currentMapLayer;
  QGVWidgetText *mapAttributionWidget;
  QGVWidgetScale *scaleWidget;
  flat_hash_map<GeographicView::ViewType, std::unique_ptr<QGVLayerTilesOnline>> tilesLayers;
};
}

#endif // GEOGRAPHIC_VIEW_GRAPHICS_VIEW_H
