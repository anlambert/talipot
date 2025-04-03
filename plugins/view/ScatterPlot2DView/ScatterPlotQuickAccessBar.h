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

#ifndef SCATTER_PLOT_QUICK_ACCESS_BAR_H
#define SCATTER_PLOT_QUICK_ACCESS_BAR_H

#include <talipot/QuickAccessBar.h>

namespace tlp {

class ScatterPlot2DOptionsWidget;

class ScatterPlotQuickAccessBar : public tlp::QuickAccessBarImpl {
    Q_OBJECT

    ScatterPlot2DOptionsWidget *_optionsWidget;

  public:
    ScatterPlotQuickAccessBar(ScatterPlot2DOptionsWidget *opt, QWidget *parent = nullptr);

  public slots:
    void reset() override;
    void setEdgesVisible(bool) override;
    void setBackgroundColor(const QColor &) override;
    void setLabelsVisible(bool) override;
    void setLabelsScaled(bool) override;
};
}
#endif // SCATTER_PLOT_QUICK_ACCESS_BAR_H
