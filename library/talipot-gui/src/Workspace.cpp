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

#include "talipot/Workspace.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QRegularExpression>
#include <QTimer>
#include <QXmlStreamWriter>

#include <talipot/MetaTypes.h>
#include <talipot/View.h>
#include <talipot/WorkspacePanel.h>
#include <talipot/Project.h>
#include <talipot/Mimes.h>
#include <talipot/GraphHierarchiesModel.h>
#include <talipot/FontIconManager.h>

#include "ui_Workspace.h"

using namespace tlp;

/*
  Helper storage class to ensure synchronization between panels list and model passed down to opened
  panels
  */
Workspace::Workspace(QWidget *parent)
    : QWidget(parent), _ui(new Ui::Workspace), _currentPanelIndex(0), _oldWorkspaceMode(nullptr),
      _focusedPanel(nullptr), _focusedPanelHighlighting(false), _model(nullptr),
      _autoCenterViews(false) {
  _ui->setupUi(this);
  _ui->nextPageButton->setVisible(false);
  _ui->previousPageButton->setVisible(false);
  _ui->importButton->setIcon(FontIconManager::icon(MaterialDesignIcons::Import));
  _ui->startupButton->setIcon(FontIconManager::icon(MaterialDesignIcons::PlusBox));
  _ui->addPanelButton->setIcon(
      FontIconManager::icon(MaterialDesignIcons::PlusBox, QColor(Qt::white)));
  _ui->exposeButton->setIcon(
      FontIconManager::icon(MaterialDesignIcons::ViewGridOutline, QColor(Qt::white)));
  _ui->startupMainFrame->hide();
  _ui->workspaceContents->setCurrentWidget(_ui->startupPage);
  connect(_ui->addPanelButton, &QAbstractButton::clicked, [this] { addPanelRequest(); });
  connect(_ui->startupButton, &QPushButton::clicked, [this] { addPanelRequest(); });
  connect(_ui->importButton, &QAbstractButton::clicked, this, &Workspace::importGraphRequest);
  connect(_ui->exposeMode, &WorkspaceExposeWidget::exposeFinished, this,
          &Workspace::hideExposeMode);

  setToolTipWithCtrlShortcut(_ui->exposeButton, "Toggle the Expose mode", "E");

  setToolTipWithCtrlShortcut(_ui->previousPageButton, "Show previous panel", "Shift+Left");
  setToolTipWithCtrlShortcut(_ui->nextPageButton, "Show next panel", "Shift+Right");
  _ui->singleModeButton->setToolTip("Switch to 1 panel mode");
  _ui->splitModeButton->setToolTip("Switch to 2 panels mode");
  _ui->splitHorizontalModeButton->setToolTip("Switch to 2 panels mode");
  _ui->split3ModeButton->setToolTip("Switch to 2-top 1-bottom panels mode");
  _ui->split32ModeButton->setToolTip("Switch to 1-left 2-right panels mode");
  _ui->split33ModeButton->setToolTip("Switch to 2-left 1-right panels mode");
  _ui->gridModeButton->setToolTip("Switch to 4 panels mode");
  _ui->sixModeButton->setToolTip("Switch to 6 panels mode");

  // This map allows us to know how much slots we have for each mode and which widget corresponds to
  // those slots
  _modeToSlots[_ui->startupPage] = {};
  _modeToSlots[_ui->singlePage] = {_ui->singlePage};
  _modeToSlots[_ui->splitPage] = {_ui->splitPagePanel1, _ui->splitPagePanel2};
  _modeToSlots[_ui->splitPageHorizontal] = {_ui->splitPageHorizontalPanel1,
                                            _ui->splitPageHorizontalPanel2};
  _modeToSlots[_ui->split3Page] = {_ui->split3PagePanel1, _ui->split3PagePanel2,
                                   _ui->split3PagePanel3};
  _modeToSlots[_ui->split32Page] = {_ui->split32Panel1, _ui->split32Panel2, _ui->split32Panel3};
  _modeToSlots[_ui->split33Page] = {_ui->split33Panel1, _ui->split33Panel2, _ui->split33Panel3};
  _modeToSlots[_ui->gridPage] = {_ui->gridPagePanel1, _ui->gridPagePanel2, _ui->gridPagePanel3,
                                 _ui->gridPagePanel4};
  _modeToSlots[_ui->sixPage] = {_ui->sixMode1, _ui->sixMode2, _ui->sixMode3,
                                _ui->sixMode4, _ui->sixMode5, _ui->sixMode6};

  // This map allows us to know which widget can toggle a mode
  _modeSwitches[_ui->singlePage] = _ui->singleModeButton;
  _modeSwitches[_ui->splitPage] = _ui->splitModeButton;
  _modeSwitches[_ui->splitPageHorizontal] = _ui->splitHorizontalModeButton;
  _modeSwitches[_ui->split3Page] = _ui->split3ModeButton;
  _modeSwitches[_ui->split32Page] = _ui->split32ModeButton;
  _modeSwitches[_ui->split33Page] = _ui->split33ModeButton;
  _modeSwitches[_ui->gridPage] = _ui->gridModeButton;
  _modeSwitches[_ui->sixPage] = _ui->sixModeButton;
  updateAvailableModes();
}

