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

#ifndef GLYPH_SCALE_CONFIG_DIALOG_H
#define GLYPH_SCALE_CONFIG_DIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class GlyphScaleConfigDialog;
}

class QShowEvent;
class QWidget;

namespace tlp {

class GlyphScaleConfigDialog : public QDialog {

    Q_OBJECT

  public:
    GlyphScaleConfigDialog(QWidget *parent = nullptr);
    ~GlyphScaleConfigDialog() override;

    std::vector<int> getSelectedGlyphsId() const;

    void showEvent(QShowEvent *event) override;

  private slots:

    void nbGlyphsSpinBoxValueChanged(int value);

  private:
    QStringList glyphsNameList;
    Ui::GlyphScaleConfigDialog *_ui;
};
}
#endif // GLYPH_SCALE_CONFIG_DIALOG_H
