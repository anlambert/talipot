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

#ifndef GRAPH_HIERARCHIES_EDITOR_H
#define GRAPH_HIERARCHIES_EDITOR_H

#include <talipot/TreeViewComboBox.h>

#include <QWidget>
#include <QModelIndex>

class QAbstractButton;

namespace tlp {
class GraphHierarchiesModel;
class Graph;
}

namespace Ui {
class GraphHierarchiesEditor;
}

class CustomTreeView : public QTreeView {

  Q_OBJECT

public:
  CustomTreeView(QWidget *parent = nullptr);

  void setModel(QAbstractItemModel *model) override;
  void setAllHierarchyVisible(const QModelIndex &index, bool visible);

public slots:

  void resizeFirstColumnToContent() const;
  void resizeFirstColumnToContentImpl();

protected:
  int sizeHintForColumn(int col) const override;
  void scrollContentsBy(int dx, int dy) override;
};

class GraphHierarchiesEditor : public QWidget {
  Q_OBJECT
  Ui::GraphHierarchiesEditor *_ui;
  tlp::Graph *_contextGraph;
  QModelIndex _contextIndex;
  QAbstractButton *_linkButton;
  tlp::GraphHierarchiesModel *_model;

public:
  explicit GraphHierarchiesEditor(QWidget *parent = nullptr);
  ~GraphHierarchiesEditor() override;
  void setModel(tlp::GraphHierarchiesModel *model);
  bool synchronized() const;

protected:
  void selectGraphElements(tlp::Graph *graph, bool nodes, bool edges, bool selectValue,
                           bool resetSelection);

signals:
  void changeSynchronization(bool);

public slots:
  void setSynchronizeButtonVisible(bool);

protected slots:
  void contextMenuRequested(const QPoint &);
  void doubleClicked(const QModelIndex &);
  void clicked(const QModelIndex &);
  void currentChanged(const QModelIndex &, const QModelIndex &);

  void addSubGraph();
  void cloneSubGraph();
  void cloneSibling();
  void cloneSiblingWithProperties();
  void addInducedSubGraph();
  void delGraph();
  void delAllGraph();
  void delAllNodes();
  void delAllEdges();
  void delSelection(bool fromRoot = false);
  void delSelectionFromRoot();
  void createPanel();
  void collapseGraphHierarchy();
  void expandGraphHierarchy();
  void exportGraph();
  void renameGraph();
  void saveGraphHierarchyInTlpFile();
  void toggleSynchronization(bool);
};

#endif // GRAPH_HIERARCHIES_EDITOR_H
