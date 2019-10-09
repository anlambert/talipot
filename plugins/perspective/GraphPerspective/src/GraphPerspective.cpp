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

#include <talipot/PythonInterpreter.h>
#include <talipot/APIDataBase.h>
#include <talipot/PythonIDE.h>
#include <talipot/PythonCodeEditor.h>
#include "PythonPanel.h"

#include "GraphPerspective.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QCloseEvent>
#include <QClipboard>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>
#include <QTimer>
#include <QVBoxLayout>
#include <QDialog>
#include <QByteArray>
#include <QStatusBar>
#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>

#include <talipot/TlpTools.h>
#include <talipot/ImportModule.h>
#include <talipot/Graph.h>
#include <talipot/ExportModule.h>
#include <talipot/View.h>
#include <talipot/SimplePluginProgressWidget.h>
#include <talipot/GraphHierarchiesModel.h>
#include <talipot/CSVImportWizard.h>
#include <talipot/GraphModel.h>
#include <talipot/GraphTableItemDelegate.h>
#include <talipot/GraphPropertiesModel.h>
#include <talipot/GlMainView.h>
#include <talipot/GlMainWidget.h>
#include <talipot/GlGraphComposite.h>
#include <talipot/Settings.h>
#include <talipot/PluginsManager.h>
#include <talipot/TlpQtTools.h>
#include <talipot/Project.h>
#include <talipot/GraphTools.h>
#include <talipot/ColorScaleConfigDialog.h>
#include <talipot/AboutPage.h>
#include <talipot/ColorScalesManager.h>
#include <talipot/StableIterator.h>

#include "ui_GraphPerspectiveMainWindow.h"

#include "GraphPerspectiveLogger.h"
#include "ImportWizard.h"
#include "ExportWizard.h"
#include "PanelSelectionWizard.h"
#include "GraphHierarchiesEditor.h"
#include "PreferencesDialog.h"

#include <QDebug>

using namespace tlp;
using namespace std;

// checks if it exists a Talipot import plugin that can load the provided file based on its
// extension
static bool talipotCanOpenFile(const QString &path) {
  // Talipot project file does not use import / export plugin
  if (path.endsWith(".tlpx")) {
    return true;
  }

  std::list<std::string> imports = PluginsManager::availablePlugins<ImportModule>();

  for (auto &import : imports) {
    ImportModule *m = PluginsManager::getPluginObject<ImportModule>(import);
    std::list<std::string> fileExtensions(m->allFileExtensions());

    for (auto &ext : fileExtensions) {
      if (path.endsWith(tlpStringToQString(ext))) {
        delete m;
        return true;
      }
    }

    delete m;
  }

  return false;
}

GraphPerspective::GraphPerspective(const tlp::PluginContext *c)
    : Perspective(c), _ui(nullptr), _graphs(new GraphHierarchiesModel(this)),
      _recentDocumentsSettingsKey("perspective/recent_files"), _logger(nullptr) {
  Q_INIT_RESOURCE(GraphPerspective);

  _pythonIDE = nullptr;
  _pythonIDEDialog = nullptr;
}

