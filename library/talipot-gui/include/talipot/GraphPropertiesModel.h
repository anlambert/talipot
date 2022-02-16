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

#ifndef TALIPOT_GRAPH_PROPERTIES_MODEL_H
#define TALIPOT_GRAPH_PROPERTIES_MODEL_H

#include <talipot/Model.h>
#include <talipot/Observable.h>
#include <talipot/Graph.h>

#include <QFont>
#include <QIcon>
#include <QSet>

namespace tlp {

template <typename PROPTYPE>
class GraphPropertiesModel : public tlp::Model, public tlp::Observable {
  tlp::Graph *_graph;
  QString _placeholder;
  bool _checkable;
  QSet<PROPTYPE *> _checkedProperties;
  QVector<PROPTYPE *> _properties;
  bool _removingRows;
  bool forcingRedraw;

  void rebuildCache();

public:
  explicit GraphPropertiesModel(tlp::Graph *graph, bool checkable = false,
                                QObject *parent = nullptr);
  explicit GraphPropertiesModel(QString placeholder, tlp::Graph *graph, bool checkable = false,
                                QObject *parent = nullptr);
  ~GraphPropertiesModel() override {
    if (_graph != nullptr) {
      _graph->removeListener(this);
    }
  }

  tlp::Graph *graph() const {
    return _graph;
  }

  void setGraph(tlp::Graph *graph) {
    if (_graph == graph) {
      return;
    }

    beginResetModel();

    if (_graph != nullptr) {
      _graph->removeListener(this);
    }

    _graph = graph;

    if (_graph != nullptr) {
      _graph->addListener(this);
    }

    rebuildCache();
    endResetModel();
  }

  QSet<PROPTYPE *> checkedProperties() const {
    return _checkedProperties;
  }

  // Methods re-implemented from QAbstractItemModel
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  bool setData(const QModelIndex &quiindex, const QVariant &value, int role) override;

  // Methods inherited from the observable system
  void treatEvent(const tlp::Event &evt) override {
    if (evt.type() == EventType::TLP_DELETE) {
      // calls to *ResetModel() functions below
      // are not needed because they may cause a Free Memory Read.
      // However the current model will be soon deleted
      // beginResetModel();
      _graph = nullptr;
      _properties.clear();
      // endResetModel();
      return;
    }

    const auto *graphEvent = dynamic_cast<const GraphEvent *>(&evt);

    if (graphEvent == nullptr) {
      return;
    }

    if (graphEvent->getType() == GraphEventType::TLP_BEFORE_DEL_LOCAL_PROPERTY ||
        graphEvent->getType() == GraphEventType::TLP_BEFORE_DEL_INHERITED_PROPERTY) {

      auto *prop = dynamic_cast<PROPTYPE *>(_graph->getProperty(graphEvent->getPropertyName()));

      if (prop != nullptr) {
        int row = rowOf(prop);
        if (row > -1) {
          beginRemoveRows(QModelIndex(), row, row);
          _properties.remove(_properties.indexOf(prop));
          _removingRows = true;
          _checkedProperties.remove(prop);
        }
      }
    } else if (graphEvent->getType() == GraphEventType::TLP_AFTER_DEL_LOCAL_PROPERTY ||
               graphEvent->getType() == GraphEventType::TLP_AFTER_DEL_INHERITED_PROPERTY) {
      if (_removingRows) {
        endRemoveRows();
        _removingRows = false;
      }
    } else if (graphEvent->getType() == GraphEventType::TLP_ADD_LOCAL_PROPERTY ||
               graphEvent->getType() == GraphEventType::TLP_ADD_INHERITED_PROPERTY) {
      auto *prop = dynamic_cast<PROPTYPE *>(_graph->getProperty(graphEvent->getPropertyName()));

      if (prop != nullptr) {
        rebuildCache();
        int row = rowOf(prop);

        if (row > -1) {
          beginInsertRows(QModelIndex(), row, row);
          endInsertRows();
        }
      }
    } else if (graphEvent->getType() == GraphEventType::TLP_AFTER_RENAME_LOCAL_PROPERTY) {
      // force any needed sorting
      emit layoutAboutToBeChanged();
      changePersistentIndex(createIndex(0, 0), createIndex(_properties.size() - 1, 0));
      emit layoutChanged();
    }
  }

  int rowOf(PROPTYPE *) const;

  int rowOf(const QString &pName) const;

  Qt::ItemFlags flags(const QModelIndex &index) const override {
    Qt::ItemFlags result = QAbstractItemModel::flags(index);

    if (index.column() == 0 && _checkable) {
      result |= Qt::ItemIsUserCheckable;
    }

    return result;
  }
};
}

#include "cxx/GraphPropertiesModel.cxx"

#endif // TALIPOT_GRAPH_PROPERTIES_MODEL_H
