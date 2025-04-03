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

#ifndef TALIPOT_FONT_DIALOG_H
#define TALIPOT_FONT_DIALOG_H

#include <QDialog>
#include <QMap>
#include <QVector>

#include <talipot/config.h>
#include <talipot/Font.h>

namespace Ui {
class FontDialog;
}

namespace tlp {

class TLP_QT_SCOPE FontDialog : public QDialog {
    Q_OBJECT

    Ui::FontDialog *_ui;
    Font _previousFont;
    int _ok;
    bool _styleUpdate;

    QMap<QString, QVector<Font>> _fonts;

  public:
    explicit FontDialog(QWidget *parent = nullptr);
    ~FontDialog() override;
    Font font() const;
    int fontSize() const;
    void done(int res) override {
        _ok = res;
        QDialog::done(res);
    }
    Font getSelectedFont() const {
        return _ok == QDialog::Accepted ? font() : _previousFont;
    }
    void showEvent(QShowEvent *ev) override;

  public slots:
    void selectFont(const Font &);

  protected slots:
    void fontChanged();
};
}

#endif // TALIPOT_FONT_DIALOG_H
