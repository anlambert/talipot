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

#include "talipot/ColorButton.h"

#include <QColorDialog>
#include <QMainWindow>
#include <QStylePainter>

#include <talipot/TlpQtTools.h>

using namespace tlp;

ChooseColorButton::ChooseColorButton(QWidget *parent) : QPushButton(parent), _color(Qt::black) {
    connect(this, &QAbstractButton::clicked, this, &ChooseColorButton::chooseColor);
    setFocusPolicy(Qt::WheelFocus);
}

QColor ChooseColorButton::color() const {
    return _color;
}

QString ChooseColorButton::text() const {
    return _text;
}

void ChooseColorButton::setDialogTitle(const QString &title) {
    _dialogTitle = title;
}

tlp::Color ChooseColorButton::talipotColor() const {
    return QColorToColor(_color);
}

void ChooseColorButton::setText(const QString &text) {
    _text = text;
}

void ChooseColorButton::setColor(const QColor &c) {
    _color = c;
    update();
    emit colorChanged(_color);
    emit talipotColorChanged(QColorToColor(_color));
}

void ChooseColorButton::setColor(const tlp::Color &c) {
    setColor(colorToQColor(c));
}

void ChooseColorButton::chooseColor() {
    QColor c = QColorDialog::getColor(
        _color, getMainWindow(), _dialogTitle.isEmpty() ? QString("Choose a color") : _dialogTitle,
        // we don't use native dialog to ensure alpha channel can be set
        // it may not be shown when using gnome
        QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

    if (c.isValid()) {
        setColor(c);
    }
}

void ColorButton::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);
    QStylePainter p(this);
    float tickW = width() / 4.;
    float tickH = height() / 4.;
    p.setPen(Qt::black);
    p.setBrush(_color);
    QRectF r(tickW, tickH, tickW * 2, tickH * 2);
    p.drawRect(r);
    p.setPen(QColor(255 ^ _color.red(), 255 ^ _color.green(), 255 ^ _color.blue(), 255));

    if (!_text.isEmpty()) {
        p.drawText(r, Qt::AlignCenter, _text);
    }
}
