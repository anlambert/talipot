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

#include "PropertiesEditor.h"
#include "ui_PropertiesEditor.h"

#include <QMenu>
#include <QMainWindow>

#include <talipot/GraphModel.h>
#include <talipot/CopyPropertyDialog.h>
#include <talipot/PropertyCreationDialog.h>
#include <talipot/RenamePropertyDialog.h>
#include <talipot/ItemDelegate.h>

Q_DECLARE_METATYPE(Qt::CheckState)

using namespace tlp;

PropertiesEditor::PropertiesEditor(QWidget *parent)
    : QWidget(parent), _ui(new Ui::PropertiesEditor), _contextProperty(nullptr), _graph(nullptr),
      _delegate(new tlp::ItemDelegate), _sourceModel(nullptr), filteringProperties(false),
      editorParent(parent), _caseSensitiveSearch(Qt::CaseSensitive) {
  _ui->setupUi(this);
  _ui->newButton->setIcon(FontIcon::icon(MaterialDesignIcons::PlusBox));
  connect(_ui->newButton, &QAbstractButton::clicked, this, &PropertiesEditor::newProperty);
  connect(_ui->tableView, &QTableView::customContextMenuRequested, this,
          &PropertiesEditor::showCustomContextMenu);
  connect(_ui->propsVisibilityCheck, &QCheckBox::stateChanged, this,
          &PropertiesEditor::setPropsVisibility);
  connect(_ui->visualPropertiesCheck, &QCheckBox::clicked, this,
          &PropertiesEditor::showVisualProperties);
}

PropertiesEditor::~PropertiesEditor() {
  delete _ui;
  delete _delegate;
  delete _sourceModel;
}

void PropertiesEditor::setCaseSensitive(Qt::CaseSensitivity cs) {
  _caseSensitiveSearch = cs;
  setPropertiesFilter(_ui->propertiesFilterEdit->text());
}

void PropertiesEditor::setGraph(tlp::Graph *g) {
  _graph = g;
  auto *model = new QSortFilterProxyModel(_ui->tableView);
  delete _sourceModel;
  _sourceModel = new GraphPropertiesModel<PropertyInterface>(g, true);
  model->setSourceModel(_sourceModel);
  model->setFilterCaseSensitivity(Qt::CaseInsensitive);
  // the 3 signal-to-slot connections below ensure the propagation
  // of the displayed properties filtering
  connect(_ui->propertiesFilterEdit, &QLineEdit::textChanged, this,
          &PropertiesEditor::setPropertiesFilter);
  connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this,
          &PropertiesEditor::displayedPropertiesRemoved);
  connect(model, &QAbstractItemModel::rowsInserted, this,
          &PropertiesEditor::displayedPropertiesInserted);
  _ui->tableView->setModel(model);
  connect(_sourceModel, &Model::checkStateChanged, this, &PropertiesEditor::checkStateChanged);
  _ui->tableView->resizeColumnsToContents();
  _ui->tableView->sortByColumn(0, Qt::AscendingOrder);
  _ui->visualPropertiesCheck->setChecked(true);
  registerReservedProperty("viewColor");
  registerReservedProperty("viewLabelColor");
  registerReservedProperty("viewLabelBorderColor");
  registerReservedProperty("viewLabelBorderWidth");
  registerReservedProperty("viewSize");
  registerReservedProperty("viewLabel");
  registerReservedProperty("viewLabelRotation");
  registerReservedProperty("viewLabelPosition");
  registerReservedProperty("viewShape");
  registerReservedProperty("viewRotation");
  registerReservedProperty("viewSelection");
  registerReservedProperty("viewFont");
  registerReservedProperty("viewIcon");
  registerReservedProperty("viewFontSize");
  registerReservedProperty("viewTexture");
  registerReservedProperty("viewBorderColor");
  registerReservedProperty("viewBorderWidth");
  registerReservedProperty("viewLayout");
  registerReservedProperty("viewSrcAnchorShape");
  registerReservedProperty("viewSrcAnchorSize");
  registerReservedProperty("viewTgtAnchorShape");
  registerReservedProperty("viewTgtAnchorSize");
  registerReservedProperty("viewAnimationFrame");
}

void PropertiesEditor::setPropertiesFilter(QString filter) {
  filteringProperties = true;
  auto *model = static_cast<QSortFilterProxyModel *>(_ui->tableView->model());
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
  model->setFilterRegExp(filter);
#else
  model->setFilterRegularExpression(filter);
#endif
  model->setFilterCaseSensitivity(_caseSensitiveSearch);
  filteringProperties = false;
}

