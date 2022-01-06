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

#include "TableView.h"
#include "ui_TableView.h"
#include "PropertiesEditor.h"

#include <talipot/GraphModel.h>
#include <talipot/GraphTableItemDelegate.h>
#include <talipot/CopyPropertyDialog.h>
#include <talipot/PropertyCreationDialog.h>

#include <QGraphicsView>
#include <QMenu>
#include <QMainWindow>
#include <QGraphicsProxyWidget>

using namespace tlp;

const QString anyProperty = "Any property";

TableView::TableView(tlp::PluginContext *)
    : ViewWidget(), _ui(new Ui::TableView), propertiesEditor(nullptr), _model(nullptr),
      isNewGraph(false), filteringColumns(false), previousGraph(nullptr), minFontSize(-1) {}

TableView::~TableView() {
  delete _ui;
}

#define NODES_DISPLAYED (_ui->eltTypeCombo->currentIndex() == 0)
#define EDGES_DISPLAYED (_ui->eltTypeCombo->currentIndex() == 1)

tlp::BooleanProperty *TableView::getFilteringProperty() const {
  auto *model =
      static_cast<GraphPropertiesModel<BooleanProperty> *>(_ui->filteringPropertyCombo->model());
  auto *pi =
      model->data(model->index(_ui->filteringPropertyCombo->currentIndex(), 0), Model::PropertyRole)
          .value<PropertyInterface *>();
  return pi ? static_cast<BooleanProperty *>(pi) : nullptr;
}

bool TableView::hasEffectiveFiltering() {
  auto *sortModel = static_cast<GraphSortFilterProxyModel *>(_ui->table->model());

  return sortModel->rowCount() != sortModel->sourceModel()->rowCount();
}

tlp::DataSet TableView::state() const {
  DataSet data;
  data.set("show_nodes", NODES_DISPLAYED);
  data.set("show_edges", EDGES_DISPLAYED);

  BooleanProperty *pi = getFilteringProperty();

  if (pi != nullptr) {
    data.set("filtering_property", pi->getName());
  }

  return data;
}

void TableView::setState(const tlp::DataSet &data) {
  bool showNodes = true;
  std::string filterPropertyName;
  data.get<bool>("show_nodes", showNodes);

  _ui->eltTypeCombo->setCurrentIndex(showNodes ? 0 : 1);

  if (data.exists("filtering_property")) {
    data.get<std::string>("filtering_property", filterPropertyName);
  }

  auto *model =
      static_cast<GraphPropertiesModel<BooleanProperty> *>(_ui->filteringPropertyCombo->model());
  int r = 0;

  if (!filterPropertyName.empty()) {
    r = model->rowOf(model->graph()->getBooleanProperty(filterPropertyName));
  }

  if (r < 0) {
    r = 0;
  }

  _ui->filteringPropertyCombo->setCurrentIndex(r);
}

bool TableView::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::Resize) {
    // ensure automatic resize of the viewport
    auto *resizeEvent = static_cast<QResizeEvent *>(event);
    graphicsView()->viewport()->setFixedSize(resizeEvent->size());
    // same for PropertiesEditor
    QSize pSize = propertiesEditor->parentWidget()->parentWidget()->size();
    pSize.setHeight(resizeEvent->size().height() - 30);
    propertiesEditor->parentWidget()->parentWidget()->resize(pSize);
    pSize = propertiesEditor->size();
    pSize.setHeight(resizeEvent->size().height() - 40);
    propertiesEditor->resize(pSize);
    return true;
  } else {
    // standard event processing
    return QObject::eventFilter(obj, event);
  }
}

void TableView::setZoomLevel(int level) {
  int fs = (level * minFontSize) / 100;
  _ui->table->setStyleSheet(
      QString("QTableView { font-size: %1pt; } QHeaderView::section:horizontal { font: bold; "
              "font-size: %2pt; margin-bottom: 5px; margin-left: 12px; margin-right: 12px;}")
          .arg(fs)
          .arg(fs - 1));
}

