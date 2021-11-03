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

#include <talipot/GlLines.h>

#include "HistoStatsConfigWidget.h"
#include "HistogramView.h"

#include "HistogramStatistics.h"

using namespace std;

// return f(g(x), h(x))
template <class F, class G, class H>
auto compose_fn(F f, G g, H h) {
  return [f, g, h](auto &&...args) {
    return f(g(forward<decltype(args)>(args)...), h(forward<decltype(args)>(args)...));
  };
}

template <class K, class V>
class map_value_greater_equal {
private:
  V value;

public:
  map_value_greater_equal(const V &v) : value(v) {}

  bool operator()(const pair<K, V> &elem) const {
    return elem.second >= value;
  }
};

template <class K, class V>
class map_value_less_equal {
private:
  V value;

public:
  map_value_less_equal(const V &v) : value(v) {}

  bool operator()(const pair<K, V> &elem) const {
    return elem.second <= value;
  }
};

namespace {
inline double square(double x) {
  return x * x;
}

} // namespace
static void drawComposite(tlp::GlComposite *composite, float lod, tlp::Camera *camera) {
  for (const auto &it : composite->getGlEntities()) {
    tlp::GlEntity *entity = it.second;
    auto *compositeEntity = dynamic_cast<tlp::GlComposite *>(entity);

    if (compositeEntity != nullptr) {
      drawComposite(compositeEntity, lod, camera);
    } else {
      entity->draw(lod, camera);
    }
  }
}

