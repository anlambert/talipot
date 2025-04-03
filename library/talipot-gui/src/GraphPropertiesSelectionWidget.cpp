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

#include "talipot/GraphPropertiesSelectionWidget.h"
#include "talipot/Graph.h"

using namespace std;

namespace tlp {

GraphPropertiesSelectionWidget::GraphPropertiesSelectionWidget(
    QWidget *parent, const StringsListSelectionWidget::ListType &listType,
    const uint maxNbSelectedProperties)
    : StringsListSelectionWidget(parent, listType, maxNbSelectedProperties) {}

GraphPropertiesSelectionWidget::GraphPropertiesSelectionWidget(
    Graph *graph, QWidget *parent, const StringsListSelectionWidget::ListType &listType,
    const uint maxNbSelectedProperties, const std::vector<std::string> &propertiesTypes,
    const bool includeViewProperties)
    : StringsListSelectionWidget(parent, listType, maxNbSelectedProperties), graph(graph),
      propertiesTypes(propertiesTypes), includeViewProperties(includeViewProperties) {
    initWidget();
}

void GraphPropertiesSelectionWidget::setWidgetParameters(
    Graph *graph, const std::vector<std::string> &propertiesTypes,
    const bool includeViewProperties) {
    this->graph = graph;
    this->propertiesTypes = propertiesTypes;
    this->includeViewProperties = includeViewProperties;

    if (graph) {
        initWidget();
    }
}

void GraphPropertiesSelectionWidget::initWidget() {
    vector<string> inputProperties;

    setUnselectedStringsListLabel("Available properties");
    setSelectedStringsListLabel("Selected properties");

    for (const string &propertyName : graph->getProperties()) {

        if (propertySelectable(propertyName)) {
            inputProperties.push_back(propertyName);
        }
    }

    setUnselectedStringsList(inputProperties);
}

bool GraphPropertiesSelectionWidget::propertySelectable(const std::string &propertyName) {
    bool selectProperty = false;

    if (propertiesTypes.size() > 0) {
        string propertyType = graph->getProperty(propertyName)->getTypename();

        if (std::find(propertiesTypes.begin(), propertiesTypes.end(), propertyType) !=
            propertiesTypes.end()) {
            selectProperty = true;
        }
    } else {
        selectProperty = true;
    }

    if (selectProperty && !includeViewProperties && propertyName.find("view") == 0 &&
        propertyName != "viewMetric") {
        selectProperty = false;
    }

    return selectProperty;
}

void GraphPropertiesSelectionWidget::setInputPropertiesList(vector<string> &inputPropertiesList) {
    vector<string> inputPropertiesListFiltered;

    for (const auto &prop : inputPropertiesList) {
        if (propertySelectable(prop)) {
            inputPropertiesListFiltered.push_back(prop);
        }
    }

    setUnselectedStringsList(inputPropertiesListFiltered);
}

void GraphPropertiesSelectionWidget::setOutputPropertiesList(vector<string> &outputPropertiesList) {
    vector<string> outputPropertiesListFiltered;

    for (const auto &prop : outputPropertiesList) {
        if (propertySelectable(prop)) {
            outputPropertiesListFiltered.push_back(prop);
        }
    }

    setSelectedStringsList(outputPropertiesListFiltered);
}

vector<string> GraphPropertiesSelectionWidget::getSelectedProperties() const {
    return getSelectedStringsList();
}

void GraphPropertiesSelectionWidget::clearLists() {
    clearSelectedStringsList();
    clearUnselectedStringsList();
}
}