void TableView::setupWidget() {
  // install this as event filter
  // for automatic resizing of the viewport
  graphicsView()->viewport()->parentWidget()->installEventFilter(this);
  auto *centralWidget = new QWidget();
  _ui->setupUi(centralWidget);
  activateTooltipAndUrlManager(_ui->table->viewport());
  // no need to display standard View context menu
  setShowContextMenu(false);
  setCentralWidget(centralWidget);

  propertiesEditor =
      new PropertiesEditor(static_cast<QGraphicsProxyWidget *>(centralItem())->widget());

  connect(propertiesEditor, &PropertiesEditor::propertyVisibilityChanged, this,
          &TableView::setPropertyVisible);
  connect(propertiesEditor, &PropertiesEditor::mapToGraphSelection, this,
          &TableView::mapToGraphSelection);

  _ui->table->setItemDelegate(new GraphTableItemDelegate(_ui->table));
  _ui->table->horizontalHeader()->setSectionsMovable(true);
  _ui->table->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(_ui->table->horizontalHeader(), &QWidget::customContextMenuRequested, this,
          &TableView::showHorizontalHeaderCustomContextMenu);
  connect(_ui->table, &QWidget::customContextMenuRequested, this,
          &TableView::showCustomContextMenu);
  connect(_ui->zoomSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &TableView::setZoomLevel);
  minFontSize = _ui->table->font().pointSize();
  connect(_ui->filterEdit, &QLineEdit::returnPressed, this, &TableView::filterChanged);
  connect(_ui->filtercase, &QCheckBox::stateChanged, this, &TableView::filterChanged);

  _ui->eltTypeCombo->addItem("Nodes");
  _ui->eltTypeCombo->addItem("Edges");
  _ui->eltTypeCombo->setCurrentIndex(0);
  connect(_ui->eltTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &TableView::readSettings);
  connect(_ui->filteringPropertyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &TableView::readSettings);
  // columns/properties filtering
  filteringColumns = false;
  connect(_ui->columnsFilterEdit, &QLineEdit::textChanged, this, &TableView::setColumnsFilter);
  connect(_ui->columnsfiltercase, &QCheckBox::stateChanged, this, &TableView::setColumnsFilterCase);
  connect(propertiesEditor->getPropertiesFilterEdit(), &QLineEdit::textChanged, this,
          &TableView::setPropertiesFilter);
}

QList<QWidget *> TableView::configurationWidgets() const {
  return QList<QWidget *>() << propertiesEditor;
}

void TableView::graphChanged(tlp::Graph *g) {
  isNewGraph = true;
  QSet<QString> visibleProperties;

  if (g && propertiesEditor->getGraph() &&
      (g->getRoot() == propertiesEditor->getGraph()->getRoot())) {
    for (auto *pi : propertiesEditor->visibleProperties()) {
      visibleProperties.insert(tlpStringToQString(pi->getName()));
    }
  }

  auto *model = new GraphPropertiesModel<BooleanProperty>("no selection", g, false,
                                                          _ui->filteringPropertyCombo);
  _ui->filteringPropertyCombo->setModel(model);
  _ui->filteringPropertyCombo->setCurrentIndex(0);

  propertiesEditor->setGraph(g);

  _ui->table->horizontalHeader()->show();
  _ui->table->verticalHeader()->show();

  _ui->matchPropertyCombo->clear();
  _ui->matchPropertyCombo->addItem(anyProperty);
  // Show all the properties
  if (_model != nullptr) {
    for (int i = 0; i < _model->columnCount(); ++i) {
      QString propName = _model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
      _ui->matchPropertyCombo->addItem(propName);
      // a property is visible only if it was previously visible
      bool checked = !visibleProperties.isEmpty() ? visibleProperties.contains(propName) : true;

      // unless the property did not exist in the previous graph
      if (previousGraph && !previousGraph->existProperty(QStringToTlpString(propName))) {
        checked = true;
      }

      propertiesEditor->setPropertyChecked(propName, checked);
    }
  }

  previousGraph = g;
  isNewGraph = false;
  setColumnsFilterCase();
}

void TableView::graphDeleted(Graph *ancestor) {
  // if the current graph is deleted
  // just inform the WorkspacePanel
  // that we can display its ancestor instead
  assert(ancestor == nullptr || graph()->getSuperGraph() == ancestor);

  if (ancestor) {
    emit graphSet(ancestor);
  } else {
    setGraph(nullptr);
    readSettings();
  }
}

void TableView::readSettings() {
  if (isNewGraph ||
      ((_ui->eltTypeCombo->currentIndex() == 0) &&
       dynamic_cast<NodesGraphModel *>(_model) == nullptr) ||
      ((_ui->eltTypeCombo->currentIndex() == 1) &&
       dynamic_cast<EdgesGraphModel *>(_model) == nullptr)) {
    _ui->table->setModel(nullptr);

    delete _model;

    if (_ui->eltTypeCombo->currentIndex() == 0)
      _model = new NodesGraphModel(_ui->table);
    else
      _model = new EdgesGraphModel(_ui->table);

    _model->setGraph(graph());
    auto *sortModel = new GraphSortFilterProxyModel(_ui->table);
    sortModel->setSourceModel(_model);
    _ui->table->setModel(sortModel);
    connect(_model, &QAbstractItemModel::columnsInserted, this, &TableView::columnsInserted);
    connect(_model, &QAbstractItemModel::dataChanged, this, &TableView::dataChanged);
    filterChanged();
  }

  auto *sortModel = static_cast<GraphSortFilterProxyModel *>(_ui->table->model());

  sortModel->setFilterProperty(getFilteringProperty());

  QSet<tlp::PropertyInterface *> visibleProperties = propertiesEditor->visibleProperties();

  for (int i = 0; i < _model->columnCount(); ++i) {
    auto *pi = _model->headerData(i, Qt::Horizontal, Model::PropertyRole)
                   .value<tlp::PropertyInterface *>();

    if (!visibleProperties.contains(pi))
      _ui->table->setColumnHidden(i, true);
  }

  // reset columns filtering
  _ui->columnsFilterEdit->setText("");
}

