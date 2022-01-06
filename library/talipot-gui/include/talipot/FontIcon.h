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

#ifndef TALIPOT_FONT_ICON_MANAGER_H
#define TALIPOT_FONT_ICON_MANAGER_H

#include <QIcon>
#include <QString>

#include <talipot/config.h>
#include <talipot/TlpQtTools.h>

namespace tlp {

class TLP_QT_SCOPE FontIcon {

public:
  static const QIcon &icon(const QString &iconName, const double scaleFactor = 1.0,
                           const double rotation = 0, const QPointF &translation = QPointF(0, 0));

  static const QIcon &icon(const QString &iconName, const QColor &color,
                           const double scaleFactor = 1.0, const double rotation = 0,
                           const QPointF &translation = QPointF(0, 0));

  static const QIcon &icon(const QString &iconName, const QColor &color,
                           const QColor &colorDisabled, const QColor &colorActive,
                           const QColor &colorSelected, const double scaleFactor = 1.0,
                           const double rotation = 0, const QPointF &translation = QPointF(0, 0));

  static QIcon stackIcons(const QIcon &backIcon, const QIcon &frontIcon);
};

}

#endif // TALIPOT_FONT_ICON_MANAGER_H
