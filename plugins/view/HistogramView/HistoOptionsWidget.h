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

#ifndef HISTO_OPTIONS_WIDGET_H
#define HISTO_OPTIONS_WIDGET_H

#include <QWidget>

#include <talipot/Color.h>

namespace Ui {
class HistoOptionsWidget;
}

namespace tlp {

class HistoOptionsWidget : public QWidget {

    Q_OBJECT

  public:
    HistoOptionsWidget(QWidget *parent = nullptr);
    ~HistoOptionsWidget() override;

    void setWidgetEnabled(const bool enabled);

    void setNbOfHistogramBins(const uint nbOfHistogramBins);
    uint getNbOfHistogramBins();

    void setNbXGraduations(const uint nbXGraduations);
    uint getNbXGraduations();

    void setYAxisIncrementStep(const uint yAxisIncrementStep);
    uint getYAxisIncrementStep();

    void setCumulativeFrequenciesHistogram(const bool cumulHisto);
    bool cumulativeFrequenciesHisto();

    void setUniformQuantification(const bool uniformQuantification);
    bool uniformQuantification();

    void setXAxisLogScale(const bool xAxisLogScale);
    bool xAxisLogScaleSet() const;

    void setYAxisLogScale(const bool yAxisLogScale);
    bool yAxisLogScaleSet() const;

    bool useCustomXAxisScale() const;
    void useCustomXAxisScale(const bool value);

    bool useCustomYAxisScale() const;
    void useCustomYAxisScale(const bool value);

    std::pair<double, double> getXAxisScale() const;
    void setXAxisScale(const std::pair<double, double> &value);

    std::pair<double, double> getYAxisScale() const;
    void setYAxisScale(const std::pair<double, double> &value);

    void setInitXAxisScale(const std::pair<double, double> &value);
    void setInitYAxisScale(const std::pair<double, double> &value);

    void resetAxisScale();

    void setBinWidth(const double binWidth);

    Color getBackgroundColor() const;
    void setBackgroundColor(const Color &color);

    bool showGraphEdges() const;
    void setShowGraphEdges(const bool showGraphEdges);

    void enableShowGraphEdgesCB(const bool enable);

    bool configurationChanged();

  private slots:

    void enableOrDisableNbXGraduationsSP(int uniQuantState);
    void pressXScaleCheckBox(bool checked);
    void pressYScaleCheckBox(bool checked);

  private:
    bool oldValueInitialized;
    uint oldNbOfHistogramBins;
    uint oldNbXGraduations;
    uint oldYAxisIncrementStep;
    bool oldCumulativeFrequenciesHistogram;
    bool oldUniformQuantification;
    bool oldXAxisLogScale;
    bool oldYAxisLogScale;
    Color oldBackgroundColor;
    bool oldShowGraphEdges;
    bool oldUseCustomXAxisScale;
    bool oldUseCustomYAxisScale;
    std::pair<double, double> oldXAxisScale, oldYAxisScale; //<min, max>
    std::pair<double, double> initXAxisScale, initYAxisScale;
    Ui::HistoOptionsWidget *_ui;
};
}

#endif // HISTO_OPTIONS_WIDGET_H
