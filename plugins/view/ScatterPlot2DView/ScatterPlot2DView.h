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

#ifndef SCATTER_PLOT2D_VIEW_H
#define SCATTER_PLOT2D_VIEW_H

#include <talipot/hash.h>

#include <talipot/Graph.h>
#include <talipot/GlView.h>
#include <talipot/BoundingBox.h>
#include <talipot/Coord.h>

#include <talipot/OpenGlIncludes.h>

#include "../../utils/PluginNames.h"

namespace tlp {

/** \file
 *  \brief  Scatter Plot view

 * This view plugin allows to create 2D scatter plots with numerical properties attached to graph
 nodes or edges.
 * A scatter plot is a type of mathematical diagram using Cartesian coordinates to display values
 for two variables for a set of data.
 * The data is displayed as a collection of points, each having the value of one variable
 determining the position
 * on the horizontal axis and the value of the other variable determining the position on the
 vertical axis.
 *
 * By selecting a set of graph properties, a scatter plot matrix will be displayed for each
 combination
 * of two different properties. Each scatter plot can then be displayed individually in a more
 detailed way.
 *
 * A set of interactors are bundled with the view to perform selection, statistical analysis, ...
 *
 *
 */

class ScatterPlot2D;
class GlLayer;
class GlComposite;
class SizeProperty;
class GlGraph;
class ScatterPlot2DOptionsWidget;
class ViewGraphPropertiesSelectionWidget;
class ScatterPlotQuickAccessBar;

class ScatterPlot2DView : public GlView {

  Q_OBJECT

  void registerTriggers();

public:
  PLUGININFORMATION(ViewName::ScatterPlot2DViewName, "Antoine Lambert", "03/2009",
                    "<p>The Scatter Plot 2D view allows to create 2d scatter plots of graph nodes "
                    "from graph properties (supported types are Double and Integer).</p>"
                    "<p>A scatter plot is a type of mathematical diagram using Cartesian "
                    "coordinates to display values for two variables for a set of data. The data "
                    "are displayed as a collection of points, each having the value of one "
                    "variable determining the position on the horizontal axis and the value of the "
                    "other variable determining the position on the vertical axis.</p>"
                    "<p>By selecting a set of graph properties, a scatter plot matrix will be "
                    "displayed for each combination of two different properties. Each scatter plot "
                    "can then be displayed individually in a more detailed way.</p>"
                    "<p>A set of interactors are bundled with the view to perform selection, "
                    "statistical analysis, ...</p>",
                    "1.0", "View")

  ScatterPlot2DView(const PluginContext *);
  ~ScatterPlot2DView() override;
  std::string icon() const override {
    return ":/scatter_plot2d_view.png";
  }

  QuickAccessBar *getQuickAccessBarImpl() override;

  void setState(const DataSet &dataSet) override;
  DataSet state() const override;
  void graphChanged(Graph *graph) override;
  Graph *getScatterPlotGraph();

  QList<QWidget *> configurationWidgets() const override;

  std::vector<ScatterPlot2D *> getSelectedScatterPlots() const;
  bool matrixViewSet() const {
    return matrixView;
  }
  void setMatrixView(const bool matrixView) {
    this->matrixView = matrixView;
  }

  void switchFromMatrixToDetailView(ScatterPlot2D *scatterPlot, bool center);
  void switchFromDetailViewToMatrixView();
  BoundingBox getMatrixBoundingBox();
  ScatterPlot2D *getDetailedScatterPlot() const {
    return detailedScatterPlot;
  }

  void toggleInteractors(const bool activate);

  ElementType getDataLocation() const {
    return dataLocation;
  }

  void generateScatterPlot(ScatterPlot2D *scatterPlot, GlWidget *glWidget = nullptr);

  //
  void computeNodeSizes();
  void buildScatterPlotsMatrix();

  void draw() override;
  void refresh() override;

  void treatEvent(const Event &message) override;

  void afterSetNodeValue(PropertyInterface *, const node);
  void afterSetEdgeValue(PropertyInterface *, const edge);
  void afterSetAllNodeValue(PropertyInterface *);
  void afterSetAllEdgeValue(PropertyInterface *);

  virtual void addEdge(Graph *, const edge);
  virtual void delNode(Graph *, const node);
  virtual void delEdge(Graph *, const edge);

  // return the id of the corresponding graph elt
  // see ScatterPlot2DMouseShowElementInfo
  // in ScatterPlot2DInteractors.cpp
  uint getMappedId(uint id);

public slots:

  void init();
  void viewConfigurationChanged();

  // inherited from GlView
  void centerView(bool graphChanged = false) override;
  void applySettings() override;

private:
  void interactorsInstalled(const QList<tlp::Interactor *> &) override;
  void initGlWidget(Graph *graph);
  void generateScatterPlots();

  void destroyOverviewsIfNeeded();
  void destroyOverviews();
  void cleanupGlScene();
  void addEmptyViewLabel();
  void removeEmptyViewLabel();

  ViewGraphPropertiesSelectionWidget *propertiesSelectionWidget;
  ScatterPlot2DOptionsWidget *optionsWidget;

  Graph *scatterPlotGraph, *emptyGraph;
  GlLayer *mainLayer;
  GlGraph *glGraph;
  SizeProperty *scatterPlotSize;
  std::vector<std::string> selectedGraphProperties;
  GlComposite *matrixComposite, *axisComposite, *labelsComposite;
  std::map<std::pair<std::string, std::string>, ScatterPlot2D *> scatterPlotsMap;
  ScatterPlot2D *detailedScatterPlot;
  std::pair<std::string, std::string> detailedScatterPlotPropertyName;
  std::map<std::pair<std::string, std::string>, bool> scatterPlotsGenMap;
  bool center;
  bool matrixView;
  double sceneRadiusBak;
  double zoomFactorBak;
  Coord eyesBak;
  Coord centerBak;
  Coord upBak;
  bool matrixUpdateNeeded;
  bool newGraphSet;

  int lastViewWindowWidth, lastViewWindowHeight;

  bool initialized;

  ElementType dataLocation;
  Graph *edgeAsNodeGraph;
  flat_hash_map<edge, node> edgeToNode;
  flat_hash_map<node, edge> nodeToEdge;
};
}

#endif // SCATTER_PLOT2D_VIEW_H