void TableView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
  auto *model = static_cast<QAbstractItemModel *>(sender());

  for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
    auto *pi = model->headerData(topLeft.column(), Qt::Horizontal, Model::PropertyRole)
                   .value<PropertyInterface *>();

    if (pi->getTypename() == "string" && pi->getName() != "viewTexture" &&
        pi->getName() != "viewFont")
      _ui->table->resizeRowToContents(i);
  }
}

void TableView::columnsInserted(const QModelIndex &, int start, int end) {
  auto *model = static_cast<QAbstractItemModel *>(sender());

  for (int c = start; c <= end; c++) {
    auto *pi =
        model->headerData(c, Qt::Horizontal, Model::PropertyRole).value<PropertyInterface *>();
    setPropertyVisible(pi, false);
  }
}

void TableView::setPropertyVisible(PropertyInterface *pi, bool v) {
  if (_model == nullptr) {
    return;
  }

  QString propName = tlpStringToQString(pi->getName());

  for (int i = 0; i < _model->columnCount(); ++i) {
    if (_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() == propName) {
      _ui->table->horizontalHeader()->setSectionHidden(i, !v);
    }
  }

  if (_ui->matchPropertyCombo->currentText() == propName) {
    // set to Any
    _ui->matchPropertyCombo->setCurrentText(anyProperty);
  }

  if (!v) {
    _ui->matchPropertyCombo->removeItem(_ui->matchPropertyCombo->findText(propName));
  } else if (_ui->matchPropertyCombo->findText(propName) == -1) {
    _ui->matchPropertyCombo->addItem(propName);
    _ui->matchPropertyCombo->model()->sort(0);
  }

  // Hide table if no more column is displayed
  bool visible = false;

  for (int i = 0; i < _model->columnCount(); ++i) {
    if (!_ui->table->isColumnHidden(i)) {
      visible = true;
      break;
    }
  }

  _ui->table->horizontalHeader()->setVisible(visible);
  _ui->table->verticalHeader()->setVisible(visible);
}

void TableView::setColumnsFilterCase() {
  if (filteringColumns) {
    return;
  }

  filteringColumns = true;
  propertiesEditor->setCaseSensitive(_ui->columnsfiltercase->isChecked() ? Qt::CaseSensitive
                                                                         : Qt::CaseInsensitive);
  filteringColumns = false;
}

void TableView::setColumnsFilter(const QString &text) {
  if (filteringColumns) {
    return;
  }

  filteringColumns = true;
  propertiesEditor->getPropertiesFilterEdit()->setText(text);
  filteringColumns = false;
}

void TableView::setPropertiesFilter(const QString &text) {
  if (filteringColumns) {
    return;
  }

  filteringColumns = true;
  _ui->columnsFilterEdit->setText(text);
  filteringColumns = false;
}

void TableView::filterChanged() {
  QString filter = _ui->filterEdit->text();
  auto *sortModel = static_cast<GraphSortFilterProxyModel *>(_ui->table->model());
  QVector<PropertyInterface *> props;

  Graph *g = graph();

  QString property = _ui->matchPropertyCombo->currentText();

  if (property == anyProperty) {
    for (int i = 0; i < _model->columnCount(); ++i) {
      if (!_ui->table->horizontalHeader()->isSectionHidden(i))
        props +=
            _model->headerData(i, Qt::Horizontal, Model::PropertyRole).value<PropertyInterface *>();
    }
  } else if (!property.isEmpty()) {
    // a visible column
    props += g->getProperty(QStringToTlpString(_ui->matchPropertyCombo->currentText()));
  }

  sortModel->setProperties(props);
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
  sortModel->setFilterRegExp(filter);
#else
  sortModel->setFilterRegularExpression(filter);
#endif
  sortModel->setFilterCaseSensitivity(_ui->filtercase->isChecked() ? Qt::CaseSensitive
                                                                   : Qt::CaseInsensitive);
}

void TableView::mapToGraphSelection() {
  BooleanProperty *out = graph()->getBooleanProperty("viewSelection");

  if (NODES_DISPLAYED) {
    out->setAllNodeValue(false);
    QItemSelectionModel *selectionModel = _ui->table->selectionModel();

    for (const QModelIndex &idx : selectionModel->selectedRows()) {
      node n(idx.data(Model::ElementIdRole).toUInt());
      out->setNodeValue(n, true);
    }
  } else {
    out->setAllEdgeValue(false);
    QItemSelectionModel *selectionModel = _ui->table->selectionModel();

    for (const QModelIndex &idx : selectionModel->selectedRows()) {
      edge e(idx.data(Model::ElementIdRole).toUInt());
      out->setEdgeValue(e, true);
    }
  }
}

