/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include "talipot/PythonInterpreter.h"
#include "talipot/PythonTabWidget.h"

#include <QPainter>
#include <QTabBar>
#include <QLinearGradient>

using namespace tlp;

PythonTabWidget::PythonTabWidget(QWidget *parent)
    : QTabWidget(parent), _drawGradient(false), _textColor(Qt::white) {}

void PythonTabWidget::paintEvent(QPaintEvent *event) {
  QTabWidget::paintEvent(event);
  static QString pythonVersion(PythonInterpreter::instance().getPythonVersionStr());
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                         QPainter::SmoothPixmapTransform);

  painter.fillRect(x(), tabBar()->pos().y(), width(), tabBar()->height(), QColor("#404244"));

  painter.setPen(_textColor);
#ifndef __APPLE__
  painter.setFont(QFont("Arial", int(10 * tabBar()->height() / 27.0)));
#else
  painter.setFont(QFont("Arial", int(12 * tabBar()->height() / 27.0)));
#endif
  int imageWidth = 20;
  int labelWidth = int(80 * tabBar()->height() / 27.0);
  int offset = tabBar()->height() - imageWidth - 2;
  QRect rect(width() - (imageWidth + labelWidth), tabBar()->pos().y() + offset / 2, imageWidth,
             imageWidth);
  QRectF rect2(width() - labelWidth, tabBar()->pos().y(), labelWidth, tabBar()->height());
  painter.drawPixmap(rect, FontIcon::icon(MaterialDesignIcons::LanguagePython, _textColor)
                               .pixmap(QSize(imageWidth, imageWidth)));
  painter.drawText(rect2, Qt::AlignCenter, QString("Python ") + pythonVersion);
}
