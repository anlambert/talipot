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

#ifndef PROPERTIES_EDITOR_H
#define PROPERTIES_EDITOR_H

#include <QWidget>
#include <QModelIndex>
#include <QLineEdit>

#include <talipot/GraphPropertiesModel.h>
#include <talipot/PropertyInterface.h>

namespace Ui {
class PropertiesEditor;
}

namespace tlp {
class Graph;
class ItemDelegate;
}

#define OF_PROPERTY QString(" of the graph owning the property")
#define OF_GRAPH QString(" of the current graph")

class PropertiesEditor : public QWidget {
  Q_OBJECT

  Ui::PropertiesEditor *_ui;
  tlp::PropertyInterface *_contextProperty;
  QList<tlp::PropertyInterface *> _contextPropertyList;
  tlp::Graph *_graph;
  tlp::ItemDelegate *_delegate;
  tlp::GraphPropertiesModel<tlp::PropertyInterface> *_sourceModel;
  bool filteringProperties;
  QWidget *editorParent;
  Qt::CaseSensitivity _caseSensitiveSearch;

  QSet<QString> _reservedProperties;

public:
  explicit PropertiesEditor(QWidget *parent = nullptr);
  ~PropertiesEditor() override;

  void setGraph(tlp::Graph *g);
  tlp::Graph *getGraph() {
    return _graph;
  }
  QSet<tlp::PropertyInterface *> visibleProperties() const;

  void setPropertyChecked(int index, bool state);
  void setPropertyChecked(const QString &pName, bool state);
  bool isPropertyChecked(const QString &pName) const;
  void setCaseSensitive(Qt::CaseSensitivity cs);
  QLineEdit *getPropertiesFilterEdit();
  void toLabels(tlp::PropertyInterface *prop, bool nodes, bool edges, bool selectedOnly = false);
  bool setAllValues(tlp::PropertyInterface *prop, bool nodes, bool selectedOnly,
                    bool graphOnly = false);
  void setDefaultValue(tlp::PropertyInterface *prop, bool nodes);

  bool renameProperty(tlp::PropertyInterface *prop);

  tlp::PropertyInterface *contextProperty() const;

  void registerReservedProperty(const QString &s);

  bool isReservedPropertyName(const QString &name);

signals:
  void propertyVisibilityChanged(tlp::PropertyInterface *, bool);
  void setFilteredNodes();
  void setFilteredEdges();
  void mapToGraphSelection();

protected slots:
  void checkStateChanged(QModelIndex, Qt::CheckState);
  void showCustomContextMenu(const QPoint &);
  void copyProperty();
  void newProperty();
  void delProperty();
  void delProperties();
  void toLabels();
  void toNodesLabels();
  void toEdgesLabels();
  void toSelectedLabels();
  void toSelectedNodesLabels();
  void toSelectedEdgesLabels();
  void setPropsVisibility(int);
  void setPropsNotVisibleExcept();
  void showVisualProperties(bool);
  void displayedPropertiesInserted(const QModelIndex &parent, int start, int end);
  void displayedPropertiesRemoved(const QModelIndex &parent, int start, int end);
  void setPropertiesFilter(QString filter);
};

#endif // PROPERTIES_EDITOR_H