QLineEdit *PropertiesEditor::getPropertiesFilterEdit() {
  return _ui->propertiesFilterEdit;
}

void PropertiesEditor::showCustomContextMenu(const QPoint &p) {
  _contextProperty =
      _ui->tableView->indexAt(p).data(Model::PropertyRole).value<PropertyInterface *>();
  _contextPropertyList.clear();

  for (const QModelIndex &sidx : _ui->tableView->selectionModel()->selectedRows()) {
    _contextPropertyList += sidx.data(Model::PropertyRole).value<PropertyInterface *>();
  }

  if (_contextProperty == nullptr) {
    return;
  }

  QString pname = tlpStringToQString(_contextProperty->getName());

  if (pname.length() > 30) {
    pname.truncate(30);
    pname += "...";
  }

  QMenu menu;
  menu.setToolTipsVisible(true);
  QAction *action;

  if (_contextPropertyList.size() > 1) {
    bool enabled = true;

    for (auto *pi : _contextPropertyList) {
      if (isReservedPropertyName(pi->getName().c_str()) &&
          (_graph == _graph->getRoot() || !_graph->existLocalProperty(pi->getName()))) {
        enabled = false;
        break;
      }
    }

    if (enabled) {
      action = menu.addAction("Delete highlighted properties");
      action->setToolTip("Delete the highlighted properties");
      connect(action, &QAction::triggered, this, &PropertiesEditor::delProperties);
      action = menu.addAction("Hide all other properties");
      action->setToolTip("Show only the columns corresponding to the highlighted properties");
      connect(action, &QAction::triggered, this, &PropertiesEditor::setPropsNotVisibleExcept);
    }

    menu.exec(QCursor::pos());
  } else {
    // the style sheet below allows to display disabled items
    // as "title" items in the "mainMenu"
    menu.setStyleSheet("QMenu[mainMenu = \"true\"]::item:disabled {color: white; background-color: "
                       "qlineargradient(spread:pad, x1:0, y1:0, x2:, y2:1, stop:0 rgb(75,75,75), "
                       "stop:1 rgb(60, 60, 60))}");
    // so it is the "mainMenu"
    menu.setProperty("mainMenu", true);
    menu.addAction(pname)->setEnabled(false);
    menu.addSeparator();
    action = menu.addAction(FontIcon::icon(MaterialDesignIcons::EyeOffOutline),
                            "Hide all other properties");
    action->setToolTip("Show only the column corresponding to this property");
    connect(action, &QAction::triggered, this, &PropertiesEditor::setPropsNotVisibleExcept);

    const std::string &propName = _contextProperty->getName();

    action =
        menu.addAction(FontIcon::icon(MaterialDesignIcons::EyeOffOutline), "Show / Hide property");
    action->setToolTip("Show or hide property column in the table");
    connect(action, &QAction::triggered, [this, propName]() {
      auto qPropName = tlpStringToQString(propName);
      setPropertyChecked(qPropName, !isPropertyChecked(qPropName));
    });

    menu.addSeparator();

    connect(menu.addAction(FontIcon::icon(MaterialDesignIcons::ContentDuplicate), "Copy"),
            &QAction::triggered, this, &PropertiesEditor::copyProperty);

    bool enabled = true;
    if (isReservedPropertyName(propName.c_str())) {
      // Enable deletion of reserved properties when on a subgraph and that properties are local
      if (_graph == _graph->getRoot() || !_graph->existLocalProperty(propName)) {
        enabled = false;
      }
    }

    if (enabled) {
      action = menu.addAction(FontIcon::icon(MaterialDesignIcons::Delete), "Delete");
      action->setToolTip("Delete the property \"" + tlpStringToQString(propName) + '"');
      connect(action, &QAction::triggered, this, &PropertiesEditor::delProperty);
    }

    QAction *rename = nullptr;

    if (!isReservedPropertyName(propName.c_str())) {
      rename = menu.addAction("Rename");
      rename->setToolTip("Rename the property \"" + tlpStringToQString(propName) + '"');
    }

    menu.addSeparator();

    QMenu *subMenu = menu.addMenu(FontIcon::icon(MaterialDesignIcons::Pen), "Set values of");
    QAction *nodesSetAll =
        subMenu->addAction("All nodes" + OF_PROPERTY + " to a new default value");
    nodesSetAll->setToolTip("Choose a new node default value to reset the values of all nodes" +
                            OF_PROPERTY);
    QAction *edgesSetAll =
        subMenu->addAction("All edges" + OF_PROPERTY + " to a new default value");
    edgesSetAll->setToolTip("Choose a new edge default value to reset the values of all edges " +
                            OF_PROPERTY);
    QAction *nodesSetAllGraph = subMenu->addAction("All nodes" + OF_GRAPH);
    nodesSetAllGraph->setToolTip("Choose a value to be assigned to all the existing nodes" +
                                 OF_GRAPH);
    QAction *edgesSetAllGraph = subMenu->addAction("All edges" + OF_GRAPH);
    edgesSetAllGraph->setToolTip("Choose a value to be assigned to all the existing edges" +
                                 OF_GRAPH);
    QAction *selectedNodesSetAll = subMenu->addAction("Selected nodes" + OF_GRAPH);
    selectedNodesSetAll->setToolTip("Choose a value to be assigned to the selected nodes" +
                                    OF_GRAPH);
    QAction *selectedEdgesSetAll = subMenu->addAction("Selected edges" + OF_GRAPH);
    selectedEdgesSetAll->setToolTip("Choose a value to be assigned to the selected edges" +
                                    OF_GRAPH);

    enabled = (pname != "viewLabel");

    if (enabled) {
      subMenu = menu.addMenu(FontIcon::icon(MaterialDesignIcons::OrderAlphabeticalAscending),
                             "To labels of");
      action = subMenu->addAction("All elements" + OF_GRAPH);
      action->setToolTip("Set the values of the current property as labels of all elements" +
                         OF_GRAPH);
      connect(action, &QAction::triggered, [this] { toLabels(); });
      action = subMenu->addAction("All nodes" + OF_GRAPH);
      action->setToolTip("Set the values of the current property as labels of the nodes" +
                         OF_GRAPH);
      connect(action, &QAction::triggered, this, &PropertiesEditor::toNodesLabels);
      action = subMenu->addAction("All edges" + OF_GRAPH);
      action->setToolTip("Set the values of the current property as labels of the edges" +
                         OF_GRAPH);
      connect(action, &QAction::triggered, this, &PropertiesEditor::toEdgesLabels);
      action = subMenu->addAction("All selected elements" + OF_GRAPH);
      action->setToolTip(
          "Set the values of the current property as labels of the selected elements" + OF_GRAPH);
      connect(action, &QAction::triggered, this, &PropertiesEditor::toSelectedLabels);
      action = subMenu->addAction("Selected nodes" + OF_GRAPH);
      action->setToolTip("Set the values of the current property as labels of the selected nodes" +
                         OF_GRAPH);
      connect(action, &QAction::triggered, this, &PropertiesEditor::toSelectedNodesLabels);
      action = subMenu->addAction("Selected edges" + OF_GRAPH);
      action->setToolTip("Set the values of the current property as labels of the selected edges" +
                         OF_GRAPH);
      connect(action, &QAction::triggered, this, &PropertiesEditor::toSelectedEdgesLabels);
    }

    QAction *action = menu.exec(QCursor::pos());

    /*if (action == nodesSetDefault || action == edgesSetDefault) {
      setDefaultValue(_contextProperty, action == nodesSetDefault);
    }

    else*/
    if (action != nullptr) {
      bool result = false;

      _graph->push();

      if (action == nodesSetAll) {
        result = setAllValues(_contextProperty, true, false);
      }

      if (action == nodesSetAllGraph) {
        result = setAllValues(_contextProperty, true, false, true);
      }

      if (action == edgesSetAll) {
        result = setAllValues(_contextProperty, false, false);
      }

      if (action == edgesSetAllGraph) {
        result = setAllValues(_contextProperty, false, false, true);
      }

      if (action == selectedNodesSetAll) {
        result = setAllValues(_contextProperty, true, true);
      }

      if (action == selectedEdgesSetAll) {
        result = setAllValues(_contextProperty, false, true);
      }

      if (action == rename) {
        result = renameProperty(_contextProperty);
      }

      if (!result) {
        // edition cancelled
        _graph->pop();
      }
    }
  }

  _contextProperty = nullptr;
}