void GraphPerspective::reserveDefaultProperties() {
  registerReservedProperty("viewColor");
  registerReservedProperty("viewLabelColor");
  registerReservedProperty("viewLabelBorderColor");
  registerReservedProperty("viewLabelBorderWidth");
  registerReservedProperty("viewSize");
  registerReservedProperty("viewLabel");
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

void GraphPerspective::buildRecentDocumentsMenu() {
  _ui->menuOpen_recent_file->clear();

  for (const QString &s : Settings::instance().recentDocuments()) {
    if (!QFileInfo(s).exists() || !talipotCanOpenFile(s))
      continue;

    QAction *action = _ui->menuOpen_recent_file->addAction(
        QIcon(":/talipot/graphperspective/icons/16/archive.png"), s, this, SLOT(openRecentFile()));
    action->setData(s);
  }

  _ui->menuOpen_recent_file->addSeparator();

  for (const QString &s : Settings::instance().value(_recentDocumentsSettingsKey).toStringList()) {
    if (!QFileInfo(s).exists() || !talipotCanOpenFile(s))
      continue;

    QAction *action = _ui->menuOpen_recent_file->addAction(
        QIcon(":/talipot/graphperspective/icons/16/empty-file.png"), s, this,
        SLOT(openRecentFile()));
    action->setData(s);
  }
  _ui->menuOpen_recent_file->setEnabled(!_ui->menuOpen_recent_file->isEmpty());
}

void GraphPerspective::addRecentDocument(const QString &path) {
  QStringList recents = Settings::instance().value(_recentDocumentsSettingsKey).toStringList();

  if (recents.contains(path) || !talipotCanOpenFile(path)) {
    return;
  }

  recents += path;

  if (recents.size() > 10)
    recents.pop_front();

  Settings::instance().setValue(_recentDocumentsSettingsKey, recents);
  Settings::instance().sync();
  buildRecentDocumentsMenu();
}

static void logMsgToStdErr(const QString &msg) {
  if (msg.startsWith("[Python")) {
    // remove quotes around message added by Qt
    QString msgClean = msg.mid(14).mid(2, msg.length() - 17);

    if (msg.startsWith("[PythonStdOut]")) {
      std::cout << QStringToTlpString(msgClean) << std::endl;
    } else {
      std::cerr << QStringToTlpString(msgClean) << std::endl;
    }
  } else {
    std::cerr << QStringToTlpString(msg) << std::endl;
  }
}

void GraphPerspective::updateLogIconsAndCounters() {
  GraphPerspectiveLogger::LogType logType = _logger->getLastLogType();
  QFrame *logIconCounterFrame = nullptr;
  QLabel *logIconLabel = nullptr;
  QLabel *logCounterLabel = nullptr;

  if (logType == GraphPerspectiveLogger::Info) {
    logIconCounterFrame = _ui->loggerFrameInfo;
    logIconLabel = _ui->loggerIconInfo;
    logCounterLabel = _ui->loggerMessageInfo;
  } else if (logType == GraphPerspectiveLogger::Warning) {
    logIconCounterFrame = _ui->loggerFrameWarning;
    logIconLabel = _ui->loggerIconWarning;
    logCounterLabel = _ui->loggerMessageWarning;
  } else if (logType == GraphPerspectiveLogger::Error) {
    logIconCounterFrame = _ui->loggerFrameError;
    logIconLabel = _ui->loggerIconError;
    logCounterLabel = _ui->loggerMessageError;
  } else {
    logIconCounterFrame = _ui->loggerFramePython;
    logIconLabel = _ui->loggerIconPython;
    logCounterLabel = _ui->loggerMessagePython;
  }

  logIconCounterFrame->setVisible(true);
  logIconLabel->setPixmap(_logger->icon(logType));
  logIconLabel->setToolTip("Click here to show/hide the message log window");
  logCounterLabel->setText(QString::number(_logger->countByType(logType)));
  logCounterLabel->setToolTip("Click here to show/hide the message log window");
}

void graphPerspectiveLogger(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  logMsgToStdErr(msg);
  static_cast<GraphPerspective *>(Perspective::instance())->log(type, context, msg);
}

void GraphPerspective::log(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  _logger->log(type, context, msg);
  updateLogIconsAndCounters();
}

GraphPerspective::~GraphPerspective() {
  // uninstall Qt message handler only if it is the current active perspective
  if (Perspective::instance() == this) {
    qInstallMessageHandler(nullptr);
  }

  // ensure all loaded graphs are deleted
  for (auto graph : _graphs->graphs()) {
    delete graph;
  }

  delete _pythonIDEDialog;
  if (Perspective::instance() == this) {
    PythonCodeEditor::deleteStaticResources();
  }

  delete _ui;
}

void GraphPerspective::destroyWorkspace() {
  // disconnect to avoid any possible segfaults when deleting graphs
  disconnect(_graphs, SIGNAL(currentGraphChanged(tlp::Graph *)), this,
             SLOT(currentGraphChanged(tlp::Graph *)));

  // delete the workspace, which causes views deletion, before the graphs
  // to avoid any possible segfaults when closing Talipot
  if (_ui) {
    delete _ui->workspace;
    _ui->workspace = nullptr;
    // more disconnection
    disconnect(_graphs, SIGNAL(currentGraphChanged(tlp::Graph *)), _ui->algorithmRunner,
               SLOT(setGraph(tlp::Graph *)));
  }
}

bool GraphPerspective::terminated() {

  _pythonIDE->savePythonFilesAndWriteToProject(true);
  _pythonIDEDialog->hide();

  if (_graphs->needsSaving() || mainWindow()->isWindowModified()) {
    QString message("The project has been modified (loaded graphs or Python files opened in the "
                    "IDE).\nDo you want to save your changes?");
    QMessageBox::StandardButton answer = QMessageBox::question(
        _mainWindow, "Save", message,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel | QMessageBox::Escape);

    if ((answer == QMessageBox::Yes && !save()) || (answer == QMessageBox::Cancel)) {
      return false;
    }
  }

  // force workspace and views destruction here to avoid hanging on exit
  // when linking against QtWebEngine binaries provided by qt.io
  destroyWorkspace();
  return true;
}

void GraphPerspective::logCleared() {
  _ui->loggerMessageInfo->clear();
  _ui->loggerIconInfo->clear();
  _ui->loggerFrameInfo->setVisible(false);
  _ui->loggerFrameWarning->setVisible(false);
  _ui->loggerFrameError->setVisible(false);
  _ui->loggerFramePython->setVisible(false);
}

void GraphPerspective::findPlugins() {
  _ui->algorithmRunner->findPlugins();
}

bool GraphPerspective::eventFilter(QObject *obj, QEvent *ev) {
  if (ev->type() == QEvent::DragEnter) {
    QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent *>(ev);

    if (dragEvent->mimeData()->hasUrls()) {
      dragEvent->accept();
    }
  }

  if (ev->type() == QEvent::Drop) {
    QDropEvent *dropEvent = static_cast<QDropEvent *>(ev);

    for (const QUrl &url : dropEvent->mimeData()->urls()) {
      open(url.toLocalFile());
    }
  }

  if (obj == _ui->loggerFrame && ev->type() == QEvent::MouseButtonPress)
    showHideLogger();

  if (obj == _mainWindow && _logger &&
      (ev->type() == QEvent::Resize || ev->type() == QEvent::Move ||
       ev->type() == QEvent::WindowStateChange)) {

    if (_logger->anchored()) {
      resetLoggerDialogPosition();
    }
  }

  return false;
}

void GraphPerspective::showLogger() {
  _logger->show();

  static bool firstTime = true;

  if (firstTime) {
    resetLoggerDialogPosition();
    firstTime = false;
  }
}

void GraphPerspective::showHideLogger() {

  if (!_logger->isVisible()) {
    showLogger();
  } else {
    _logger->hide();
  }
}

void GraphPerspective::redrawPanels(bool center) {
  _ui->workspace->redrawPanels(center);
}

class PythonIDEDialog : public QDialog {

  QByteArray _windowGeometry;

public:
  PythonIDEDialog(QWidget *parent, Qt::WindowFlags flags) : QDialog(parent, flags) {}

protected:
  void showEvent(QShowEvent *e) override {
    QDialog::showEvent(e);

    if (!_windowGeometry.isEmpty()) {
      restoreGeometry(_windowGeometry);
    }
  }

  void closeEvent(QCloseEvent *e) override {
    _windowGeometry = saveGeometry();
    QDialog::closeEvent(e);
  }
};

void GraphPerspective::start(tlp::PluginProgress *progress) {
  reserveDefaultProperties();
  _ui = new Ui::GraphPerspectiveMainWindowData;
  _ui->setupUi(_mainWindow);
  _pythonPanel = new PythonPanel();
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_pythonPanel);
  layout->setContentsMargins(0, 0, 0, 0);
  _ui->pythonPanel->setLayout(layout);
  _pythonIDE = new PythonIDE();
  QVBoxLayout *dialogLayout = new QVBoxLayout();
  dialogLayout->addWidget(_pythonIDE);
  dialogLayout->setContentsMargins(0, 0, 0, 0);
  _pythonIDEDialog = new PythonIDEDialog(nullptr, Qt::Window);
  _pythonIDEDialog->setStyleSheet(_mainWindow->styleSheet());
  _pythonIDEDialog->setWindowIcon(_mainWindow->windowIcon());
  _pythonIDEDialog->setLayout(dialogLayout);
  _pythonIDEDialog->resize(800, 600);
  _pythonIDEDialog->setWindowTitle("Talipot Python IDE");

  currentGraphChanged(nullptr);
  // set win/Mac dependent tooltips with ctrl shortcut
  setToolTipWithCtrlShortcut(_ui->exposeModeButton, "Toggle the Expose mode", "E");
  setToolTipWithCtrlShortcut(_ui->searchButton, "Show/hide the graph's elements search panel", "F");
  setToolTipWithCtrlShortcut(_ui->pythonButton,
                             "Show/hide the Python interpreter (Read Eval Print Loop) panel",
                             "Shift+P");
  setToolTipWithCtrlShortcut(_ui->previousPageButton, "Show previous panel", "Shift+Left");
  setToolTipWithCtrlShortcut(_ui->nextPageButton, "Show next panel", "Shift+Right");
  setToolTipWithCtrlShortcut(_ui->actionNewProject, "Open a new  empty Talipot perspective",
                             "Shift+N");
  setToolTipWithCtrlShortcut(
      _ui->actionSave_Project,
      "Save the current project (current graphs with current views) in the attached file", "S");
  setToolTipWithCtrlShortcut(_ui->actionSave_Project_as, "Save Project as a new file name",
                             "Shift+S");
  setToolTipWithCtrlShortcut(_ui->actionImport, "Display the Graph importing wizard", "Shift+O");
  setToolTipWithCtrlShortcut(_ui->actionExit, "Exit from Talipot perspective", "Q");
  setToolTipWithCtrlShortcut(_ui->actionUndo, "Undo the latest update of the current graph", "Z");
  setToolTipWithCtrlShortcut(_ui->actionRedo, "Redo the latest update of the current graph", "Y");
  setToolTipWithCtrlShortcut(_ui->actionCut,
                             "Move the selected elements of the current graph into the "
                             "clipboard (the selected edges ends are selected too)",
                             "X");
  setToolTipWithCtrlShortcut(_ui->actionCopy,
                             "Copy the selected elements of the current graph into the "
                             "clipboard (the selected edges ends are selected too)",
                             "C");
  setToolTipWithCtrlShortcut(_ui->actionPaste,
                             "Paste the clipboard elements into the current graph", "V");
  setToolTipWithCtrlShortcut(_ui->actionSelect_All, "Select all elements of the current graph",
                             "A");
  setToolTipWithCtrlShortcut(_ui->actionInvert_selection,
                             "Invert the selection of the current "
                             "graph elements, deselect if "
                             "selected and select if not selected",
                             "I");
  setToolTipWithCtrlShortcut(_ui->actionCancel_selection,
                             "Deselect all selected elements of the current grap", "Shift+A");
  setToolTipWithCtrlShortcut(_ui->actionGroup_elements,
                             "Create a meta-node representing a newly created subgraph "
                             "containing all selected elements of the current graph",
                             "G");
  setToolTipWithCtrlShortcut(
      _ui->actionCreate_sub_graph,
      "Create a subgraph containing all selected elements of the current graph", "Shift+G");
  setToolTipWithCtrlShortcut(_ui->actionPreferences, "Show Talipot preferences dialog", ",");
  setToolTipWithCtrlShortcut(_ui->actionShowUserDocumentation,
                             "Display the User handbook in a navigator", "?");
  setToolTipWithCtrlShortcut(_ui->actionShowDevelDocumentation,
                             "Display the Developer handbook in a navigator", "D");
  setToolTipWithCtrlShortcut(_ui->actionShowPythonDocumentation,
                             "Display the Talipot python documentation in a navigator", "P");
  setToolTipWithCtrlShortcut(_ui->actionMessages_log, "Show the message log", "M");
  setToolTipWithCtrlShortcut(_ui->actionPython_IDE, "Show the Python IDE", "Alt+P");
  setToolTipWithCtrlShortcut(_ui->actionExport, "Show the Graph exporting wizard", "E");
  setToolTipWithCtrlShortcut(_ui->actionOpen_Project, "Open a graph file", "O");
  setToolTipWithCtrlShortcut(_ui->actionFind_plugins,
                             "Allow to find algorithms in typing text in a search field", "Alt+H");
  setToolTipWithCtrlShortcut(_ui->actionNew_graph, "Create a new empty graph", "N");
  // set portable tooltips
  _ui->undoButton->setToolTip("Undo the latest update of the current graph");
  _ui->redoButton->setToolTip("Redo the latest undone update of the current graph");
  _ui->workspaceButton->setToolTip("Display the existing graph views");
  _ui->developButton->setToolTip(
      "Display the Talipot Python IDE for developing scripts and plugins to "
      "execute on the loaded graphs");
  _ui->loggerMessageInfo->setToolTip("Show/Hide the Messages log panel");
  _ui->loggerMessagePython->setToolTip(_ui->loggerMessageInfo->toolTip());
  _ui->loggerMessageWarning->setToolTip(_ui->loggerMessageInfo->toolTip());
  _ui->loggerMessageError->setToolTip(_ui->loggerMessageInfo->toolTip());
  _ui->exportButton->setToolTip("Display the Graph exporting wizard");
  _ui->csvImportButton->setToolTip("Import data in the current graph using a csv formatted file");
  _ui->importButton->setToolTip("Display the Graph importing wizard");
  _ui->pluginsButton->setToolTip("Display the Plugin center");
  _ui->sidebarButton->setToolTip("Hide Sidebar");
  _ui->menubarButton->setToolTip("Hide Menubar");
  _ui->addPanelButton->setToolTip("Open a new visualization panel on the current graph");
  _ui->singleModeButton->setToolTip("Switch to 1 panel mode");
  _ui->splitModeButton->setToolTip("Switch to 2 panels mode");
  _ui->splitHorizontalModeButton->setToolTip("Switch to 2 panels mode");
  _ui->split3ModeButton->setToolTip("Switch to 2-top 1-bottom panels mode");
  _ui->split32ModeButton->setToolTip("Switch to 1-left 2-right panels mode");
  _ui->split33ModeButton->setToolTip("Switch to 2-left 1-right panels mode");
  _ui->gridModeButton->setToolTip("Switch to 4 panels mode");
  _ui->sixModeButton->setToolTip("Switch to 6 panels mode");
  _ui->menuDelete->setToolTip("Delete elements of the current graph");
  _ui->menuOpen_recent_file->setToolTip(
      "Choose a file to open among the recently opened/saved graphs or projects");
  _ui->actionDelete->setToolTip("Delete the selected elements from the current graph [Del]");
  _ui->actionFull_screen->setToolTip("Display the Talipot perspective in full screen [F11]");
  _ui->actionShow_Menubar->setToolTip("Show/Hide the main menu bar [Ctrl+Shift+M]");
  _ui->actionAbout_us->setToolTip("Display the 'About Talipot' information dialog [F1]");
  _ui->actionPlugins_Center->setToolTip(_ui->pluginsButton->toolTip());
  _ui->actionImport_CSV->setToolTip(_ui->csvImportButton->toolTip());
  _ui->actionSave_graph_to_file->setToolTip("Write the current graph into a file");
  _ui->actionCreate_empty_sub_graph->setToolTip("Create an empty subgraph of the current graph");
  _ui->actionClone_sub_graph->setToolTip(
      "Create a subgraph containing the same elements as the current graph");
  _ui->action_Close_All->setToolTip("Close all opened workspace views");
  _ui->actionColor_scales_management->setToolTip("Manage Talipot color scales");
  _ui->actionMake_selection_a_graph->setToolTip(
      "Add the non selected ends of the selected edges to the current graph selection");
  _ui->actionDelete_from_the_root_graph->setToolTip("Delete the selected elements from the whole "
                                                    "graph hierarchy (not only from the current "
                                                    "graph) [Shift+Del]");
  _ui->actionReverse_selected_edges->setToolTip(
      "Reverse the source and target nodes of the selected edges");
  _ui->actionDelete_all->setToolTip("Delete all elements of the current graph");
  _ui->actionSelect_All_Nodes->setToolTip("Select all nodes of the current graph");
  _ui->actionSelect_All_Edges->setToolTip("Select all edges of the current graph");

  _ui->singleModeButton->setEnabled(false);
  _ui->singleModeButton->hide();
  _ui->workspace->setSingleModeSwitch(_ui->singleModeButton);
  _ui->workspace->setFocusedPanelHighlighting(true);
  _ui->splitModeButton->setEnabled(false);
  _ui->splitModeButton->hide();
  _ui->workspace->setSplitModeSwitch(_ui->splitModeButton);
  _ui->splitHorizontalModeButton->setEnabled(false);
  _ui->splitHorizontalModeButton->hide();
  _ui->workspace->setSplitHorizontalModeSwitch(_ui->splitHorizontalModeButton);
  _ui->split3ModeButton->setEnabled(false);
  _ui->split3ModeButton->hide();
  _ui->workspace->setSplit3ModeSwitch(_ui->split3ModeButton);
  _ui->split32ModeButton->setEnabled(false);
  _ui->split32ModeButton->hide();
  _ui->workspace->setSplit32ModeSwitch(_ui->split32ModeButton);
  _ui->split33ModeButton->setEnabled(false);
  _ui->split33ModeButton->hide();
  _ui->workspace->setSplit33ModeSwitch(_ui->split33ModeButton);
  _ui->gridModeButton->setEnabled(false);
  _ui->gridModeButton->hide();
  _ui->workspace->setGridModeSwitch(_ui->gridModeButton);
  _ui->sixModeButton->setEnabled(false);
  _ui->sixModeButton->hide();
  _ui->workspace->setSixModeSwitch(_ui->sixModeButton);
  _ui->workspace->setPageCountLabel(_ui->pageCountLabel);
  _ui->workspace->setExposeModeSwitch(_ui->exposeModeButton);
  _ui->outputFrame->hide();
  _logger = new GraphPerspectiveLogger(_mainWindow);
  _ui->loggerFrame->installEventFilter(this);
  _mainWindow->installEventFilter(this);
  _mainWindow->setAcceptDrops(true);

  connect(_logger, SIGNAL(cleared()), this, SLOT(logCleared()));
  connect(_logger, SIGNAL(resetLoggerPosition()), this, SLOT(resetLoggerDialogPosition()));

  _colorScalesDialog =
      new ColorScaleConfigDialog(ColorScalesManager::getLatestColorScale(), mainWindow());

  // redirection of various output
  redirectDebugOutputToQDebug();
  redirectWarningOutputToQWarning();
  redirectErrorOutputToQCritical();

  _ui->menuFile->setToolTipsVisible(true);
  _ui->menuEdit->setToolTipsVisible(true);
  _ui->menuHelp->setToolTipsVisible(true);
  _ui->menuWindow->setToolTipsVisible(true);

  Settings::instance().synchronizeViewSettings();

  qInstallMessageHandler(graphPerspectiveLogger);

  connect(_ui->workspaceButton, SIGNAL(clicked()), this, SLOT(workspaceButtonClicked()));
  connect(_ui->workspace, SIGNAL(addPanelRequest(tlp::Graph *)), this,
          SLOT(createPanel(tlp::Graph *)));
  connect(_ui->workspace, SIGNAL(focusedPanelSynchronized()), this,
          SLOT(focusedPanelSynchronized()));
  connect(_graphs, SIGNAL(currentGraphChanged(tlp::Graph *)), this,
          SLOT(currentGraphChanged(tlp::Graph *)));
  connect(_graphs, SIGNAL(currentGraphChanged(tlp::Graph *)), _ui->algorithmRunner,
          SLOT(setGraph(tlp::Graph *)));
  connect(_ui->graphHierarchiesEditor, SIGNAL(changeSynchronization(bool)), this,
          SLOT(changeSynchronization(bool)));

  // Connect actions
  connect(_ui->actionMessages_log, SIGNAL(triggered()), this, SLOT(showLogger()));
  connect(_ui->actionPython_IDE, SIGNAL(triggered()), this, SLOT(showPythonIDE()));
  connect(_ui->actionFull_screen, SIGNAL(triggered(bool)), this, SLOT(showFullScreen(bool)));
  connect(_ui->actionImport, SIGNAL(triggered()), this, SLOT(importGraph()));
  connect(_ui->actionExport, SIGNAL(triggered()), this, SLOT(exportGraph()));
  connect(_ui->actionSave_graph_to_file, SIGNAL(triggered()), this,
          SLOT(saveGraphHierarchyInTlpFile()));
  connect(_ui->workspace, SIGNAL(panelFocused(tlp::View *)), this, SLOT(panelFocused(tlp::View *)));
  connect(_ui->actionSave_Project, SIGNAL(triggered()), this, SLOT(save()));
  connect(_ui->actionSave_Project_as, SIGNAL(triggered()), this, SLOT(saveAs()));
  connect(_ui->actionOpen_Project, SIGNAL(triggered()), this, SLOT(open()));
  connect(_ui->actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedElements()));
  connect(_ui->actionDelete_from_the_root_graph, SIGNAL(triggered()), this,
          SLOT(deleteSelectedElementsFromRootGraph()));
  connect(_ui->actionDelete_all, SIGNAL(triggered()), this, SLOT(clearGraph()));
  connect(_ui->actionInvert_selection, SIGNAL(triggered()), this, SLOT(invertSelection()));
  connect(_ui->actionCancel_selection, SIGNAL(triggered()), this, SLOT(cancelSelection()));
  connect(_ui->actionReverse_selected_edges, SIGNAL(triggered()), this,
          SLOT(reverseSelectedEdges()));
  connect(_ui->actionMake_selection_a_graph, SIGNAL(triggered()), this, SLOT(make_graph()));
  connect(_ui->actionSelect_All, SIGNAL(triggered()), this, SLOT(selectAll()));
  connect(_ui->actionSelect_All_Nodes, SIGNAL(triggered()), this, SLOT(selectAllNodes()));
  connect(_ui->actionSelect_All_Edges, SIGNAL(triggered()), this, SLOT(selectAllEdges()));
  connect(_ui->actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
  connect(_ui->actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
  connect(_ui->actionCut, SIGNAL(triggered()), this, SLOT(cut()));
  connect(_ui->actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
  connect(_ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
  connect(_ui->actionGroup_elements, SIGNAL(triggered()), this, SLOT(group()));
  connect(_ui->actionCreate_sub_graph, SIGNAL(triggered()), this, SLOT(createSubGraph()));
  connect(_ui->actionClone_sub_graph, SIGNAL(triggered()), this, SLOT(cloneSubGraph()));
  connect(_ui->actionCreate_empty_sub_graph, SIGNAL(triggered()), this, SLOT(addEmptySubGraph()));
  connect(_ui->actionImport_CSV, SIGNAL(triggered()), this, SLOT(CSVImport()));
  connect(_ui->actionFind_plugins, SIGNAL(triggered()), this, SLOT(findPlugins()));
  connect(_ui->actionNew_graph, SIGNAL(triggered()), this, SLOT(addNewGraph()));
  connect(_ui->actionNewProject, SIGNAL(triggered()), this, SLOT(newProject()));
  connect(_ui->actionPreferences, SIGNAL(triggered()), this, SLOT(openPreferences()));
  connect(_ui->searchButton, SIGNAL(clicked(bool)), this, SLOT(setSearchOutput(bool)));
  connect(_ui->workspace, SIGNAL(importGraphRequest()), this, SLOT(importGraph()));
  connect(_ui->action_Close_All, SIGNAL(triggered()), _ui->workspace, SLOT(closeAll()));
  connect(_ui->addPanelButton, SIGNAL(clicked()), this, SLOT(createPanel()));
  connect(_ui->actionColor_scales_management, SIGNAL(triggered()), this,
          SLOT(displayColorScalesDialog()));
  connect(_ui->exportButton, SIGNAL(clicked()), this, SLOT(exportGraph()));

  // Agent actions
  connect(_ui->actionPlugins_Center, SIGNAL(triggered()), this, SLOT(showPluginsCenter()));
  connect(_ui->actionAbout_us, SIGNAL(triggered()), this, SLOT(showAboutPage()));
  connect(_ui->actionAbout_us, SIGNAL(triggered()), this, SLOT(showAboutPage()));

  if (QFile(tlpStringToQString(tlp::TalipotShareDir) +
            "../doc/talipot/talipot-user/html/index.html")
          .exists()) {
    connect(_ui->actionShowUserDocumentation, SIGNAL(triggered()), this,
            SLOT(showUserDocumentation()));
    connect(_ui->actionShowDevelDocumentation, SIGNAL(triggered()), this,
            SLOT(showDevelDocumentation()));
    connect(_ui->actionShowPythonDocumentation, SIGNAL(triggered()), this,
            SLOT(showPythonDocumentation()));
  } else {
    _ui->actionShowUserDocumentation->setVisible(false);
    _ui->actionShowDevelDocumentation->setVisible(false);
    _ui->actionShowPythonDocumentation->setVisible(false);
  }

  if (QFile(tlpStringToQString(tlp::TalipotShareDir) + "../doc/talipot/doxygen/html/index.html")
          .exists()) {
    connect(_ui->actionShowAPIDocumentation, SIGNAL(triggered()), this,
            SLOT(showAPIDocumentation()));
  } else {
    _ui->actionShowAPIDocumentation->setVisible(false);
  }

  // Setting initial sizes for splitters
  _ui->mainSplitter->setSizes(QList<int>() << 350 << 850);
  _ui->mainSplitter->setStretchFactor(0, 0);
  _ui->mainSplitter->setStretchFactor(1, 1);
  _ui->mainSplitter->setCollapsible(1, false);

  // Open project with model
  QMap<QString, tlp::Graph *> rootIds;

  if (!_project->projectFile().isEmpty()) {
    rootIds = _graphs->readProject(_project, progress);

    if (rootIds.empty())
      QMessageBox::critical(_mainWindow,
                            QString("Error while loading project ").append(_project->projectFile()),
                            QString("The Talipot project file is probably corrupted.<br>") +
                                tlpStringToQString(progress->getError()));
  }

  // these ui initializations are needed here
  // in case of a call to showStartPanels in the open method
  _ui->graphHierarchiesEditor->setModel(_graphs);
  _ui->workspace->setModel(_graphs);

  if (!rootIds.empty())
    _ui->workspace->readProject(_project, rootIds, progress);

  _ui->searchPanel->setModel(_graphs);

  connect(_ui->pythonButton, SIGNAL(clicked(bool)), this, SLOT(setPythonPanel(bool)));
  connect(_ui->developButton, SIGNAL(clicked()), this, SLOT(showPythonIDE()));
  _pythonPanel->setModel(_graphs);
  _pythonIDE->setGraphsModel(_graphs);
  connect(_pythonIDE, SIGNAL(anchoredRequest(bool)), this, SLOT(anchoredPythonIDE(bool)));
  tlp::PluginsManager::instance()->addListener(this);
  QTimer::singleShot(100, this, SLOT(initPythonIDE()));

  if (!_externalFile.isEmpty() && QFileInfo(_externalFile).exists()) {
    open(_externalFile);
  }

  for (auto h : _ui->docksSplitter->findChildren<HeaderFrame *>()) {
    connect(h, SIGNAL(expanded(bool)), this, SLOT(refreshDockExpandControls()));
  }

  connect(_ui->sidebarButton, SIGNAL(clicked()), this, SLOT(showHideSideBar()));
  connect(_ui->menubarButton, SIGNAL(clicked()), this, SLOT(showHideMenuBar()));

#if !defined(__APPLE__) && !defined(_WIN32)
  // Hide plugins center when not on MacOS or Windows
  _ui->pluginsButton->hide();
  _ui->menuHelp->removeAction(_ui->actionPlugins_Center);
#else
  // show the 'Plugins center' menu entry and button only if connected to the Talipot agent
  _ui->pluginsButton->setVisible(checkSocketConnected());
  _ui->actionPlugins_Center->setVisible(checkSocketConnected());
#endif

  // fill menu with recent documents
  buildRecentDocumentsMenu();

  showTrayMessage("GraphPerspective started");

  logCleared();
}

void GraphPerspective::openExternalFile() {
  open(_externalFile);
}

tlp::GraphHierarchiesModel *GraphPerspective::model() const {
  return _graphs;
}

void GraphPerspective::refreshDockExpandControls() {
  QList<HeaderFrame *> expandedHeaders, collapsedHeaders;

  for (auto h : _ui->docksSplitter->findChildren<HeaderFrame *>()) {
    h->expandControl()->setEnabled(true);

    if (h->isExpanded())
      expandedHeaders.push_back(h);
    else
      collapsedHeaders.push_back(h);
  }

  if (expandedHeaders.size() == 1)
    expandedHeaders[0]->expandControl()->setEnabled(false);
}

void GraphPerspective::exportGraph(Graph *g) {
  if (g == nullptr)
    g = _graphs->currentGraph();

  if (g == nullptr)
    return;

  static QString exportFile;
  ExportWizard wizard(g, exportFile, _mainWindow);
  wizard.setWindowTitle(QString("Exporting graph \"") + tlpStringToQString(g->getName()) + '"');

  if (wizard.exec() != QDialog::Accepted || wizard.algorithm().isEmpty() ||
      wizard.outputFile().isEmpty())
    return;

  std::string filename = QStringToTlpString(exportFile = wizard.outputFile());
  std::string exportPluginName = QStringToTlpString(wizard.algorithm());

  DataSet data = wizard.parameters();
  PluginProgress *prg = progress(NoProgressOption);
  prg->setTitle(exportPluginName);
  // take time before run
  QTime start = QTime::currentTime();
  bool result = tlp::saveGraph(g, filename, prg, &data);

  if (!result) {
    QMessageBox::critical(_mainWindow, "Export error",
                          QString("<i>") + wizard.algorithm() +
                              "</i> failed to export graph.<br/><br/><b>" +
                              tlp::tlpStringToQString(prg->getError()) + "</b>");
  } else {
    // log export plugin call
    if (Settings::instance().logPluginCall() != Settings::NoLog) {
      std::stringstream log;
      log << exportPluginName.c_str() << " - " << data.toString().c_str();

      if (Settings::instance().logPluginCall() == Settings::LogCallWithExecutionTime)
        log << ": " << start.msecsTo(QTime::currentTime()) << "ms";

      qDebug() << log.str().c_str();
    }

    addRecentDocument(wizard.outputFile());
  }

  delete prg;
}

void GraphPerspective::saveGraphHierarchyInTlpFile(Graph *g) {
  if (g == nullptr)
    g = _graphs->currentGraph();

  if (g == nullptr)
    return;

  static QString savedFile;
  QString filter("TLP format (*.tlp *.tlp.gz *.tlpz);;TLPB format (*.tlpb *.tlpb.gz *.tlpbz)");
  QString filename = QFileDialog::getSaveFileName(
      _mainWindow, tr("Save graph hierarchy in tlp/tlpb file"), savedFile, filter);

  if (!filename.isEmpty()) {
    bool result = tlp::saveGraph(g, tlp::QStringToTlpString(filename));

    if (!result)
      QMessageBox::critical(_mainWindow, "Save error", "Failed to save graph hierarchy");
    else {
      savedFile = filename;
      addRecentDocument(filename);
    }
  }
}

void GraphPerspective::importGraph(const std::string &module, DataSet &data) {
  Graph *g;

  if (!module.empty()) {
    PluginProgress *prg = progress(IsStoppable | IsCancellable);
    prg->setTitle(module);
    // take time before run
    QTime start = QTime::currentTime();
    g = tlp::importGraph(module, data, prg);

    if (g == nullptr) {
      QMessageBox::critical(_mainWindow, "Import error",
                            QString("<i>") + tlp::tlpStringToQString(module) +
                                "</i> failed to import data.<br/><br/><b>" +
                                tlp::tlpStringToQString(prg->getError()) + "</b>");
      delete prg;
      return;
    }

    delete prg;

    // log import plugin call
    if (Settings::instance().logPluginCall() != Settings::NoLog) {
      std::stringstream log;
      log << module.c_str() << " import - " << data.toString().c_str();

      if (Settings::instance().logPluginCall() == Settings::LogCallWithExecutionTime)
        log << ": " << start.msecsTo(QTime::currentTime()) << "ms";

      qDebug() << log.str().c_str();
    }

    if (g->getName().empty()) {
      QString n =
          tlp::tlpStringToQString(module) + " - " + tlp::tlpStringToQString(data.toString());
      n.replace(QRegExp("[\\w]*::"), ""); // remove words before "::"
      g->setName(tlp::QStringToTlpString(n));
    }
  } else {
    g = tlp::newGraph();
  }

  _graphs->addGraph(g);
  std::string fileName;

  if (data.get("file::filename", fileName))
    // set current directory to the directory of the loaded file
    // to ensure a correct loading of the associated texture files if any
    QDir::setCurrent(QFileInfo(tlpStringToQString(fileName)).absolutePath());

  applyRandomLayout(g);
  showStartPanels(g);
}

void GraphPerspective::importGraph() {
  ImportWizard wizard(_mainWindow);

  if (wizard.exec() == QDialog::Accepted) {
    DataSet data = wizard.parameters();
    importGraph(tlp::QStringToTlpString(wizard.algorithm()), data);
  }
}

void GraphPerspective::createPanel(tlp::Graph *g) {
  if (_graphs->empty())
    return;

  PanelSelectionWizard wizard(_graphs, _mainWindow);

  if (g != nullptr)
    wizard.setSelectedGraph(g);
  else
    wizard.setSelectedGraph(_graphs->currentGraph());

  int result = wizard.exec();

  if (result == QDialog::Accepted && wizard.panel() != nullptr) {
    // expose mode is not safe to add a new panel
    // so hide it if needed
    _ui->workspace->hideExposeMode();
    _ui->workspace->addPanel(wizard.panel());
    _ui->workspace->setActivePanel(wizard.panel());
    wizard.panel()->applySettings();
  }
}

void GraphPerspective::panelFocused(tlp::View *view) {
  disconnect(this, SLOT(focusedPanelGraphSet(tlp::Graph *)));

  if (!_ui->graphHierarchiesEditor->synchronized())
    return;

  connect(view, SIGNAL(graphSet(tlp::Graph *)), this, SLOT(focusedPanelGraphSet(tlp::Graph *)));
  focusedPanelGraphSet(view->graph());
}

void GraphPerspective::changeSynchronization(bool s) {
  _ui->workspace->setFocusedPanelHighlighting(s);
}

void GraphPerspective::focusedPanelGraphSet(Graph *g) {
  _graphs->setCurrentGraph(g);
}

void GraphPerspective::focusedPanelSynchronized() {
  _ui->workspace->setGraphForFocusedPanel(_graphs->currentGraph());
}

bool GraphPerspective::save() {
  return saveAs(_project->projectFile());
}

bool GraphPerspective::saveAs(const QString &path) {
  if (_graphs->empty())
    return false;

  if (path.isEmpty()) {
    QString path = QFileDialog::getSaveFileName(_mainWindow, "Save project", QString(),
                                                "Talipot Project (*.tlpx)");

    if (!path.isEmpty()) {
      if (!path.endsWith(".tlpx"))
        path += ".tlpx";

      _project->setProjectFile(path);
      return saveAs(path);
    }

    return false;
  }

  SimplePluginProgressDialog progress(_mainWindow);
  progress.showPreview(false);
  progress.show();
  QMap<Graph *, QString> rootIds = _graphs->writeProject(_project, &progress);
  _ui->workspace->writeProject(_project, rootIds, &progress);
  _pythonIDE->savePythonFilesAndWriteToProject();
  bool ret = _project->write(path, &progress);

  if (ret)
    Settings::instance().addToRecentDocuments(path);

  return ret;
}

void GraphPerspective::open(QString fileName) {
  QMap<std::string, std::string> modules;
  std::list<std::string> imports = PluginsManager::availablePlugins<ImportModule>();

  std::string filters("Talipot project (*.tlpx);;");
  std::string filterAny("Any supported format (");

  for (auto &import : imports) {
    ImportModule *m = PluginsManager::getPluginObject<ImportModule>(import);
    std::list<std::string> fileExtension(m->allFileExtensions());

    std::string currentFilter;

    for (auto &ext : fileExtension) {

      if (ext.empty())
        continue;

      filterAny += "*." + ext + " ";
      currentFilter += "*." + ext + " ";

      modules[ext] = import;
    }

    if (!currentFilter.empty())
      filters += import + "(" + currentFilter + ");;";

    delete m;
  }

  filterAny += " *.tlpx);;";

  filters += "All files (*)";
  filters.insert(0, filterAny);

  if (fileName.isEmpty()) // If open() was called without a parameter, open the file dialog
    fileName = QFileDialog::getOpenFileName(_mainWindow, tr("Open graph"), _lastOpenLocation,
                                            filters.c_str(), nullptr,
                                            static_cast<QFileDialog::Options>(0));

  if (!fileName.isEmpty()) {
    QFileInfo fileInfo(fileName);

    _lastOpenLocation = fileInfo.absolutePath();

    for (const std::string &extension : modules.keys()) {
      if (fileName.endsWith(".tlpx")) {
        openProjectFile(fileName);
        Settings::instance().addToRecentDocuments(fileInfo.absoluteFilePath());
        break;
      } else if (fileName.endsWith(QString::fromStdString(extension))) {
        DataSet params;
        params.set("file::filename", QStringToTlpString(fileName));
        addRecentDocument(fileName);
        importGraph(modules[extension], params);
        break;
      }
    }
  }
}

void GraphPerspective::openProjectFile(const QString &path) {
  if (_graphs->empty()) {
    PluginProgress *prg = progress(NoProgressOption);
    if (_project->openProjectFile(path, prg)) {
      QMap<QString, tlp::Graph *> rootIds = _graphs->readProject(_project, prg);
      _ui->workspace->readProject(_project, rootIds, prg);
      QTimer::singleShot(100, this, SLOT(initPythonIDE()));
    } else {
      QMessageBox::critical(_mainWindow,
                            QString("Error while loading project ").append(_project->projectFile()),
                            QString("The Talipot project file is probably corrupted:<br>") +
                                tlpStringToQString(prg->getError()));
    }
    delete prg;
  } else {
    Perspective::openProjectFile(path);
  }
}

void GraphPerspective::initPythonIDE() {
  _pythonIDE->setProject(_project);
  if (Settings::instance().pythonIDEAnchored()) {
    _pythonIDE->setVisible(false);
    _pythonIDE->setAnchored(true);
    _pythonIDE->setParent(nullptr);
    _ui->mainSplitter->addWidget(_pythonIDE);
    _ui->mainSplitter->setCollapsible(2, false);
    _ui->developButton->setCheckable(true);
  }
}

void GraphPerspective::anchoredPythonIDE(bool anchored) {
  _ui->developButton->setCheckable(anchored);
  Settings::instance().setPythonIDEAnchored(anchored);
  if (anchored) {
    _pythonIDEDialog->hide();
    _pythonIDE->setParent(nullptr);
    _ui->mainSplitter->addWidget(_pythonIDE);
    _ui->mainSplitter->setCollapsible(2, false);
    _ui->developButton->setChecked(anchored);
  } else {
    _pythonIDE->setParent(nullptr);
    _pythonIDEDialog->layout()->addWidget(_pythonIDE);
    _pythonIDEDialog->show();
    _ui->developButton->setChecked(false);
  }
}

void GraphPerspective::deleteSelectedElementsFromRootGraph() {
  deleteSelectedElements(true);
}

void GraphPerspective::clearGraph() {
  if (QMessageBox::question(_mainWindow, "Clear graph content",
                            "Do you really want to remove all nodes and edges from the current "
                            "graph. This action cannot be undone",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    _graphs->currentGraph()->clear();
}

void GraphPerspective::deleteSelectedElements(bool fromRoot) {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");

  vector<tlp::edge> edgesToDelete = iteratorVector(selection->getEdgesEqualTo(true, graph));
  bool hasPush = !edgesToDelete.empty();

  if (hasPush) {
    graph->push();
    graph->delEdges(edgesToDelete, fromRoot);
  }

  vector<tlp::node> nodesToDelete = iteratorVector(selection->getNodesEqualTo(true, graph));

  if (!hasPush && !nodesToDelete.empty())
    graph->push();

  graph->delNodes(nodesToDelete, fromRoot);

  Observable::unholdObservers();
}

void GraphPerspective::invertSelection() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->reverse(graph);
  Observable::unholdObservers();
}

void GraphPerspective::reverseSelectedEdges() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->reverseEdgeDirection(graph);
  graph->popIfNoUpdates();
  Observable::unholdObservers();
}

void GraphPerspective::cancelSelection() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->setValueToGraphNodes(false, graph);
  selection->setValueToGraphEdges(false, graph);
  graph->popIfNoUpdates();
  Observable::unholdObservers();
}

void GraphPerspective::selectAll(bool nodes, bool edges) {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->setAllNodeValue(false);
  selection->setAllEdgeValue(false);

  if (nodes) {
    selection->setValueToGraphNodes(true, graph);
  }

  if (edges) {
    selection->setValueToGraphEdges(true, graph);
  }

  Observable::unholdObservers();
}

void GraphPerspective::selectAllEdges() {
  selectAll(false, true);
}

void GraphPerspective::selectAllNodes() {
  selectAll(true, false);
}

void GraphPerspective::undo() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();

  if (graph != nullptr)
    graph->pop();

  Observable::unholdObservers();

  for (auto v : _ui->workspace->panels()) {
    if (v->graph() == graph)
      v->undoCallback();
  }
}

void GraphPerspective::redo() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();

  if (graph != nullptr)
    graph->unpop();

  Observable::unholdObservers();

  for (auto v : _ui->workspace->panels()) {
    if (v->graph() == graph)
      v->undoCallback();
  }
}

