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

#ifndef SIZE_SCALE_CONFIG_DIALOG_H
#define SIZE_SCALE_CONFIG_DIALOG_H

#include <QDialog>

namespace Ui {
class SizeScaleConfigDialog;
}

class QWidget;

namespace tlp {
class SizeScaleConfigDialog : public QDialog {

    Q_OBJECT

    Ui::SizeScaleConfigDialog *_ui;

  public:
    SizeScaleConfigDialog(QWidget *d = nullptr);
    ~SizeScaleConfigDialog() override;

    bool doMappingOnViewSize() const;

    float getMinSize() const;
    float getMaxSize() const;

    bool applySizeMappingOnX() const;
    bool applySizeMappingOnY() const;
    bool applySizeMappingOnZ() const;

  private slots:

    void minSizeValueChanged(double newValue);
    void maxSizeValueChanged(double newValue);
    void viewSizeRadioButtonToggled(bool checked);
};
}
#endif // SIZE_SCALE_CONFIG_DIALOG_H
