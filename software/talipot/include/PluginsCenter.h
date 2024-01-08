/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef PLUGINS_CENTER_H
#define PLUGINS_CENTER_H

#include <talipot/Plugin.h>
#include <talipot/Release.h>

#include <QWidget>
#include <QMap>
#include <QNetworkReply>

#include <functional>
#include <vector>

namespace Ui {
class PluginsCenter;
}

class PluginInformationListItem;

class PluginsCenter : public QWidget {
  Q_OBJECT

  Ui::PluginsCenter *_ui;

  QStringList _categoryFilters;
  QString _nameFilter;

  PluginInformationListItem *_currentItem;

public:
  explicit PluginsCenter(QWidget *parent = nullptr);
  ~PluginsCenter() override;

protected:
  void showEvent(QShowEvent *showEvent) override;

public slots:
  void reportPluginErrors(const QMap<QString, QString> &errors);

  void showErrorsPage();
  void showWelcomePage();

  void searchAll();
  void searchAlgorithms();
  void searchImportExport();
  void searchGlyphs();
  void searchViews();
  void searchInteractors();

  void setNameFilter(const QString &filter);
  void setCategoryFilter(const QString &filter);
  void setCategoryFilters(const QStringList &filters);

  void refreshFilter();
protected slots:
  void sideListRowChanged(int i);
  void itemFocused();

private:
  typedef std::reference_wrapper<const tlp::Plugin> PluginRef;
  std::vector<PluginRef> listPlugins(const QString &nameFilter, const QString &categoryFilter);
};

#endif // PLUGINS_CENTER_H
