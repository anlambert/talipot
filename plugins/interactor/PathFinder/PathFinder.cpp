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

#include "PathFinder.h"

#include <talipot/MouseInteractors.h>
#include <talipot/NodeLinkDiagramView.h>

#include "PathFinderComponent.h"
#include "PathFinderConfigurationWidget.h"

#include "../../utils/InteractorIcons.h"

#include <QListWidget>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>

using namespace tlp;
using namespace std;

PLUGIN(PathFinder)

PathFinder::PathFinder(const tlp::PluginContext *)
    : GLInteractorComposite(interactorIcon(InteractorType::PathFinding),
                            "Select the path(s) between two nodes"),
      weightMetric(NO_METRIC), edgeOrientation(DEFAULT_ORIENTATION), pathsTypes(DEFAULT_PATHS_TYPE),
      _configurationWidget(nullptr) {

  edgeOrientationLabels[PathAlgorithm::Directed] = "Consider edges as directed";
  edgeOrientationLabels[PathAlgorithm::Undirected] = "Consider edges as undirected";
  edgeOrientationLabels[PathAlgorithm::Reversed] = "Consider edges as reversed";
  pathsTypesLabels[PathAlgorithm::AllShortest] = "Select all shortest paths";
  pathsTypesLabels[PathAlgorithm::OneShortest] = "Select one of the shortest paths";
}

PathFinder::~PathFinder() {
  delete _configurationWidget;
}

bool PathFinder::isCompatible(const std::string &viewName) const {
  return (viewName == NodeLinkDiagramView::viewName);
}

void PathFinder::construct() {
  if (view() == nullptr) {
    return;
  }

  push_back(new MousePanNZoomNavigator);
  auto *component = new PathFinderComponent(this);
  push_back(component);

  _configurationWidget = new PathFinderConfigurationWidget();

  Graph *g = view()->graph();

  _configurationWidget->addWeightComboItem(NO_METRIC);
  for (const string &s : g->getProperties()) {
    if (g->getProperty(s)->getTypename() == "double") {
      _configurationWidget->addWeightComboItem(s.c_str());
    }
  }
  _configurationWidget->setCurrentweightComboIndex(
      _configurationWidget->weightComboFindText(weightMetric.c_str()));

  for (const auto &it : edgeOrientationLabels) {
    _configurationWidget->addEdgeOrientationComboItem(it.second.c_str());
  }

  _configurationWidget->setCurrentedgeOrientationComboIndex(
      _configurationWidget->edgeOrientationComboFindText(
          edgeOrientationLabels[edgeOrientation].c_str()));

  for (const auto &it : pathsTypesLabels) {
    _configurationWidget->addPathsTypeComboItem(it.second.c_str());
  }

  setPathsType(pathsTypesLabels[pathsTypes].c_str());

  connect(_configurationWidget, &PathFinderConfigurationWidget::setWeightMetric, this,
          &PathFinder::setWeightMetric);
  connect(_configurationWidget, &PathFinderConfigurationWidget::setEdgeOrientation, this,
          &PathFinder::setEdgeOrientation);
  connect(_configurationWidget, &PathFinderConfigurationWidget::setPathsType, this,
          &PathFinder::setPathsType);
}

QWidget *PathFinder::configurationWidget() const {
  return _configurationWidget;
}

void PathFinder::setWeightMetric(const QString &metric) {
  weightMetric = QStringToTlpString(metric);
}

void PathFinder::setEdgeOrientation(const QString &metric) {
  string cmp(QStringToTlpString(metric));

  for (const auto &it : edgeOrientationLabels) {
    if (it.second == cmp) {
      edgeOrientation = it.first;
    }
  }
}

void PathFinder::setPathsType(const QString &pathType) {
  string pathTypeStr = QStringToTlpString(pathType);

  for (const auto &it : pathsTypesLabels) {
    if (it.second == pathTypeStr) {
      pathsTypes = it.first;
      break;
    }
  }
}

PathFinderComponent *PathFinder::getPathFinderComponent() {
  // Look upon all the installed components and stop as soon as we get a PathFinderComponent *
  // object.
  for (auto *ic : *this) {
    auto *pfc = dynamic_cast<PathFinderComponent *>(ic);

    if (pfc) {
      return pfc;
    }
  }

  return nullptr;
}
