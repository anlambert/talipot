/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef HISTO_STATS_CONFIG_WIDGET_H
#define HISTO_STATS_CONFIG_WIDGET_H

#include <QWidget>

namespace Ui {
class HistoStatsConfigWidget;
}

namespace tlp {

class HistoStatsConfigWidget : public QWidget {

    Q_OBJECT

    Ui::HistoStatsConfigWidget *_ui;

  public:
    HistoStatsConfigWidget(QWidget *parent = nullptr);
    ~HistoStatsConfigWidget() override;

    void setMinMaxMeanAndSd(double min, double max, double mean, double standardDeviation);
    bool densityEstimation() const;
    double getSampleStep() const;
    double getBandwidth() const;
    QString getKernelFunctionName() const;
    bool displayMeanAndStandardDeviation() const;
    bool nodesSelection() const;
    double getSelectionLowerBound() const;
    double getSelectionUpperBound() const;

  private:
    double getBoundFromString(const QString &bound) const;

    double min, max;
    double mean, standardDeviation;

  signals:
    void computeAndDrawInteractor();
};
}

#endif // HISTO_STATS_CONFIG_WIDGET_H
