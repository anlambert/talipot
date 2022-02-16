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

#include "ViewGraphPropertiesSelectionWidget.h"
#include "ui_ViewGraphPropertiesSelectionWidget.h"

using namespace std;

namespace tlp {

ViewGraphPropertiesSelectionWidget::ViewGraphPropertiesSelectionWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::ViewGraphPropertiesSelectionWidget), graph(nullptr),
      lastDataLocation(ElementType::NODE) {
  _ui->setupUi(this);
}

ViewGraphPropertiesSelectionWidget::~ViewGraphPropertiesSelectionWidget() {
  delete _ui;
}

void ViewGraphPropertiesSelectionWidget::enableEdgesButton(const bool enable) {
  _ui->edgesButton->setEnabled(enable);
}

void ViewGraphPropertiesSelectionWidget::setWidgetParameters(
    Graph *graph, vector<string> graphPropertiesTypesFilter) {

  bool notSameGraph = true;

  if (graph == this->graph) {
    notSameGraph = false;
  }

  if (notSameGraph && this->graph != nullptr && graph != nullptr) {
    this->graph->removeListener(this);
  }

  this->graph = graph;

  if (graph != nullptr && notSameGraph) {
    graph->addListener(this);
  }

  this->graphPropertiesTypesFilter = graphPropertiesTypesFilter;

  vector<string> lastSelectedProperties = getSelectedGraphProperties();

  _ui->graphPropertiesSelectionWidget->clearLists();
  _ui->graphPropertiesSelectionWidget->setWidgetParameters(graph, graphPropertiesTypesFilter);

  if (!lastSelectedProperties.empty() && graph) {
    vector<string> stringList;

    for (const auto &prop : lastSelectedProperties) {

      if (graph->existProperty(prop)) {
        stringList.push_back(prop);
      }
    }

    _ui->graphPropertiesSelectionWidget->setOutputPropertiesList(stringList);

    stringList.clear();
    for (const string &propertyName : graph->getProperties()) {
      if (graph->existProperty(propertyName) &&
          std::find(lastSelectedProperties.begin(), lastSelectedProperties.end(), propertyName) ==
              lastSelectedProperties.end()) {
        stringList.push_back(propertyName);
      }
    }
    _ui->graphPropertiesSelectionWidget->setInputPropertiesList(stringList);
  } else {
    this->lastSelectedProperties.clear();
  }
}

vector<string> ViewGraphPropertiesSelectionWidget::getSelectedGraphProperties() {
  return _ui->graphPropertiesSelectionWidget->getSelectedProperties();
}

void ViewGraphPropertiesSelectionWidget::setWidgetEnabled(const bool enabled) {
  _ui->groupBox->setEnabled(enabled);
  _ui->groupBox2->setEnabled(enabled);
}

void ViewGraphPropertiesSelectionWidget::setSelectedProperties(vector<string> selectedProperties) {
  if (!graph) {
    return;
  }

  vector<string> stringList = iteratorVector(graph->getProperties());
  vector<string> finalStringList;
  _ui->graphPropertiesSelectionWidget->clearLists();

  vector<string> selectedPropertiesCopy(selectedProperties);

  for (const auto &s : stringList) {
    if (std::find(selectedProperties.begin(), selectedProperties.end(), s) !=
        selectedProperties.end()) {
      finalStringList.push_back(selectedPropertiesCopy.front());
      selectedPropertiesCopy.erase(remove(selectedPropertiesCopy.begin(),
                                          selectedPropertiesCopy.end(),
                                          selectedPropertiesCopy.front()),
                                   selectedPropertiesCopy.end());
    } else {
      finalStringList.push_back(s);
    }
  }

  _ui->graphPropertiesSelectionWidget->setInputPropertiesList(finalStringList);
  _ui->graphPropertiesSelectionWidget->setOutputPropertiesList(selectedProperties);
}

ElementType ViewGraphPropertiesSelectionWidget::getDataLocation() const {
  if (_ui->nodesButton->isChecked()) {
    return ElementType::NODE;
  } else {
    return ElementType::EDGE;
  }
}

void ViewGraphPropertiesSelectionWidget::setDataLocation(const ElementType location) {
  if (location == ElementType::NODE) {
    _ui->edgesButton->setChecked(false);
    _ui->nodesButton->setChecked(true);
  } else {
    _ui->edgesButton->setChecked(true);
    _ui->nodesButton->setChecked(false);
  }

  lastDataLocation = location;
}

void ViewGraphPropertiesSelectionWidget::treatEvent(const Event &evt) {
  const auto *graphEvent = dynamic_cast<const GraphEvent *>(&evt);

  if (graphEvent) {
    switch (graphEvent->getType()) {

    case GraphEventType::TLP_ADD_LOCAL_PROPERTY:
    case GraphEventType::TLP_AFTER_DEL_LOCAL_PROPERTY:
    case GraphEventType::TLP_AFTER_RENAME_LOCAL_PROPERTY: {
      setWidgetParameters(graph, graphPropertiesTypesFilter);
      break;
    }

    default:
      break;
    }
  }
}

bool ViewGraphPropertiesSelectionWidget::configurationChanged() {
  ElementType dataLocation = getDataLocation();

  if (lastDataLocation != dataLocation) {
    lastDataLocation = dataLocation;
    return true;
  }

  vector<string> selectedProperties = getSelectedGraphProperties();

  if (selectedProperties.size() != lastSelectedProperties.size()) {
    lastSelectedProperties = selectedProperties;
    return true;
  }

  bool sameSelectedProperties = true;
  auto itLast = lastSelectedProperties.begin();

  for (const auto &prop : selectedProperties) {
    if (prop != (*itLast)) {
      sameSelectedProperties = false;
      break;
    }

    ++itLast;
  }

  if (!sameSelectedProperties) {
    lastSelectedProperties = selectedProperties;
    return true;
  }

  return false;
}
}
