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

#ifndef SCATTER_PLOT_TREND_LINE_H
#define SCATTER_PLOT_TREND_LINE_H

#include <talipot/GLInteractor.h>

namespace tlp {

class ScatterPlot2DView;

class ScatterPlotTrendLine : public GLInteractorComponent {

  public:
    ScatterPlotTrendLine();
    ~ScatterPlotTrendLine() override;

    bool eventFilter(QObject *, QEvent *) override;
    bool draw(GlWidget *glWidget) override;
    bool compute(GlWidget *glWidget) override;
    void viewChanged(View *view) override;

  private:
    ScatterPlot2DView *scatterView;
    float a, b;
};
}

#endif // SCATTER_PLOT_TREND_LINE_H