void GraphPerspective::cut() {
  copy(_graphs->currentGraph(), true);
}

void GraphPerspective::paste() {
  if (_graphs->currentGraph() == nullptr)
    return;

  Graph *outGraph = _graphs->currentGraph();
  std::stringstream ss;
  ss << QStringToTlpString(QApplication::clipboard()->text());

  Observable::holdObservers();
  outGraph->push();
  DataSet data;
  data.set("file::data", ss.str());
  Graph *inGraph = tlp::importGraph("TLP Import", data);
  tlp::copyToGraph(outGraph, inGraph);
  outGraph->popIfNoUpdates();
  delete inGraph;
  Observable::unholdObservers();
  centerPanelsForGraph(outGraph);
}

void GraphPerspective::copy() {
  copy(_graphs->currentGraph());
}

void GraphPerspective::copy(Graph *g, bool deleteAfter) {
  if (g == nullptr)
    return;

  BooleanProperty *selection = g->getBooleanProperty("viewSelection");

  Graph *copyGraph = tlp::newGraph();
  tlp::copyToGraph(copyGraph, g, selection);

  if (!copyGraph->isEmpty()) {
    std::stringstream ss;
    DataSet data;
    tlp::exportGraph(copyGraph, ss, "TLP Export", data);
    delete copyGraph;
    QApplication::clipboard()->setText(tlpStringToQString(ss.str()));

    if (deleteAfter) {
      Observable::holdObservers();
      g->push();

      for (auto n : stableIterator(selection->getNonDefaultValuatedNodes(g))) {
        g->delNode(n);
      }
      Observable::unholdObservers();
    }
  }
}

