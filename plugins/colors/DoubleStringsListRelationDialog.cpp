/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "DoubleStringsListRelationDialog.h"
#include "ui_DoubleStringsListRelationDialog.h"

#include <talipot/ColorScale.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include <QScrollBar>

using namespace std;

namespace tlp {

DoubleStringsListRelationDialog::DoubleStringsListRelationDialog(
    const std::vector<std::string> &firstValues, const std::vector<Color> &secondValues,
    QWidget *parent)
    : QDialog(parent), _ui(new Ui::DoubleStringsListRelationDialog),
      lastNonInterpolateValues(secondValues) {
  _ui->setupUi(this);
  _ui->upButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowUpBold));
  _ui->upButtonColor->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowUpBold));
  _ui->downButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowDownBold));
  _ui->downButtonColor->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowDownBold));

  for (const auto &s : firstValues) {
    _ui->firstListWidget->addItem(tlpStringToQString(s));
  }

  for (const auto &c : secondValues) {
    auto *item = new QListWidgetItem;
    item->setBackground(QColor(c[0], c[1], c[2], c[3]));
    _ui->secondListWidget->addItem(item);
  }

  connect(_ui->upButton, &QAbstractButton::clicked, this,
          &DoubleStringsListRelationDialog::upButtonClicked);
  connect(_ui->downButton, &QAbstractButton::clicked, this,
          &DoubleStringsListRelationDialog::downButtonClicked);
  connect(_ui->upButtonColor, &QAbstractButton::clicked, this,
          &DoubleStringsListRelationDialog::upButtonColorClicked);
  connect(_ui->downButtonColor, &QAbstractButton::clicked, this,
          &DoubleStringsListRelationDialog::downButtonColorClicked);
  connect(_ui->firstListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged, this,
          &DoubleStringsListRelationDialog::scrollBarValueChanged);
  connect(_ui->secondListWidget->verticalScrollBar(), &QAbstractSlider::valueChanged, this,
          &DoubleStringsListRelationDialog::scrollBarValueChanged);
  connect(_ui->interpolateColorsCheckBox, QCheckBoxStateChangedSignal, this,
          &DoubleStringsListRelationDialog::interpolateCheckBoxChange);
}

DoubleStringsListRelationDialog::~DoubleStringsListRelationDialog() {
  delete _ui;
}

void DoubleStringsListRelationDialog::getResult(
    std::vector<std::pair<std::string, Color>> &result) {
  for (int i = 0; (i < _ui->firstListWidget->count()) && (i < _ui->secondListWidget->count());
       ++i) {
    QColor color = _ui->secondListWidget->item(i)->background().color();
    result.push_back(pair<string, Color>(QStringToTlpString(_ui->firstListWidget->item(i)->text()),
                                         QColorToColor(color)));
  }
}

void DoubleStringsListRelationDialog::upButtonClicked() {
  int currentRow = _ui->firstListWidget->currentRow();

  if (currentRow == 0) {
    return;
  }

  QListWidgetItem *item = _ui->firstListWidget->takeItem(currentRow);
  _ui->firstListWidget->insertItem(currentRow - 1, item);
  _ui->firstListWidget->setCurrentItem(item);
}

void DoubleStringsListRelationDialog::downButtonClicked() {
  int currentRow = _ui->firstListWidget->currentRow();

  if (currentRow == _ui->firstListWidget->count() + 1) {
    return;
  }

  QListWidgetItem *item = _ui->firstListWidget->takeItem(currentRow);
  _ui->firstListWidget->insertItem(currentRow + 1, item);
  _ui->firstListWidget->setCurrentItem(item);
}

void DoubleStringsListRelationDialog::upButtonColorClicked() {
  int currentRow = _ui->secondListWidget->currentRow();

  if (currentRow == 0) {
    return;
  }

  QListWidgetItem *item = _ui->secondListWidget->takeItem(currentRow);
  _ui->secondListWidget->insertItem(currentRow - 1, item);
  _ui->secondListWidget->setCurrentItem(item);
}

void DoubleStringsListRelationDialog::downButtonColorClicked() {
  int currentRow = _ui->secondListWidget->currentRow();

  if (currentRow == _ui->secondListWidget->count() + 1) {
    return;
  }

  QListWidgetItem *item = _ui->secondListWidget->takeItem(currentRow);
  _ui->secondListWidget->insertItem(currentRow + 1, item);
  _ui->secondListWidget->setCurrentItem(item);
}

void DoubleStringsListRelationDialog::scrollBarValueChanged(int value) {
  if (_ui->firstListWidget->verticalScrollBar()->value() != value) {
    _ui->firstListWidget->verticalScrollBar()->setSliderPosition(value);
  }

  if (_ui->secondListWidget->verticalScrollBar()->value() != value) {
    _ui->secondListWidget->verticalScrollBar()->setSliderPosition(value);
  }
}

void DoubleStringsListRelationDialog::interpolateCheckBoxChange(int state) {
  if (state == 0) {
    // If going back to no interpolated color values
    // then replace the color columns with value in
    // vector lastNonInterpolateValues
    _ui->secondListWidget->clear();
    for (const auto &c : lastNonInterpolateValues) {
      auto *item = new QListWidgetItem;
      item->setBackground(QColor(c[0], c[1], c[2], c[3]));
      _ui->secondListWidget->addItem(item);
    }
  } else {
    // If we choose to interpolate then
    // Save the current color values
    lastNonInterpolateValues.clear();
    for (int i = 0; i < _ui->secondListWidget->count(); ++i) {
      QColor color = _ui->secondListWidget->item(i)->background().color();
      lastNonInterpolateValues.push_back(QColorToColor(color));
    }
    // replace the color columns with interpolated values
    ColorScale tempCS = ColorScale(lastNonInterpolateValues);
    auto nbOfValues = static_cast<float>(_ui->firstListWidget->count());
    _ui->secondListWidget->clear();
    for (float i = 0.; i < nbOfValues; ++i) {
      auto *item = new QListWidgetItem;
      Color ic = tempCS.getColorAtPos(i / (nbOfValues - 1.));
      item->setBackground(QColor(ic[0], ic[1], ic[2], ic[3]));
      _ui->secondListWidget->addItem(item);
    }
  }
}
}