Workspace::~Workspace() {
  for (auto *p : _panels) {
    disconnect(p, &QObject::destroyed, this, &Workspace::panelDestroyed);
    delete p;
  }

  delete _ui;
}

void Workspace::setModel(tlp::GraphHierarchiesModel *model) {
  if (_model != nullptr) {
    disconnect(_model, &GraphHierarchiesModel::currentGraphChanged, this,
               &Workspace::updateStartupMode);
  }

  _model = model;

  if (_model != nullptr) {
    for (auto *panel : _panels) {
      panel->setGraphsModel(_model);
    }

    connect(_model, &GraphHierarchiesModel::currentGraphChanged, this,
            &Workspace::updateStartupMode);
  }
}

tlp::GraphHierarchiesModel *Workspace::graphModel() const {
  return _model;
}

void Workspace::closeAll() {
  // if expose mode activated, close it before closing views to prevent a crash
  hideExposeMode();

  // use a _panels copy to get a stable iteration because
  // each panel destroyed signal is connected to panelDestroyed slot
  // which modifies the _panels vector
  auto panels = _panels;

  for (auto *p : panels) {
    delete p;
  }
  _panels.clear();
}

QList<tlp::View *> Workspace::panels() const {
  QList<tlp::View *> result;

  for (auto *panel : _panels) {
    result.push_back(panel->view());
  }

  return result;
}

QString Workspace::panelTitle(tlp::WorkspacePanel *panel) const {
  int digit = 0;

  static QRegularExpression regExp("^.*(?:<([^>])*>){1}$");

  for (auto *other : _panels) {
    if (other == panel) {
      continue;
    }

    if (other->viewName() == panel->viewName()) {
      QRegularExpressionMatch match;
      if (other->windowTitle().indexOf(regExp, 0, &match) != -1) {
        digit = std::max<int>(digit, match.captured(1).toInt());
      } else {
        digit = std::max<int>(digit, 1);
      }
    }
  }

  if (digit == 0) {
    return panel->viewName();
  }

  return panel->viewName() + " <" + QString::number(digit + 1) + ">";
}

int Workspace::addPanel(tlp::View *view) {
  auto *panel = new WorkspacePanel(view);

  if (_model != nullptr) {
    panel->setGraphsModel(_model);
  }

  panel->setWindowTitle(panelTitle(panel));
  connect(panel, &WorkspacePanel::drawNeeded, this, &Workspace::viewNeedsDraw);
  connect(panel, &WorkspacePanel::swapWithPanels, this, &Workspace::swapPanelsRequested);
  connect(panel, &QObject::destroyed, this, &Workspace::panelDestroyed);
  view->graphicsView()->installEventFilter(this);

  // Add it to the list
  _panels.push_back(panel);

  // activate available modes
  updateAvailableModes();

  // If on startup mode, switch to single
  if (currentModeWidget() == _ui->startupPage) {
    switchToSingleMode();
  } else {
    updatePanels();
  }

  // Force the first panel's graph combo box update when underlying model has been updated.
  panel->viewGraphSet(view->graph());
  setFocusedPanel(panel);
  // Slightly delay view content centering as the panel widget
  // can take some time to get correctly resized in the workspace
  QTimer::singleShot(100, [view] { view->centerView(); });
  return _panels.size() - 1;
}

