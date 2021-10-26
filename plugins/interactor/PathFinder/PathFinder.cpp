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
#include <talipot/StringsListSelectionWidget.h>
#include <talipot/NodeLinkDiagramView.h>

#include "PathFinderComponent.h"
#include "PathFinderConfigurationWidget.h"
#include "highlighters/EnclosingCircleHighlighter.h"
#include "highlighters/ZoomAndPanHighlighter.h"

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
      _configurationWidget(nullptr), highlightersListWidget(nullptr),
      configureHighlighterBtn(nullptr) {

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
  // installing path highlighters on the component
  component->addHighlighter(new EnclosingCircleHighlighter);
  component->addHighlighter(new ZoomAndPanHighlighter);
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

  highlightersListWidget = new StringsListSelectionWidget(
      _configurationWidget, StringsListSelectionWidget::SIMPLE_LIST, 0);
  vector<string> activeList, inactiveList;
  QSet<PathHighlighter *> highlighters(getPathFinderComponent()->getHighlighters());

  for (auto *h : highlighters) {
    inactiveList.push_back(h->getName());
  }

  highlightersListWidget->setSelectedStringsList(activeList);
  highlightersListWidget->setUnselectedStringsList(inactiveList);

  if (activeList.empty() && inactiveList.empty()) {
    highlightersListWidget->setDisabled(true);
    _configurationWidget->highlightersLabelDisabled(true);
  }

  _configurationWidget->addBottomWidget(highlightersListWidget);
  configureHighlighterBtn = new QPushButton("Configure", _configurationWidget);
  auto *hlLayout = highlightersListWidget->findChild<QHBoxLayout *>("horizontalLayout_2");

  if (hlLayout) {
    hlLayout->addWidget(configureHighlighterBtn);
  }

  connect(configureHighlighterBtn, &QAbstractButton::clicked, this,
          &PathFinder::configureHighlighterButtonPressed);
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

vector<string> PathFinder::getActiveHighlighters() {
  return highlightersListWidget->getSelectedStringsList();
}

vector<string> PathFinder::getInactiveHighlighters() {
  return highlightersListWidget->getUnselectedStringsList();
}

vector<string> PathFinder::getHighlighters() {
  if (highlightersListWidget) {
    return highlightersListWidget->getCompleteStringsList();
  }

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
  string text;

  for (auto *item : lst) {
    text = QStringToTlpString(item->text());
  }

  QSet<PathHighlighter *> highlighters(getPathFinderComponent()->getHighlighters());
  PathHighlighter *hler = nullptr;

  for (auto *h : highlighters) {
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
    auto *dialog = new QDialog;
    auto *verticalLayout = new QVBoxLayout(dialog);
    verticalLayout->setObjectName("verticalLayout");
    auto *mainLayout = new QVBoxLayout();
    mainLayout->setObjectName("mainLayout");

    verticalLayout->addLayout(mainLayout);

    auto *buttonBox = new QDialogButtonBox(dialog);
    buttonBox->setObjectName("buttonBox");
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    mainLayout->addWidget(hler->getConfigurationWidget());
    dialog->setWindowTitle(tlpStringToQString(hler->getName()));
    dialog->exec();
    delete dialog;
  } else {
    QMessageBox::warning(nullptr, tlpStringToQString(hler->getName()),
                         "No configuration available for this highlighter");
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
