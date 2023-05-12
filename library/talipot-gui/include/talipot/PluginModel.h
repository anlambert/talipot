/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PLUGIN_MODEL_H
#define TALIPOT_PLUGIN_MODEL_H

#include <algorithm>

#include <QIcon>
#include <QFont>

#include <talipot/Model.h>
#include <talipot/TlpQtTools.h>
#include <talipot/PluginsManager.h>
#include <talipot/IconicFont.h>
#include <talipot/FontIcon.h>

#include <string>

namespace tlp {
/*
 * @brief Build and manage a Qt Model of a list of plugins
 */
class TLP_QT_SCOPE SimplePluginListModel : public tlp::Model {
private:
  QList<std::string> _list;

public:
  SimplePluginListModel(const QList<std::string> &plugins, QObject *parent = nullptr);
  ~SimplePluginListModel() override;
  int columnCount(const QModelIndex & = QModelIndex()) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QList<std::string> plugins() const;
  std::string pluginName(const QModelIndex &index) const;
};

template <typename PLUGIN>
class PluginModel : public tlp::Model {
  struct TreeItem {
    TreeItem(QString name, QString info = "", TreeItem *parent = nullptr)
        : name(name), info(info), parent(parent) {}
    virtual ~TreeItem() {
      qDeleteAll(children);
    }
    TreeItem *addChild(QString name, QString info = "") {
      auto *result = new TreeItem(name, info, this);
      children.push_back(result);
      return result;
    }

    QString name;
    QString info;
    TreeItem *parent;
    QList<TreeItem *> children;
  };
  TreeItem *_root;

  // FIXME: Non-optimized piece of crap, should be fixed
  void buildTree() {
    delete _root;
    _root = new TreeItem("root");
    QMap<QString, QMap<QString, QStringList>> pluginTree;
    std::list<std::string> plugins = PluginsManager::availablePlugins<PLUGIN>();

    for (const auto &name : plugins) {
      const Plugin &plugin = PluginsManager::pluginInformation(name);
      pluginTree[tlp::tlpStringToQString(plugin.category())]
                [tlp::tlpStringToQString(plugin.group())]
                    .append(tlp::tlpStringToQString(name));
    }

    for (const QString &cat : pluginTree.keys()) {
      TreeItem *catItem = _root->addChild(cat);

      for (const QString &group : pluginTree[cat].keys()) {
        TreeItem *groupItem = catItem;

        if ((!group.isEmpty()) && (pluginTree[cat].keys().size() > 1)) {
          groupItem = catItem->addChild(group);
        }

        // sort in case insensitive alphabetic order
        std::sort(pluginTree[cat][group].begin(), pluginTree[cat][group].end(), QStringCaseCmp);

        for (const QString &alg : pluginTree[cat][group]) {
          std::string algName = tlp::QStringToTlpString(alg);
          const Plugin &plugin = PluginsManager::pluginInformation(algName);
          std::string info = plugin.info();

          // set info only if they contain more than one word
          if (info.find(' ') != std::string::npos) {
            groupItem->addChild(alg, tlp::tlpStringToQString(info));
          } else {
            groupItem->addChild(alg);
          }
        }
      }
    }
  }

  QList<int> indexHierarchy(TreeItem *item) const {
    QList<int> result;
    TreeItem *parent = item->parent;
    TreeItem *child = item;

    while (child != _root) {
      result.push_front(parent->children.indexOf(child));
      parent = parent->parent;
      child = child->parent;
    }

    return result;
  }

public:
  explicit PluginModel(QObject *parent = nullptr) : Model(parent), _root(nullptr) {
    buildTree();
  }
  ~PluginModel() override {
    delete _root;
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    TreeItem *item = _root;

    if (parent.isValid()) {
      item = static_cast<TreeItem *>(parent.internalPointer());
    }

    return item->children.size();
  }

  int columnCount(const QModelIndex & = QModelIndex()) const override {
    return 1;
  }

  QModelIndex parent(const QModelIndex &child) const override {
    if (!child.isValid()) {
      return QModelIndex();
    }

    auto *childItem = static_cast<TreeItem *>(child.internalPointer());

    if (childItem->parent == _root) {
      return QModelIndex();
    }

    QList<int> indexes = indexHierarchy(childItem->parent);
    int row = indexes[indexes.size() - 1];
    return createIndex(row, child.column(), childItem->parent);
  }

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
    TreeItem *parentItem = _root;

    if (parent.isValid()) {
      parentItem = static_cast<TreeItem *>(parent.internalPointer());
    }

    if (row >= parentItem->children.size()) {
      return QModelIndex();
    }

    return createIndex(row, column, parentItem->children[row]);
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
    auto *item = static_cast<TreeItem *>(index.internalPointer());

    if (role == Qt::DisplayRole) {
      return item->name;
    } else if (role == Qt::ToolTipRole) {
      if (item->info.isEmpty()) {
        return item->name;
      } else {
        return QString("<table><tr><td>%1</td></tr><tr><td><i>%2</i></td></tr></table>")
            .arg(item->name + ":")
            .arg(item->info);
      }
    } else if (role == Qt::FontRole && !index.parent().parent().isValid()) {
      QFont f;
      f.setBold(true);
      return f;
    } else if (role == Qt::DecorationRole && item->children.isEmpty()) {
      std::string pluginName = tlp::QStringToTlpString(item->name);
      if (tlp::PluginsManager::pluginExists(pluginName)) {
        const tlp::Plugin &p = tlp::PluginsManager::pluginInformation(pluginName);
        if (IconicFont::isIconSupported(p.icon())) {
          return FontIcon::icon(tlp::tlpStringToQString(p.icon()));
        } else {
          return QIcon(tlp::tlpStringToQString(p.icon()));
        }
      }
    }

    return QVariant();
  }

  Qt::ItemFlags flags(const QModelIndex &index) const override {
    Qt::ItemFlags result(QAbstractItemModel::flags(index));

    if (index.isValid()) {
      auto *item = static_cast<TreeItem *>(index.internalPointer());

      if (!PluginsManager::pluginExists<PLUGIN>(tlp::QStringToTlpString(item->name))) {
        result = Qt::ItemIsEnabled;
      }
    }

    return result;
  }
};
}

#endif // TALIPOT_PLUGIN_MODEL_H
