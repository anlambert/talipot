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

#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>

#include "ColorScaleWidget.h"

using namespace std;
using namespace tlp;

ColorScaleWidget::ColorScaleWidget(QWidget *parent) : QWidget(parent), colorScale(nullptr) {}

vector<Color> ColorScaleWidget::getColorScale() const {
  vector<Color> ret;
  map<float, Color> colorMap = colorScale->getColorMap();
  map<float, Color>::const_iterator it;
  for (it = colorMap.begin(); it != colorMap.end(); ++it) {
    ret.push_back(it->second);
  }
  return ret;
}

static float clamp(qreal f, qreal minVal, qreal maxVal) {
  return min(max(f, minVal), maxVal);
}

void ColorScaleWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  vector<Color> colors = getColorScale();
  if (colorScale->isGradient()) {

    QLinearGradient qLinearGradient;
    if (event->rect().width() > event->rect().height()) {
      qLinearGradient.setStart(0, event->rect().height() / 2);
      qLinearGradient.setFinalStop(event->rect().width() - 1, event->rect().height() / 2);
    } else {
      qLinearGradient.setStart(event->rect().width() / 2, 0);
      qLinearGradient.setFinalStop(event->rect().width() / 2, event->rect().height() - 1);
    }
    for (unsigned int i = 0; i < colors.size(); ++i) {
      qLinearGradient.setColorAt(
          clamp(qreal(i) / qreal(colors.size() - 1), 0.0, 1.0),
          QColor(colors[i].getR(), colors[i].getG(), colors[i].getB(), colors[i].getA()));
    }
    if (event->rect().width() > event->rect().height()) {
      painter.fillRect(0, 0, event->rect().width(), event->rect().height(), qLinearGradient);
    } else {
      painter.fillRect(0, 0, event->rect().height(), event->rect().width(), qLinearGradient);
    }
  } else {
    if (event->rect().width() > event->rect().height()) {
      float rectWidth = event->rect().width() / colors.size();
      for (unsigned int i = 0; i < colors.size(); ++i) {
        painter.fillRect(
            i * rectWidth, 0, (i + 1) * rectWidth, event->rect().height(),
            QBrush(QColor(colors[i].getR(), colors[i].getG(), colors[i].getB(), colors[i].getA())));
      }
    } else {
      float rectHeight = event->rect().height() / colors.size();
      for (unsigned int i = 0; i < colors.size(); ++i) {
        painter.fillRect(
            0, i * rectHeight, event->rect().width(), (i + 1) * rectHeight,
            QBrush(QColor(colors[i].getR(), colors[i].getG(), colors[i].getB(), colors[i].getA())));
      }
    }
  }
}
