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

#ifndef TALIPOT_FONT_ICON_DIALOG_H
#define TALIPOT_FONT_ICON_DIALOG_H

#include <talipot/config.h>

#include <QDialog>
#include <QString>
#include <QIcon>

namespace Ui {
class FontIconDialog;
}

namespace tlp {

class TLP_QT_SCOPE FontIconDialog : public QDialog {

    Q_OBJECT

    Ui::FontIconDialog *_ui;
    QString _selectedIconName;

  public:
    FontIconDialog(QWidget *parent = nullptr);

    QString getSelectedIconName() const;

    void setSelectedIconName(const QString &iconName);

    void accept() override;

    void showEvent(QShowEvent *) override;

  protected slots:

    void updateIconList();

    void openUrlInBrowser(const QString &url);
};
}

#endif // TALIPOT_FONT_ICON_DIALOG_H