namespace tlp {

class UniformKernel : public KernelFunction {

public:
  double operator()(double val) override {
    if (fabs(val) < 1) {
      return 1. / 2.;
    } else {
      return 0.;
    }
  }
};

class GaussianKernel : public KernelFunction {

public:
  double operator()(double val) override {
    return (1. / M_PI) * exp(-square(val) / 2.);
  }
};

class TriangleKernel : public KernelFunction {

public:
  double operator()(double val) override {
    double valAbs = fabs(val);

    if (valAbs < 1) {
      return 1 - valAbs;
    } else {
      return 0.;
    }
  }
};

class EpanechnikovKernel : public KernelFunction {

public:
  double operator()(double val) override {
    double valAbs = fabs(val);

    if (valAbs < 1) {
      return (3. / 4.) * (1 - square(val));
    } else {
      return 0.;
    }
  }
};

class QuarticKernel : public KernelFunction {

public:
  double operator()(double val) override {
    double valAbs = fabs(val);

    if (valAbs < 1) {
      return (15. / 16.) * square(1 - square(val));
    } else {
      return 0.;
    }
  }
};

class CubicKernel : public KernelFunction {

public:
  double operator()(double val) override {
    double valAbs = fabs(val);

    if (valAbs < 1.) {
      double d = (35. / 32.) * pow((1. - square(val)), 3);
      return d;
    } else {
      return 0.;
    }
  }
};

class CosineKernel : public KernelFunction {

public:
  double operator()(double val) override {
    double valAbs = fabs(val);

    if (valAbs < 1) {
      return (M_PI / 4.) * cos((M_PI / 2.) * val);
    } else {
      return 0.;
    }
  }
};

HistogramStatistics::HistogramStatistics(HistoStatsConfigWidget *ConfigWidget)
    : histoView(nullptr), histoStatsConfigWidget(ConfigWidget), propertyMean(0),
      propertyStandardDeviation(0), densityAxis(nullptr), meanAxis(nullptr),
      standardDeviationPosAxis(nullptr), standardDeviationNegAxis(nullptr),
      standardDeviation2PosAxis(nullptr), standardDeviation2NegAxis(nullptr),
      standardDeviation3PosAxis(nullptr), standardDeviation3NegAxis(nullptr) {
  initKernelFunctionsMap();
}

HistogramStatistics::HistogramStatistics(const HistogramStatistics &histoStats)
    : histoView(histoStats.histoView), histoStatsConfigWidget(histoStats.histoStatsConfigWidget),
      propertyMean(0), propertyStandardDeviation(0), densityAxis(nullptr), meanAxis(nullptr),
      standardDeviationPosAxis(nullptr), standardDeviationNegAxis(nullptr),
      standardDeviation2PosAxis(nullptr), standardDeviation2NegAxis(nullptr),
      standardDeviation3PosAxis(nullptr), standardDeviation3NegAxis(nullptr) {
  initKernelFunctionsMap();
}

HistogramStatistics::~HistogramStatistics() {
  cleanupAxis();

  for (const auto &it : kernelFunctionsMap) {
    delete it.second;
  }
}

void HistogramStatistics::viewChanged(View *view) {
  histoView = static_cast<HistogramView *>(view);
  connect(histoStatsConfigWidget, &HistoStatsConfigWidget::computeAndDrawInteractor, this,
          &HistogramStatistics::computeAndDrawInteractor);
  // computeAndDrawInteractor();
}

void HistogramStatistics::initKernelFunctionsMap() {
  kernelFunctionsMap["Uniform"] = new UniformKernel();
  kernelFunctionsMap["Gaussian"] = new GaussianKernel();
  kernelFunctionsMap["Cubic"] = new CubicKernel();
  kernelFunctionsMap["Quartic"] = new QuarticKernel();
  kernelFunctionsMap["Triangle"] = new TriangleKernel();
  kernelFunctionsMap["Epanechnikov"] = new EpanechnikovKernel();
  kernelFunctionsMap["Cosine"] = new CosineKernel();
}

bool HistogramStatistics::eventFilter(QObject *, QEvent *e) {

  if (e->type() == QEvent::MouseMove) {
    histoView->refresh();
    return true;
  }

  return false;
}

void HistogramStatistics::cleanupAxis() {
  if (densityAxis != nullptr) {
    delete densityAxis;
    densityAxis = nullptr;
  }

  if (meanAxis != nullptr) {
    delete meanAxis;
    meanAxis = nullptr;
  }

  if (standardDeviationNegAxis != nullptr) {
    delete standardDeviationNegAxis;
    standardDeviationNegAxis = nullptr;
    delete standardDeviationPosAxis;
    standardDeviationPosAxis = nullptr;
  }

  if (standardDeviation2PosAxis != nullptr) {
    delete standardDeviation2PosAxis;
    standardDeviation2PosAxis = nullptr;
    delete standardDeviation2NegAxis;
    standardDeviation2NegAxis = nullptr;
  }

  if (standardDeviation3PosAxis != nullptr) {
    delete standardDeviation3PosAxis;
    standardDeviation3PosAxis = nullptr;
    delete standardDeviation3NegAxis;
    standardDeviation3NegAxis = nullptr;
  }
}

void HistogramStatistics::computeInteractor() {
  GlQuantitativeAxis *histoXAxis = histoView->getDetailedHistogram()->getXAxis();
  GlQuantitativeAxis *histoYAxis = histoView->getDetailedHistogram()->getYAxis();

  if (histoYAxis == nullptr) {
    return;
  }

  Graph *graph = histoView->graph();
  string selectedProperty(histoView->getDetailedHistogram()->getPropertyName());

  double sampleStep = histoStatsConfigWidget->getSampleStep();

  graphPropertyValueSet.clear();
  densityEstimationCurvePoints.clear();
  propertyMean = 0;
  propertyStandardDeviation = 0;

  cleanupAxis();

  string propertyType(graph->getProperty(selectedProperty)->getTypename());
  double min, max;

  if (propertyType == "double") {
    if (histoView->getDataLocation() == NODE) {
      min = graph->getDoubleProperty(selectedProperty)->getNodeMin();
      max = graph->getDoubleProperty(selectedProperty)->getNodeMax();
    } else {
      min = graph->getDoubleProperty(selectedProperty)->getEdgeMin();
      max = graph->getDoubleProperty(selectedProperty)->getEdgeMax();
    }
  } else {
    if (histoView->getDataLocation() == NODE) {
      min = graph->getIntegerProperty(selectedProperty)->getNodeMin();
      max = graph->getIntegerProperty(selectedProperty)->getNodeMax();
    } else {
      min = graph->getIntegerProperty(selectedProperty)->getEdgeMin();
      max = graph->getIntegerProperty(selectedProperty)->getEdgeMax();
    }
  }

  uint nbElements = 0;

  if (histoView->getDataLocation() == NODE) {
    nbElements = graph->numberOfNodes();
    for (auto n : graph->nodes()) {
      double nodeVal;

      if (propertyType == "double") {
        nodeVal = graph->getDoubleProperty(selectedProperty)->getNodeValue(n);
      } else {
        nodeVal = graph->getIntegerProperty(selectedProperty)->getNodeValue(n);
      }

      graphPropertyValueSet[n.id] = nodeVal;
      propertyMean += nodeVal;
    }

  } else {
    nbElements = graph->numberOfEdges();
    for (auto e : graph->edges()) {
      double edgeVal;

      if (propertyType == "double") {
        edgeVal = graph->getDoubleProperty(selectedProperty)->getEdgeValue(e);
      } else {
        edgeVal = graph->getIntegerProperty(selectedProperty)->getEdgeValue(e);
      }

      graphPropertyValueSet[e.id] = edgeVal;
      propertyMean += edgeVal;
    }
  }

  propertyMean /= (nbElements);

  for (const auto &it : graphPropertyValueSet) {
    propertyStandardDeviation += square(it.second - propertyMean);
  }

  propertyStandardDeviation = sqrt(propertyStandardDeviation / (nbElements - 1));

  histoStatsConfigWidget->setMinMaxMeanAndSd(min, max, propertyMean, propertyStandardDeviation);

  if (histoStatsConfigWidget->densityEstimation()) {
    double bandwidth = histoStatsConfigWidget->getBandwidth();

    vector<double> estimatedDensity;
    float maxDensityValue = 0.;

    KernelFunction *kf = kernelFunctionsMap[histoStatsConfigWidget->getKernelFunctionName()];

    for (double val = min; val <= max; val += sampleStep) {
      float fx = 0;

      for (const auto &it : graphPropertyValueSet) {
        fx += float((*kf)((val - (it.second)) / (bandwidth / 2.)));
      }

      fx *= (1.f / float(graphPropertyValueSet.size() * (bandwidth / 2.)));
      estimatedDensity.push_back(fx);

      if (fx > maxDensityValue) {
        maxDensityValue = fx;
      }
    }

    double val;
    uint i;

    for (val = min, i = 0; val <= max; val += sampleStep, ++i) {
      float x = histoXAxis->getAxisPointCoordForValue(val).getX();
      float y =
          histoYAxis
              ->getAxisPointCoordForValue(
                  (estimatedDensity[i] * (histoView->getDetailedHistogram()->getMaxBinSize())) /
                  maxDensityValue)
              .getY();
      densityEstimationCurvePoints.push_back(Coord(x, y, 0));
    }

    densityAxis = new GlQuantitativeAxis(
        "density", Coord(histoXAxis->getAxisBaseCoord().getX() + histoXAxis->getAxisLength(), 0, 0),
        histoYAxis->getAxisLength(), GlAxis::VERTICAL_AXIS, Color(255, 0, 0), true);
    densityAxis->setAxisParameters(0.0, double(maxDensityValue), 15, GlAxis::RIGHT_OR_ABOVE, true);
    densityAxis->updateAxis();
    densityAxis->addCaption(GlAxis::LEFT, densityAxis->getSpaceBetweenAxisGrads(), false);
  }

  if (histoStatsConfigWidget->displayMeanAndStandardDeviation()) {
    float axisExtension = 2 * histoXAxis->getAxisGradsWidth();
    float y = histoXAxis->getAxisBaseCoord().getY() - axisExtension;
    float axisLength = histoYAxis->getAxisLength() + axisExtension;
    float captionHeight = histoXAxis->getAxisGradsWidth();
    float x = histoXAxis->getAxisPointCoordForValue(propertyMean).getX();
    meanAxis = new GlAxis("m", Coord(x, y, 0), axisLength, GlAxis::VERTICAL_AXIS, Color(255, 0, 0));
    meanAxis->addCaption(GlAxis::LEFT, captionHeight, false);
    x = histoXAxis->getAxisPointCoordForValue(propertyMean + propertyStandardDeviation).getX();
    standardDeviationPosAxis =
        new GlAxis("+sd", Coord(x, y, 0), axisLength, GlAxis::VERTICAL_AXIS, Color(255, 0, 0));
    standardDeviationPosAxis->addCaption(GlAxis::LEFT, captionHeight, false);
    x = histoXAxis->getAxisPointCoordForValue(propertyMean - propertyStandardDeviation).getX();
    standardDeviationNegAxis =
        new GlAxis("-sd", Coord(x, y, 0), axisLength, GlAxis::VERTICAL_AXIS, Color(255, 0, 0));
    standardDeviationNegAxis->addCaption(GlAxis::LEFT, captionHeight, false);

    if (propertyMean - 2 * propertyStandardDeviation > min) {
      x = histoXAxis->getAxisPointCoordForValue(propertyMean + 2 * propertyStandardDeviation)
              .getX();
      standardDeviation2PosAxis =
          new GlAxis("+2sd", Coord(x, y, 0), axisLength, GlAxis::VERTICAL_AXIS, Color(255, 0, 0));
      standardDeviation2PosAxis->addCaption(GlAxis::LEFT, captionHeight, false);
      x = histoXAxis->getAxisPointCoordForValue(propertyMean - 2 * propertyStandardDeviation)
              .getX();
      standardDeviation2NegAxis =
          new GlAxis("-2sd", Coord(x, y, 0), axisLength, GlAxis::VERTICAL_AXIS, Color(255, 0, 0));
      standardDeviation2NegAxis->addCaption(GlAxis::LEFT, captionHeight, false);
    } else {
      standardDeviation2NegAxis = nullptr;
      standardDeviation2PosAxis = nullptr;
    }

    if (propertyMean - 3 * propertyStandardDeviation > min) {
      x = histoXAxis->getAxisPointCoordForValue(propertyMean + 3 * propertyStandardDeviation)
              .getX();
      standardDeviation3PosAxis =
          new GlAxis("+3sd", Coord(x, y, 0), axisLength, GlAxis::VERTICAL_AXIS, Color(255, 0, 0));
      standardDeviation3PosAxis->addCaption(GlAxis::LEFT, captionHeight, false);
      x = histoXAxis->getAxisPointCoordForValue(propertyMean - 3 * propertyStandardDeviation)
              .getX();
      standardDeviation3NegAxis =
          new GlAxis("-3sd", Coord(x, y, 0), axisLength, GlAxis::VERTICAL_AXIS, Color(255, 0, 0));
      standardDeviation3NegAxis->addCaption(GlAxis::LEFT, captionHeight, false);
    } else {
      standardDeviation3NegAxis = nullptr;
      standardDeviation3PosAxis = nullptr;
    }

    if (histoStatsConfigWidget->nodesSelection()) {
      Observable::holdObservers();
      BooleanProperty *viewSelection = graph->getBooleanProperty("viewSelection");
      viewSelection->setAllNodeValue(false);
      viewSelection->setAllEdgeValue(false);
      double lowerBound = histoStatsConfigWidget->getSelectionLowerBound();
      double upperBound = histoStatsConfigWidget->getSelectionUpperBound();
      auto pos =
          find_if(graphPropertyValueSet.begin(), graphPropertyValueSet.end(),
                  compose_fn(logical_and<bool>(), map_value_greater_equal<uint, double>(lowerBound),
                             map_value_less_equal<uint, double>(upperBound)));

      while (pos != graphPropertyValueSet.end()) {
        if (histoView->getDataLocation() == EDGE) {
          viewSelection->setNodeValue(node(pos->first), true);
        } else {
          viewSelection->setEdgeValue(edge(pos->first), true);
        }

        pos = find_if(++pos, graphPropertyValueSet.end(),
                      compose_fn(logical_and<bool>(),
                                 map_value_greater_equal<uint, double>(lowerBound),
                                 map_value_less_equal<uint, double>(upperBound)));
      }

      Observable::unholdObservers();
    }
  }
}

void HistogramStatistics::computeAndDrawInteractor() {
  computeInteractor();
  histoView->refresh();
}

bool HistogramStatistics::draw(GlWidget *glWidget) {

  Camera &camera = glWidget->scene()->getLayer("Main")->getCamera();
  camera.initGl();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  if (!densityEstimationCurvePoints.empty()) {
    Color curveColor = {255, 0, 0};
    Coord startPoint = densityEstimationCurvePoints[0];
    Coord endPoint = densityEstimationCurvePoints[densityEstimationCurvePoints.size() - 1];
    vector<Coord> densityEstimationCurvePointsCp(densityEstimationCurvePoints);
    densityEstimationCurvePointsCp.erase(densityEstimationCurvePointsCp.begin());
    densityEstimationCurvePointsCp.pop_back();
    GlLines::glDrawCurve(startPoint, densityEstimationCurvePointsCp, endPoint, 2.,
                         GlLines::TLP_PLAIN, curveColor, curveColor);
    drawComposite(densityAxis, 0, &camera);
  }

  if (meanAxis != nullptr) {
    drawComposite(meanAxis, 0, &camera);
  }

  if (standardDeviationPosAxis != nullptr) {
    drawComposite(standardDeviationPosAxis, 0, &camera);
    drawComposite(standardDeviationNegAxis, 0, &camera);
  }

  if (standardDeviation2PosAxis != nullptr) {
    drawComposite(standardDeviation2PosAxis, 0, &camera);
    drawComposite(standardDeviation2NegAxis, 0, &camera);
  }

  if (standardDeviation3PosAxis != nullptr) {
    drawComposite(standardDeviation3PosAxis, 0, &camera);
    drawComposite(standardDeviation3NegAxis, 0, &camera);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  return true;
}
}
