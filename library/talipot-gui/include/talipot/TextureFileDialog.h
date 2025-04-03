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

#ifndef TALIPOT_TEXTURE_FILE_DIALOG_H
#define TALIPOT_TEXTURE_FILE_DIALOG_H

#include <QDialog>

#include <talipot/config.h>
#include <talipot/MetaTypes.h>
#include "ui_TextureFileDialog.h"

namespace Ui {
class TextureFileDialog;
}

namespace tlp {

/**
 * @brief Provide a dialog that allow the user to choose
 * a file whose name may be empty
 *
 *
 **/
class TLP_QT_SCOPE TextureFileDialog : public QDialog {
    Q_OBJECT
  public:
    Ui::TextureFileDialog *ui;
    TextureFile _data;
    int ok;
    TextureFileDialog(QWidget *parent = nullptr);

    ~TextureFileDialog() override;

    void done(int res) override;

    void setData(const TextureFile &tf);

    const TextureFile &data() const {
        return _data;
    }

    void showEvent(QShowEvent *ev) override;

  public slots:
    void browse();
};
}
#endif // TALIPOT_TEXTURE_FILE_DIALOG_H
