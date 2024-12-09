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

#ifndef SCATTER_PLOT2D_H
#define SCATTER_PLOT2D_H

#include <talipot/GlComposite.h>
#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/Graph.h>

namespace tlp {

class SizeProperty;
class LayoutProperty;
class GlWidget;
class GlQuantitativeAxis;
class GlGraph;
class GlRect;
class GlLabel;
class GlProgressBar;
class Graph;

const std::string backgroundTextureId = ":/background_texture.png";

class ScatterPlot2D : public GlComposite {

public:
  ScatterPlot2D(Graph *graph, Graph *edgeGraph, flat_hash_map<node, edge> &nodeMap,
                const std::string &xDim, const std::string &yDim, const ElementType &dataLocation,
                Coord blCorner, uint size, const Color &backgroundColor,
                const Color &foregroundColor);
  ~ScatterPlot2D() override;

  void setBLCorner(const Coord &blCorner);
  void setUniformBackgroundColor(const Color &backgroundColor);
  void mapBackgroundColorToCorrelCoeff(const bool mapBackgroundColor, const Color &minusOneColor,
                                       const Color &zeroColor, const Color &oneColor);
  void setForegroundColor(const Color &foregroundColor);

  const Color &getBackgroundColor() const {
    return backgroundColor;
  }

  void generateOverview(GlWidget *glWidget = nullptr, LayoutProperty *reverseLayout = nullptr);
  bool overviewGenerated() const {
    return overviewGen;
  }

  const std::string &getXDim() const {
    return xDim;
  }
  const std::string &getYDim() const {
    return yDim;
  }
  Coord getOverviewCenter() const;
  float getOverviewSize() const {
    return float(size);
  }
  LayoutProperty *getScatterPlotLayout() const {
    return scatterLayout;
  }
  GlQuantitativeAxis *getXAxis() const {
    return xAxis;
  }
  GlQuantitativeAxis *getYAxis() const {
    return yAxis;
  }

  bool getXAxisScaleDefined() const {
    return xAxisScaleDefined;
  }
  void setXAxisScaleDefined(const bool value) {
    xAxisScaleDefined = value;
  }
  bool getYAxisScaleDefined() const {
    return yAxisScaleDefined;
  }
  void setYAxisScaleDefined(const bool value) {
    yAxisScaleDefined = value;
  }
  std::pair<double, double> getXAxisScale() const {
    return xAxisScale;
  }
  void setXAxisScale(const std::pair<double, double> &value) {
    xAxisScale = value;
  }
  std::pair<double, double> getYAxisScale() const {
    return yAxisScale;
  }
  void setYAxisScale(const std::pair<double, double> &value) {
    yAxisScale = value;
  }

  std::pair<double, double> getInitXAxisScale() const {
    return initXAxisScale;
  }
  void setInitXAxisScale(const std::pair<double, double> &value) {
    initXAxisScale = value;
  }
  std::pair<double, double> getInitYAxisScale() const {
    return initYAxisScale;
  }
  void setInitYAxisScale(const std::pair<double, double> &value) {
    initYAxisScale = value;
  }

  double getCorrelationCoefficient() const {
    return correlationCoeff;
  }

  GlGraph *glGraph() const {
    return _glGraph;
  }
  void setDisplayGraphEdges(const bool displayGraphEdges) {
    displayEdges = displayGraphEdges;
  }

  void setDisplayNodeLabels(const bool displayNodelabels) {
    displaylabels = displayNodelabels;
  }

  void setLabelsScaled(const bool scalelabel) {
    scale = scalelabel;
  }

  void setDataLocation(const ElementType &dataLocation);

private:
  void computeBoundingBox() {
    GlBoundingBoxSceneVisitor glBBSV(nullptr);
    acceptVisitor(&glBBSV);
    boundingBox = glBBSV.getBoundingBox();
  }

  void createAxis();
  void computeScatterPlotLayout(GlWidget *glWidget, LayoutProperty *reverseLayout);
  void clean();

  std::string xDim, yDim;
  std::string xType, yType;
  Coord blCorner;
  uint size;
  Graph *graph;
  GlGraph *_glGraph;
  LayoutProperty *scatterLayout, *scatterEdgeLayout;
  GlQuantitativeAxis *xAxis, *yAxis;
  std::string textureName;
  GlProgressBar *glProgressBar;
  int currentStep;
  int maxStep;
  int drawStep;
  bool overviewGen;
  Color backgroundColor, foregroundColor;
  GlLabel *clickLabel;
  GlRect *backgroundRect;

  bool mapBackgroundColorToCoeff;
  Color minusOneColor, zeroColor, oneColor;

  Graph *edgeAsNodeGraph;
  flat_hash_map<node, edge> &nodeToEdge;
  ElementType dataLocation;
  bool xAxisScaleDefined, yAxisScaleDefined;
  std::pair<double, double> xAxisScale, yAxisScale;
  std::pair<double, double> initXAxisScale, initYAxisScale;

  double correlationCoeff;

  bool displayEdges;
  bool displaylabels;
  bool scale;

  int overviewId;
  static int overviewCpt;
};
}

#endif // SCATTER_PLOT2D_H
