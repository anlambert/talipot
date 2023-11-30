/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_ABOUT_DIALOG_H
#define TALIPOT_ABOUT_DIALOG_H

#include <talipot/config.h>

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog {
  Q_OBJECT
public:
  explicit AboutDialog(QWidget *parent = nullptr);
  ~AboutDialog() override;

private slots:

  void openUrlInBrowser(const QString &url);

private:
  Ui::AboutDialog *_ui;
};

#endif // TALIPOT_ABOUT_DIALOG_H
