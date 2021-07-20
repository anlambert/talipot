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

#include "talipot/CSVGraphMappingConfigurationWidget.h"
#include "ui_CSVGraphMappingConfigurationWidget.h"

#include <talipot/CSVGraphImport.h>
#include <talipot/PropertyCreationDialog.h>
#include <talipot/TlpQtTools.h>
#include <talipot/StringsListSelectionDialog.h>

using namespace tlp;
using namespace std;

CSVGraphMappingConfigurationWidget::CSVGraphMappingConfigurationWidget(QWidget *parent)
    : QWidget(parent), graph(nullptr), ui(new Ui::CSVGraphMappingConfigurationWidget) {
  ui->setupUi(this);
  connect(ui->mappingConfigurationStackedWidget, &QStackedWidget::currentChanged, this,
          &CSVGraphMappingConfigurationWidget::mappingChanged);
  connect(ui->nodeColumnsButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectNodeColumns);
  connect(ui->nodePropertiesButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectNodeProperties);
  connect(ui->edgeColumnsButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectEdgeColumns);
  connect(ui->edgePropertiesButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectEdgeProperties);
  connect(ui->srcColumnsButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectSrcColumns);
  connect(ui->tgtColumnsButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectTgtColumns);
  connect(ui->srcPropertiesButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectSrcProperties);
  connect(ui->tgtPropertiesButton, &QAbstractButton::pressed, this,
          &CSVGraphMappingConfigurationWidget::selectTgtProperties);

  connect(ui->newPropertyOnNodesButton, &QAbstractButton::clicked, this,
          &CSVGraphMappingConfigurationWidget::createNewProperty, Qt::QueuedConnection);
  connect(ui->newPropertyOnEdgesButton, &QAbstractButton::clicked, this,
          &CSVGraphMappingConfigurationWidget::createNewProperty, Qt::QueuedConnection);
}

CSVGraphMappingConfigurationWidget::~CSVGraphMappingConfigurationWidget() {
  delete ui;
}

void CSVGraphMappingConfigurationWidget::updateWidget(tlp::Graph *graph,
                                                      const CSVImportParameters &importParameters) {
  this->graph = graph;

  // initialize columns info
  columns.clear();
  srcColumnIds.clear();
  tgtColumnIds.clear();
  nodeColumnIds.clear();
  edgeColumnIds.clear();
  int srcColumn = -1, tgtColumn = -1;

  for (uint i = 0; i < importParameters.columnNumber(); ++i) {
    if (importParameters.importColumn(i)) {
      columns.push_back(importParameters.getColumnName(i));

      if (srcColumn == -1) {
        srcColumn = i;
        srcColumnIds.push_back(i);
        nodeColumnIds.push_back(i);
        edgeColumnIds.push_back(i);
      } else if (tgtColumn == -1) {
        tgtColumn = i;
        tgtColumnIds.push_back(i);
      }
    } else {
      columns.push_back("");
    }
  }

  // Init mapping widgets.
  // Update node mapping widget
  ui->nodeColumnsButton->setEnabled(false);
  // update edge from source and target mapping widget
  ui->srcColumnsButton->setEnabled(false);
  ui->tgtColumnsButton->setEnabled(false);
  // Update edge mapping widget
  ui->edgeColumnsButton->setEnabled(false);

  // Init default values
  if (importParameters.columnNumber() > 0) {
    if (srcColumn != -1) {
      ui->nodeColumnsButton->setText(tlpStringToQString(importParameters.getColumnName(srcColumn)));
      ui->nodeColumnsButton->setEnabled(true);
      ui->edgeColumnsButton->setText(tlpStringToQString(importParameters.getColumnName(srcColumn)));
      ui->edgeColumnsButton->setEnabled(true);

      // Select default columns for relations import
      if (tgtColumn != -1) {
        // Choose the first column as source id column
        ui->srcColumnsButton->setEnabled(true);
        ui->srcColumnsButton->setText(
            tlpStringToQString(importParameters.getColumnName(srcColumn)));
        // Choose the second column as target id column
        ui->tgtColumnsButton->setEnabled(true);
        ui->tgtColumnsButton->setText(
            tlpStringToQString(importParameters.getColumnName(tgtColumn)));
      }
    }
  }

  // initialize properties info
  nodeProperties.clear();
  edgeProperties.clear();
  srcProperties.clear();
  tgtProperties.clear();

  // Choose viewLabel as default property
  ui->nodePropertiesButton->setText("viewLabel");
  nodeProperties.push_back("viewLabel");
  ui->edgePropertiesButton->setText("viewLabel");
  edgeProperties.push_back("viewLabel");
  ui->srcPropertiesButton->setText("viewLabel");
  srcProperties.push_back("viewLabel");
  ui->tgtPropertiesButton->setText("viewLabel");
  tgtProperties.push_back("viewLabel");
}