void Workspace::delView(tlp::View *view) {
  for (auto *it : _panels) {
    if (it->view() == view) {
      delete it;
      _panels.removeOne(it);
      return;
    }
  }
}

void Workspace::panelDestroyed(QObject *obj) {
  auto *panel = static_cast<WorkspacePanel *>(obj);

  if (panel == _focusedPanel) {
    _focusedPanel = nullptr;
  }

  int removeCount = _panels.removeAll(panel);

  if (removeCount == 0) {
    return;
  }

  // To prevent segfaults due to Qt's event queue handling when deleting views, we reset the
  // placeholder widget that contained this panel
  for (auto *mode : _modeToSlots.keys()) {
    for (auto *p : _modeToSlots[mode]) {
      if (p->widget() == panel) {
        p->resetWidget();
      }
    }
  }

  if (currentModeWidget() == _ui->exposePage) {
    return;
  }

  updateAvailableModes();

  updatePanels();
}

void Workspace::viewNeedsDraw() {
  auto *panel = static_cast<WorkspacePanel *>(sender());

  if (_autoCenterViews) {
    // we assume graph has changed
    panel->view()->centerView(true);
  } else {
    panel->view()->draw();
  }
}

void Workspace::switchToStartupMode() {
  switchWorkspaceMode(_ui->startupPage);
}
void Workspace::switchToSingleMode() {
  switchWorkspaceMode(_ui->singlePage);
}

void Workspace::setSingleModeSwitch(QWidget *w) {
  _modeSwitches[_ui->singlePage] = w;
}

void Workspace::switchToSplitMode() {
  switchWorkspaceMode(_ui->splitPage);
}

void Workspace::setSplitModeSwitch(QWidget *w) {
  _modeSwitches[_ui->splitPage] = w;
}

void Workspace::switchToSplitHorizontalMode() {
  switchWorkspaceMode(_ui->splitPageHorizontal);
}

void Workspace::setSplitHorizontalModeSwitch(QWidget *w) {
  _modeSwitches[_ui->splitPageHorizontal] = w;
}

void Workspace::switchToSplit3Mode() {
  switchWorkspaceMode(_ui->split3Page);
}

void Workspace::setSplit3ModeSwitch(QWidget *w) {
  _modeSwitches[_ui->split3Page] = w;
}
void Workspace::switchToSplit32Mode() {
  switchWorkspaceMode(_ui->split32Page);
}

void Workspace::setSplit32ModeSwitch(QWidget *w) {
  _modeSwitches[_ui->split32Page] = w;
}
void Workspace::switchToSplit33Mode() {
  switchWorkspaceMode(_ui->split33Page);
}

void Workspace::setSplit33ModeSwitch(QWidget *w) {
  _modeSwitches[_ui->split33Page] = w;
}
void Workspace::switchToGridMode() {
  switchWorkspaceMode(_ui->gridPage);
}

void Workspace::setGridModeSwitch(QWidget *w) {
  _modeSwitches[_ui->gridPage] = w;
}

void Workspace::switchToSixMode() {
  switchWorkspaceMode(_ui->sixPage);
}

void Workspace::setSixModeSwitch(QWidget *w) {
  _modeSwitches[_ui->sixPage] = w;
}

void Workspace::switchWorkspaceMode(QWidget *page) {
  if (currentModeWidget() == page) {
    return;
  }

  _ui->workspaceContents->setCurrentWidget(page);
  if (_model) {
    setBottomFrameEnabled(!_model->empty());
  }
  updateStartupMode();
  updatePanels();
}

