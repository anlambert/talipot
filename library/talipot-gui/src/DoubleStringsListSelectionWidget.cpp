/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/DoubleStringsListSelectionWidget.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include "ui_DoubleStringsListSelectionWidget.h"

using namespace std;

namespace tlp {

DoubleStringsListSelectionWidget::DoubleStringsListSelectionWidget(
    QWidget *parent, const uint maxSelectedStringsListSize)
    : QWidget(parent), _ui(new Ui::DoubleStringsListSelectionWidget()) {
  _ui->setupUi(this);
  _ui->upButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowUpBold));
  _ui->downButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowDownBold));
  _ui->addButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowRightBold));
  _ui->removeButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowLeftBold));
  _ui->outputList->setMaxListSize(maxSelectedStringsListSize);
  _ui->selectButton->setEnabled(maxSelectedStringsListSize == 0);

  qtWidgetsConnection();
}

DoubleStringsListSelectionWidget::~DoubleStringsListSelectionWidget() {
  delete _ui;
}

void DoubleStringsListSelectionWidget::setUnselectedStringsList(
    const std::vector<std::string> &unselectedStringsList) {
  for (const auto &s : unselectedStringsList) {
    _ui->inputList->addItemList(tlpStringToQString(s));
  }
}

void DoubleStringsListSelectionWidget::setSelectedStringsList(
    const std::vector<std::string> &selectedStringsList) {
  for (const auto &s : selectedStringsList) {
    _ui->outputList->addItemList(tlpStringToQString(s));
  }
}

void DoubleStringsListSelectionWidget::clearUnselectedStringsList() {
  _ui->inputList->clear();
}

void DoubleStringsListSelectionWidget::clearSelectedStringsList() {
  _ui->outputList->clear();
}

void DoubleStringsListSelectionWidget::setUnselectedStringsListLabel(
    const std::string &unselectedStringsListLabel) {
  _ui->inputListLabel->setText(tlpStringToQString(unselectedStringsListLabel));
}

void DoubleStringsListSelectionWidget::setSelectedStringsListLabel(
    const std::string &selectedStringsListLabel) {
  _ui->outputListLabel->setText(tlpStringToQString(selectedStringsListLabel));
}

void DoubleStringsListSelectionWidget::setMaxSelectedStringsListSize(
    const uint maxSelectedStringsListSize) {
  _ui->outputList->setMaxListSize(maxSelectedStringsListSize);

  if (maxSelectedStringsListSize != 0) {
    _ui->selectButton->setEnabled(false);
  } else {
    _ui->selectButton->setEnabled(true);
  }
}

vector<string> DoubleStringsListSelectionWidget::getSelectedStringsList() const {
  vector<string> outputStringList;

  for (int i = 0; i < _ui->outputList->count(); ++i) {
    outputStringList.push_back(tlp::QStringToTlpString(_ui->outputList->item(i)->text()));
  }

  return outputStringList;
}

vector<string> DoubleStringsListSelectionWidget::getUnselectedStringsList() const {
  vector<string> inputStringList;

  for (int i = 0; i < _ui->inputList->count(); ++i) {
    inputStringList.push_back(tlp::QStringToTlpString(_ui->inputList->item(i)->text()));
  }

  return inputStringList;
}

void DoubleStringsListSelectionWidget::selectAllStrings() {
  if (_ui->outputList->getMaxListSize() == 0) {
    for (int i = 0; i < _ui->inputList->count(); ++i) {
      _ui->outputList->addItem(new QListWidgetItem(*(_ui->inputList->item(i))));
    }

    clearUnselectedStringsList();
  }
}

void DoubleStringsListSelectionWidget::unselectAllStrings() {
  for (int i = 0; i < _ui->outputList->count(); ++i) {
    _ui->inputList->addItem(new QListWidgetItem(*(_ui->outputList->item(i))));
  }

  clearSelectedStringsList();
}

void DoubleStringsListSelectionWidget::qtWidgetsConnection() {
  connect(_ui->addButton, &QAbstractButton::clicked, this,
          &DoubleStringsListSelectionWidget::pressButtonAdd);
  connect(_ui->removeButton, &QAbstractButton::clicked, this,
          &DoubleStringsListSelectionWidget::pressButtonRem);
  connect(_ui->upButton, &QAbstractButton::clicked, this,
          &DoubleStringsListSelectionWidget::pressButtonUp);
  connect(_ui->downButton, &QAbstractButton::clicked, this,
          &DoubleStringsListSelectionWidget::pressButtonDown);
  connect(_ui->selectButton, &QAbstractButton::clicked, this,
          &DoubleStringsListSelectionWidget::pressButtonSelectAll);
  connect(_ui->unselectButton, &QAbstractButton::clicked, this,
          &DoubleStringsListSelectionWidget::pressButtonUnselectAll);
}

void DoubleStringsListSelectionWidget::pressButtonAdd() {
  if (_ui->inputList->currentItem() != nullptr) {
    if (_ui->outputList->addItemList(_ui->inputList->currentItem()->text())) {
      _ui->inputList->deleteItemList(_ui->inputList->currentItem());
    }
  }
}

void DoubleStringsListSelectionWidget::pressButtonRem() {
  if (_ui->outputList->currentItem() != nullptr) {
    _ui->inputList->addItemList(_ui->outputList->currentItem()->text());
    _ui->outputList->deleteItemList(_ui->outputList->currentItem());
  }
}

void DoubleStringsListSelectionWidget::pressButtonUp() {
  if (_ui->outputList->count() > 0) {
    int row = _ui->outputList->currentRow();

    if (row > 0) {
      QString s = _ui->outputList->currentItem()->text();
      QString s2 = _ui->outputList->item(row - 1)->text();
      _ui->outputList->deleteItemList(_ui->outputList->item(row - 1));
      _ui->outputList->deleteItemList(_ui->outputList->item(row - 1));
      _ui->outputList->insertItem(row - 1, s2);
      _ui->outputList->insertItem(row - 1, s);
      _ui->outputList->setCurrentRow(row - 1);
    }
  }
}

void DoubleStringsListSelectionWidget::pressButtonDown() {
  if (_ui->outputList->count() > 0) {
    int row = _ui->outputList->currentRow();

    if (row != -1 && row < (_ui->outputList->count() - 1)) {
      QString s = _ui->outputList->currentItem()->text();
      QString s2 = _ui->outputList->item(row + 1)->text();
      _ui->outputList->deleteItemList(_ui->outputList->item(row));
      _ui->outputList->deleteItemList(_ui->outputList->item(row));
      _ui->outputList->insertItem(row, s);
      _ui->outputList->insertItem(row, s2);
      _ui->outputList->setCurrentRow(row + 1);
    }
  }
}

void DoubleStringsListSelectionWidget::pressButtonSelectAll() {
  selectAllStrings();
}

void DoubleStringsListSelectionWidget::pressButtonUnselectAll() {
  unselectAllStrings();
}
}
