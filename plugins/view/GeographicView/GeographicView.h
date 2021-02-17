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

#ifndef GEOGRAPHIC_VIEW_H
#define GEOGRAPHIC_VIEW_H

#include <talipot/OcclusionTest.h>
#include <talipot/SceneConfigWidget.h>
#include <talipot/SceneLayersConfigWidget.h>
#include <talipot/Interactor.h>
#include <talipot/View.h>
#include <talipot/GlMainView.h>
#include <talipot/ViewActionsManager.h>

#include <QGraphicsScene>
#include <QResizeEvent>
#include <QGraphicsItem>
#include <QDialog>
#include <QThread>
#include <QMap>

#include <map>

#include "GeographicViewConfigWidget.h"
#include "GeolocalisationConfigWidget.h"
#include "LeafletMaps.h"
#include "GeographicViewGraphicsView.h"

#include "../../utils/PluginNames.h"

namespace tlp {

/*@{*/
/** \file
 *  \brief Geographic View

 * This view plugin allows to visualize a geolocated Talipot graph on top of maps.
 * If geographic properties are attached to graph nodes (address or latitude/longitude), this
 * plugin uses them to layout the nodes on the map.
 *
 * An interactor for performing selection on graph elements is also bundled
 * with the view.
 *
 *
 */
class GeographicView : public View {

  Q_OBJECT

  PLUGININFORMATION(
      ViewName::GeographicViewName, "Antoine Lambert and Morgan Mathiaut", "06/2012",
      "<p>The Geographic view allows to visualize a geolocated Talipot graph on top of "
      "maps or projected on a globe.</p>"
      "<p>If geographic properties are attached to graph nodes (address or "
      "latitude/longitude), they are used to layout the nodes on the maps or on the globe.</p>"
      "<p>An interactor for performing selection on graph elements is also bundled "
      "with the view.</p>",
      "3.1", "View")

public:
  enum ViewType {
    OpenStreetMap = 0,
    EsriSatellite,
    EsriTerrain,
    EsriGrayCanvas,
    LeafletCustomTileLayer,
    Polygon,
    Globe
  };

  GeographicView(PluginContext *);
  ~GeographicView() override;

  std::string icon() const override {
    return ":/talipot/view/geographic/geographic_view.png";
  }

  void setupUi() override;

  QPixmap snapshot(const QSize &) const override;

  void setState(const DataSet &dataSet) override;
  DataSet state() const override;

  QGraphicsView *graphicsView() const override {
    return geoViewGraphicsView;
  }

  QList<QWidget *> configurationWidgets() const override;

  QGraphicsItem *centralItem() const override;

  LeafletMaps *getLeafletMap() {
    return geoViewGraphicsView->getLeafletMapsPage();
  }

  void registerTriggers();

  ViewType viewType() {
    return _viewType;
  }

  // inherited from View
  void centerView(bool) override {
    // call the Qt slot declared below
    centerView();
  }

  GeographicViewGraphicsView *getGeographicViewGraphicsView() const {
    return geoViewGraphicsView;
  }

  bool getNodeOrEdgeAtViewportPos(int x, int y, node &n, edge &e) const override {
    return GlMainView::getNodeOrEdgeAtViewportPos(geoViewGraphicsView->getGlWidget(), x, y, n, e);
  }

  GeographicViewConfigWidget *getConfigWidget() const {
    return geoViewConfigWidget;
  }

public slots:

  void computeGeoLayout();

  void draw() override;

  void refresh() override;

  void graphChanged(Graph *) override {
    setState(DataSet());
  }

  void graphDeleted(tlp::Graph *) override {}

  void applySettings() override;

  void updateSharedProperties();

  void currentInteractorChanged(tlp::Interactor *i) override {
    i->install(geoViewGraphicsView->getGlWidget());
  }

  void mapToPolygon() {
    geoViewGraphicsView->mapToPolygon();
  }

  void centerView();

  void viewTypeChanged(int);

  void zoomIn();
  void zoomOut();
  void currentZoomChanged();

  ViewType getViewTypeFromName(const QString &name) const;
  QString getViewNameFromType(ViewType viewType) const;

protected slots:

  void fillContextMenu(QMenu *, const QPointF &) override;

  void initMap();

private:
  void viewTypeChanged(const QString &viewTypeName);

  void updatePoly(bool force = false);

  void loadStoredPolyInformation(const DataSet &dataset);

  void saveStoredPolyInformation(DataSet &dataset) const;

  GeographicViewGraphicsView *geoViewGraphicsView;
  GeographicViewConfigWidget *geoViewConfigWidget;
  GeolocalisationConfigWidget *geolocalisationConfigWidget;
  SceneConfigWidget *sceneConfigurationWidget;
  SceneLayersConfigWidget *sceneLayersConfigurationWidget;

  QAction *centerViewAction;
  QAction *showConfPanelAction;

  ViewType _viewType;

  bool useSharedLayoutProperty;
  bool useSharedSizeProperty;
  bool useSharedShapeProperty;

  double mapCenterLatitudeInit, mapCenterLongitudeInit;
  int mapZoomInit;
  ViewActionsManager *_viewActionsManager;

  QMap<ViewType, QString> _viewTypeToName;
};
}

#endif // GEOGRAPHIC_VIEW_H