void TableView::delHighlightedRows() {
  Graph *g = graph();
  QModelIndexList rows = _ui->table->selectionModel()->selectedRows();

  for (const auto &idx : rows) {
    if (NODES_DISPLAYED)
      g->delNode(node(idx.data(Model::ElementIdRole).toUInt()));
    else
      g->delEdge(edge(idx.data(Model::ElementIdRole).toUInt()));
  }
}

void TableView::toggleHighlightedRows() {
  Graph *g = graph();
  BooleanProperty *selection = g->getBooleanProperty("viewSelection");
  QModelIndexList rows = _ui->table->selectionModel()->selectedRows();

  auto *sortModel = static_cast<GraphSortFilterProxyModel *>(_ui->table->model());

  if (sortModel->filterProperty() == selection) {
    selection->removeListener(sortModel);
  }

  for (const auto &idx : rows) {
    if (NODES_DISPLAYED) {
      node n(idx.data(Model::ElementIdRole).toUInt());
      selection->setNodeValue(n, !selection->getNodeValue(n));
    } else {
      edge e(idx.data(Model::ElementIdRole).toUInt());
      selection->setEdgeValue(e, !selection->getEdgeValue(e));
    }
  }

  if (sortModel->filterProperty() == selection) {
    selection->addListener(sortModel);
  }
}

void TableView::selectHighlightedRows() {
  Graph *g = graph();
  BooleanProperty *selection = g->getBooleanProperty("viewSelection");
  QModelIndexList rows = _ui->table->selectionModel()->selectedRows();

  auto *sortModel = static_cast<GraphSortFilterProxyModel *>(_ui->table->model());

  if (sortModel->filterProperty() == selection) {
    selection->removeListener(sortModel);
  }

  selection->setAllNodeValue(false);
  selection->setAllEdgeValue(false);

  for (const auto &idx : rows) {
    if (NODES_DISPLAYED)
      selection->setNodeValue(node(idx.data(Model::ElementIdRole).toUInt()), true);
    else
      selection->setEdgeValue(edge(idx.data(Model::ElementIdRole).toUInt()), true);
  }

  if (sortModel->filterProperty() == selection) {
    selection->addListener(sortModel);
  }
}

bool TableView::setAllHighlightedRows(PropertyInterface *prop) {
  Graph *g = graph();
  QModelIndexList rows = _ui->table->selectionModel()->selectedRows();
  uint eltId = UINT_MAX;
  if (rows.size() == 1) {
    eltId = rows[0].data(Model::ElementIdRole).toUInt();
  }

  QVariant val =
      ItemDelegate::showEditorDialog(NODES_DISPLAYED ? NODE : EDGE, prop, g,
                                     static_cast<ItemDelegate *>(_ui->table->itemDelegate()),
                                     graphicsView()->viewport()->parentWidget(), eltId);

  // Check if edition has been cancelled
  if (!val.isValid()) {
    return false;
  }

  for (const auto &idx : rows) {
    if (NODES_DISPLAYED)
      GraphModel::setNodeValue(idx.data(Model::ElementIdRole).toUInt(), prop, val);
    else
      GraphModel::setEdgeValue(idx.data(Model::ElementIdRole).toUInt(), prop, val);
  }

  return true;
}

bool TableView::setCurrentValue(PropertyInterface *prop, uint eltId) {
  QVariant val =
      ItemDelegate::showEditorDialog(NODES_DISPLAYED ? NODE : EDGE, prop, graph(),
                                     static_cast<ItemDelegate *>(_ui->table->itemDelegate()),
                                     graphicsView()->viewport()->parentWidget(), eltId);

  // Check if edition has been cancelled
  if (!val.isValid()) {
    return false;
  }

  if (NODES_DISPLAYED)
    GraphModel::setNodeValue(eltId, prop, val);
  else
    GraphModel::setEdgeValue(eltId, prop, val);

  return true;
}

void TableView::setLabelsOfHighlightedRows(PropertyInterface *prop) {
  QModelIndexList rows = _ui->table->selectionModel()->selectedRows();

  StringProperty *label = graph()->getStringProperty("viewLabel");

  for (const auto &idx : rows) {
    if (NODES_DISPLAYED) {
      node n(idx.data(Model::ElementIdRole).toUInt());
      label->setNodeStringValue(n, prop->getNodeStringValue(n));
    } else {
      edge e(idx.data(Model::ElementIdRole).toUInt());
      label->setEdgeStringValue(e, prop->getEdgeStringValue(e));
    }
  }
}

