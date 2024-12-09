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

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <talipot/GlComposite.h>
#include <talipot/GlGraph.h>
#include <talipot/GlQuantitativeAxis.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/GlBoundingBoxSceneVisitor.h>

namespace tlp {

const std::string BIN_RECT_TEXTURE = ":/histo_texture.png";

class Histogram : public GlComposite {

public:
  Histogram(Graph *graph, Graph *edgeGraph, flat_hash_map<edge, node> &edgeMap,
            const std::string &propertyName, const ElementType &dataLocation, const Coord &blCorner,
            uint size, const Color &backgroundColor, const Color &textColor);
  ~Histogram() override;

  const std::string &getPropertyName() const {
    return propertyName;
  }
  GlQuantitativeAxis *getXAxis() const {
    return xAxis;
  }
  GlQuantitativeAxis *getYAxis() const {
    return yAxis;
  }
  GlComposite *getBinsComposite() const {
    return histoBinsComposite;
  }
  LayoutProperty *getHistogramLayout() const {
    return histogramLayout;
  }
  SizeProperty *getHistogramSize() const {
    return histogramSize;
  }
  GlGraph *glGraph() const {
    return _glGraph;
  }

  void setNbHistogramBins(const uint nbHistogramBins) {
    this->nbHistogramBins = nbHistogramBins;
  }
  uint getNbHistogramBins() const {
    return nbHistogramBins;
  }

  uint getMaxBinSize() const {
    return maxBinSize;
  }
  double getHistogramBinsWidth() const {
    return binWidth;
  }

  void setYAxisIncrementStep(const uint yAxisIncrementStep) {
    this->yAxisIncrementStep = yAxisIncrementStep;
  }
  uint getYAxisIncrementStep() const {
    return yAxisIncrementStep;
  }

  void setLastCumulativeHistogram(const bool lastCumulHist) {
    lastCumulHisto = lastCumulHist;
  }
  void setCumulativeHistogram(const bool cumulHisto) {
    cumulativeFreqHisto = cumulHisto;
  }
  bool cumulativeFrequenciesHistogram() const {
    return cumulativeFreqHisto;
  }

  void setUniformQuantification(const bool uniformQuantification) {
    this->uniformQuantification = uniformQuantification;
  }
  bool uniformQuantificationHistogram() const {
    return uniformQuantification;
  }

  void setXAxisLogScale(const bool xAxisLogScale) {
    this->xAxisLogScale = xAxisLogScale;
  }
  bool xAxisLogScaleSet() const {
    return xAxisLogScale;
  }

  void setYAxisLogScale(const bool yAxisLogScale) {
    this->yAxisLogScale = yAxisLogScale;
  }
  bool yAxisLogScaleSet() const {
    return yAxisLogScale;
  }

  void setNbXGraduations(const uint nbXGraduations) {
    this->nbXGraduations = nbXGraduations;
  }
  uint getNbXGraduations() const {
    return nbXGraduations;
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
  const std::pair<double, double> &getXAxisScale() const {
    return xAxisScale;
  }
  void setXAxisScale(const std::pair<double, double> &value) {
    xAxisScale = value;
  }
  const std::pair<double, double> &getYAxisScale() const {
    return yAxisScale;
  }
  void setYAxisScale(const std::pair<double, double> &value) {
    yAxisScale = value;
  }

  const std::pair<double, double> &getInitXAxisScale() const {
    return initXAxisScale;
  }
  void setInitXAxisScale(const std::pair<double, double> &value) {
    initXAxisScale = value;
  }
  const std::pair<double, double> &getInitYAxisScale() const {
    return initYAxisScale;
  }
  void setInitYAxisScale(const std::pair<double, double> &value) {
    initYAxisScale = value;
  }

  void setBLCorner(const Coord &blCorner);

  void setLayoutUpdateNeeded();
  void setSizesUpdateNeeded();
  void setUpdateNeeded();
  void update();

  void setBackgroundColor(const Color &color) {
    backgroundColor = color;
  }
  void setTextColor(const Color &color);

  void setDataLocation(const ElementType &dataLocation);

  void setDisplayGraphEdges(const bool displayGraphEdges) {
    displayEdges = displayGraphEdges;
  }

  bool displayGraphEdges() const {
    return displayEdges;
  }

private:
  void computeBoundingBox() {
    GlBoundingBoxSceneVisitor glBBSV(nullptr);
    acceptVisitor(&glBBSV);
    boundingBox = glBBSV.getBoundingBox();
  }

  void computeHistogram();
  void createAxis();
  void updateLayout();
  void updateSizes();

  Graph *graph;
  std::string propertyName;
  Coord blCorner;
  uint size;
  uint nbHistogramBins;
  std::vector<std::vector<uint>> histogramBins;
  double binWidth;
  uint maxBinSize;
  GlQuantitativeAxis *xAxis, *yAxis;
  bool xAxisLogScale, yAxisLogScale;
  uint nbXGraduations;
  uint yAxisIncrementStep;
  LayoutProperty *histogramLayout, *histogramEdgeLayout;
  SizeProperty *histogramSize;
  GlComposite *histoBinsComposite;
  double min, max;
  float refSize, refSizeX, refSizeY;
  std::vector<std::string> uniformQuantificationAxisLabels;
  bool uniformQuantification;
  bool cumulativeFreqHisto, lastCumulHisto;
  flat_hash_map<uint, std::pair<double, double>> binMinMaxMap;
  std::string textureName;
  GlGraph *_glGraph;
  Graph *edgeAsNodeGraph;
  flat_hash_map<edge, node> &edgeToNode;
  Color backgroundColor;
  Color textColor;
  bool integerScale;
  ElementType dataLocation;
  bool displayEdges;
  bool layoutUpdateNeeded;
  bool sizesUpdateNeeded;
  bool updateNeeded;

  bool xAxisScaleDefined, yAxisScaleDefined;
  std::pair<double, double> xAxisScale, yAxisScale;
  std::pair<double, double> initXAxisScale, initYAxisScale;

  int overviewId;
  static int overviewCpt;
};
}

#endif // HISTOGRAM_H
