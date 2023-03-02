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

#ifndef COLORSCALEWIDGET_H_
#define COLORSCALEWIDGET_H_

#include <QWidget>

#include <talipot/ColorScale.h>

class ColorScaleWidget : public QWidget {

public:
  ColorScaleWidget(QWidget *parent = 0);

  void setColorScale(tlp::ColorScale *colorScale) {
    this->colorScale = colorScale;
  }

protected:
  void paintEvent(QPaintEvent *event);

private:
  std::vector<tlp::Color> getColorScale() const;

  tlp::ColorScale *colorScale;
};

#endif /* COLORSCALEWIDGET_H_ */
