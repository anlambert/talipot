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

#ifndef ADDRESS_SELECTION_DIALOG_H
#define ADDRESS_SELECTION_DIALOG_H

#include <QDialog>

class QWidget;
class QString;
class QShowEvent;

namespace Ui {
class AddressSelectionDialog;
}

namespace tlp {
class AddressSelectionDialog : public QDialog {

    Ui::AddressSelectionDialog *_ui;

  public:
    AddressSelectionDialog(QWidget *parent = nullptr);
    ~AddressSelectionDialog() override;

    void setBaseAddress(const QString &address);

    void addResultToList(const QString &result);

    void clearList();

    int getPickedResultIdx();

    bool pickFirstResult() const;

    void setPickFirstResult(bool pickFirstResult);

    void showEvent(QShowEvent *showEvt) override;
};
}

#endif // ADDRESS_SELECTION_DIALOG_H
