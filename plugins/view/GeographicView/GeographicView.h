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

#ifndef GEOGRAPHIC_VIEW_H
#define GEOGRAPHIC_VIEW_H

#include <talipot/OcclusionTest.h>
#include <talipot/SceneConfigWidget.h>
#include <talipot/SceneLayersConfigWidget.h>
#include <talipot/Interactor.h>
#include <talipot/View.h>
#include <talipot/GlView.h>
#include <talipot/ViewActionsManager.h>

#include <QGraphicsScene>
#include <QResizeEvent>
#include <QGraphicsItem>
#include <QDialog>
#include <QThread>
#include <QMap>

#include "../../utils/PluginNames.h"

namespace tlp {

class GeographicViewGraphicsView;
class GeographicViewConfigWidget;
class GeolocationConfigWidget;

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
    GeoportailPlan,
    GeoportailSatellite,
    Google,
    Bing,
    CustomTilesLayer,
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

  QGraphicsView *graphicsView() const override;

  QList<QWidget *> configurationWidgets() const override;

  QGraphicsItem *centralItem() const override;

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

  bool getNodeOrEdgeAtViewportPos(int x, int y, node &n, edge &e) const override;

  GeographicViewConfigWidget *getConfigWidget() const {
    return geoViewConfigWidget;
  }

  static ViewType getViewTypeFromName(const QString &name);

  static QString getViewNameFromType(ViewType viewType);

  static QList<ViewType> getViewTypes();

public slots:

  void computeGeoLayout();

  void draw() override;

  void refresh() override;

  void graphChanged(Graph *) override;

  void graphDeleted(tlp::Graph *) override {}

  void applySettings() override;

  void updateSharedProperties();

  void currentInteractorChanged(tlp::Interactor *i) override;

  void mapToPolygon();

  void centerView();

  void viewTypeChanged(int);

  void zoomIn();
  void zoomOut();

protected slots:

  void fillContextMenu(QMenu *, const QPointF &) override;

private:
  void viewTypeChanged(const QString &viewTypeName);

  void updatePoly(bool force = false);

  void loadStoredPolyInformation(const DataSet &dataset);

  void saveStoredPolyInformation(DataSet &dataset) const;

  GeographicViewGraphicsView *geoViewGraphicsView;
  GeographicViewConfigWidget *geoViewConfigWidget;
  GeolocationConfigWidget *geolocationConfigWidget;
  SceneConfigWidget *sceneConfigurationWidget;
  SceneLayersConfigWidget *sceneLayersConfigurationWidget;

  QAction *centerViewAction;
  QAction *showConfPanelAction;

  ViewType _viewType;

  bool useSharedLayoutProperty;
  bool useSharedSizeProperty;
  bool useSharedShapeProperty;

  ViewActionsManager *_viewActionsManager;

  static const QMap<ViewType, QString> viewTypeToName;
};
}

#endif // GEOGRAPHIC_VIEW_H
