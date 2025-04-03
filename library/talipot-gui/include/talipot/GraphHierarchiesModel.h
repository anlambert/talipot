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

#ifndef TALIPOT_GRAPH_HIERARCHIES_MODEL_H
#define TALIPOT_GRAPH_HIERARCHIES_MODEL_H

#include <talipot/config.h>
#include <talipot/Model.h>
#include <talipot/Observable.h>

#include <QList>
#include <QSet>

namespace tlp {
class Graph;
class GraphNeedsSavingObserver;
class Project;
class PluginProgress;

class TLP_QT_SCOPE GraphHierarchiesModel : public tlp::Model, public tlp::Observable {
    Q_OBJECT

    QList<tlp::Graph *> _graphs;
    QString generateName(tlp::Graph *) const;

    tlp::Graph *_currentGraph;
    QMap<const tlp::Graph *, QModelIndex> _indexCache;
    QMap<const tlp::Graph *, GraphNeedsSavingObserver *> _saveNeeded;
    void initIndexCache(tlp::Graph *root);

  public:
    bool needsSaving();

    explicit GraphHierarchiesModel(QObject *parent = nullptr);
    GraphHierarchiesModel(const GraphHierarchiesModel &);
    ~GraphHierarchiesModel() override;

    // Allows the model to behave like a list and to be iterable
    typedef QList<tlp::Graph *>::iterator iterator;
    typedef QList<tlp::Graph *>::const_iterator const_iterator;
    tlp::Graph *operator[](int i) const {
        return _graphs[i];
    }
    tlp::Graph *operator[](int i) {
        return _graphs[i];
    }
    int size() const {
        return _graphs.size();
    }

    iterator begin() {
        return _graphs.begin();
    }
    iterator end() {
        return _graphs.end();
    }
    const_iterator begin() const {
        return _graphs.begin();
    }
    const_iterator end() const {
        return _graphs.end();
    }

    QList<tlp::Graph *> graphs() const {
        return _graphs;
    }
    bool empty() const {
        return _graphs.isEmpty();
    }

    // Methods re-implemented from QAbstractItemModel
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    QModelIndex indexOf(const Graph *);
    QModelIndex forceGraphIndex(Graph *);

    // Methods inherited from the observable system
    void treatEvent(const tlp::Event &) override;

    void treatEvents(const std::vector<tlp::Event> &) override;

    // active graph handling
    void setCurrentGraph(tlp::Graph *);
    tlp::Graph *currentGraph() const;

    void clear(tlp::Project *);

  signals:
    void currentGraphChanged(tlp::Graph *);

  public slots:
    void addGraph(tlp::Graph *);
    void removeGraph(tlp::Graph *);

    QMap<QString, tlp::Graph *> readProject(tlp::Project *, tlp::PluginProgress *);
    QMap<tlp::Graph *, QString> writeProject(tlp::Project *, tlp::PluginProgress *);

  private:
    QSet<const Graph *> _graphsChanged;
};
}

#endif // TALIPOT_GRAPH_HIERARCHIES_MODEL_H
