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

#ifndef SCATTER_PLOT2D_OPTIONS_WIDGET_H
#define SCATTER_PLOT2D_OPTIONS_WIDGET_H

#include <QWidget>

#include <talipot/Color.h>
#include <talipot/Size.h>

namespace Ui {
class ScatterPlot2DOptionsWidget;
}

class QPushButton;

namespace tlp {

class ColorButton;

class ScatterPlot2DOptionsWidget : public QWidget {

    Q_OBJECT

  public:
    ScatterPlot2DOptionsWidget(QWidget *parent = nullptr);
    ~ScatterPlot2DOptionsWidget() override;

    void setWidgetEnabled(const bool enabled);
    bool uniformBackground() const;
    Color getBackgroundColor() const;
    Color getMinusOneColor() const;
    Color getZeroColor() const;
    Color getOneColor() const;
    void setBackgroundColor(const Color &color);
    Size getMinSizeMapping() const;
    Size getMaxSizeMapping() const;
    void setMinSizeMapping(const float minSize);
    void setMaxSizeMapping(const float maxSize);

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

    bool displayGraphEdges() const;
    void setDisplayGraphEdges(const bool showEdges);

    bool displayNodeLabels() const;
    void setDisplayNodeLabels(const bool showLabels);

    bool displayScaleLabels() const;
    void setDisplayScaleLabels(const bool scaleLabels);

    bool configurationChanged();

  protected:
    void showEvent(QShowEvent *event) override;

  private slots:
    void setButtonColor(tlp::ColorButton *button, const Color &color);
    void minSizeSpinBoxValueChanged(int);
    void maxSizeSpinBoxValueChanged(int);
    void pressXScaleCheckBox(bool checked);
    void pressYScaleCheckBox(bool checked);
    void updateColorScale();

  private:
    bool oldValuesInitialized;
    bool oldUniformBackground;
    Color oldUniformBackgroundColor;
    Color oldMinusOneColor;
    Color oldZeroColor;
    Color oldOneColor;
    Size oldMinSizeMapping;
    Size oldMaxSizeMapping;
    bool oldDisplayGraphEdges;
    bool oldDisplayNodeLabels;
    bool oldlabelscaled;
    bool oldUseCustomXAxisScale;
    bool oldUseCustomYAxisScale;
    std::pair<double, double> oldXAxisScale, oldYAxisScale; //<min, max>
    std::pair<double, double> initXAxisScale, initYAxisScale;
    Ui::ScatterPlot2DOptionsWidget *_ui;
};
}

#endif // SCATTER_PLOT2D_OPTIONS_WIDGET_H