void Workspace::updatePageCountLabel() {
  int current = _currentPanelIndex + 1;
  int total = _panels.size() - currentSlotsCount() + 1;

  _ui->pagesLabel->setText(QString::number(current) + " / " + QString::number(total));
}

QWidget *Workspace::currentModeWidget() const {
  return _ui->workspaceContents->currentWidget();
}

QVector<PlaceHolderWidget *> Workspace::currentModeSlots() const {
  return _modeToSlots[currentModeWidget()];
}

uint Workspace::currentSlotsCount() const {
  return currentModeSlots().size();
}

void Workspace::updateAvailableModes() {
  for (auto *page : _modeSwitches.keys()) {
    _modeSwitches[page]->setVisible(_panels.size() >= _modeToSlots[page].size());
    _modeSwitches[page]->setEnabled(_panels.size() >= _modeToSlots[page].size());
  }

  bool enableNavigation = !_panels.empty();
  _ui->nextPageButton->setVisible(enableNavigation);
  _ui->previousPageButton->setVisible(enableNavigation);
  _ui->nextPageButton->setEnabled(enableNavigation);
  _ui->previousPageButton->setEnabled(enableNavigation);
  _ui->exposeButton->setEnabled(enableNavigation);
  if (_model) {
    _ui->addPanelButton->setEnabled(!_model->empty());
  }
}

void Workspace::updatePanels() {
  for (auto *mode : _modeToSlots.keys()) {
    if (mode == currentModeWidget()) {
      continue;
    }

    QVector<PlaceHolderWidget *> panelSlots = _modeToSlots[mode];

    for (auto *panel : panelSlots) {
      panel->setWidget(nullptr);
    }
  }

  if (_currentPanelIndex < 0) {
    _currentPanelIndex = 0;
  }

  if (uint(_currentPanelIndex) > uint(_panels.size() - currentSlotsCount())) {
    _currentPanelIndex = _panels.size() - currentSlotsCount();
  }

  // Fill up slots according to the current index until there is no panel to show
  int i = _currentPanelIndex;

  for (auto *slt : currentModeSlots()) {
    if (i >= _panels.size()) {
      slt->setWidget(nullptr);
    } else if (slt->widget() != _panels[i]) {
      slt->setWidget(nullptr);
    }

    i++;
  }

  i = _currentPanelIndex;

  for (auto *slt : currentModeSlots()) {
    if (i >= _panels.size()) {
      break;
    } else if (slt->widget() != _panels[i]) {
      slt->setWidget(_panels[i]);
      _panels[i]->resetInteractorsScrollButtonsVisibility();
      _panels[i]->updateCurrentInteractorButtonText();
    }

    i++;
  }

  if (currentModeWidget() == _ui->startupPage) {
    updatePageCountLabel();
    return;
  }

  if (!_modeSwitches[currentModeWidget()]->isEnabled()) {
    int maxSize = 0;
    QWidget *fallbackMode = _ui->startupPage;

    // Current mode is not available, fallback to the largest available mode
    for (auto *it : _modeToSlots.keys()) {
      if (_panels.size() >= _modeToSlots[it].size() && _modeToSlots[it].size() > maxSize) {
        maxSize = _modeToSlots[it].size();
        fallbackMode = it;
      }
    }

    switchWorkspaceMode(fallbackMode);
  }

  updatePageCountLabel();
}

void Workspace::nextPage() {
  if (_ui->nextPageButton->isEnabled()) {
    _currentPanelIndex++;
    updatePanels();
  }
}

void Workspace::previousPage() {
  if (_ui->previousPageButton->isEnabled()) {
    _currentPanelIndex--;
    updatePanels();
  }
}

void Workspace::setExposeModeSwitch(QPushButton *b) {
  _ui->exposeButton = b;
}

void Workspace::setActivePanel(tlp::View *view) {
  int newIndex = panels().indexOf(view);
  _currentPanelIndex = newIndex;
  updatePanels();
}