void PropertiesEditor::setPropsVisibility(int state) {
  if (state == Qt::PartiallyChecked) {
    return;
  }

  _ui->propsVisibilityCheck->setTristate(false);

  if (state == Qt::Checked) {
    // reset property name filter
    _ui->propertiesFilterEdit->setText(QString());
    // no filter
    static_cast<QSortFilterProxyModel *>(_ui->tableView->model())->setFilterFixedString("");
  }

  bool showVisualP = _ui->visualPropertiesCheck->isChecked();

  for (int i = 0; i < _sourceModel->rowCount(); ++i) {
    if (_sourceModel->index(i, 0).data().toString().indexOf("view") == 0) {
      setPropertyChecked(i, showVisualP);
    } else {
      _sourceModel->setData(_sourceModel->index(i, 0), state, Qt::CheckStateRole);
    }
  }
}

void PropertiesEditor::setPropsNotVisibleExcept() {
  std::set<std::string> ctxPropNames;

  for (auto *pi : _contextPropertyList) {
    ctxPropNames.insert(pi->getName());
  }

  for (int i = 0; i < _sourceModel->rowCount(); ++i) {
    setPropertyChecked(i, ctxPropNames.count(QStringToTlpString(
                              _sourceModel->index(i, 0).data().toString())) == 1);
  }

  _ui->propsVisibilityCheck->setTristate(true);
  _ui->propsVisibilityCheck->setCheckState(Qt::PartiallyChecked);
}