bool TableView::getNodeOrEdgeAtViewportPos(int x, int y, node &n, edge &e) const {
  QPoint pos = graphicsView()->viewport()->mapToGlobal(QPoint(x, y));
  if (pos.x() < propertiesEditor->mapToGlobal(QPoint(0, 0)).x()) {
    pos = graphicsView()->viewport()->mapToGlobal(
              QPoint(0, y - _ui->table->horizontalHeader()->height())) -
          _ui->table->mapToGlobal(QPoint(0, 0));
    if (_ui->table->rowAt(pos.y()) >= 0) {
      QModelIndex idx = _ui->table->indexAt(pos);
      uint eltId = idx.data(Model::ElementIdRole).toUInt();
      if (NODES_DISPLAYED) {
        n = node(eltId);
        return n.isValid();
      } else {
        e = edge(eltId);
        return e.isValid();
      }
    }
  }
  return false;
}

void TableView::showCustomContextMenu(const QPoint &pos) {
  if (_ui->table->model()->rowCount() == 0)
    return;

  QModelIndex idx = _ui->table->indexAt(pos);
  uint eltId = idx.data(Model::ElementIdRole).toUInt();

  QString eltsName(NODES_DISPLAYED ? "nodes" : "edges");
  QString eltName(NODES_DISPLAYED ? "node" : "edge");
  std::string propName = QStringToTlpString(
      _model->headerData(idx.column(), Qt::Horizontal, Qt::DisplayRole).toString());

  if (propName.empty()) {
    return;
  }

  PropertyInterface *prop = graph()->getProperty(propName);
  bool propIsInherited = prop->getGraph() != graph();

  QModelIndexList highlightedRows = _ui->table->selectionModel()->selectedRows();

  QMenu contextMenu;
  contextMenu.setToolTipsVisible(true);
  // the style sheet below allows to display disabled items
  // as "title" items in the "mainMenu"
  contextMenu.setStyleSheet("QMenu[mainMenu = \"true\"]::item:disabled {color: white; "
                            "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:, y2:1, "
                            "stop:0 rgb(75,75,75), stop:1 rgb(60, 60, 60))}");
  // so it is the "mainMenu"
  contextMenu.setProperty("mainMenu", true);

  QAction *action = contextMenu.addAction(tlpStringToQString(propName));
  action->setEnabled(false);
  contextMenu.addSeparator();

  QMenu *subMenu =
      contextMenu.addMenu(FontIcon::icon(MaterialDesignIcons::Pen), "Set value(s) of ");
  QAction *setAll = nullptr;
  if (propIsInherited) {
    setAll = subMenu->addAction("All " + eltsName + OF_PROPERTY + " to a new default value");
    setAll->setToolTip("Choose a new " + eltsName + " default value to reset the values of all " +
                       eltsName + OF_PROPERTY);
  }
  QAction *setAllGraph = subMenu->addAction("All " + eltsName + OF_GRAPH);
  setAllGraph->setToolTip("Choose a value to be assigned to all the existing " + eltsName +
                          OF_GRAPH);

  QAction *selectedSetAll = subMenu->addAction("Selected " + eltsName + OF_GRAPH);
  selectedSetAll->setToolTip("Choose a value to be assigned to the selected " + eltsName +
                             OF_GRAPH);

  QAction *highlightedSetAll;
  if (highlightedRows.size() > 1) {
    highlightedSetAll = subMenu->addAction("Rows highlighted " + eltsName);
    highlightedSetAll->setToolTip("Choose a value to be assigned to the " + eltsName +
                                  " displayed in the currently highlighted row(s)");
  } else {
    highlightedSetAll = subMenu->addAction(QString("%1 #%2").arg(eltName).arg(eltId));
    highlightedSetAll->setToolTip(
        QString("Choose a value for to be assigned to the current property of %1 #%2")
            .arg(eltName)
            .arg(eltId));
  }

  QAction *toLabels, *selectedToLabels, *highlightedToLabels;
  toLabels = selectedToLabels = highlightedToLabels = nullptr;
  if (propName != "viewLabel") {
    subMenu = contextMenu.addMenu(FontIcon::icon(MaterialDesignIcons::OrderAlphabeticalAscending),
                                  "To label(s) of ");
    toLabels = subMenu->addAction("All " + eltsName + OF_GRAPH);
    toLabels->setToolTip("Set the values of the current property as labels of the " + eltsName +
                         OF_GRAPH);
    selectedToLabels = subMenu->addAction("Selected " + eltsName + OF_GRAPH);
    selectedToLabels->setToolTip(
        "Set the values of the current property as labels of the selected " + eltsName + OF_GRAPH);

    if (highlightedRows.size() > 1) {
      highlightedToLabels = subMenu->addAction("Rows highlighted " + eltsName);
      highlightedToLabels->setToolTip("Set the values of the current property as labels of the " +
                                      eltsName + " displayed in the currently highlighted row(s)");
    } else {
      highlightedToLabels = subMenu->addAction(QString("%1 #%2").arg(eltName).arg(eltId));
      highlightedToLabels->setToolTip(
          QString("Set the value of the current property as label of %1 #%2")
              .arg(eltName)
              .arg(eltId));
    }
  }

  contextMenu.addSeparator();
  action =
      contextMenu.addAction(highlightedRows.size() > 1 ? "Rows highlighted " + eltsName
                                                       : QString("%1 #%2").arg(eltName).arg(eltId));
  action->setEnabled(false);
  contextMenu.addSeparator();
  QAction *toggleAction =
      contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::SelectionOff), "Toggle selection");
  toggleAction->setToolTip("Invert the selection of the " + action->text() +
                           ": deselect if selected or select if not selected");
  QAction *selectAction =
      contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::Selection), "Select");
  selectAction->setToolTip("Set the selection with the " + action->text());
  QAction *deleteAction =
      contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::Delete), "Delete");
  deleteAction->setToolTip("Delete the " + action->text());
  QAction *setValueAction =
      contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::Pen),
                            (highlightedRows.size() > 1) ? "Set values" : "Set value");
  setValueAction->setToolTip(highlightedSetAll->toolTip());

  contextMenu.addSeparator();
  View::fillContextMenu(&contextMenu, QPointF());

  // display the menu with the mouse inside to allow
  // keyboard navigation
  action = contextMenu.exec(QCursor::pos() - QPoint(5, 5));

  if (!action) {
    return;
  }

  /*if (action == setDefault) {
    propertiesEditor->setDefaultValue(prop, NODES_DISPLAYED);
    return;
    }*/

  // hold/unhold observers
  tlp::ObserverHolder oh;

  // allow to undo
  graph()->push();

  if (action == deleteAction) {
    // delete elts corresponding to highlighted rows
    delHighlightedRows();
    // no more highlighted rows
    _ui->table->clearSelection();
    return;
  }

  if (action == toggleAction) {
    // select/deselect elts corresponding to highlighted rows
    toggleHighlightedRows();
    return;
  }

  if (action == selectAction) {
    // select elts corresponding to highlighted rows
    selectHighlightedRows();
    return;
  }

  if (action == setAll) {
    if (!propertiesEditor->setAllValues(prop, NODES_DISPLAYED, false))
      // cancelled so undo
      graph()->pop();

    return;
  }

  if (action == setAllGraph) {
    if (!propertiesEditor->setAllValues(prop, NODES_DISPLAYED, false, graph()))
      // cancelled so undo
      graph()->pop();

    return;
  }

  if (action == selectedSetAll) {
    // set values for all rows elts
    if (!propertiesEditor->setAllValues(prop, NODES_DISPLAYED, true))
      // cancelled so undo
      graph()->pop();

    return;
  }

  if ((action == highlightedSetAll) || (action == setValueAction)) {
    // set values for elts corresponding to highlighted rows
    if (!((highlightedRows.size() > 1) ? setAllHighlightedRows(prop)
                                       : setCurrentValue(prop, eltId))) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == toLabels) {
    bool nodes = NODES_DISPLAYED;
    propertiesEditor->toLabels(prop, nodes, !nodes);
    return;
  }

  if (action == selectedToLabels) {
    // set values as labels
    bool nodes = NODES_DISPLAYED;
    propertiesEditor->toLabels(prop, nodes, !nodes, true);
    return;
  }

  if (action == highlightedToLabels) {
    // set values as labels for elts corresponding to highlighted rows
    setLabelsOfHighlightedRows(prop);
    return;
  }
}

