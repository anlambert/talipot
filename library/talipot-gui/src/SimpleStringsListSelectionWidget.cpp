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

#ifndef SIMPLESTRINGSLISTSELECTIONWIDGET_CPP_
#define SIMPLESTRINGSLISTSELECTIONWIDGET_CPP_

#include <talipot/SimpleStringsListSelectionWidget.h>
#include <talipot/TlpQtTools.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include "ui_SimpleStringsListSelectionWidget.h"

using namespace std;

namespace tlp {

SimpleStringsListSelectionWidget::SimpleStringsListSelectionWidget(
    QWidget *parent, const uint maxSelectedStringsListSize)
    : QWidget(parent), _ui(new Ui::SimpleStringsListSelectionWidget()),
      maxSelectedStringsListSize(maxSelectedStringsListSize) {

  _ui->setupUi(this);
  _ui->upButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowUpBold));
  _ui->downButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowDownBold));

  if (maxSelectedStringsListSize != 0) {
    _ui->selectButton->setEnabled(false);
  } else {
    _ui->selectButton->setEnabled(true);
  }

  qtWidgetsConnection();
}

SimpleStringsListSelectionWidget::~SimpleStringsListSelectionWidget() {
  delete _ui;
}

void SimpleStringsListSelectionWidget::qtWidgetsConnection() {
  connect(_ui->listWidget, &QListWidget::itemClicked, this,
          &SimpleStringsListSelectionWidget::listItemClicked);
  connect(_ui->upButton, &QAbstractButton::clicked, this,
          &SimpleStringsListSelectionWidget::pressButtonUp);
  connect(_ui->downButton, &QAbstractButton::clicked, this,
          &SimpleStringsListSelectionWidget::pressButtonDown);
  connect(_ui->selectButton, &QAbstractButton::clicked, this,
          &SimpleStringsListSelectionWidget::pressButtonSelectAll);
  connect(_ui->unselectButton, &QAbstractButton::clicked, this,
          &SimpleStringsListSelectionWidget::pressButtonUnselectAll);
}

void SimpleStringsListSelectionWidget::setUnselectedStringsList(
    const std::vector<std::string> &unselectedStringsList) {
  for (const auto &s : unselectedStringsList) {
    QList<QListWidgetItem *> items =
        _ui->listWidget->findItems(tlpStringToQString(s), Qt::MatchExactly);

    if (items.size() > 0) {
      items[0]->setFlags(items[0]->flags() | Qt::ItemIsUserCheckable);
      items[0]->setCheckState(Qt::Unchecked);
    } else {
      auto *item = new QListWidgetItem(tlpStringToQString(s));
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(Qt::Unchecked);
      _ui->listWidget->addItem(item);
    }
  }
}

void SimpleStringsListSelectionWidget::setSelectedStringsList(
    const std::vector<std::string> &selectedStringsList) {
  for (const auto &s : selectedStringsList) {
    if (maxSelectedStringsListSize != 0 &&
        getSelectedStringsList().size() == maxSelectedStringsListSize) {
      break;
    }

    QList<QListWidgetItem *> items =
        _ui->listWidget->findItems(tlpStringToQString(s), Qt::MatchExactly);

    if (items.size() > 0) {
      items[0]->setFlags(items[0]->flags() | Qt::ItemIsUserCheckable);
      items[0]->setCheckState(Qt::Checked);
    } else {
      auto *item = new QListWidgetItem(tlpStringToQString(s));
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(Qt::Checked);
      _ui->listWidget->addItem(item);
    }
  }
}

void SimpleStringsListSelectionWidget::clearUnselectedStringsList() {
  vector<QListWidgetItem *> itemsToDelete;

  for (int i = 0; i < _ui->listWidget->count(); ++i) {
    QListWidgetItem *item = _ui->listWidget->item(i);

    if (item->checkState() == Qt::Unchecked) {
      itemsToDelete.push_back(item);
    }
  }

  for (auto *item : itemsToDelete) {
    delete item;
  }
}

