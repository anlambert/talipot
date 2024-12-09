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

#ifndef HISTOGRAM_VIEW_H
#define HISTOGRAM_VIEW_H

#include <talipot/GlView.h>

#include <vector>
#include <talipot/hash.h>

#include "../../utils/PluginNames.h"
#include "Histogram.h"

#include <talipot/OpenGlIncludes.h>

namespace tlp {

class Graph;
class GlLayer;
class GlComposite;
class Interactor;
class GlRect;
class ViewGraphPropertiesSelectionWidget;
class HistoOptionsWidget;
class QuickAccessBarImpl;

/** \file
 *  \brief Histogram view

 * This view plugin allows to create frequency histograms from graph properties (supported types are
 Double and Integer).
 * By selecting a set of graph properties, frequency histograms are computed and displayed for
 values associated
 * to nodes (or edges).
 *
 * This view also allows to map visual properties (colors, sizes, glyphs, ..) of the graph elements
 * with respect to a graph metric in a visual way. These operations can be done with the "Metric
 mapping"
 * interactor. Many interactors are also bundled with the view to perform elements selection,
 statistical analysis, ...
 *
 *
 */
class HistogramView : public GlView {

  Q_OBJECT

public:
  PLUGININFORMATION(ViewName::HistogramViewName, "Antoine Lambert", "02/2009",
                    "<p>The Histogram view allows to create frequency histograms from graph "
                    "properties (supported types are Double and Integer). "
                    "By selecting a set of graph properties, frequency histograms are computed and "
                    "displayed for values associated to nodes (or edges).</p>"
                    "<p>This view also allows to map visual properties (colors, sizes, glyphs, ..) "
                    "of the graph elements with respect to a graph metric in a visual way. These "
                    "operations can be done with the \"Metric mapping\" interactor. Many "
                    "interactors are also bundled with the view to perform elements selection, "
                    "statistical analysis, ...</p>",
                    "1.1", "View")

  HistogramView(const PluginContext *);
  ~HistogramView() override;

  std::string icon() const override {
    return ":/histogram_view.png";
  }

  void setState(const DataSet &dataSet) override;
  DataSet state() const override;
  void graphChanged(Graph *graph) override;
  Graph *histoGraph() {
    return _histoGraph;
  }
  bool eventFilter(QObject *object, QEvent *event) override;
  QList<QWidget *> configurationWidgets() const override;

  std::vector<Histogram *> getHistograms() const;
  bool smallMultiplesViewSet() const {
    return smallMultiplesView;
  }

  QuickAccessBar *getQuickAccessBarImpl() override;

  void switchFromSmallMultiplesToDetailedView(Histogram *histogramToDetail);
  void switchFromDetailedViewToSmallMultiples();
  BoundingBox getSmallMultiplesBoundingBox() const;
  Histogram *getDetailedHistogram() const {
    return detailedHistogram;
  }

  void toggleInteractors(const bool activate);

  ElementType getDataLocation() const {
    return dataLocation;
  }

  void updateHistograms(Histogram *detailOverview = nullptr);

  void treatEvent(const Event &message) override;

  void afterSetNodeValue(PropertyInterface *, const node);
  void afterSetEdgeValue(PropertyInterface *, const edge);
  void afterSetAllNodeValue(PropertyInterface *);
  void afterSetAllEdgeValue(PropertyInterface *);

  virtual void addNode(Graph *, const node);
  virtual void addEdge(Graph *, const edge);
  virtual void delNode(Graph *, const node);
  virtual void delEdge(Graph *, const edge);

  // return the id of the corresponding graph elt
  // see ScatterPlot2DMouseShowElementInfo
  // in ScatterPlot2DInteractors.cpp
  uint getMappedId(uint id);

public slots:

  void viewConfigurationChanged();
  void draw() override;
  void refresh() override;
  void applySettings() override;

protected slots:

private:
  void interactorsInstalled(const QList<tlp::Interactor *> &) override;
  void registerTriggers();
  void initGlWidget(Graph *graph);
  void buildHistograms();

  void updateDetailedHistogramAxis();
  void destroyHistogramsIfNeeded();
  void cleanupGlScene();
  void addEmptyViewLabel();
  void removeEmptyViewLabel();

  void setLayoutUpdateNeeded() {
    if (detailedHistogram) {
      detailedHistogram->setLayoutUpdateNeeded();
    }
  }

  void setSizesUpdateNeeded() {
    if (detailedHistogram) {
      detailedHistogram->setSizesUpdateNeeded();
    }
  }

  void setUpdateNeeded() {
    if (detailedHistogram) {
      detailedHistogram->setUpdateNeeded();
    }
  }

  ViewGraphPropertiesSelectionWidget *propertiesSelectionWidget;
  HistoOptionsWidget *histoOptionsWidget;

  GlQuantitativeAxis *xAxisDetail, *yAxisDetail;
  Graph *_histoGraph, *emptyGraph;
  GlGraph *emptyGlGraph;

  GlComposite *histogramsComposite, *labelsComposite, *axisComposite;
  std::map<std::string, Histogram *> histogramsMap;

  bool smallMultiplesView;
  GlLayer *mainLayer;
  std::vector<std::string> selectedProperties;

  Histogram *detailedHistogram;
  std::string detailedHistogramPropertyName;

  double sceneRadiusBak;
  double zoomFactorBak;
  Coord eyesBak;
  Coord centerBak;
  Coord upBak;

  GlLabel *noDimsLabel, *noDimsLabel1;
  GlLabel *noDimsLabel2;
  GlRect *emptyRect;
  GlRect *emptyRect2;

  bool isConstruct;
  uint lastNbHistograms;

  ElementType dataLocation;
  bool needUpdateHistogram;

  Graph *edgeAsNodeGraph;
  flat_hash_map<edge, node> edgeToNode;
  flat_hash_map<node, edge> nodeToEdge;
};
}

#endif // HISTOGRAM_VIEW_H