void GraphPerspective::group() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  std::vector<node> groupedNodes;
  for (auto n : selection->getNodesEqualTo(true, graph)) {
    groupedNodes.push_back(n);
  }

  if (groupedNodes.empty()) {
    Observable::unholdObservers();
    qCritical() << "[Group] Cannot create meta-nodes from empty selection";
    return;
  }

  graph->push();

  bool changeGraph = false;

  if (graph == graph->getRoot()) {
    qWarning() << "[Group] Grouping can not be done on the root graph. A subgraph has "
                  "automatically been created";
    graph = graph->addCloneSubGraph("groups");
    changeGraph = true;
  }

  graph->createMetaNode(groupedNodes, false);

  selection->setAllNodeValue(false);
  selection->setAllEdgeValue(false);

  Observable::unholdObservers();

  if (!changeGraph)
    return;

  for (auto v : _ui->workspace->panels()) {
    if (v->graph() == graph->getRoot())
      v->setGraph(graph);
  }
}

void GraphPerspective::make_graph() {
  Graph *graph = _graphs->currentGraph();
  makeSelectionGraph(_graphs->currentGraph(), graph->getBooleanProperty("viewSelection"));
}

Graph *GraphPerspective::createSubGraph(Graph *graph) {
  if (graph == nullptr)
    return nullptr;

  graph->push();
  Observable::holdObservers();
  BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  makeSelectionGraph(graph, selection);
  Graph *result = graph->addSubGraph(selection, "selection subgraph");
  Observable::unholdObservers();
  return result;
}