void SimpleStringsListSelectionWidget::clearSelectedStringsList() {
  vector<QListWidgetItem *> itemsToDelete;

  for (int i = 0; i < _ui->listWidget->count(); ++i) {
    QListWidgetItem *item = _ui->listWidget->item(i);

    if (item->checkState() == Qt::Checked) {
      itemsToDelete.push_back(item);
    }
  }

  for (auto *i : itemsToDelete) {
    delete i;
  }
}

void SimpleStringsListSelectionWidget::setMaxSelectedStringsListSize(
    const uint maxSelectedStringsListSize) {
  this->maxSelectedStringsListSize = maxSelectedStringsListSize;

  if (maxSelectedStringsListSize != 0) {
    _ui->selectButton->setEnabled(false);
  } else {
    _ui->selectButton->setEnabled(true);
  }
}

vector<string> SimpleStringsListSelectionWidget::getSelectedStringsList() const {
  vector<string> ret;

  for (int i = 0; i < _ui->listWidget->count(); ++i) {
    QListWidgetItem *item = _ui->listWidget->item(i);

    if (item->checkState() == Qt::Checked) {
      ret.push_back(QStringToTlpString(item->text()));
    }
  }

  return ret;
}

vector<string> SimpleStringsListSelectionWidget::getUnselectedStringsList() const {
  vector<string> ret;

  for (int i = 0; i < _ui->listWidget->count(); ++i) {
    QListWidgetItem *item = _ui->listWidget->item(i);

    if (item->checkState() == Qt::Unchecked) {
      ret.push_back(QStringToTlpString(item->text()));
    }
  }

  return ret;
}

void SimpleStringsListSelectionWidget::selectAllStrings() {
  for (int i = 0; i < _ui->listWidget->count(); ++i) {
    QListWidgetItem *item = _ui->listWidget->item(i);
    item->setCheckState(Qt::Checked);
  }
}

void SimpleStringsListSelectionWidget::unselectAllStrings() {
  for (int i = 0; i < _ui->listWidget->count(); ++i) {
    QListWidgetItem *item = _ui->listWidget->item(i);
    item->setCheckState(Qt::Unchecked);
  }
}

void SimpleStringsListSelectionWidget::pressButtonUp() {
  if (_ui->listWidget->count() > 0) {
    int row = _ui->listWidget->currentRow();

    if (row > 0) {
      auto *item1 = new QListWidgetItem(*(_ui->listWidget->currentItem()));
      auto *item2 = new QListWidgetItem(*(_ui->listWidget->item(row - 1)));
      delete _ui->listWidget->item(row - 1);
      delete _ui->listWidget->item(row - 1);
      _ui->listWidget->insertItem(row - 1, item2);
      _ui->listWidget->insertItem(row - 1, item1);
      _ui->listWidget->setCurrentRow(row - 1);
    }
  }
}

void SimpleStringsListSelectionWidget::pressButtonDown() {
  if (_ui->listWidget->count() > 0) {
    int row = _ui->listWidget->currentRow();

    if (row != -1 && row < (_ui->listWidget->count() - 1)) {
      auto *item1 = new QListWidgetItem(*(_ui->listWidget->currentItem()));
      auto *item2 = new QListWidgetItem(*(_ui->listWidget->item(row + 1)));
      delete _ui->listWidget->item(row);
      delete _ui->listWidget->item(row);
      _ui->listWidget->insertItem(row, item1);
      _ui->listWidget->insertItem(row, item2);
      _ui->listWidget->setCurrentRow(row + 1);
    }
  }
}

void SimpleStringsListSelectionWidget::pressButtonSelectAll() {
  selectAllStrings();
}

void SimpleStringsListSelectionWidget::pressButtonUnselectAll() {
  unselectAllStrings();
}

void SimpleStringsListSelectionWidget::listItemClicked(QListWidgetItem *item) const {
  if (maxSelectedStringsListSize != 0 &&
      getSelectedStringsList().size() > maxSelectedStringsListSize) {
    if (item->checkState() == Qt::Checked) {
      item->setCheckState(Qt::Unchecked);
    }
  }
}
}

#endif /* SIMPLESTRINGSLISTSELECTIONWIDGET_CPP_ */
