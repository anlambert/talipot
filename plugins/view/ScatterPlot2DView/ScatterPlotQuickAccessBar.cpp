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

#include "ScatterPlotQuickAccessBar.h"
#include "ScatterPlot2DOptionsWidget.h"

#include <talipot/TlpQtTools.h>
#include <talipot/ColorButton.h>

namespace tlp {

ScatterPlotQuickAccessBar::ScatterPlotQuickAccessBar(ScatterPlot2DOptionsWidget *opt,
                                                     QWidget *parent)
    : QuickAccessBarImpl(nullptr,
                         QuickAccessBarImpl::QuickAccessButtons(
                             QuickAccessBarImpl::SCREENSHOT | QuickAccessBarImpl::BACKGROUNDCOLOR |
                             QuickAccessBarImpl::SHOWLABELS |
                             QuickAccessBarImpl::COLORINTERPOLATION |
                             QuickAccessBarImpl::SHOWEDGES | QuickAccessBarImpl::LABELSSCALED |
                             QuickAccessBarImpl::NODECOLOR | QuickAccessBarImpl::EDGECOLOR |
                             QuickAccessBarImpl::NODEBORDERCOLOR | QuickAccessBarImpl::LABELCOLOR |
                             QuickAccessBarImpl::NODESIZE),
                         parent),
      _optionsWidget(opt) {}

void ScatterPlotQuickAccessBar::setEdgesVisible(bool visible) {
    _optionsWidget->setDisplayGraphEdges(visible);
    showEdgesButton()->setIcon((visible ? QIcon(":/talipot/gui/icons/20/edges_enabled.png")
                                        : QIcon(":/talipot/gui/icons/20/edges_disabled.png")));
    emit settingsChanged();
}

void ScatterPlotQuickAccessBar::reset() {
    QuickAccessBarImpl::reset();
    showEdgesButton()->setChecked(_optionsWidget->displayGraphEdges());
    showEdgesButton()->setIcon((_optionsWidget->displayGraphEdges()
                                    ? QIcon(":/talipot/gui/icons/20/edges_enabled.png")
                                    : QIcon(":/talipot/gui/icons/20/edges_disabled.png")));
    showLabelsButton()->setChecked(_optionsWidget->displayNodeLabels());
    showLabelsButton()->setIcon((_optionsWidget->displayNodeLabels()
                                     ? QIcon(":/talipot/gui/icons/20/labels_enabled.png")
                                     : QIcon(":/talipot/gui/icons/20/labels_disabled.png")));
    backgroundColorButton()->setColor(_optionsWidget->getBackgroundColor());
    showLabelScaled()->setChecked(_optionsWidget->displayScaleLabels());
    showLabelScaled()->setIcon((_optionsWidget->displayScaleLabels()
                                    ? QIcon(":/talipot/gui/icons/20/labels_scaled_enabled.png")
                                    : QIcon(":/talipot/gui/icons/20/labels_scaled_disabled.png")));
}

void ScatterPlotQuickAccessBar::setBackgroundColor(const QColor &col) {
    _optionsWidget->setBackgroundColor(tlp::QColorToColor(col));
    emit settingsChanged();
}

void ScatterPlotQuickAccessBar::setLabelsVisible(bool visible) {
    _optionsWidget->setDisplayNodeLabels(visible);
    showLabelsButton()->setIcon((visible ? QIcon(":/talipot/gui/icons/20/labels_enabled.png")
                                         : QIcon(":/talipot/gui/icons/20/labels_disabled.png")));
    emit settingsChanged();
}

void ScatterPlotQuickAccessBar::setLabelsScaled(bool scaled) {
    _optionsWidget->setDisplayScaleLabels(scaled);
    showLabelScaled()->setIcon((scaled
                                    ? QIcon(":/talipot/gui/icons/20/labels_scaled_enabled.png")
                                    : QIcon(":/talipot/gui/icons/20/labels_scaled_disabled.png")));
    emit settingsChanged();
}
}
