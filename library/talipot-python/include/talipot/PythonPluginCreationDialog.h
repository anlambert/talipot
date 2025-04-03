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

#ifndef TALIPOT_PYTHON_PLUGIN_CREATION_DIALOG_H
#define TALIPOT_PYTHON_PLUGIN_CREATION_DIALOG_H

#include <QDialog>

#include <talipot/config.h>

namespace Ui {
class PythonPluginCreationDialog;
}

namespace tlp {

class TLP_PYTHON_SCOPE PythonPluginCreationDialog : public QDialog {

    Q_OBJECT
    Ui::PythonPluginCreationDialog *_ui;

  public:
    PythonPluginCreationDialog(QWidget *parent = nullptr);
    ~PythonPluginCreationDialog() override;
    QString getPluginFileName() const;

    QString getPluginType() const;

    QString getPluginClassName() const;

    QString getPluginName() const;

    QString getPluginAuthor() const;

    QString getPluginDate() const;

    QString getPluginInfo() const;

    QString getPluginRelease() const;

    QString getPluginGroup() const;

  protected slots:

    void accept() override;

    void selectPluginSourceFile();
};
}

#endif // TALIPOT_PYTHON_PLUGIN_CREATION_DIALOG_H
