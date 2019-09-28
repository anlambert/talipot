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

#ifndef HISTOGRAM_STATISTICS_H
#define HISTOGRAM_STATISTICS_H

#include <talipot/GLInteractor.h>
#include <talipot/Coord.h>
#include <map>

#include <QString>

namespace tlp {

class HistoStatsConfigWidget;
class HistogramView;

class GlAxis;
class GlQuantitativeAxis;

class KernelFunction : public std::unary_function<double, double> {

public:
  virtual ~KernelFunction() {}

  virtual double operator()(double val) = 0;
};

class HistogramStatistics : public GLInteractorComponent {

  Q_OBJECT

public:
  HistogramStatistics(HistoStatsConfigWidget *ConfigWidget);
  HistogramStatistics(const HistogramStatistics &histoStats);
  ~HistogramStatistics() override;

  bool eventFilter(QObject *, QEvent *) override;
  bool draw(GlMainWidget *glMainWidget) override;

  void viewChanged(View *view) override;

private slots:

  void computeAndDrawInteractor();

public slots:
  void computeInteractor();

private:
  void cleanupAxis();
  void initKernelFunctionsMap();

protected:
  HistogramView *histoView;
  HistoStatsConfigWidget *histoStatsConfigWidget;
  std::map<unsigned int, double> graphPropertyValueSet;
  double propertyMean;
  double propertyStandardDeviation;
  std::vector<Coord> densityEstimationCurvePoints;
  std::map<QString, KernelFunction *> kernelFunctionsMap;
  GlQuantitativeAxis *densityAxis;
  GlAxis *meanAxis, *standardDeviationPosAxis, *standardDeviationNegAxis;
  GlAxis *standardDeviation2PosAxis, *standardDeviation2NegAxis;
  GlAxis *standardDeviation3PosAxis, *standardDeviation3NegAxis;
};
} // namespace tlp

#endif // HISTOGRAM_STATISTICS_H
