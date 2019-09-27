/**
 *
 * Copyright (C) 2019  The Talipot developers
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
#include <talipot/View.h>
#include <talipot/StringsListSelectionWidget.h>
#include <talipot/NodeLinkDiagramComponent.h>
#include <talipot/Graph.h>
#include <talipot/TlpQtTools.h>

#include "PathFinderComponent.h"
#include "PathFinderConfigurationWidget.h"
#include "highlighters/EnclosingCircleHighlighter.h"
#include "highlighters/ZoomAndPanHighlighter.h"

#include <QListWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>

using namespace tlp;
using namespace std;

PLUGIN(PathFinder)

PathFinder::PathFinder(const tlp::PluginContext *)
    : GLInteractorComposite(QIcon(":/pathfinder.png"), "Select the path(s) between two nodes"),
      weightMetric(NO_METRIC), selectAllPaths(false), edgeOrientation(DEFAULT_ORIENTATION),
      pathsTypes(DEFAULT_PATHS_TYPE), toleranceActivated(DEFAULT_TOLERANCE_ACTIVATION),
      tolerance(DEFAULT_TOLERANCE), _configurationWidget(nullptr), highlightersListWidget(nullptr),
      configureHighlighterBtn(nullptr) {

  edgeOrientationLabels[PathAlgorithm::Directed] = "Consider edges as directed";
  edgeOrientationLabels[PathAlgorithm::Undirected] = "Consider edges as undirected";
  edgeOrientationLabels[PathAlgorithm::Reversed] = "Consider edges as reversed";
  pathsTypesLabels[PathAlgorithm::AllPaths] = "Select all paths";
  pathsTypesLabels[PathAlgorithm::AllShortest] = "Select all shortest paths";
  pathsTypesLabels[PathAlgorithm::OneShortest] = "Select one of the shortest paths";
}

PathFinder::~PathFinder() {
  delete _configurationWidget;
}

bool PathFinder::isCompatible(const std::string &viewName) const {
  return (viewName == NodeLinkDiagramComponent::viewName);
}

void PathFinder::construct() {
  if (view() == nullptr)
    return;

  push_back(new MousePanNZoomNavigator);
  PathFinderComponent *component = new PathFinderComponent(this);
  // installing path highlighters on the component
  component->addHighlighter(new EnclosingCircleHighlighter);
  component->addHighlighter(new ZoomAndPanHighlighter);
  push_back(component);

  _configurationWidget = new PathFinderConfigurationWidget();

  Graph *g = view()->graph();

  _configurationWidget->addweightComboItem(NO_METRIC);
  for (const string &s : g->getProperties()) {
    if (g->getProperty(s)->getTypename().compare("double") == 0)
      _configurationWidget->addweightComboItem(s.c_str());
  }
  _configurationWidget->setCurrentweightComboIndex(
      _configurationWidget->weightComboFindText(weightMetric.c_str()));

  for (map<PathAlgorithm::EdgeOrientation, string>::iterator it = edgeOrientationLabels.begin();
       it != edgeOrientationLabels.end(); ++it)
    _configurationWidget->addedgeOrientationComboItem(it->second.c_str());

  _configurationWidget->setCurrentedgeOrientationComboIndex(
      _configurationWidget->edgeOrientationComboFindText(
          edgeOrientationLabels[edgeOrientation].c_str()));

  for (map<PathAlgorithm::PathType, string>::iterator it = pathsTypesLabels.begin();
       it != pathsTypesLabels.end(); ++it)
    _configurationWidget->addpathsTypeComboItem(it->second.c_str());

  setPathsType(pathsTypesLabels[pathsTypes].c_str());

  _configurationWidget->toleranceChecked(toleranceActivated);
  _configurationWidget->setToleranceSpinValue(tolerance);

  highlightersListWidget = new StringsListSelectionWidget(
      _configurationWidget, StringsListSelectionWidget::SIMPLE_LIST, 0);
  vector<string> activeList, inactiveList;
  QSet<PathHighlighter *> highlighters(getPathFinderComponent()->getHighlighters());

  for (auto h : highlighters)
    inactiveList.push_back(h->getName());

  highlightersListWidget->setSelectedStringsList(activeList);
  highlightersListWidget->setUnselectedStringsList(inactiveList);

  if (activeList.empty() && inactiveList.empty()) {
    highlightersListWidget->setDisabled(true);
    _configurationWidget->highlightersLabelDisabled(true);
  }

  _configurationWidget->addbottomWidget(highlightersListWidget);
  configureHighlighterBtn = new QPushButton("Configure", _configurationWidget);
  QHBoxLayout *hlLayout = highlightersListWidget->findChild<QHBoxLayout *>("horizontalLayout_2");

  if (hlLayout)
    hlLayout->addWidget(configureHighlighterBtn);

  connect(configureHighlighterBtn, SIGNAL(clicked(bool)), this,
          SLOT(configureHighlighterButtonPressed()));
  connect(_configurationWidget, SIGNAL(setWeightMetric(const QString &)), this,
          SLOT(setWeightMetric(const QString &)));
  connect(_configurationWidget, SIGNAL(setEdgeOrientation(const QString &)), this,
          SLOT(setEdgeOrientation(const QString &)));
  connect(_configurationWidget, SIGNAL(setPathsType(const QString &)), this,
          SLOT(setPathsType(const QString &)));
  connect(_configurationWidget, SIGNAL(activateTolerance(bool)), this,
          SLOT(activateTolerance(bool)));
  connect(_configurationWidget, SIGNAL(setTolerance(int)), this, SLOT(setTolerance(int)));
}

QWidget *PathFinder::configurationWidget() const {
  return _configurationWidget;
}

void PathFinder::setWeightMetric(const QString &metric) {
  weightMetric = QStringToTlpString(metric);
}

void PathFinder::setEdgeOrientation(const QString &metric) {
  string cmp(QStringToTlpString(metric));

  for (map<PathAlgorithm::EdgeOrientation, string>::iterator it = edgeOrientationLabels.begin();
       it != edgeOrientationLabels.end(); ++it) {
    if (it->second.compare(cmp) == 0)
      edgeOrientation = it->first;
  }
}

void PathFinder::setSelectAllPaths(bool s) {
  selectAllPaths = s;
}

void PathFinder::setPathsType(const QString &pathType) {
  string cmp(QStringToTlpString(pathType));

  for (map<PathAlgorithm::PathType, string>::iterator it = pathsTypesLabels.begin();
       it != pathsTypesLabels.end(); ++it) {
    if (it->second.compare(cmp) == 0)
      pathsTypes = it->first;
  }

  bool disabled(pathsTypes != PathAlgorithm::AllPaths);
  _configurationWidget->toleranceDisabled(disabled);
}

double PathFinder::getTolerance() {
  if (!toleranceActivated)
    return DBL_MAX;

  return tolerance / 100;
}

void PathFinder::setTolerance(int i) {
  tolerance = i;
}

void PathFinder::activateTolerance(bool activated) {
  toleranceActivated = activated;
}

vector<string> PathFinder::getActiveHighlighters() {
  return highlightersListWidget->getSelectedStringsList();
}

vector<string> PathFinder::getInactiveHighlighters() {
  return highlightersListWidget->getUnselectedStringsList();
}

vector<string> PathFinder::getHighlighters() {
  if (highlightersListWidget)
    return highlightersListWidget->getCompleteStringsList();

  return vector<string>();
}

void PathFinder::configureHighlighterButtonPressed() {
  /*
   * Each highlighter has it's own configuration widget.
   * We build a QDialog and integrate this widget into it to display highlighter-specific
   * configuration to the user.
   */
  QListWidget *listWidget =
      static_cast<QListWidget *>(highlightersListWidget->findChild<QListWidget *>("listWidget"));

  QList<QListWidgetItem *> lst = listWidget->selectedItems();
  string text("");

  for (QList<QListWidgetItem *>::iterator it = lst.begin(); it != lst.end(); ++it)
    text = QStringToTlpString((*it)->text());

  QSet<PathHighlighter *> highlighters(getPathFinderComponent()->getHighlighters());
  PathHighlighter *hler = nullptr;

  for (auto h : highlighters) {
    if (h->getName() == text) {
      hler = h;
      break;
    }
  }

  if (hler == nullptr) {
    QMessageBox::warning(nullptr, "Nothing selected", "No highlighter selected");
    return;
  }

  if (hler->isConfigurable()) {
    QDialog *dialog = new QDialog;
    QVBoxLayout *verticalLayout = new QVBoxLayout(dialog);
    verticalLayout->setObjectName("verticalLayout");
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setObjectName("mainLayout");

    verticalLayout->addLayout(mainLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
    buttonBox->setObjectName("buttonBox");
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    mainLayout->addWidget(hler->getConfigurationWidget());
    dialog->setWindowTitle(tlpStringToQString(hler->getName()));
    dialog->exec();
    delete dialog;
  } else
    QMessageBox::warning(nullptr, tlpStringToQString(hler->getName()),
                         "No configuration available for this highlighter");
}

PathFinderComponent *PathFinder::getPathFinderComponent() {
  // Look upon all the installed components and stop as soon as we get a PathFinderComponent *
  // object.
  for (iterator it = begin(); it != end(); ++it) {
    PathFinderComponent *c = dynamic_cast<PathFinderComponent *>(*it);

    if (c)
      return c;
  }

  return nullptr;
}
