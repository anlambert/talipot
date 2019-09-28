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

#include <tulip/Glyph.h>
#include <tulip/Iterator.h>
#include <tulip/TlpQtTools.h>

#include <QComboBox>
#include <QPushButton>

#include "GlyphScaleConfigDialog.h"
#include "ui_GlyphScaleConfigDialog.h"

using namespace std;
using namespace tlp;

GlyphScaleConfigDialog::GlyphScaleConfigDialog(QWidget *parent)
    : QDialog(parent), _ui(new Ui::GlyphScaleConfigDialogData) {
  _ui->setupUi(this);
  _ui->tableWidget->setRowCount(5);
  list<string> pluginsList = PluginLister::availablePlugins<Glyph>();

  for (list<string>::iterator it = pluginsList.begin(); it != pluginsList.end(); ++it) {
    string glyphName = *it;
    glyphsNameList << QString(glyphName.c_str());
  }

  for (int i = 0; i < 5; ++i) {
    QComboBox *glyphNameComboBox = new QComboBox(this);
    glyphNameComboBox->addItems(glyphsNameList);
    glyphNameComboBox->setCurrentIndex(i);
    _ui->tableWidget->setCellWidget(i, 0, glyphNameComboBox);
  }

  connect(_ui->nbGlyphsSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(nbGlyphsSpinBoxValueChanged(int)));
}

GlyphScaleConfigDialog::~GlyphScaleConfigDialog() {
  delete _ui;
}

vector<int> GlyphScaleConfigDialog::getSelectedGlyphsId() const {
  vector<int> ret;

  for (int i = 0; i < _ui->tableWidget->rowCount(); ++i) {
    string glyphName = QStringToTlpString(
        static_cast<QComboBox *>(_ui->tableWidget->cellWidget(i, 0))->currentText());
    ret.push_back(PluginLister::pluginInformation(glyphName).id());
  }

  reverse(ret.begin(), ret.end());
  return ret;
}

void GlyphScaleConfigDialog::nbGlyphsSpinBoxValueChanged(int value) {
  if (value > glyphsNameList.size()) {
    _ui->nbGlyphsSpinBox->setValue(value - 1);
  }

  int lastCount = _ui->tableWidget->rowCount();
  _ui->tableWidget->setRowCount(value);

  if (lastCount < value) {
    QComboBox *glyphNameComboBox = new QComboBox(this);
    glyphNameComboBox->addItems(glyphsNameList);
    _ui->tableWidget->setCellWidget(value - 1, 0, glyphNameComboBox);
  }
}

void GlyphScaleConfigDialog::showEvent(QShowEvent *event) {
  QDialog::showEvent(event);
  _ui->tableWidget->setColumnWidth(0, _ui->tableWidget->width() - 23);
}
