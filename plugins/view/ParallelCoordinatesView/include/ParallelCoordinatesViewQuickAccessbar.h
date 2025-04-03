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

#ifndef PARALLEL_COORDINATES_VIEW_QUICK_ACCESSBAR_H
#define PARALLEL_COORDINATES_VIEW_QUICK_ACCESSBAR_H

#include <talipot/QuickAccessBar.h>

namespace tlp {

class ParallelCoordsDrawConfigWidget;

class ParallelCoordinatesViewQuickAccessBar : public tlp::QuickAccessBarImpl {
    Q_OBJECT

    ParallelCoordsDrawConfigWidget *_optionsWidget;

  public:
    ParallelCoordinatesViewQuickAccessBar(ParallelCoordsDrawConfigWidget *opt,
                                          QWidget *parent = nullptr);

  public slots:
    void reset() override;
    void setNodesVisible(bool) override;
    void setBackgroundColor(const QColor &) override;
    void setLabelsVisible(bool) override;
};
}
#endif // PARALLEL_COORDINATES_VIEW_QUICK_ACCESSBAR_H
