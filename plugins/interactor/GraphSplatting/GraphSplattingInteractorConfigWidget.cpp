/**
 *
 * Copyright (C) 2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/ColorScaleConfigDialog.h>

#include <QPainter>
#include <QLinearGradient>
#include <QColorDialog>

#include "GraphSplattingInteractorConfigWidget.h"

using namespace tlp;
using namespace std;

GraphSplattingInteractorConfigWidget::GraphSplattingInteractorConfigWidget(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);
  connect(configureColorScaleButton, SIGNAL(clicked()), this, SLOT(configureColorScale()));
  vector<Color> colors;
  colors.push_back(Color(0, 255, 0, 255));
  colors.push_back(Color(0, 0, 255));
  colors.push_back(Color(255, 255, 0));
  colors.push_back(Color(255, 0, 0));
  colors.push_back(Color(79, 0, 0));
  colorScale.setColorScale(colors);
  setButtonBackgroundColor(ambientButton, Color(255, 255, 255, 255));
  setButtonBackgroundColor(diffuseButton, Color(0, 0, 0, 255));
  setButtonBackgroundColor(specularButton, Color(150, 150, 150, 255));
  normalMapFilterCB->addItem(SOBEL_3X3.c_str());
  normalMapFilterCB->addItem(SOBEL_5X5.c_str());
  normalMapFilterCB->addItem(PREWITT_3X3.c_str());
  normalMapFilterCB->addItem(PREWITT_5X5.c_str());
  normalMapFilterCB->addItem(FILTER_9X9.c_str());
  normalMapFilterCB->setCurrentIndex(4);
  colorScaleWidget->setColorScale(&colorScale);
  connect(logMappingRB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(linearMappingRB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(diffuseSplattingGB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(diffuseRadiusSP, SIGNAL(valueChanged(int)), this, SLOT(emitconfigModifiedSignal()));
  connect(edgesSplattingSigmaSP, SIGNAL(valueChanged(double)), this,
          SLOT(emitconfigModifiedSignal()));
  connect(edgeSplattingRB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(nodeSplattingRB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(adjustSplattingToZoomCB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(ambientButton, SIGNAL(clicked()), this, SLOT(pressAmbientColorButton()));
  connect(diffuseButton, SIGNAL(clicked()), this, SLOT(pressDiffuseColorButton()));
  connect(specularButton, SIGNAL(clicked()), this, SLOT(pressSpecularColorButton()));
  connect(bumpmapGB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(specularCB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(specularExponentSP, SIGNAL(valueChanged(int)), this, SLOT(emitconfigModifiedSignal()));
  connect(normalMapFilterCB, SIGNAL(currentIndexChanged(int)), this,
          SLOT(emitconfigModifiedSignal()));
  connect(keepGraphImageCB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(scaleFactorSP, SIGNAL(valueChanged(double)), this, SLOT(emitconfigModifiedSignal()));
  connect(edgeSplattingRestrictionCB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(densityColorMappingRB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(graphColorsRB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
  connect(meanGraphColorsRB, SIGNAL(clicked()), this, SLOT(emitconfigModifiedSignal()));
}

bool GraphSplattingInteractorConfigWidget::gradientColorScale() {
  return colorScale.isGradient();
}

void GraphSplattingInteractorConfigWidget::configureColorScale() {
  ColorScaleConfigDialog colorScaleConfigDialog(colorScale, this);
  if (colorScaleConfigDialog.exec() == QDialog::Accepted) {
    colorScale = colorScaleConfigDialog.getColorScale();
    colorScaleWidget->update();
    emitconfigModifiedSignal();
  }
}

MappingType GraphSplattingInteractorConfigWidget::getMappingType() const {
  if (logMappingRB->isChecked()) {
    return LOGARITHMIC;
  } else {
    return LINEAR;
  }
}

bool GraphSplattingInteractorConfigWidget::splattingEnabled() const {
  return diffuseSplattingGB->isChecked();
}

int GraphSplattingInteractorConfigWidget::getSplattingRadius() const {
  return diffuseRadiusSP->value();
}

float GraphSplattingInteractorConfigWidget::getSplattingSigma() const {
  return float(edgesSplattingSigmaSP->value());
}

void GraphSplattingInteractorConfigWidget::emitconfigModifiedSignal() {
  emit configModified();
}

bool GraphSplattingInteractorConfigWidget::edgeSplatting() const {
  return edgeSplattingRB->isChecked();
}

bool GraphSplattingInteractorConfigWidget::adjustSplattingToZoom() const {
  return adjustSplattingToZoomCB->isChecked();
}

Color GraphSplattingInteractorConfigWidget::getButtonColor(QPushButton *button) const {
  QString buttonStyleSheet = button->styleSheet();
  int pos = buttonStyleSheet.indexOf("rgba(") + 5;
  QString backgroundColorCode = buttonStyleSheet.mid(pos, buttonStyleSheet.length() - pos - 2);
  bool ok;
  QStringList rgbaStr = backgroundColorCode.split(",");
  return Color(rgbaStr.at(0).toInt(&ok), rgbaStr.at(1).toInt(&ok), rgbaStr.at(2).toInt(&ok),
               rgbaStr.at(3).toInt(&ok));
}

void GraphSplattingInteractorConfigWidget::changeButtonBackgroundColor(QPushButton *button) {
  QColor currentButtonColor = button->palette().color(QPalette::Button);
  QColor newColor = QColorDialog::getColor(currentButtonColor, this);
  if (newColor.isValid()) {
    setButtonBackgroundColor(
        button, Color(newColor.red(), newColor.green(), newColor.blue(), newColor.alpha()));
  }
  emit configModified();
}

void GraphSplattingInteractorConfigWidget::setButtonBackgroundColor(QPushButton *button,
                                                                    const Color &color) {
  QString colorStr = "rgba(";
  QString str;
  str.setNum(color.getR());
  str.append(",");
  colorStr.append(str);
  str.setNum(color.getG());
  str.append(",");
  colorStr.append(str);
  str.setNum(color.getB());
  str.append(",");
  colorStr.append(str);
  str.setNum(color.getA());
  str.append(")");
  colorStr.append(str);
  button->setStyleSheet("QPushButton { background-color: " + colorStr + "}");
}

void GraphSplattingInteractorConfigWidget::pressAmbientColorButton() {
  changeButtonBackgroundColor(ambientButton);
}

void GraphSplattingInteractorConfigWidget::pressDiffuseColorButton() {
  changeButtonBackgroundColor(diffuseButton);
}

void GraphSplattingInteractorConfigWidget::pressSpecularColorButton() {
  changeButtonBackgroundColor(specularButton);
}

bool GraphSplattingInteractorConfigWidget::bumpmapSplatting() const {
  return bumpmapGB->isChecked();
}

Color GraphSplattingInteractorConfigWidget::getAmbientColor() const {
  return getButtonColor(ambientButton);
}

Color GraphSplattingInteractorConfigWidget::getDiffuseColor() const {
  return getButtonColor(diffuseButton);
}

bool GraphSplattingInteractorConfigWidget::useSpecular() const {
  return specularCB->isChecked();
}

Color GraphSplattingInteractorConfigWidget::getSpecularColor() const {
  return getButtonColor(specularButton);
}

float GraphSplattingInteractorConfigWidget::getSpecularExponent() const {
  return specularExponentSP->value();
}

string GraphSplattingInteractorConfigWidget::getNormalMapFilterName() const {
  return normalMapFilterCB->currentText().toStdString();
}

bool GraphSplattingInteractorConfigWidget::keepOriginalGraphImageInBackground() const {
  return keepGraphImageCB->isChecked();
}

float GraphSplattingInteractorConfigWidget::getBumpmappingScaleFactor() const {
  return scaleFactorSP->value();
}

bool GraphSplattingInteractorConfigWidget::edgeSplattingRestriction() const {
  return edgeSplattingRestrictionCB->isChecked();
}

bool GraphSplattingInteractorConfigWidget::useGraphColorsForDiffuseMap() const {
  return graphColorsRB->isChecked();
}

bool GraphSplattingInteractorConfigWidget::useMeanGraphColorsForDiffuseMap() const {
  return meanGraphColorsRB->isChecked();
}
