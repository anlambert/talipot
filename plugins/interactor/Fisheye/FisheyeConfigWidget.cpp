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

#include "FisheyeConfigWidget.h"

#include "ui_FisheyeConfigWidget.h"

using namespace tlp;

FisheyeConfigWidget::FisheyeConfigWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::FisheyeConfigWidget) {
    _ui->setupUi(this);
}

FisheyeConfigWidget::~FisheyeConfigWidget() {
    delete _ui;
}

float FisheyeConfigWidget::getFisheyeRadius() const {
    return float(_ui->radiusValSP->value());
}

void FisheyeConfigWidget::setFisheyeRadius(const float radius) {
    _ui->radiusValSP->setValue(radius);
}

float FisheyeConfigWidget::getFisheyeHeight() const {
    return float(_ui->heightValSP->value());
}

void FisheyeConfigWidget::setFisheyeHeight(const float height) {
    _ui->heightValSP->setValue(height);
}

float FisheyeConfigWidget::getFisheyeRadiusIncrementStep() const {
    return float(_ui->radiusIncrementStepSP->value());
}

float FisheyeConfigWidget::getFisheyeHeightIncrementStep() const {
    return float(_ui->heightIncrementStepSP->value());
}
