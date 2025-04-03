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

#include "AddressSelectionDialog.h"
#include "ui_AddressSelectionDialog.h"

namespace tlp {

AddressSelectionDialog::AddressSelectionDialog(QWidget *parent)
    : QDialog(parent), _ui(new Ui::AddressSelectionDialog) {
    _ui->setupUi(this);
    connect(_ui->okButton, &QAbstractButton::clicked, this, &QDialog::accept);
}

AddressSelectionDialog::~AddressSelectionDialog() {
    delete _ui;
}

void AddressSelectionDialog::setBaseAddress(const QString &address) {
    _ui->addressLabel->setText(address);
}

void AddressSelectionDialog::addResultToList(const QString &result) {
    _ui->resultsList->addItem(result);
}

void AddressSelectionDialog::clearList() {
    _ui->resultsList->clear();
}

bool AddressSelectionDialog::pickFirstResult() const {
    return _ui->pickFirstResultCB->isChecked();
}

void AddressSelectionDialog::setPickFirstResult(bool pickFirstResult) {
    _ui->pickFirstResultCB->setChecked(pickFirstResult);
}

int AddressSelectionDialog::getPickedResultIdx() {
    return _ui->resultsList->currentRow();
}

void AddressSelectionDialog::showEvent(QShowEvent *showEvt) {
    if (_ui->resultsList->count() > 0) {
        _ui->resultsList->setCurrentRow(0);
    }
    QDialog::showEvent(showEvt);
}

}