void GraphPerspective::createSubGraph() {
  createSubGraph(_graphs->currentGraph());
}

void GraphPerspective::cloneSubGraph() {
  if (_graphs->currentGraph() == nullptr)
    return;

  tlp::BooleanProperty prop(_graphs->currentGraph());
  prop.setAllNodeValue(true);
  prop.setAllEdgeValue(true);
  _graphs->currentGraph()->push();
  _graphs->currentGraph()->addSubGraph(&prop, "clone subgraph");
}

void GraphPerspective::addEmptySubGraph() {
  if (_graphs->currentGraph() == nullptr)
    return;

  _graphs->currentGraph()->push();
  _graphs->currentGraph()->addSubGraph(nullptr, "empty subgraph");
}

void GraphPerspective::currentGraphChanged(Graph *graph) {
  bool enabled(graph != nullptr);
  _ui->actionUndo->setEnabled(enabled);
  _ui->actionRedo->setEnabled(enabled);
  _ui->actionCut->setEnabled(enabled);
  _ui->actionCopy->setEnabled(enabled);
  _ui->actionPaste->setEnabled(enabled);
  _ui->actionDelete->setEnabled(enabled);
  _ui->actionDelete_from_the_root_graph->setEnabled(enabled && (graph != graph->getRoot()));
  _ui->actionDelete_all->setEnabled(enabled);
  _ui->actionInvert_selection->setEnabled(enabled);
  _ui->actionReverse_selected_edges->setEnabled(enabled);
  _ui->actionSelect_All->setEnabled(enabled);
  _ui->actionSelect_All_Nodes->setEnabled(enabled);
  _ui->actionSelect_All_Edges->setEnabled(enabled);
  _ui->actionCancel_selection->setEnabled(enabled);
  _ui->actionMake_selection_a_graph->setEnabled(enabled);
  _ui->actionGroup_elements->setEnabled(enabled);
  _ui->actionCreate_sub_graph->setEnabled(enabled);
  _ui->actionCreate_empty_sub_graph->setEnabled(enabled);
  _ui->actionClone_sub_graph->setEnabled(enabled);
  _ui->actionExport->setEnabled(enabled);
  _ui->singleModeButton->setEnabled(enabled);
  _ui->splitModeButton->setEnabled(enabled);
  _ui->splitHorizontalModeButton->setEnabled(enabled);
  _ui->split3ModeButton->setEnabled(enabled);
  _ui->split32ModeButton->setEnabled(enabled);
  _ui->split33ModeButton->setEnabled(enabled);
  _ui->gridModeButton->setEnabled(enabled);
  _ui->sixModeButton->setEnabled(enabled);
  _ui->exposeModeButton->setEnabled(enabled);
  _ui->searchButton->setEnabled(enabled);
  _ui->pythonButton->setEnabled(enabled);
  _ui->exportButton->setEnabled(enabled);
  _ui->previousPageButton->setVisible(enabled);
  _ui->pageCountLabel->setVisible(enabled);
  _ui->nextPageButton->setVisible(enabled);
  _ui->actionSave_Project->setEnabled(enabled);
  _ui->actionSave_Project_as->setEnabled(enabled);

  if (graph == nullptr) {
    _ui->workspace->switchToStartupMode();
    _ui->exposeModeButton->setChecked(false);
    _ui->searchButton->setChecked(false);
    _ui->pythonButton->setChecked(false);
    setSearchOutput(false);
    _ui->actionSave_Project->setEnabled(false);
    _ui->actionSave_Project_as->setEnabled(false);
  } else {
    _ui->workspace->setGraphForFocusedPanel(graph);
  }

  if (_graphs->empty()) {
    _pythonIDE->clearPythonCodeEditors();
    _pythonIDEDialog->hide();
    _ui->developButton->setEnabled(false);
    _ui->actionPython_IDE->setEnabled(false);
  } else {
    _ui->developButton->setEnabled(true);
    _ui->actionPython_IDE->setEnabled(true);
  }
}

