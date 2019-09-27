/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */
///@cond DOXYGEN_HIDDEN

#ifndef GRAPHELEMENTMODEL_H
#define GRAPHELEMENTMODEL_H

#include <tulip/TulipModel.h>
#include <tulip/GraphModel.h>

#include <QVector>

namespace tlp {

class TLP_QT_SCOPE GraphElementModel : public TulipModel {

public:
  GraphElementModel(Graph *graph, unsigned int id, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  virtual QString headerText(unsigned int id) const = 0;
  virtual QVariant value(unsigned int id, PropertyInterface *prop) const = 0;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  const static int PropertyNameRole = 33;

protected:
  QVector<PropertyInterface *> getGraphProperties() const;

  Graph *_graph;
  unsigned int _id;
};

class TLP_QT_SCOPE GraphNodeElementModel : public GraphElementModel {

public:
  GraphNodeElementModel(Graph *graph, unsigned int id, QObject *parent = nullptr)
      : GraphElementModel(graph, id, parent) {}

  QString headerText(unsigned int id) const override {
    return QString("node: ") + QString::number(id);
  }

  QVariant value(unsigned int id, PropertyInterface *prop) const override {
    return GraphModel::nodeValue(id, prop);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};

class TLP_QT_SCOPE GraphEdgeElementModel : public GraphElementModel {

public:
  GraphEdgeElementModel(Graph *graph, unsigned int id, QObject *parent = nullptr)
      : GraphElementModel(graph, id, parent) {}

  QString headerText(unsigned int id) const override {
    return QString("edge: ") + QString::number(id);
  }

  QVariant value(unsigned int id, PropertyInterface *prop) const override {
    return GraphModel::edgeValue(id, prop);
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};
} // namespace tlp

#endif // GRAPHELEMENTMODEL_H
///@endcond