void TableView::showHorizontalHeaderCustomContextMenu(const QPoint &pos) {
  if (_ui->table->model()->columnCount() == 0)
    return;

  QModelIndex idx = _ui->table->indexAt(pos);

  QString eltsName(NODES_DISPLAYED ? "nodes" : "edges");
  std::string propName = QStringToTlpString(
      _model->headerData(idx.column(), Qt::Horizontal, Qt::DisplayRole).toString());

  if (propName.empty()) {
    return;
  }

  PropertyInterface *prop = graph()->getProperty(propName);
  bool propIsInherited = prop->getGraph() != graph();

  QModelIndexList highlightedRows = _ui->table->selectionModel()->selectedRows();

  QMenu contextMenu;
  contextMenu.setToolTipsVisible(true);
  // the style sheet below allows to display disabled items
  // as "title" items in the "mainMenu"
  contextMenu.setStyleSheet("QMenu[mainMenu = \"true\"]::item:disabled {color: white; "
                            "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:, y2:1, "
                            "stop:0 rgb(75,75,75), stop:1 rgb(60, 60, 60))}");
  // so it is the "mainMenu"
  contextMenu.setProperty("mainMenu", true);

  QAction *action = contextMenu.addAction(tlpStringToQString(propName));
  action->setEnabled(false);
  contextMenu.addSeparator();
  QAction *hideProp =
      contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::EyeOffOutline), "Hide property");
  hideProp->setToolTip("Hide property column in the table");
  QAction *copyProp =
      contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::ContentDuplicate), "Copy");
  copyProp->setToolTip("Copy the values of \"" + action->text() +
                       "\" in a property of the same type");
  QAction *deleteProp = nullptr;

  if (!propertiesEditor->isReservedPropertyName(propName.c_str()) ||
      // Enable deletion of reserved properties when on a subgraph and that properties are local
      (graph() != graph()->getRoot() && graph()->existLocalProperty(propName))) {
    deleteProp = contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::Delete), "Delete");
    deleteProp->setToolTip("Delete the property \"" + action->text() + '"');
  }

  QAction *renameProp = nullptr;

  if (!propertiesEditor->isReservedPropertyName(propName.c_str())) {
    renameProp = contextMenu.addAction(FontIcon::icon(MaterialDesignIcons::RenameBox), "Rename");
    renameProp->setToolTip("Rename the property \"" + action->text() + '"');
  }

  contextMenu.addSeparator();

  QMenu *subMenu = contextMenu.addMenu(FontIcon::icon(MaterialDesignIcons::Pen), "Set values of ");
  QAction *nodesSetAll = nullptr;
  QAction *edgesSetAll = nullptr;
  if (propIsInherited) {
    nodesSetAll =
        subMenu->addAction(QString("All nodes") + OF_PROPERTY + " to a new default value");
    nodesSetAll->setToolTip("Choose a new node default value to reset the values of all nodes" +
                            OF_PROPERTY);
    edgesSetAll = subMenu->addAction("All edges" + OF_PROPERTY + " to a new default value");
    edgesSetAll->setToolTip("Choose a new edge default value to reset the values of all edges " +
                            OF_PROPERTY);
  }
  QAction *nodesSetAllGraph = subMenu->addAction("All nodes" + OF_GRAPH);
  nodesSetAllGraph->setToolTip("Choose a value to be assigned to all the existing nodes" +
                               OF_GRAPH);
  QAction *edgesSetAllGraph = subMenu->addAction("All edges" + OF_GRAPH);
  edgesSetAllGraph->setToolTip("Choose a value to be assigned to all the existing edges" +
                               OF_GRAPH);
  QAction *nodesSelectedSetAll = subMenu->addAction("Selected nodes" + OF_GRAPH);
  nodesSelectedSetAll->setToolTip("Choose a value to be assigned to the selected nodes" + OF_GRAPH);
  QAction *edgesSelectedSetAll = subMenu->addAction("Selected edges" + OF_GRAPH);
  edgesSelectedSetAll->setToolTip("Choose a value to be assigned to the selected edges" + OF_GRAPH);
  QAction *highlightedSetAll = nullptr;

  if (!highlightedRows.isEmpty()) {
    highlightedSetAll = subMenu->addAction(
        "Rows highlighted " + eltsName +
        (highlightedRows.size() > 1
             ? ""
             : QString(NODES_DISPLAYED ? " (Node #%1)" : " (Edge #%1)")
                   .arg(highlightedRows[0].data(Model::ElementIdRole).toUInt())));
    highlightedSetAll->setToolTip("Choose a value to be assigned to the " + eltsName +
                                  " displayed in the currently highlighted row(s)");
  }

  QAction *toLabels = nullptr;
  QAction *nodesToLabels = nullptr;
  QAction *edgesToLabels = nullptr;
  QAction *selectedToLabels = nullptr;
  QAction *nodesSelectedToLabels = nullptr;
  QAction *edgesSelectedToLabels = nullptr;
  QAction *highlightedToLabels = nullptr;

  if (propName != "viewLabel") {
    subMenu = contextMenu.addMenu(FontIcon::icon(MaterialDesignIcons::OrderAlphabeticalAscending),
                                  "To labels of ");
    toLabels = subMenu->addAction("All elements" + OF_GRAPH);
    toLabels->setToolTip("Set the values of the current property as labels of all elements" +
                         OF_GRAPH);
    nodesToLabels = subMenu->addAction("All nodes" + OF_GRAPH);
    nodesToLabels->setToolTip("Set the values of the current property as labels of the nodes" +
                              OF_GRAPH);
    edgesToLabels = subMenu->addAction("All edges" + OF_GRAPH);
    edgesToLabels->setToolTip("Set the values of the current property as labels of the edges" +
                              OF_GRAPH);
    selectedToLabels = subMenu->addAction("Selected elements" + OF_GRAPH);
    selectedToLabels->setToolTip(
        "Set the values of the current property as labels of the selected elements" + OF_GRAPH);
    nodesSelectedToLabels = subMenu->addAction("Selected nodes" + OF_GRAPH);
    nodesSelectedToLabels->setToolTip(
        "Set the values of the current property as labels of the selected nodes" + OF_GRAPH);
    edgesSelectedToLabels = subMenu->addAction("Selected edges" + OF_GRAPH);
    edgesSelectedToLabels->setToolTip(
        "Set the values of the current property as labels of the selected edges" + OF_GRAPH);

    if (!highlightedRows.isEmpty()) {
      highlightedToLabels = subMenu->addAction(
          "Rows highlighted " + eltsName +
          (highlightedRows.size() > 1
               ? ""
               : QString(NODES_DISPLAYED ? " (Node #%1)" : " (Edge #%1)")
                     .arg(highlightedRows[0].data(Model::ElementIdRole).toUInt())));
      highlightedToLabels->setToolTip("Set the values of the current property as labels of the " +
                                      eltsName + " displayed in the currently highlighted row(s)");
    }
  }

  contextMenu.addSeparator();
  QAction *sortById = contextMenu.addAction(
      FontIcon::icon(MaterialDesignIcons::SortNumericAscending), "Sort the rows by id");
  sortById->setToolTip("Display the rows in ordering of the id of the " + eltsName);

  // display the menu with the mouse inside to give it the focus
  // and thus allow keyboard navigation
  action = contextMenu.exec(QCursor::pos() - QPoint(5, 5));

  if (!action) {
    return;
  }

  if (action == sortById) {
    if (_ui->table->horizontalHeader()->sortIndicatorSection() != -1) {
      _ui->table->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
      auto *sortModel = static_cast<GraphSortFilterProxyModel *>(_ui->table->model());
      QAbstractItemModel *model = sortModel->sourceModel();
      sortModel->setSourceModel(nullptr);
      sortModel->setSourceModel(model);
      sortModel->setFilterProperty(getFilteringProperty());

      QSet<tlp::PropertyInterface *> visibleProperties = propertiesEditor->visibleProperties();

      for (int i = 0; i < model->columnCount(); ++i) {
        auto *pi = _model->headerData(i, Qt::Horizontal, Model::PropertyRole)
                       .value<tlp::PropertyInterface *>();

        if (!visibleProperties.contains(pi))
          _ui->table->setColumnHidden(i, true);
      }
    }

    return;
  }

  /*if (action == nodesSetDefault || action == edgesSetDefault) {
    propertiesEditor->setDefaultValue(prop, action == nodesSetDefault);
    return;
    }*/

  // hold/unhold observers
  tlp::ObserverHolder oh;

  // allow to undo
  graph()->push();

  if (action == copyProp) {
    if (CopyPropertyDialog::copyProperty(graph(), prop, true, getMainWindow()) == nullptr) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == deleteProp) {
    prop->getGraph()->delLocalProperty(propName);
    return;
  }

  if (action == renameProp) {
    if (!propertiesEditor->renameProperty(prop)) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == hideProp) {
    propertiesEditor->setPropertyChecked(tlpStringToQString(propName), false);
    // no graph state to keep
    graph()->pop();
    return;
  }

  if (action == nodesSetAll) {
    if (!propertiesEditor->setAllValues(prop, true, false)) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == nodesSetAllGraph) {
    if (!propertiesEditor->setAllValues(prop, true, false, true)) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == edgesSetAll) {
    if (!propertiesEditor->setAllValues(prop, false, false)) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == edgesSetAllGraph) {
    if (!propertiesEditor->setAllValues(prop, false, false, true)) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == nodesSelectedSetAll) {
    // set values for all rows elts
    if (!propertiesEditor->setAllValues(prop, true, true)) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == edgesSelectedSetAll) {
    // set values for all rows elts
    if (!propertiesEditor->setAllValues(prop, false, true)) {
      // cancelled so undo
      graph()->pop();
    }

    return;
  }

  if (action == highlightedSetAll) {
    // set values for elts corresponding to highlighted rows
    setAllHighlightedRows(prop);
    return;
  }

  if (action == toLabels) {
    propertiesEditor->toLabels(prop, true, true);
    return;
  }

  if (action == nodesToLabels) {
    propertiesEditor->toLabels(prop, true, false);
    return;
  }

  if (action == edgesToLabels) {
    propertiesEditor->toLabels(prop, false, true);
    return;
  }

  if (action == selectedToLabels) {
    // set values as labels
    propertiesEditor->toLabels(prop, true, true, true);
    return;
  }

  if (action == nodesSelectedToLabels) {
    // set values as labels
    propertiesEditor->toLabels(prop, true, false, true);
    return;
  }

  if (action == edgesSelectedToLabels) {
    // set values as labels
    propertiesEditor->toLabels(prop, false, true, true);
    return;
  }

  if (action == highlightedToLabels) {
    // set values as labels for elts corresponding to highlighted rows
    setLabelsOfHighlightedRows(prop);
    return;
  }
}

PLUGIN(TableView)
