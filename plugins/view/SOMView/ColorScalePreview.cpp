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

#include "ColorScalePreview.h"

#include <talipot/ColorScaleConfigDialog.h>

using namespace tlp;

ColorScalePreview::ColorScalePreview(ColorScale *colorScale) : currentColorScale(colorScale) {
    setAutoFillBackground(true);
    fillLabel();
}

ColorScalePreview::~ColorScalePreview() {
    // TODO Auto-generated destructor stub
}

void ColorScalePreview::fillLabel() {

    if (currentColorScale != nullptr && width() != 0 && height() != 0) {

        QPalette p = this->palette();
        QLinearGradient grad(0, height() * 0.5, width(), height() * 0.5);

        for (const auto &it : currentColorScale->getColorMap()) {
            tlp::Color col = it.second;
            QColor c(col.getR(), col.getG(), col.getB(), col.getA());
            grad.setColorAt(it.first, c);
        }

        p.setBrush(QPalette::Window, QBrush(grad));
        this->setPalette(p);
    }
}

void ColorScalePreview::setColorScale(ColorScale *colorScale) {
    currentColorScale = colorScale;
    fillLabel();
}

void ColorScalePreview::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    fillLabel();
}

void ColorScalePreview::mousePressEvent(QMouseEvent *) {
    tlp::ColorScaleConfigDialog dialog(*currentColorScale, this);
    dialog.exec();
    fillLabel();
}
