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

#include <talipot/ItemDelegate.h>

#include "ui_VectorEditor.h"

using namespace tlp;

VectorEditor::VectorEditor(QWidget *parent)
    : QDialog(parent), _ui(new Ui::VectorEditor), _userType(0) {
  _ui->setupUi(this);
  _ui->addButton->setIcon(FontIcon::icon(MaterialDesignIcons::Plus));
  _ui->removeButton->setIcon(FontIcon::icon(MaterialDesignIcons::Minus));
  _ui->list->setItemDelegate(new ItemDelegate(_ui->list));
}

VectorEditor::~VectorEditor() {
  delete _ui;
}

void VectorEditor::setVector(const QVector<QVariant> &d, int userType) {
  _userType = userType;
  _ui->list->clear();

  for (const QVariant &v : d) {
    auto *i = new QListWidgetItem();

    if (_userType == qMetaTypeId<std::string>()) {
      i->setData(Qt::DisplayRole, QVariant::fromValue(tlpStringToQString(v.value<std::string>())));
    } else {
      i->setData(Qt::DisplayRole, v);
    }

    i->setFlags(i->flags() | Qt::ItemIsEditable);
    _ui->list->addItem(i);
  }

  _ui->countLabel->setText(QString::number(_ui->list->model()->rowCount()));
  currentVector = d;
}

void VectorEditor::add() {
  auto *i = new QListWidgetItem();

  if (_userType == qMetaTypeId<std::string>()) {
    // workaround to indicate there is a new string to edit
    // because the height of the line if very small with an empty string
    i->setData(Qt::DisplayRole, QVariant::fromValue(QString("edit this string")));
  } else {
    i->setData(Qt::DisplayRole, QVariant(_userType));
  }

  // needed for color
  i->setSizeHint(QSize(i->sizeHint().width(), 15));
  i->setFlags(i->flags() | Qt::ItemIsEditable);
  _ui->list->addItem(i);
  _ui->countLabel->setText(QString::number(_ui->list->model()->rowCount()));
}

void VectorEditor::remove() {
  for (auto *i : _ui->list->selectedItems()) {
    delete i;
  }

  _ui->countLabel->setText(QString::number(_ui->list->model()->rowCount()));
}

void VectorEditor::done(int r) {
  if (r == QDialog::Accepted) {
    QAbstractItemModel *model = _ui->list->model();
    currentVector.clear();

    if (_userType == qMetaTypeId<std::string>()) {
      for (int i = 0; i < model->rowCount(); ++i) {
        currentVector.push_back(
            QVariant::fromValue(QStringToTlpString(model->data(model->index(i, 0)).toString())));
      }
    } else {
      for (int i = 0; i < model->rowCount(); ++i) {
        currentVector.push_back(model->data(model->index(i, 0)));
      }
    }
  }

  QDialog::done(r);
}
