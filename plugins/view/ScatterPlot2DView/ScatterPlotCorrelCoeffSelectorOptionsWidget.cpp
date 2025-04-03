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

#include <QPainter>

#include "ScatterPlotCorrelCoeffSelectorOptionsWidget.h"
#include "ui_ScatterPlotCorrelCoeffSelectorOptionsWidget.h"

namespace tlp {

ScatterPlotCorrelCoeffSelectorOptionsWidget::ScatterPlotCorrelCoeffSelectorOptionsWidget(
    QWidget *parent)
    : QWidget(parent), _ui(new Ui::ScatterPlotCorrelCoeffSelectorOptionsWidget) {
    _ui->setupUi(this);
    _ui->minusOneColorButton->setDialogTitle("Choose the color for -1");
    _ui->zeroColorButton->setDialogTitle("Choose the color for 0");
    _ui->oneColorButton->setDialogTitle("Choose the color for 1");
    setButtonColor(_ui->minusOneColorButton, Color(0, 0, 255, 150));
    setButtonColor(_ui->zeroColorButton, Color(255, 0, 0, 150));
    setButtonColor(_ui->oneColorButton, Color(0, 255, 0, 150));
    updateColorScale();

    connect(_ui->minusOneColorButton, &QAbstractButton::clicked, this,
            &ScatterPlotCorrelCoeffSelectorOptionsWidget::updateColorScale);
    connect(_ui->zeroColorButton, &QAbstractButton::clicked, this,
            &ScatterPlotCorrelCoeffSelectorOptionsWidget::updateColorScale);
    connect(_ui->oneColorButton, &QAbstractButton::clicked, this,
            &ScatterPlotCorrelCoeffSelectorOptionsWidget::updateColorScale);
}

ScatterPlotCorrelCoeffSelectorOptionsWidget::~ScatterPlotCorrelCoeffSelectorOptionsWidget() {
    delete _ui;
}

void ScatterPlotCorrelCoeffSelectorOptionsWidget::setButtonColor(ColorButton *button,
                                                                 const Color &color) {
    button->setColor(color);
}

Color ScatterPlotCorrelCoeffSelectorOptionsWidget::getMinusOneColor() const {
    return _ui->minusOneColorButton->talipotColor();
}

Color ScatterPlotCorrelCoeffSelectorOptionsWidget::getZeroColor() const {
    return _ui->zeroColorButton->talipotColor();
}

Color ScatterPlotCorrelCoeffSelectorOptionsWidget::getOneColor() const {
    return _ui->oneColorButton->talipotColor();
}

void ScatterPlotCorrelCoeffSelectorOptionsWidget::updateColorScale() {
    QPixmap pixmap(_ui->colorScaleLabel->width(), _ui->colorScaleLabel->height());
    pixmap.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pixmap);
    QLinearGradient qLinearGradient(0, _ui->colorScaleLabel->height() / 2,
                                    _ui->colorScaleLabel->width() - 1,
                                    _ui->colorScaleLabel->height() / 2);
    qLinearGradient.setColorAt(0, _ui->minusOneColorButton->color());
    qLinearGradient.setColorAt(1. / 2., _ui->zeroColorButton->color());
    qLinearGradient.setColorAt(1, _ui->oneColorButton->color());
    painter.fillRect(0, 0, _ui->colorScaleLabel->width(), _ui->colorScaleLabel->height(),
                     qLinearGradient);
    painter.end();
    _ui->colorScaleLabel->setPixmap(pixmap);
}

void ScatterPlotCorrelCoeffSelectorOptionsWidget::showEvent(QShowEvent *) {
    updateColorScale();
}
}
