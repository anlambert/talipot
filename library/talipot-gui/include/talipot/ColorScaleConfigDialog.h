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

#ifndef TALIPOT_COLOR_SCALE_CONFIG_DIALOG_H
#define TALIPOT_COLOR_SCALE_CONFIG_DIALOG_H

#include <talipot/ColorScale.h>

#include <QDialog>

namespace Ui {
class ColorScaleDialog;
}

class QTableWidgetItem;
class QListWidgetItem;
class QLabel;

namespace tlp {

class TLP_QT_SCOPE ColorScaleConfigDialog : public QDialog {

    Q_OBJECT

  public:
    ColorScaleConfigDialog(const ColorScale &colorScale = ColorScale(), QWidget *parent = nullptr);
    ~ColorScaleConfigDialog() override;
    void setColorScale(const ColorScale &colorScale);
    const ColorScale &getColorScale() const;
    static ColorScale getColorScaleFromImageFile(const std::string &imageFilePath,
                                                 bool gradient = true);

  protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

  private slots:

    void accept() override;
    void pressButtonBrowse();
    void nbColorsValueChanged(int value);
    void colorTableItemDoubleClicked(QTableWidgetItem *item);
    void displaySavedGradientPreview();
    void displayUserGradientPreview();
    void saveCurrentColorScale();
    void deleteSavedColorScale();
    void reeditSaveColorScale(QListWidgetItem *savedColorScaleItem);
    void importColorScaleFromImageFile();
    void invertEditedColorScale();
    void applyGlobalAlphaToColorScale();

  private:
    Ui::ColorScaleDialog *_ui;

    void setColorScaleFromImage(const QString &imageFilePath);

    void loadUserSavedColorScales();
    void displayGradientPreview(const QList<QColor> &colorsVector, bool gradient,
                                QLabel *displayLabel);
    void importColorScaleFromFile(const QString &currentDir);

    ColorScale colorScale;
    ColorScale latestColorScale;
    std::string gradientsImageDirectory;

    static std::map<QString, std::vector<Color>> talipotImageColorScales;
    static void loadImageColorScales();
    static void loadImageColorScalesFromDir(const QString &colorScalesDir);
    static std::vector<Color> getColorScaleFromImageFile(const QString &imageFilePath);
};
}

#endif // TALIPOT_COLOR_SCALE_CONFIG_DIALOG_H