CSVToGraphDataMapping *CSVGraphMappingConfigurationWidget::buildMappingObject() const {
  if (ui->mappingConfigurationStackedWidget->currentWidget() == ui->importNewNodesPage) {
    return new CSVToNewNodeIdMapping(graph);
  } else if (ui->mappingConfigurationStackedWidget->currentWidget() == ui->importNodesPage) {
    if (nodeProperties.empty() || nodeColumnIds.empty()) {
      return nullptr;
    }

    bool createMissingElement = ui->createMissingNodesCheckBox->isChecked();
    return new CSVToGraphNodeIdMapping(graph, nodeColumnIds, nodeProperties, createMissingElement);
  } else if (ui->mappingConfigurationStackedWidget->currentWidget() == ui->importEdgesPages) {
    if (edgeProperties.empty() || edgeColumnIds.empty()) {
      return nullptr;
    }

    return new CSVToGraphEdgeIdMapping(graph, edgeColumnIds, edgeProperties);
  } else if (ui->mappingConfigurationStackedWidget->currentWidget() ==
             ui->importEdgesFromNodesPage) {
    // src and tgt columns must be different
    for (uint srcColumnId : srcColumnIds) {
      for (uint tgtColumnId : tgtColumnIds) {
        if (srcColumnId == tgtColumnId) {
          QMessageBox::critical(parentWidget(), "Import of new relations failed",
                                "Source columns and destination columns are not different.");
          return nullptr;
        }
      }
    }

    bool createMissingElement = ui->addMissingEdgeAndNodeCheckBox->isChecked();
    return new CSVToGraphEdgeSrcTgtMapping(graph, srcColumnIds, tgtColumnIds, srcProperties,
                                           tgtProperties, createMissingElement);
  } else {
    return nullptr;
  }
}

bool CSVGraphMappingConfigurationWidget::isValid() const {
  if (ui->mappingConfigurationStackedWidget->currentWidget() == ui->importNewNodesPage) {
    return true;
  } else if (ui->mappingConfigurationStackedWidget->currentWidget() == ui->importNodesPage) {
    return !nodeProperties.empty() && !nodeColumnIds.empty();
  } else if (ui->mappingConfigurationStackedWidget->currentWidget() == ui->importEdgesPages) {
    return !edgeProperties.empty() && !edgeColumnIds.empty();
  } else if (ui->mappingConfigurationStackedWidget->currentWidget() ==
             ui->importEdgesFromNodesPage) {
    // src and tgt columns must be different
    for (uint srcColumnId : srcColumnIds) {
      for (uint tgtColumnId : tgtColumnIds) {
        if (srcColumnId == tgtColumnId) {
          return false;
        }
      }
    }

    return true;
  } else {
    return false;
  }
}

void CSVGraphMappingConfigurationWidget::createNewProperty() {
  PropertyCreationDialog::createNewProperty(graph, this);
}

void CSVGraphMappingConfigurationWidget::selectProperties(const QString &title,
                                                          std::vector<std::string> &selProperties,
                                                          QPushButton *button) {
  vector<string> graphProperties;
  for (const string &propertyName : graph->getProperties()) {
    graphProperties.push_back(propertyName);
  }

  if (StringsListSelectionDialog::choose(title, graphProperties, selProperties, this)) {
    if (selProperties.empty()) {
      selProperties.push_back("viewLabel");
      button->setText("viewLabel");
    } else {
      QString buttonName;

      for (uint i = 0; i < selProperties.size(); ++i) {
        if (i) {
          buttonName.append(", ");
        }

        buttonName.append(tlpStringToQString(selProperties[i]));
      }

      button->setText(buttonName);
    }
  }
}

void CSVGraphMappingConfigurationWidget::selectSrcProperties() {
  selectProperties("Choose source node properties", srcProperties, ui->srcPropertiesButton);
}

void CSVGraphMappingConfigurationWidget::selectTgtProperties() {
  selectProperties("Choose target node properties", tgtProperties, ui->tgtPropertiesButton);
}

void CSVGraphMappingConfigurationWidget::selectNodeProperties() {
  selectProperties("Choose node identification properties", nodeProperties,
                   ui->nodePropertiesButton);
}

void CSVGraphMappingConfigurationWidget::selectEdgeProperties() {
  selectProperties("Choose edge identification properties", edgeProperties,
                   ui->edgePropertiesButton);
}

void CSVGraphMappingConfigurationWidget::selectColumns(const QString &title,
                                                       std::vector<uint> &columnIds,
                                                       QPushButton *button) {
  vector<string> tmpColumns;
  vector<string> selColumns;

  for (const auto &column : columns) {
    if (!column.empty()) {
      tmpColumns.push_back(column);
    }
  }

  for (uint columnId : columnIds) {
    selColumns.push_back(columns[columnId]);
  }

  if (StringsListSelectionDialog::choose(title, tmpColumns, selColumns, this)) {
    if (selColumns.empty()) {
      columnIds.clear();

      for (uint i = 0; i < columns.size(); ++i) {
        if (!columns[i].empty()) {
          columnIds.push_back(i);
          break;
        }
      }
    } else {
      columnIds.clear();
      QString buttonName;

      for (uint i = 0; i < selColumns.size(); ++i) {
        if (i) {
          buttonName.append(", ");
        }

        buttonName.append(tlpStringToQString(selColumns[i]));

        for (uint j = 0; j < columns.size(); ++j) {
          if (selColumns[i] == columns[j]) {
            columnIds.push_back(j);
            break;
          }
        }
      }

      button->setText(buttonName);
    }
  }
}

void CSVGraphMappingConfigurationWidget::selectNodeColumns() {
  selectColumns("Choose columns for node identifier", nodeColumnIds, ui->nodeColumnsButton);
}

void CSVGraphMappingConfigurationWidget::selectEdgeColumns() {
  selectColumns("Choose columns for edge identifier", edgeColumnIds, ui->edgeColumnsButton);
}

void CSVGraphMappingConfigurationWidget::selectSrcColumns() {
  selectColumns("Choose columns for source", srcColumnIds, ui->srcColumnsButton);
}

void CSVGraphMappingConfigurationWidget::selectTgtColumns() {
  selectColumns("Choose columns for target", tgtColumnIds, ui->tgtColumnsButton);
}
