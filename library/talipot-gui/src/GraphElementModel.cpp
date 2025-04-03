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

#include <talipot/GraphElementModel.h>
#include <talipot/MetaTypes.h>

using namespace tlp;
using namespace std;

GraphElementModel::GraphElementModel(Graph *graph, uint id, QObject *parent)
    : Model(parent), _graph(graph), _id(id) {}

int GraphElementModel::rowCount(const QModelIndex &parent) const {
    if (_graph == nullptr || parent.isValid()) {
        return 0;
    }
    return getGraphProperties().size();
}

int GraphElementModel::columnCount(const QModelIndex &parent) const {
    if (_graph == nullptr || parent.isValid()) {
        return 0;
    }

    return 1;
}

QModelIndex GraphElementModel::parent(const QModelIndex & /*child*/) const {
    return QModelIndex();
}

QVariant GraphElementModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
            return headerText(_id);
        } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        }
    } else if (role == Qt::DisplayRole) {
        return getGraphProperties()[section]->getName().c_str();
    }

    return Model::headerData(section, orientation, role);
}

QModelIndex GraphElementModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    return createIndex(row, column, getGraphProperties()[row]);
}

QVariant GraphElementModel::data(const QModelIndex &index, int role) const {
    if (role == PropertyNameRole) {
        return static_cast<PropertyInterface *>(index.internalPointer())->getName().c_str();
    } else if (role == Qt::DisplayRole) {
        return value(_id, static_cast<PropertyInterface *>(index.internalPointer()));
    } else if (role == Model::PropertyRole) {
        return QVariant::fromValue<PropertyInterface *>(
            static_cast<PropertyInterface *>(index.internalPointer()));
    }

    return QVariant();
}

Qt::ItemFlags GraphElementModel::flags(const QModelIndex &index) const {
#ifdef NDEBUG
    return Model::flags(index) | Qt::ItemIsEditable;
#else

    if (static_cast<PropertyInterface *>(index.internalPointer())->getName() == "viewMetaGraph")
        return Model::flags(index);

    return Model::flags(index) | Qt::ItemIsEditable;
#endif
}

QVector<PropertyInterface *> GraphElementModel::getGraphProperties() const {
    QVector<PropertyInterface *> properties;
    for (PropertyInterface *prop : _graph->getObjectProperties()) {
#ifdef NDEBUG
        if (prop->getName() == "viewMetaGraph") {
            continue;
        }
#endif
        properties.append(prop);
    }
    return properties;
}

bool GraphNodeElementModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        auto *prop = static_cast<PropertyInterface *>(index.internalPointer());
        _graph->push();
        bool result = GraphModel::setNodeValue(_id, prop, value);
        _graph->popIfNoUpdates();
        return result;
    }

    return false;
}

bool GraphEdgeElementModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        auto *prop = static_cast<PropertyInterface *>(index.internalPointer());
        _graph->push();
        bool result = GraphModel::setEdgeValue(_id, prop, value);
        _graph->popIfNoUpdates();
        return result;
    }

    return false;
}
