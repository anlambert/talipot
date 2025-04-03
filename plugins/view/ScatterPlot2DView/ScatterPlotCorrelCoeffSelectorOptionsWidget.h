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

#ifndef SCATTER_PLOT_CORREL_COEFF_SELECTOR_OPTIONS_WIDGET_H
#define SCATTER_PLOT_CORREL_COEFF_SELECTOR_OPTIONS_WIDGET_H

#include <QWidget>

#include <talipot/Color.h>

class QPushButton;

namespace Ui {
class ScatterPlotCorrelCoeffSelectorOptionsWidget;
}

namespace tlp {

class ColorButton;
class ScatterPlotCorrelCoeffSelectorOptionsWidget : public QWidget {

    Q_OBJECT

  public:
    ScatterPlotCorrelCoeffSelectorOptionsWidget(QWidget *parent = nullptr);
    ~ScatterPlotCorrelCoeffSelectorOptionsWidget() override;

    Color getMinusOneColor() const;
    Color getZeroColor() const;
    Color getOneColor() const;

  protected:
    void showEvent(QShowEvent *event) override;

  private slots:
    void updateColorScale();

  private:
    void setButtonColor(tlp::ColorButton *button, const Color &color);
    Ui::ScatterPlotCorrelCoeffSelectorOptionsWidget *_ui;
};
}

#endif // SCATTER_PLOT_CORREL_COEFF_SELECTOR_OPTIONS_WIDGET_H
