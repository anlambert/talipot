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

#include <talipot/PluginModel.h>

using namespace tlp;
using namespace std;

SimplePluginListModel::SimplePluginListModel(const QList<std::string> &plugins, QObject *parent)
    : Model(parent), _list(plugins) {}

SimplePluginListModel::~SimplePluginListModel() = default;

string SimplePluginListModel::pluginName(const QModelIndex &idx) const {
  int index = idx.row();

  if (index > -1 && index < _list.size()) {
    return _list.at(index);
  } else {
    return string();
  }
}

QList<std::string> SimplePluginListModel::plugins() const {
  return _list;
}

int SimplePluginListModel::columnCount(const QModelIndex &) const {
  return 1;
}

int SimplePluginListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _list.size();
}

QModelIndex SimplePluginListModel::parent(const QModelIndex &) const {
  return QModelIndex();
}

QModelIndex SimplePluginListModel::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid()) {
    return QModelIndex();
  }

  return createIndex(row, column);
}

QVariant SimplePluginListModel::data(const QModelIndex &index, int role) const {
  if (index.row() < _list.size()) {
    std::string name(_list[index.row()]);

    if (role == Qt::DisplayRole) {
      return tlp::tlpStringToQString(name);
    } else if (role == Qt::DecorationRole) {
      const Plugin &p = PluginsManager::pluginInformation(name);
      if (IconicFont::isIconSupported(p.icon())) {
        return FontIcon::icon(tlp::tlpStringToQString(p.icon())).pixmap(QSize(32, 32));
      } else {
        return QPixmap(tlp::tlpStringToQString(p.icon()));
      }
    }
  }

  return QVariant();
}
