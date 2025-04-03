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

#include "talipot/ColorScaleButton.h"

#include <QPainter>
#include <QPaintEvent>

#include <talipot/TlpQtTools.h>
#include <talipot/ColorScaleConfigDialog.h>

using namespace tlp;

void ColorScaleButton::paintScale(QPainter *painter, const QRect &baseRect,
                                  const ColorScale &colorScale) {
    QRect rect = baseRect;
    rect.setX(rect.x() + 2);
    rect.setY(rect.y() + 2);
    rect.setWidth(rect.width() - 4);
    rect.setHeight(rect.height() - 4);

    QLinearGradient grad(QPointF(rect.x(), rect.y()), QPointF(rect.x() + rect.width(), rect.y()));

    for (const auto &it : colorScale.getColorMap()) {
        grad.setColorAt(it.first, colorToQColor(it.second));
    }

    painter->setBrush(QBrush(grad));

    painter->drawRect(rect);
}

ColorScaleButton::ColorScaleButton(ColorScale colorScale, QWidget *parent)
    : QPushButton(parent), _colorScale(colorScale), dlg(nullptr) {
    connect(this, &QPushButton::clicked, [this] { editColorScale(); });
}

const ColorScale &ColorScaleButton::colorScale() const {
    return _colorScale;
}

void ColorScaleButton::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);
    QPainter painter(this);
    paintScale(&painter, event->rect(), colorScale());
}

void ColorScaleButton::editColorScale(const ColorScale &cs) {
    if (!dlg) {
        dlg = new ColorScaleConfigDialog(cs, this);
    }

    dlg->setColorScale(cs);

    if (dlg->exec() == QDialog::Accepted) {
        _colorScale = dlg->getColorScale();
    } else if (&cs != &_colorScale) {
        _colorScale = cs;
    }
}

void ColorScaleButton::editColorScale() {
    editColorScale(_colorScale);
}