void GraphPerspective::CSVImport() {
  bool mustDeleteGraph = false;

  if (_graphs->empty()) {
    _graphs->addGraph(tlp::newGraph());
    mustDeleteGraph = true;
  }

  Graph *g = _graphs->currentGraph();

  if (g == nullptr)
    return;

  CSVImportWizard wizard(_mainWindow);

  if (mustDeleteGraph) {
    wizard.setWindowTitle("Import CSV data into a new graph");
    wizard.setButtonText(QWizard::FinishButton, QString("Import into a new graph"));
  } else {
    wizard.setWindowTitle(QString("Import CSV data into current graph: ") + g->getName().c_str());
    wizard.setButtonText(QWizard::FinishButton, QString("Import into current graph"));
  }

  // get the number of line displayed in the logger
  unsigned int nbLogsBefore = _logger->countByType(GraphPerspectiveLogger::Error);
  nbLogsBefore += _logger->countByType(GraphPerspectiveLogger::Warning);

  wizard.setGraph(g);

  tlp::ObserverHolder oh;

  g->push();
  int result = wizard.exec();

  if (result == QDialog::Rejected) {
    if (mustDeleteGraph) {
      _graphs->removeGraph(g);
      delete g;
    } else {
      g->pop();
    }

    return;
  } else {
    unsigned int nbLogsAfter = _logger->countByType(GraphPerspectiveLogger::Error);
    nbLogsAfter += _logger->countByType(GraphPerspectiveLogger::Warning);
    applyRandomLayout(g);
    bool openPanels = true;

    for (auto v : _ui->workspace->panels()) {
      if (v->graph() == g) {
        openPanels = false;
        break;
      }
    }

    if (openPanels)
      showStartPanels(g);
    else
      centerPanelsForGraph(g);

    unsigned nb_error = nbLogsAfter - nbLogsBefore;
    if ((nb_error == 1) &&
        (QMessageBox::question(_mainWindow, "CSV parse error",
                               "When parsing your CSV file,<br/> one error has been "
                               "encountered.<br/>Do you want to see it?",
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes))
      showLogger();
    if ((nb_error > 1) &&
        (QMessageBox::question(_mainWindow, "CSV parse errors",
                               QString("When parsing your CSV file,<br/> %1 errors have been "
                                       "encountered.<br/>Do you want to see them?")
                                   .arg(nb_error),
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes))
      showLogger();

    g->popIfNoUpdates();
  }
}

void GraphPerspective::showStartPanels(Graph *g) {
  if (Settings::instance().displayDefaultViews() == false)
    return;

  // expose mode is not safe to add a new panel
  // so hide it if needed
  _ui->workspace->hideExposeMode();
  View *firstPanel = nullptr;

  for (auto panelName : {"Spreadsheet view", "Node Link Diagram view"}) {
    View *view = PluginsManager::getPluginObject<View>(panelName);

    if (firstPanel == nullptr) {
      firstPanel = view;
    }

    view->setupUi();
    view->setGraph(g);
    view->setState(DataSet());
    _ui->workspace->addPanel(view);
  }

  _ui->workspace->setActivePanel(firstPanel);
  _ui->workspace->switchToSplitMode();
}

void GraphPerspective::applyRandomLayout(Graph *g) {
  Observable::holdObservers();
  LayoutProperty *viewLayout = g->getLayoutProperty("viewLayout");

  if (!viewLayout->hasNonDefaultValuatedNodes(g)) {
    std::string str;
    g->applyPropertyAlgorithm("Random layout", viewLayout, str);
  }

  Observable::unholdObservers();
}

void GraphPerspective::centerPanelsForGraph(tlp::Graph *g, bool graphChanged, bool onlyGlMainView) {
  for (auto v : _ui->workspace->panels()) {
    if ((v->graph() == g) && (!onlyGlMainView || dynamic_cast<tlp::GlMainView *>(v)))
      v->centerView(graphChanged);
  }
}

void GraphPerspective::closePanelsForGraph(tlp::Graph *g) {
  list<View *> viewsToDelete;

  for (auto v : _ui->workspace->panels()) {
    if (v->graph() == g || g->isDescendantGraph(v->graph()))
      viewsToDelete.push_back(v);
  }

  if (!viewsToDelete.empty()) {
    // expose mode is not safe when deleting a panel
    // so hide it first
    _ui->workspace->hideExposeMode();
    for (auto v : viewsToDelete) {
      _ui->workspace->delView(v);
    }
  }
}

bool GraphPerspective::setGlMainViewPropertiesForGraph(
    tlp::Graph *g, const std::map<std::string, tlp::PropertyInterface *> &propsMap) {
  bool result = false;

  for (auto v : _ui->workspace->panels()) {
    GlMainView *glMainView = dynamic_cast<tlp::GlMainView *>(v);

    if (v->graph() == g && glMainView != nullptr) {
      if (glMainView->getGlMainWidget()
              ->getScene()
              ->getGlGraphComposite()
              ->getInputData()
              ->installProperties(propsMap))
        result = true;
    }
  }

  return result;
}

void GraphPerspective::setSearchOutput(bool f) {
  if (f) {
    _ui->outputFrame->setCurrentWidget(_ui->searchPanel);
    _ui->pythonButton->setChecked(false);
  }

  _ui->outputFrame->setVisible(f);
}

void GraphPerspective::setPythonPanel(bool f) {
  if (f) {
    _ui->outputFrame->setCurrentWidget(_ui->pythonPanel);
    _ui->searchButton->setChecked(false);
  }

  _ui->outputFrame->setVisible(f);
}

void GraphPerspective::openPreferences() {
  PreferencesDialog dlg(_ui->mainWidget);
  dlg.readSettings();

  if (dlg.exec() == QDialog::Accepted) {
    dlg.writeSettings();

    for (auto v : _ui->workspace->panels()) {
      GlMainView *glMainView = dynamic_cast<tlp::GlMainView *>(v);

      if (glMainView != nullptr) {
        if (glMainView->getGlMainWidget() != nullptr) {
          glMainView->getGlMainWidget()
              ->getScene()
              ->getGlGraphComposite()
              ->getRenderingParametersPointer()
              ->setSelectionColor(Settings::instance().defaultSelectionColor());
          glMainView->redraw();
        }
      }
    }
  }
}

void GraphPerspective::setAutoCenterPanelsOnDraw(bool f) {
  _ui->workspace->setAutoCenterPanelsOnDraw(f);
}

void GraphPerspective::pluginsListChanged() {
  _ui->algorithmRunner->refreshPluginsList();
}

void GraphPerspective::addNewGraph() {
  Graph *g = tlp::newGraph();
  _graphs->addGraph(g);
  showStartPanels(g);
}

void GraphPerspective::newProject() {
  createPerspective(name().c_str());
}

void GraphPerspective::openRecentFile() {
  QAction *action = static_cast<QAction *>(sender());
  open(action->data().toString());
}

void GraphPerspective::treatEvent(const tlp::Event &ev) {
  if (dynamic_cast<const tlp::PluginEvent *>(&ev)) {
    pluginsListChanged();
  }
}

void GraphPerspective::showPythonIDE() {
  if (!_pythonIDE->isAnchored()) {
    _pythonIDEDialog->raise();
    _pythonIDEDialog->show();
    _pythonIDEDialog->raise();
  } else {
    _pythonIDE->setVisible(!_pythonIDE->isVisible());
    _ui->developButton->setChecked(_pythonIDE->isVisible());
  }
}

#ifdef APPIMAGE_BUILD
// When running the appimage
// the LD_LIBRARY_PATH variable must be unset to ensure a successful launch
// of the default web browser to show the Talipot documentation
#define UNSET_LD_LIBRARY_PATH()                                                                    \
  auto ldPath = qgetenv("LD_LIBRARY_PATH");                                                        \
  qunsetenv("LD_LIBRARY_PATH")
#define RESTORE_LD_LIBRARY_PATH() qputenv("LD_LIBRARY_PATH", ldPath);
#else
#define UNSET_LD_LIBRARY_PATH()
#define RESTORE_LD_LIBRARY_PATH()
#endif

void GraphPerspective::showUserDocumentation() {
  UNSET_LD_LIBRARY_PATH();
  QDesktopServices::openUrl(QUrl::fromLocalFile(tlpStringToQString(tlp::TalipotShareDir) +
                                                "../doc/talipot/talipot-user/html/index.html"));
  RESTORE_LD_LIBRARY_PATH();
}

void GraphPerspective::showDevelDocumentation() {
  UNSET_LD_LIBRARY_PATH();
  QDesktopServices::openUrl(QUrl::fromLocalFile(tlpStringToQString(tlp::TalipotShareDir) +
                                                "../doc/talipot/talipot-dev/html/index.html"));
  RESTORE_LD_LIBRARY_PATH();
}

void GraphPerspective::showPythonDocumentation() {
  UNSET_LD_LIBRARY_PATH();
  QDesktopServices::openUrl(QUrl::fromLocalFile(tlpStringToQString(tlp::TalipotShareDir) +
                                                "../doc/talipot/talipot-python/html/index.html"));
  RESTORE_LD_LIBRARY_PATH();
}

void GraphPerspective::showAPIDocumentation() {
  UNSET_LD_LIBRARY_PATH();
  QDesktopServices::openUrl(QUrl::fromLocalFile(tlpStringToQString(tlp::TalipotShareDir) +
                                                "../doc/talipot/doxygen/html/index.html"));
  RESTORE_LD_LIBRARY_PATH();
}

void GraphPerspective::showHideSideBar() {
  if (_ui->docksWidget->isVisible()) {
    _ui->docksWidget->setVisible(false);
    _ui->sidebarButton->setToolTip("Show Sidebar");
  } else {
    _ui->docksWidget->setVisible(true);
    _ui->sidebarButton->setToolTip("Hide Sidebar");
  }

  if (_logger->anchored()) {
    resetLoggerDialogPosition();
  }
}

void GraphPerspective::showHideMenuBar() {
  if (_mainWindow->menuBar()->isVisible()) {
    _mainWindow->menuBar()->setVisible(false);
    _ui->menubarButton->setToolTip("Show Menubar");
  } else {
    _mainWindow->menuBar()->setVisible(true);
    _ui->menubarButton->setToolTip("Hide Menubar");
  }
}

void GraphPerspective::displayColorScalesDialog() {
  _colorScalesDialog->show();
}

void GraphPerspective::showAboutPage() {
  if (!checkSocketConnected()) {
    tlp::AboutPage *aboutPage = new tlp::AboutPage;
    QDialog aboutDialog(mainWindow(), Qt::Window);
    aboutDialog.setWindowTitle("About Talipot");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(aboutPage);
    layout->setContentsMargins(0, 0, 0, 0);
    aboutDialog.setLayout(layout);
    aboutDialog.resize(800, 600);
    aboutDialog.exec();
  }
}

void GraphPerspective::workspaceButtonClicked() {
  _ui->workspaceButton->setChecked(true);
}

void GraphPerspective::resetLoggerDialogPosition() {
  QPoint pos = _mainWindow->mapToGlobal(_ui->exportButton->pos());
  pos.setX(pos.x() + _ui->loggerFrame->width());

  if (_logger->isVisible()) {
    _logger->showNormal();
  }

  // extend the logger frame width until reaching the right side of the main window
  _logger->setGeometry(pos.x(), pos.y(), _mainWindow->width() - _ui->loggerFrame->width(),
                       _mainWindow->mapToGlobal(QPoint(0, 0)).y() + _mainWindow->height() -
                           pos.y() - 2);
}

PLUGIN(GraphPerspective)