void Workspace::setGraphForFocusedPanel(tlp::Graph *g) {
  if (_focusedPanel && _focusedPanel->isGraphSynchronized() &&
      _focusedPanel->view()->graph() != g) {
    _focusedPanel->view()->setGraph(g);
  }
}

WorkspacePanel *Workspace::panelForScene(QObject *obj) {
  WorkspacePanel *p = nullptr;

  for (auto *panel : _panels) {
    if (panel->view()->graphicsView()->scene() == obj) {
      p = panel;
      break;
    }
  }

  return p;
}

bool Workspace::eventFilter(QObject *obj, QEvent *ev) {
  if (ev->type() == QEvent::ChildRemoved) {
    QObject *childObj = static_cast<QChildEvent *>(ev)->child();
    childObj->removeEventFilter(this);
    auto *graphicsView = dynamic_cast<QGraphicsView *>(childObj);

    if (graphicsView != nullptr && graphicsView->scene()) {
      graphicsView->scene()->removeEventFilter(this);
    }
  } else if (ev->type() == QEvent::FocusIn) {
    if (dynamic_cast<QGraphicsView *>(obj) != nullptr) {
      auto *panel = static_cast<tlp::WorkspacePanel *>(obj->parent());
      setFocusedPanel(panel);
    }
  }

  return false;
}

void Workspace::dragEnterEvent(QDragEnterEvent *event) {
  handleDragEnterEvent(event, event->mimeData());
}

void Workspace::dropEvent(QDropEvent *event) {
  handleDropEvent(event->mimeData());
}

bool Workspace::handleDragEnterEvent(QEvent *e, const QMimeData *mimedata) {
  if (dynamic_cast<const GraphMimeType *>(mimedata) != nullptr) {
    e->accept();
    return true;
  }

  return false;
}

bool Workspace::handleDropEvent(const QMimeData *mimedata) {
  const auto *graphMime = dynamic_cast<const GraphMimeType *>(mimedata);

  if (graphMime == nullptr) {
    return false;
  }

  if (graphMime != nullptr && graphMime->graph()) {
    emit(addPanelRequest(graphMime->graph()));
    return true;
  }

  return false;
}

void Workspace::expose(bool f) {
  if (f) {
    showExposeMode();
  } else {
    hideExposeMode();
  }
}

void Workspace::showExposeMode() {
  if (_ui->workspaceContents->currentWidget() == _ui->exposePage) {
    return;
  }

  _oldWorkspaceMode = currentModeWidget();

  for (auto *s : _modeSwitches.values()) {
    s->hide();
  }

  _ui->nextPageButton->setEnabled(false);
  _ui->previousPageButton->setEnabled(false);

  QVector<WorkspacePanel *> panels;

  for (auto *p : _panels) {
    panels << p;
  }

  _ui->exposeMode->setData(panels, _currentPanelIndex);
  _ui->workspaceContents->setCurrentWidget(_ui->exposePage);
}

void Workspace::uncheckExposeButton() {
  _ui->exposeButton->setChecked(false);
}

void Workspace::hideExposeMode() {
  if (currentModeWidget() != _ui->exposePage) {
    return;
  }

  _ui->exposeButton->setChecked(false);
  QVector<WorkspacePanel *> newPanels = _ui->exposeMode->panels();
  _panels.clear();

  for (auto *p : newPanels) {
    _panels.push_back(p);
  }

  _currentPanelIndex = _ui->exposeMode->currentPanelIndex();

  if (!_ui->exposeMode->isSwitchToSingleMode()) {
    switchWorkspaceMode(suitableMode(_oldWorkspaceMode));
  } else {
    updateAvailableModes();
    switchToSingleMode();
  }

  updatePageCountLabel();
}

