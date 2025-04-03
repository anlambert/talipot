/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GRAPH_ELEMENT_MODEL_H
#define TALIPOT_GRAPH_ELEMENT_MODEL_H

#include <talipot/Model.h>
#include <talipot/GraphModel.h>

#include <QVector>

namespace tlp {

class TLP_QT_SCOPE GraphElementModel : public Model {

  public:
    GraphElementModel(Graph *graph, uint id, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QString headerText(uint id) const = 0;
    virtual QVariant value(uint id, PropertyInterface *prop) const = 0;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    const static int PropertyNameRole = 33;

  protected:
    QVector<PropertyInterface *> getGraphProperties() const;

    Graph *_graph;
    uint _id;
};

class TLP_QT_SCOPE GraphNodeElementModel : public GraphElementModel {

  public:
    GraphNodeElementModel(Graph *graph, uint id, QObject *parent = nullptr)
        : GraphElementModel(graph, id, parent) {}

    QString headerText(uint id) const override {
        return QString("node: ") + QString::number(id);
    }

    QVariant value(uint id, PropertyInterface *prop) const override {
        return GraphModel::nodeValue(id, prop);
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};

class TLP_QT_SCOPE GraphEdgeElementModel : public GraphElementModel {

  public:
    GraphEdgeElementModel(Graph *graph, uint id, QObject *parent = nullptr)
        : GraphElementModel(graph, id, parent) {}

    QString headerText(uint id) const override {
        return QString("edge: ") + QString::number(id);
    }

    QVariant value(uint id, PropertyInterface *prop) const override {
        return GraphModel::edgeValue(id, prop);
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};
}

#endif // TALIPOT_GRAPH_ELEMENT_MODEL_H
