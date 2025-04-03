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

#include "MatrixViewQuickAccessBar.h"
#include "MatrixViewConfigurationWidget.h"

#include <talipot/ColorButton.h>

namespace tlp {

MatrixViewQuickAccessBar::MatrixViewQuickAccessBar(MatrixViewConfigurationWidget *opt,
                                                   QWidget *parent)
    : QuickAccessBarImpl(nullptr,
                         QuickAccessBarImpl::QuickAccessButtons(
                             QuickAccessBarImpl::SCREENSHOT | QuickAccessBarImpl::BACKGROUNDCOLOR |
                             QuickAccessBarImpl::SHOWLABELS |
                             QuickAccessBarImpl::COLORINTERPOLATION |
                             QuickAccessBarImpl::SHOWEDGES | QuickAccessBarImpl::LABELSSCALED |
                             QuickAccessBarImpl::LABELCOLOR),
                         parent),
      _optionsWidget(opt) {}

void MatrixViewQuickAccessBar::setEdgesVisible(bool visible) {
    _optionsWidget->setDisplayEdges(visible);
    showEdgesButton()->setIcon((visible ? QIcon(":/talipot/gui/icons/20/edges_enabled.png")
                                        : QIcon(":/talipot/gui/icons/20/edges_disabled.png")));
    emit settingsChanged();
}

void MatrixViewQuickAccessBar::reset() {
    QuickAccessBarImpl::reset();
    showEdgesButton()->setChecked(_optionsWidget->displayGraphEdges());
    showEdgesButton()->setIcon((_optionsWidget->displayGraphEdges()
                                    ? QIcon(":/talipot/gui/icons/20/edges_enabled.png")
                                    : QIcon(":/talipot/gui/icons/20/edges_disabled.png")));
    backgroundColorButton()->setColor(_optionsWidget->getBackgroundColor());
    showColorInterpolation()->setChecked(_optionsWidget->isEdgeColorInterpolation());
    showColorInterpolation()->setIcon(
        (_optionsWidget->isEdgeColorInterpolation()
             ? QIcon(":/talipot/gui/icons/20/color_interpolation_enabled.png")
             : QIcon(":/talipot/gui/icons/20/color_interpolation_disabled.png")));
}

void MatrixViewQuickAccessBar::setBackgroundColor(const QColor &col) {
    _optionsWidget->setBackgroundColor(col);
    emit settingsChanged();
}

void MatrixViewQuickAccessBar::setColorInterpolation(bool state) {
    _optionsWidget->setEdgeColorInterpolation(state);
    showColorInterpolation()->setIcon(
        (state ? QIcon(":/talipot/gui/icons/20/color_interpolation_enabled.png")
               : QIcon(":/talipot/gui/icons/20/color_interpolation_disabled.png")));
    emit settingsChanged();
}
}
