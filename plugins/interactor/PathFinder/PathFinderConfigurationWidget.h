/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef PATH_FINDER_CONFIGURATION_WIDGET_H
#define PATH_FINDER_CONFIGURATION_WIDGET_H

#include <QWidget>

#include <string>

namespace Ui {
class PathFinderConfiguration;
}

class QString;

namespace tlp {
class PathFinderConfigurationWidget : public QWidget {
  Q_OBJECT

  Ui::PathFinderConfiguration *_ui;

public:
  PathFinderConfigurationWidget(QWidget *parent = nullptr);
  ~PathFinderConfigurationWidget() override;

  void addWeightComboItem(const QString &s);
  void setCurrentweightComboIndex(const int i);
  int weightComboFindText(const QString &text) const;
  void addEdgeOrientationComboItem(const QString &s);
  void setCurrentedgeOrientationComboIndex(const int i);
  int edgeOrientationComboFindText(const QString &text) const;
  void addPathsTypeComboItem(const QString &s);
signals:
  void setPathsType(const QString &);
  void setWeightMetric(const QString &);
  void setEdgeOrientation(const QString &);
};
}
#endif // PATH_FINDER_CONFIGURATION_WIDGET_H