void PropertiesEditor::showVisualProperties(bool f) {
  // reset property name filter
  _ui->propertiesFilterEdit->setText(QString());

  static_cast<QSortFilterProxyModel *>(_ui->tableView->model())->setFilterFixedString("");

  // ensure all visual properties are shown/hidden
  for (int i = 0; i < _sourceModel->rowCount(); ++i) {
    if (_sourceModel->index(i, 0).data().toString().indexOf("view") == 0) {
      setPropertyChecked(i, f);
    }
  }
}

// properties inserted when filtering
// are visible according to their CheckState
void PropertiesEditor::displayedPropertiesInserted(const QModelIndex &parent, int start, int end) {
  auto *model = static_cast<QSortFilterProxyModel *>(sender());

  for (; start <= end; ++start) {
    QModelIndex sIndex = model->mapToSource(model->index(start, 0, parent));
    auto *pi = _sourceModel->data(sIndex, Model::PropertyRole).value<PropertyInterface *>();

    if (!filteringProperties) {
      _sourceModel->setData(sIndex, Qt::Checked, Qt::CheckStateRole);
    }

    emit propertyVisibilityChanged(pi, _sourceModel->data(sIndex, Qt::CheckStateRole).toInt() !=
                                           Qt::Unchecked);
  }
}

// properties removed when filtering
// are no longer visible
void PropertiesEditor::displayedPropertiesRemoved(const QModelIndex &parent, int start, int end) {
  auto *model = static_cast<QSortFilterProxyModel *>(sender());

  for (; start <= end; ++start) {
    auto *pi =
        _sourceModel->data(model->mapToSource(model->index(start, 0, parent)), Model::PropertyRole)
            .value<PropertyInterface *>();
    emit propertyVisibilityChanged(pi, false);
  }
}

bool PropertiesEditor::setAllValues(PropertyInterface *prop, bool nodes, bool selectedOnly,
                                    bool graphOnly) {
  QVariant val = ItemDelegate::showEditorDialog(
      nodes ? NODE : EDGE, prop, _graph, static_cast<ItemDelegate *>(_delegate), editorParent);

  // Check if edition has been cancelled
  if (!val.isValid()) {
    return false;
  }

  if (selectedOnly) {
    BooleanProperty *selection = _graph->getBooleanProperty("viewSelection");

    if (nodes) {
      for (auto n : selection->getNonDefaultValuatedNodes(_graph)) {
        GraphModel::setNodeValue(n.id, prop, val);
      }
    } else {
      for (auto e : selection->getNonDefaultValuatedEdges(_graph)) {
        GraphModel::setEdgeValue(e.id, prop, val);
      }
    }
  } else {
    Observable::holdObservers();

    if (nodes) {
      GraphModel::setAllNodeValue(prop, val, graphOnly ? _graph : nullptr);
    } else {
      GraphModel::setAllEdgeValue(prop, val, graphOnly ? _graph : nullptr);
    }

    Observable::unholdObservers();
  }

  return true;
}

