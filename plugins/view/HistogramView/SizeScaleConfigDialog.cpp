/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "SizeScaleConfigDialog.h"
#include "ui_SizeScaleConfigDialog.h"

namespace tlp {

SizeScaleConfigDialog::SizeScaleConfigDialog(QWidget *d)
    : QDialog(d), _ui(new Ui::SizeScaleConfigDialogData) {
  _ui->setupUi(this);
  connect(_ui->minSizeSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(minSizeValueChanged(double)));
  connect(_ui->maxSizeSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(maxSizeValueChanged(double)));
  connect(_ui->viewSizeRB, SIGNAL(toggled(bool)), this, SLOT(viewSizeRadioButtonToggled(bool)));
}

SizeScaleConfigDialog::~SizeScaleConfigDialog() {
  delete _ui;
}

float SizeScaleConfigDialog::getMinSize() const {
  return float(_ui->minSizeSpinBox->value());
}

float SizeScaleConfigDialog::getMaxSize() const {
  return float(_ui->maxSizeSpinBox->value());
}

bool SizeScaleConfigDialog::applySizeMappingOnX() const {
  return _ui->xCB->isChecked();
}

bool SizeScaleConfigDialog::applySizeMappingOnY() const {
  return _ui->yCB->isChecked();
}

bool SizeScaleConfigDialog::applySizeMappingOnZ() const {
  return _ui->zCB->isChecked();
}

void SizeScaleConfigDialog::minSizeValueChanged(double newValue) {
  if (newValue > _ui->maxSizeSpinBox->value()) {
    _ui->maxSizeSpinBox->setValue(newValue + 1);
  }
}

void SizeScaleConfigDialog::maxSizeValueChanged(double newValue) {
  if (newValue < _ui->minSizeSpinBox->value()) {
    _ui->minSizeSpinBox->setValue(newValue - 1);
  }
}

bool SizeScaleConfigDialog::doMappingOnViewSize() const {
  return _ui->viewSizeRB->isChecked();
}

void SizeScaleConfigDialog::viewSizeRadioButtonToggled(bool checked) {
  _ui->dimensionsMappingGB->setEnabled(checked);
}
}