QWidget *Workspace::suitableMode(QWidget *oldMode) {
  updateAvailableModes();

  if (_modeSwitches.contains(oldMode) && _modeSwitches[oldMode]->isEnabled()) {
    return oldMode;
  }

  int maxSlots = 0;
  QWidget *result = _ui->startupPage;

  for (auto *mode : _modeToSlots.keys()) {
    int slotCount = _modeToSlots[mode].size();

    if (slotCount <= _panels.size() && slotCount > maxSlots) {
      maxSlots = slotCount;
      result = mode;
    }
  }

  return result;
}

/*
  Project serialization
  */
void Workspace::writeProject(Project *project, QMap<Graph *, QString> rootIds,
                             tlp::PluginProgress *progress) {
  project->removeAllDir("views");
  int i = 0;

  for (auto *v : panels()) {
    progress->progress(i, panels().size());
    QString path = "views/" + QString::number(i);
    project->mkpath(path);
    // get view state. Do this before the rest in case state() changes some parameters inside the
    // view
    std::stringstream dataStr;
    DataSet::write(dataStr, v->state());

    Graph *g = v->graph();
    QIODevice *viewDescFile = project->fileStream(path + "/view.xml");
    QXmlStreamWriter doc(viewDescFile);
    doc.setAutoFormatting(true);
    doc.writeStartElement("view");
    doc.writeAttribute("name", tlpStringToQString(v->name()));
    doc.writeAttribute("root", rootIds[g->getRoot()]);
    doc.writeAttribute("id", QString::number(g->getId()));
    doc.writeTextElement("data", tlpStringToQString(dataStr.str()));
    doc.writeEndDocument();
    viewDescFile->close();
    delete viewDescFile;
    ++i;
  }

  QIODevice *workspaceXml =
      project->fileStream("/workspace.xml", QIODevice::Truncate | QIODevice::WriteOnly);
  QXmlStreamWriter doc(workspaceXml);
  doc.writeStartElement("workspace");
  doc.writeAttribute("current", QString::number(_currentPanelIndex));
  doc.writeAttribute("mode", QString::number(currentSlotsCount()));

  if (currentModeWidget() == _ui->splitPage) {
    doc.writeAttribute("modeWidget", "splitPage");
  } else if (currentModeWidget() == _ui->splitPageHorizontal) {
    doc.writeAttribute("modeWidget", "splitPageHorizontal");
  } else if (currentModeWidget() == _ui->split3Page) {
    doc.writeAttribute("modeWidget", "split3Page");
  } else if (currentModeWidget() == _ui->split32Page) {
    doc.writeAttribute("modeWidget", "split32Page");
  } else if (currentModeWidget() == _ui->split33Page) {
    doc.writeAttribute("modeWidget", "split33Page");
  }

  doc.writeEndDocument();
  workspaceXml->close();
  delete workspaceXml;
}

