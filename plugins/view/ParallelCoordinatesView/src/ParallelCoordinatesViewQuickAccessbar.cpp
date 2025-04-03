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

#include "ParallelCoordinatesViewQuickAccessbar.h"
#include "ParallelCoordsDrawConfigWidget.h"

#include <talipot/TlpQtTools.h>
#include <talipot/ColorButton.h>

namespace tlp {

ParallelCoordinatesViewQuickAccessBar::ParallelCoordinatesViewQuickAccessBar(
    ParallelCoordsDrawConfigWidget *opt, QWidget *parent)
    : QuickAccessBarImpl(nullptr,
                         QuickAccessBarImpl::QuickAccessButtons(
                             QuickAccessBarImpl::SCREENSHOT | QuickAccessBarImpl::BACKGROUNDCOLOR |
                             QuickAccessBarImpl::SHOWLABELS | QuickAccessBarImpl::SHOWNODES |
                             QuickAccessBarImpl::LABELSSCALED | QuickAccessBarImpl::NODECOLOR |
                             QuickAccessBarImpl::EDGECOLOR | QuickAccessBarImpl::NODEBORDERCOLOR |
                             QuickAccessBarImpl::LABELCOLOR),
                         parent),
      _optionsWidget(opt) {}

void ParallelCoordinatesViewQuickAccessBar::setNodesVisible(bool visible) {
    _optionsWidget->setDrawPointOnAxis(visible);
    showNodesButton()->setIcon((visible ? QIcon(":/talipot/gui/icons/20/nodes_enabled.png")
                                        : QIcon(":/talipot/gui/icons/20/nodes_disabled.png")));
    emit settingsChanged();
}

void ParallelCoordinatesViewQuickAccessBar::reset() {
    QuickAccessBarImpl::reset();
    showNodesButton()->setChecked(_optionsWidget->drawPointOnAxis());
    showNodesButton()->setIcon((_optionsWidget->drawPointOnAxis()
                                    ? QIcon(":/talipot/gui/icons/20/nodes_enabled.png")
                                    : QIcon(":/talipot/gui/icons/20/nodes_disabled.png")));
    showLabelsButton()->setChecked(_optionsWidget->displayNodeLabels());
    showLabelsButton()->setIcon((_optionsWidget->displayNodeLabels()
                                     ? QIcon(":/talipot/gui/icons/20/labels_enabled.png")
                                     : QIcon(":/talipot/gui/icons/20/labels_disabled.png")));
    backgroundColorButton()->setColor(_optionsWidget->getBackgroundColor());
}

void ParallelCoordinatesViewQuickAccessBar::setBackgroundColor(const QColor &col) {
    _optionsWidget->setBackgroundColor(tlp::QColorToColor(col));
    emit settingsChanged();
}

void ParallelCoordinatesViewQuickAccessBar::setLabelsVisible(bool visible) {
    _optionsWidget->setDisplayNodeLabels(visible);
    showLabelsButton()->setIcon((visible ? QIcon(":/talipot/gui/icons/20/labels_enabled.png")
                                         : QIcon(":/talipot/gui/icons/20/labels_disabled.png")));
    emit settingsChanged();
}
}
