/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef HISTOGRAM_VIEW_NAVIGATOR_H
#define HISTOGRAM_VIEW_NAVIGATOR_H

#include <talipot/GLInteractor.h>

namespace tlp {

class Coord;
class Histogram;
class HistogramView;

class HistogramViewNavigator : public GLInteractorComponent {

public:
  HistogramViewNavigator();
  ~HistogramViewNavigator() override;

  bool eventFilter(QObject *, QEvent *) override;
  bool draw(GlWidget *) override {
    return false;
  }
  bool compute(GlWidget *) override {
    return false;
  }
  void viewChanged(View *view) override;

private:
  Histogram *getOverviewUnderPointer(const Coord &sceneCoord) const;

  HistogramView *histoView;
  Histogram *selectedHistoOverview;
};
}

#endif // HISTOGRAM_VIEW_NAVIGATOR_H
