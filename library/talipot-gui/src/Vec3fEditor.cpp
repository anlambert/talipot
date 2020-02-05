/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Vec3fEditor.h>

#include "ui_Vec3fEditor.h"

#include <cfloat>

#include <QDoubleValidator>

using namespace tlp;

Vec3fEditor::Vec3fEditor(QWidget *parent, bool editSize)
    : QDialog(parent), ui(new Ui::Vec3fEditor) {
  ui->setupUi(this);

  if (editSize) {
    setWindowTitle("Edit size");
    ui->xLabel->setText("W");
    ui->yLabel->setText("H");
    ui->zLabel->setText("D");
  }

  ui->xSP->setRange(-FLT_MAX, FLT_MAX);
  ui->ySP->setRange(-FLT_MAX, FLT_MAX);
  ui->zSP->setRange(-FLT_MAX, FLT_MAX);
  setVec3f(Vec3f());
  connect(ui->xSP, SIGNAL(valueChanged(double)), this, SLOT(vecUpdated()));
  connect(ui->ySP, SIGNAL(valueChanged(double)), this, SLOT(vecUpdated()));
  connect(ui->zSP, SIGNAL(valueChanged(double)), this, SLOT(vecUpdated()));
  setModal(true);
}

Vec3fEditor::~Vec3fEditor() {
  delete ui;
}

Vec3f Vec3fEditor::vec3f() const {
  return currentVec;
}
void Vec3fEditor::setVec3f(const Vec3f &vec) {
  currentVec = vec;
  blockSignals(true);
  ui->xSP->setValue(vec[0]);
  ui->ySP->setValue(vec[1]);
  ui->zSP->setValue(vec[2]);
  blockSignals(false);
  vecUpdated();
}

void Vec3fEditor::vecUpdated() {
  currentVec = Vec3f(ui->xSP->value(), ui->ySP->value(), ui->zSP->value());
  emit(vecChanged(vec3f()));
}

void Vec3fEditor::done(int r) {
  if (r == QDialog::Accepted) {
    currentVec = Vec3f(ui->xSP->value(), ui->ySP->value(), ui->zSP->value());
  }

  QDialog::done(r);
}

// to ensure it is shown in the center of its parent
void Vec3fEditor::showEvent(QShowEvent *ev) {
  QDialog::showEvent(ev);

  if (parentWidget())
    move(parentWidget()->window()->frameGeometry().topLeft() +
         parentWidget()->window()->rect().center() - rect().center());
}