void PropertiesEditor::setDefaultValue(tlp::PropertyInterface *prop, bool nodes) {
  QVariant val = ItemDelegate::showEditorDialog(
      nodes ? NODE : EDGE, prop, _graph, static_cast<ItemDelegate *>(_delegate), editorParent);

  // Check if edition has been cancelled
  if (!val.isValid()) {
    return;
  }

  if (nodes) {
    GraphModel::setNodeDefaultValue(prop, val);
  } else {
    GraphModel::setEdgeDefaultValue(prop, val);
  }
}

void PropertiesEditor::copyProperty() {
  _graph->push();

  if (CopyPropertyDialog::copyProperty(_graph, _contextProperty, true, getMainWindow()) ==
      nullptr) {
    // copy has been cancelled
    _graph->pop();
  }
}

void PropertiesEditor::newProperty() {
  _graph->push();

  if (PropertyCreationDialog::createNewProperty(_graph, getMainWindow(),
                                                _contextProperty ? _contextProperty->getTypename()
                                                                 : std::string()) == nullptr) {
    // creation has been cancelled
    _graph->pop();
  }
}

void PropertiesEditor::delProperty() {
  _graph->push();
  _contextProperty->getGraph()->delLocalProperty(_contextProperty->getName());
}

void PropertiesEditor::delProperties() {
  _graph->push();

  for (auto *pi : _contextPropertyList) {
    pi->getGraph()->delLocalProperty(pi->getName());
  }
}

bool PropertiesEditor::renameProperty(PropertyInterface *prop) {
  emit propertyVisibilityChanged(prop, false);
  bool renamed = RenamePropertyDialog::renameProperty(prop, getMainWindow());
  emit propertyVisibilityChanged(prop, true);
  return renamed;
}

void PropertiesEditor::toLabels() {
  _graph->push();
  toLabels(_contextProperty, true, true);
}

void PropertiesEditor::toNodesLabels() {
  _graph->push();
  toLabels(_contextProperty, true, false);
}

void PropertiesEditor::toEdgesLabels() {
  _graph->push();
  toLabels(_contextProperty, false, true);
}

void PropertiesEditor::toSelectedLabels() {
  _graph->push();
  toLabels(_contextProperty, true, true, true);
}

void PropertiesEditor::toSelectedNodesLabels() {
  _graph->push();
  toLabels(_contextProperty, true, false, true);
}

void PropertiesEditor::toSelectedEdgesLabels() {
  _graph->push();
  toLabels(_contextProperty, false, true, true);
}

void PropertiesEditor::toLabels(PropertyInterface *prop, bool nodes, bool edges,
                                bool selectedOnly) {
  DataSet data;
  data.set("nodes", nodes);
  data.set("edges", edges);
  data.set("input", prop);

  if (selectedOnly) {
    data.set("selection", _graph->getBooleanProperty("viewSelection"));
  }

  std::string msg;
  // _graph->push() must be done outside of this method
  // to allow call from TabelView.cpp
  StringProperty *result = _graph->getStringProperty("viewLabel");
  _graph->applyPropertyAlgorithm("To labels", result, msg, &data);
}

void PropertiesEditor::checkStateChanged(QModelIndex index, Qt::CheckState state) {
  auto *pi = _sourceModel->data(index, Model::PropertyRole).value<PropertyInterface *>();
  emit propertyVisibilityChanged(pi, state == Qt::Checked);
}

QSet<PropertyInterface *> PropertiesEditor::visibleProperties() const {
  if (_sourceModel != nullptr) {
    return _sourceModel->checkedProperties();
  }

  return QSet<tlp::PropertyInterface *>();
}

void PropertiesEditor::setPropertyChecked(int index, bool state) {
  _sourceModel->setData(_sourceModel->index(index, 0), state ? Qt::Checked : Qt::Unchecked,
                        Qt::CheckStateRole);
}

void PropertiesEditor::setPropertyChecked(const QString &pName, bool state) {
  auto index = _sourceModel->index(_sourceModel->rowOf(pName), 0);
  if (index.isValid()) {
    _sourceModel->setData(index, state ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
  }
}

bool PropertiesEditor::isPropertyChecked(const QString &pName) const {
  auto index = _sourceModel->index(_sourceModel->rowOf(pName), 0);
  return index.isValid() && _sourceModel->data(index, Qt::CheckStateRole).toInt() == Qt::Checked;
}

PropertyInterface *PropertiesEditor::contextProperty() const {
  return _contextProperty;
}

void PropertiesEditor::registerReservedProperty(const QString &s) {
  _reservedProperties.insert(s);
}

bool PropertiesEditor::isReservedPropertyName(const QString &s) {
  return _reservedProperties.contains(s);
}