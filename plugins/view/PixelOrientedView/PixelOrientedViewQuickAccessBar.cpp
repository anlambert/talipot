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

#include "PixelOrientedViewQuickAccessBar.h"
#include "PixelOrientedOptionsWidget.h"

#include <talipot/TlpQtTools.h>
#include <talipot/ColorButton.h>

namespace tlp {

PixelOrientedViewQuickAccessBar::PixelOrientedViewQuickAccessBar(PixelOrientedOptionsWidget *opt,
                                                                 QWidget *parent)
    : QuickAccessBarImpl(nullptr,
                         QuickAccessBarImpl::QuickAccessButtons(
                             QuickAccessBarImpl::SCREENSHOT | QuickAccessBarImpl::BACKGROUNDCOLOR |
                             QuickAccessBarImpl::SHOWLABELS | QuickAccessBarImpl::NODEBORDERCOLOR |
                             QuickAccessBarImpl::NODECOLOR | QuickAccessBarImpl::LABELSSCALED |
                             QuickAccessBarImpl::LABELCOLOR | QuickAccessBarImpl::NODESHAPE),
                         parent),
      _optionsWidget(opt) {}

void PixelOrientedViewQuickAccessBar::reset() {
    QuickAccessBarImpl::reset();
    backgroundColorButton()->setColor(_optionsWidget->getBackgroundColor());
}

void PixelOrientedViewQuickAccessBar::setBackgroundColor(const QColor &col) {
    _optionsWidget->setBackgroundColor(tlp::QColorToColor(col));
    emit settingsChanged();
}
}