void Workspace::readProject(Project *project, QMap<QString, Graph *> rootIds,
                            PluginProgress *progress) {
  QStringList entries = project->entryList("views", QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
  int step = 0, max_step = entries.size();

  for (const QString &entry : entries) {
    progress->progress(step++, max_step);
    QIODevice *xmlFile = project->fileStream("views/" + entry + "/view.xml");
    QXmlStreamReader doc(xmlFile);

    if (doc.readNextStartElement()) {
      if (!doc.hasError()) {
        QString viewName = doc.attributes().value("name").toString();
        QString rootId = doc.attributes().value("root").toString();
        QString id = doc.attributes().value("id").toString();
        doc.readNextStartElement();
        QString data(doc.readElementText());
        xmlFile->close();
        delete xmlFile;

        // backward compatibility with old view names
        if (viewName.endsWith(" view")) {
          viewName = viewName.mid(0, viewName.length() - 5);
        }
        if (viewName == "Adjacency Matrix") {
          viewName = "Matrix";
        }

        View *view = PluginsManager::getPluginObject<View>(QStringToTlpString(viewName));

        if (view == nullptr) {
          continue;
        }

        view->setupUi();
        Graph *rootGraph = rootIds[rootId];
        assert(rootGraph);
        Graph *g = rootGraph->getDescendantGraph(id.toInt());

        if (g == nullptr) {
          g = rootGraph;
        }

        view->setGraph(g);
        DataSet dataSet;
        std::istringstream iss(QStringToTlpString(data));
        DataSet::read(iss, dataSet);
        view->setState(dataSet);
        addPanel(view);
      }
    }
  }

  QIODevice *workspaceXml = project->fileStream("/workspace.xml");

  if (workspaceXml == nullptr) {
    return;
  }

  QXmlStreamReader doc(workspaceXml);

  if (doc.readNextStartElement()) {
    if (!doc.hasError()) {
      int current = doc.attributes().value("current").toString().toInt();
      int mode = doc.attributes().value("mode").toString().toInt();

      for (auto *modeWidget : _modeToSlots.keys()) {
        if (_modeToSlots[modeWidget].size() == mode) {
          if (current > 0 && current < _panels.size()) {
            setActivePanel(_panels[current]->view());
          }

          QString modeWidgetName = doc.attributes().value("modeWidget").toString();

          if (!modeWidgetName.isEmpty() && (mode == 2 || mode == 3)) {
            if (modeWidgetName == "splitPage") {
              switchToSplitMode();
            } else if (modeWidgetName == "splitPageHorizontal") {
              switchToSplitHorizontalMode();
            } else if (modeWidgetName == "split3Page") {
              switchToSplit3Mode();
            } else if (modeWidgetName == "split32Page") {
              switchToSplit32Mode();
            } else {
              switchToSplit33Mode();
            }
          } else {
            switchWorkspaceMode(modeWidget);
          }
        }
      }
    }
  }

  workspaceXml->close();
  delete workspaceXml;
}

void Workspace::setBottomFrameEnabled(bool f) {
  _ui->bottomFrame->setEnabled(f);
}

void Workspace::setPageCountLabel(QLabel *l) {
  _ui->pagesLabel = l;
}

void Workspace::redrawPanels(bool center) {
  for (auto *panel : _panels) {
    if (center) {
      panel->view()->centerView();
    } else {
      panel->view()->draw();
    }
  }
}

void Workspace::setAutoCenterPanelsOnDraw(bool f) {
  _autoCenterViews = f;
}

void Workspace::swapPanelsRequested(WorkspacePanel *panel) {
  auto *sourcePanel = static_cast<WorkspacePanel *>(sender());

  if (sourcePanel) {
    auto pb = _panels.begin();
    std::iter_swap(pb + _panels.indexOf(sourcePanel), pb + _panels.indexOf(panel));
    updatePanels();
  }
}

void Workspace::updateStartupMode() {
  if (currentModeWidget() == _ui->startupPage && _model != nullptr) {
    _ui->startupImportFrame->setVisible(_model->empty());
    _ui->startupMainFrame->setVisible(!_model->empty());
  }
}

// enable/disable highlight of focused panel
void Workspace::setFocusedPanelHighlighting(bool h) {
  _focusedPanelHighlighting = h;

  if (_focusedPanel) {
    _focusedPanel->setHighlightMode(h);
  }
}

// update focused panel
void Workspace::setFocusedPanel(WorkspacePanel *panel) {
  if (_focusedPanel) {
    if (_focusedPanelHighlighting) {
      _focusedPanel->setHighlightMode(false);
    }

    disconnect(_focusedPanel, &WorkspacePanel::changeGraphSynchronization, this,
               &Workspace::changeFocusedPanelSynchronization);
  }

  _focusedPanel = panel;
  connect(_focusedPanel, &WorkspacePanel::changeGraphSynchronization, this,
          &Workspace::changeFocusedPanelSynchronization);

  if (_focusedPanelHighlighting) {
    _focusedPanel->setHighlightMode(true);
  }

  emit panelFocused(panel->view());

  if (_focusedPanel->isGraphSynchronized()) {
    emit focusedPanelSynchronized();
  }
}

void Workspace::changeFocusedPanelSynchronization(bool s) {
  if (s) {
    emit focusedPanelSynchronized();
  }
}
