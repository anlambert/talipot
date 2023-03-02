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

#ifndef EDGESPLATTINGINTERACTORCONFIGWIDGET_H_
#define EDGESPLATTINGINTERACTORCONFIGWIDGET_H_

#include <talipot/Color.h>
#include <talipot/ColorScale.h>

#include "ui_GraphSplattingInteractorConfigWidget.h"

enum MappingType { LOGARITHMIC, LINEAR };

const std::string SOBEL_3X3 = "Sobel 3x3";
const std::string SOBEL_5X5 = "Sobel 5x5";
const std::string PREWITT_3X3 = "Prewitt 3x3";
const std::string PREWITT_5X5 = "Prewitt 5x5";
const std::string FILTER_9X9 = "9x9";

class GraphSplattingInteractorConfigWidget : public QWidget,
                                             public Ui::GraphSplattingInteractorConfigWidgetData {

  Q_OBJECT

public:
  GraphSplattingInteractorConfigWidget(QWidget *parent = 0);

  const tlp::ColorScale &getColorScale() const {
    return colorScale;
  }

  bool gradientColorScale();

  MappingType getMappingType() const;

  bool splattingEnabled() const;

  int getSplattingRadius() const;

  float getSplattingSigma() const;

  bool edgeSplatting() const;

  bool adjustSplattingToZoom() const;

  bool bumpmapSplatting() const;

  tlp::Color getAmbientColor() const;

  tlp::Color getDiffuseColor() const;

  bool useSpecular() const;

  tlp::Color getSpecularColor() const;

  float getSpecularExponent() const;

  std::string getNormalMapFilterName() const;

  bool keepOriginalGraphImageInBackground() const;

  float getBumpmappingScaleFactor() const;

  bool edgeSplattingRestriction() const;

  bool useGraphColorsForDiffuseMap() const;

  bool useMeanGraphColorsForDiffuseMap() const;

signals:

  void configModified();

private slots:

  void configureColorScale();
  void emitconfigModifiedSignal();
  void pressAmbientColorButton();
  void pressDiffuseColorButton();
  void pressSpecularColorButton();

private:
  tlp::Color getButtonColor(QPushButton *button) const;
  void changeButtonBackgroundColor(QPushButton *button);
  void setButtonBackgroundColor(QPushButton *button, const tlp::Color &color);
  tlp::ColorScale colorScale;
};

#endif /* EDGESPLATTINGINTERACTORCONFIGWIDGET_H_ */
