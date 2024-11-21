/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "TalipotMainWindow.h"
#include "ui_TalipotMainWindow.h"

#include "TalipotLogger.h"
#include "ImportWizard.h"
#include "ExportWizard.h"
#include "PanelSelectionWizard.h"
#include "PreferencesDialog.h"
#include "PluginsCenter.h"
#include "AboutDialog.h"

#include <talipot/PythonIDE.h>
#include <talipot/SimplePluginProgressWidget.h>
#include <talipot/ImportModule.h>
#include <talipot/SimplePluginProgressWidget.h>
#include <talipot/GraphHierarchiesModel.h>
#include <talipot/CSVImportWizard.h>
#include <talipot/GlView.h>
#include <talipot/Settings.h>
#include <talipot/Project.h>
#include <talipot/GraphTools.h>
#include <talipot/ColorScaleConfigDialog.h>
#include <talipot/ColorScalesManager.h>
#include <talipot/StableIterator.h>
#include <talipot/TreeTest.h>

#include <QMessageBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QClipboard>
#include <QMimeData>
#include <QTimer>
#include <QScreen>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QCloseEvent>

using namespace tlp;
using namespace std;

// checks if it exists a Talipot import plugin that can load the provided file based on its
// extension
static bool talipotCanOpenFile(const QString &path) {
  // Talipot project file does not use import / export plugin
  if (path.endsWith(".tlpx")) {
    return true;
  }

  for (const auto &pluginName : PluginsManager::availablePlugins<ImportModule>()) {
    const auto &importPlugin =
        static_cast<const ImportModule &>(PluginsManager::pluginInformation(pluginName));
    for (const auto &ext : importPlugin.allFileExtensions()) {
      if (path.endsWith(tlpStringToQString(ext))) {
        return true;
      }
    }
  }

  return false;
}

static void logMsgToStdStream(const QString &msg, QtMsgType type) {
  if (msg.startsWith("[Python")) {
    // remove quotes around message added by Qt
    QString msgClean = msg.mid(14).mid(2, msg.length() - 17);

    if (msg.startsWith("[PythonStdOut]")) {
#ifndef NDEBUG
      std::cout << QStringToTlpString(msgClean) << std::endl;
#endif
    } else {
      std::cerr << QStringToTlpString(msgClean) << std::endl;
    }
  } else {
    if (type == QtCriticalMsg) {
      std::cerr << QStringToTlpString(msg) << std::endl;
    } else {
#ifndef NDEBUG
      std::cout << QStringToTlpString(msg) << std::endl;
#endif
    }
  }
}

static void talipotLogger(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QStringList msgPrefixToFilter;
  msgPrefixToFilter.append("QGraphicsScene::sendEvent");
  msgPrefixToFilter.append("QXcbConnection: XCB error:");

  for (const auto &prefix : msgPrefixToFilter) {
    if (msg.startsWith(prefix)) {
      return;
    }
  }
  logMsgToStdStream(msg, type);
  TalipotMainWindow::instance().log(type, context, msg);
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

static QIcon getMakeSelectionAGraphIcon() {
  QIcon backIcon = FontIcon::icon(MaterialDesignIcons::Select);
  QIcon frontIcon = FontIcon::icon(MaterialDesignIcons::VectorPolyline, 0.7);
  return FontIcon::stackIcons(backIcon, frontIcon);
}

static QIcon getReverseSelectedEdgesIcon() {
  QIcon backIcon = FontIcon::icon(MaterialDesignIcons::Select);
  QIcon frontIcon = FontIcon::icon(MaterialDesignIcons::SwapHorizontal, 0.8);
  return FontIcon::stackIcons(backIcon, frontIcon);
}

TalipotMainWindow::TalipotMainWindow()
    : _ui(new Ui::TalipotMainWindow), _graphs(new GraphHierarchiesModel(this)),
      _project(Project::newProject()), _pluginsCenter(new PluginsCenter()), _maximized(false) {
  Q_INIT_RESOURCE(TalipotApp);
  _ui->setupUi(this);

  _title = "Talipot ";
  QString iconPath;

  _title += TALIPOT_VERSION;

  setWindowTitle(_title);

  _ui->pythonButton->setIcon(
      FontIcon::icon(MaterialDesignIcons::LanguagePython, QColor(Qt::white)));
  _ui->graphsButton->setIcon(FontIcon::icon(MaterialDesignIcons::Graph, QColor(Qt::white)));
  _ui->algorithmsButton->setIcon(FontIcon::icon(MaterialDesignIcons::Cogs, QColor(Qt::white)));
  _ui->importButton->setIcon(FontIcon::icon(MaterialDesignIcons::Import, QColor(Qt::white)));
  _ui->exportButton->setIcon(FontIcon::icon(MaterialDesignIcons::Export, QColor(Qt::white)));
  _ui->undoButton->setIcon(FontIcon::icon(MaterialDesignIcons::Reply, QColor(Qt::white)));
  _ui->redoButton->setIcon(FontIcon::icon(MaterialDesignIcons::Share, QColor(Qt::white)));
  _ui->searchButton->setIcon(FontIcon::icon(MaterialDesignIcons::Magnify, QColor(Qt::white)));

  _ui->actionNewProject->setIcon(FontIcon::icon(MaterialDesignIcons::FilePlusOutline));
  _ui->actionOpen_Project->setIcon(FontIcon::icon(MaterialDesignIcons::FileImportOutline));
  _ui->menuOpen_recent_file->setIcon(FontIcon::icon(MaterialDesignIcons::ClockOutline));
  _ui->actionSave_Project->setIcon(FontIcon::icon(MaterialDesignIcons::FileExportOutline));
  _ui->actionSave_Project_as->setIcon(FontIcon::icon(MaterialDesignIcons::FileExportOutline));
  _ui->actionImport->setIcon(FontIcon::icon(MaterialDesignIcons::Import));
  _ui->actionImport_CSV->setIcon(FontIcon::icon(MaterialDesignIcons::Table));
  _ui->actionNew_graph->setIcon(FontIcon::icon(MaterialDesignIcons::FilePlusOutline));
  _ui->actionExit->setIcon(FontIcon::icon(MaterialDesignIcons::CloseCircleOutline));

  _ui->actionUndo->setIcon(FontIcon::icon(MaterialDesignIcons::Reply));
  _ui->actionRedo->setIcon(FontIcon::icon(MaterialDesignIcons::Share));
  _ui->actionCut->setIcon(FontIcon::icon(MaterialDesignIcons::ContentCut));
  _ui->actionPaste->setIcon(FontIcon::icon(MaterialDesignIcons::ContentPaste));
  _ui->actionCopy->setIcon(FontIcon::icon(MaterialDesignIcons::ContentCopy));
  _ui->actionDelete->setIcon(FontIcon::icon(MaterialDesignIcons::DeleteOutline));
  _ui->actionSelect_All->setIcon(FontIcon::icon(MaterialDesignIcons::SelectAll));
  _ui->actionInvert_selection->setIcon(FontIcon::icon(MaterialDesignIcons::SelectInverse));
  _ui->actionCancel_selection->setIcon(FontIcon::icon(MaterialDesignIcons::SelectOff));
  _ui->actionGroup_elements->setIcon(FontIcon::icon(MaterialDesignIcons::Group));
  _ui->actionPreferences->setIcon(FontIcon::icon(MaterialDesignIcons::CogOutline));
  _ui->actionFull_screen->setIcon(FontIcon::icon(MaterialDesignIcons::Fullscreen));
  _ui->action_Close_All->setIcon(FontIcon::icon(MaterialDesignIcons::WindowClose));
  _ui->actionMessages_log->setIcon(FontIcon::icon(MaterialDesignIcons::Information));
  _ui->actionGraphs->setIcon(FontIcon::icon(MaterialDesignIcons::Graph));
  _ui->actionAlgorithms->setIcon(FontIcon::icon(MaterialDesignIcons::Cogs));
  _ui->actionSearch->setIcon(FontIcon::icon(MaterialDesignIcons::Magnify));
  _ui->actionAlgorithms->setIcon(FontIcon::icon(MaterialDesignIcons::Magnify));
  _ui->actionAbout_us->setIcon(FontIcon::icon(MaterialDesignIcons::InformationOutline));
  _ui->actionShowUserDocumentation->setIcon(FontIcon::icon(MaterialDesignIcons::BookOpenVariant));
  _ui->actionShowAPIDocumentation->setIcon(FontIcon::icon(MaterialDesignIcons::LanguageCpp));
  _ui->actionShowPythonDocumentation->setIcon(FontIcon::icon(MaterialDesignIcons::LanguagePython));
  _ui->actionPython_IDE->setIcon(FontIcon::icon(MaterialDesignIcons::LanguagePython));
  _ui->actionPlugins_Center->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowDownBoldBox));
  _ui->menuSelect->setIcon(FontIcon::icon(MaterialDesignIcons::SelectAll));
  _ui->actionSelect_All_Edges->setIcon(FontIcon::icon(MaterialDesignIcons::SelectAll));
  _ui->actionSelect_All_Nodes->setIcon(FontIcon::icon(MaterialDesignIcons::SelectAll));
  _ui->menuDelete->setIcon(FontIcon::icon(MaterialDesignIcons::DeleteOutline));
  _ui->actionDelete_all->setIcon(FontIcon::icon(MaterialDesignIcons::DeleteOutline));
  _ui->actionDelete_from_the_root_graph->setIcon(
      FontIcon::icon(MaterialDesignIcons::DeleteOutline));
  _ui->menuCreate->setIcon(FontIcon::icon(MaterialDesignIcons::Tournament, 1.0, -90));
  _ui->actionClone_sub_graph->setIcon(FontIcon::icon(MaterialDesignIcons::Tournament, 1.0, -90));
  _ui->actionCreate_sub_graph->setIcon(FontIcon::icon(MaterialDesignIcons::Tournament, 1.0, -90));
  _ui->actionCreate_empty_sub_graph->setIcon(
      FontIcon::icon(MaterialDesignIcons::Tournament, 1.0, -90));
  _ui->actionColor_scales_management->setIcon(FontIcon::icon(MaterialDesignIcons::PaletteOutline));
  _ui->actionReverse_selected_edges->setIcon(getReverseSelectedEdgesIcon());
  _ui->actionMake_selection_a_graph->setIcon(getMakeSelectionAGraphIcon());

  _pythonIDE = new PythonIDE();

  currentGraphChanged(nullptr);
  // set win/Mac dependent tooltips with ctrl shortcut

  setToolTipWithCtrlShortcut(_ui->searchButton, "Show/hide the graph's elements search interface",
                             "F");
  setToolTipWithCtrlShortcut(_ui->actionNewProject, "Start a new empty Talipot project", "Shift+N");
  setToolTipWithCtrlShortcut(
      _ui->actionSave_Project,
      "Save the current project (current graphs with current views) in the attached file", "S");
  setToolTipWithCtrlShortcut(_ui->actionSave_Project_as, "Save Project as a new file name",
                             "Shift+S");
  setToolTipWithCtrlShortcut(_ui->actionImport, "Display the Graph importing wizard", "Shift+O");
  setToolTipWithCtrlShortcut(_ui->actionExit, "Exit from Talipot", "Q");
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
                             "Deselect all selected elements of the current graph", "Shift+A");
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
  setToolTipWithCtrlShortcut(_ui->actionShowPythonDocumentation,
                             "Display the Talipot python documentation in a navigator", "P");
  setToolTipWithCtrlShortcut(_ui->actionMessages_log, "Show the message log", "M");
  setToolTipWithCtrlShortcut(_ui->actionPython_IDE, "Show the Python IDE", "Alt+P");
  setToolTipWithCtrlShortcut(_ui->actionExport, "Show the Graph exporting wizard", "E");
  setToolTipWithCtrlShortcut(_ui->actionOpen_Project, "Open a graph file", "O");
  setToolTipWithCtrlShortcut(_ui->actionGraphs, "Browse graphs hierarchy", "Alt+G");
  setToolTipWithCtrlShortcut(_ui->actionAlgorithms, "Browse and find algorithms", "Alt+H");
  setToolTipWithCtrlShortcut(_ui->actionSearch, "Search in graph properties", "F");
  setToolTipWithCtrlShortcut(_ui->actionNew_graph, "Create a new empty graph", "N");
  // set portable tooltips
  _ui->undoButton->setToolTip("Undo the latest update of the current graph");
  _ui->redoButton->setToolTip("Redo the latest undone update of the current graph");
  _ui->graphsButton->setToolTip("Display the existing graph views");
  _ui->pythonButton->setToolTip(
      "Display the Talipot Python IDE for developing scripts and plugins to "
      "execute on the loaded graphs");
  _ui->loggerMessageInfo->setToolTip("Show/Hide the Messages log panel");
  _ui->loggerMessagePython->setToolTip(_ui->loggerMessageInfo->toolTip());
  _ui->loggerMessageWarning->setToolTip(_ui->loggerMessageInfo->toolTip());
  _ui->loggerMessageError->setToolTip(_ui->loggerMessageInfo->toolTip());
  _ui->exportButton->setToolTip("Display the Graph exporting wizard");
  _ui->importButton->setToolTip("Display the Graph importing wizard");
  _ui->sidebarButton->setToolTip("Hide Sidebar");
  _ui->menubarButton->setToolTip("Hide Menubar");
  _ui->fullscreenButton->setToolTip("Enter fullscreen");
  _ui->fullscreenButton->setIcon(
      FontIcon::icon(MaterialDesignIcons::Fullscreen, QColor(Qt::white)));
  connect(_ui->fullscreenButton, &QPushButton::toggled,
          [this](bool) { _ui->actionFull_screen->trigger(); });
  _ui->menuDelete->setToolTip("Delete elements of the current graph");
  _ui->menuOpen_recent_file->setToolTip(
      "Choose a file to open among the recently opened/saved graphs or projects");
  _ui->actionDelete->setToolTip("Delete the selected elements from the current graph [Del]");
  _ui->actionFull_screen->setToolTip("Display Talipot in full screen mode [F11]");
  _ui->actionShow_Menubar->setToolTip("Show/Hide the main menu bar [Ctrl+Shift+M]");
  _ui->actionAbout_us->setToolTip("Display the 'About Talipot' information dialog [F1]");
  _ui->actionPlugins_Center->setToolTip("Display the Plugin center");
  _ui->actionImport_CSV->setToolTip("Import data in the current graph using a csv formatted file");
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

  _ui->workspace->setFocusedPanelHighlighting(true);

  _logger = new TalipotLogger(this);
  _ui->loggerFrame->installEventFilter(this);
  installEventFilter(this);
  setAcceptDrops(true);

  connect(_logger, &TalipotLogger::cleared, this, &TalipotMainWindow::logCleared);
  connect(_logger, &TalipotLogger::resetLoggerPosition, this,
          &TalipotMainWindow::resetLoggerDialogPosition);

  _colorScalesDialog = new ColorScaleConfigDialog(ColorScalesManager::getLatestColorScale(), this);

  // redirection of various output
  redirectStreamOutputsToQt();

  _ui->menuFile->setToolTipsVisible(true);
  _ui->menuEdit->setToolTipsVisible(true);
  _ui->menuHelp->setToolTipsVisible(true);
  _ui->menuWindow->setToolTipsVisible(true);

  Settings::synchronizeViewSettings();

  qInstallMessageHandler(talipotLogger);

  connect(_ui->workspace, &Workspace::addPanelRequest, this, &TalipotMainWindow::createPanel);
  connect(_ui->workspace, &Workspace::focusedPanelSynchronized, this,
          &TalipotMainWindow::focusedPanelSynchronized);
  connect(_graphs, &GraphHierarchiesModel::currentGraphChanged, this,
          &TalipotMainWindow::currentGraphChanged);
  connect(_graphs, &GraphHierarchiesModel::currentGraphChanged, _ui->algorithmRunner,
          &AlgorithmRunner::setGraph);
  connect(_ui->graphHierarchiesEditor, &GraphHierarchiesEditor::changeSynchronization, this,
          &TalipotMainWindow::changeSynchronization);

  // Connect actions
  connect(_ui->actionMessages_log, &QAction::triggered, this, &TalipotMainWindow::showLogger);
  connect(_ui->actionFull_screen, &QAction::triggered, this, &TalipotMainWindow::showFullScreen);
  connect(_ui->actionImport, &QAction::triggered, [this] { importGraph(); });
  connect(_ui->actionExport, &QAction::triggered, [this] { exportGraph(); });
  connect(_ui->actionSave_graph_to_file, &QAction::triggered,
          [this] { saveGraphHierarchyInTlpFile(); });
  connect(_ui->workspace, &Workspace::panelFocused, this, &TalipotMainWindow::panelFocused);
  connect(_ui->actionSave_Project, &QAction::triggered, this, &TalipotMainWindow::save);
  connect(_ui->actionSave_Project_as, &QAction::triggered, [this] { saveAs(); });
  connect(_ui->actionOpen_Project, &QAction::triggered, [this] { open(); });
  connect(_ui->actionDelete, &QAction::triggered, this, &TalipotMainWindow::deleteSelectedElements);
  connect(_ui->actionDelete_from_the_root_graph, &QAction::triggered, this,
          &TalipotMainWindow::deleteSelectedElementsFromRootGraph);
  connect(_ui->actionDelete_all, &QAction::triggered, this, &TalipotMainWindow::clearGraph);
  connect(_ui->actionInvert_selection, &QAction::triggered, this,
          &TalipotMainWindow::invertSelection);
  connect(_ui->actionCancel_selection, &QAction::triggered, this,
          &TalipotMainWindow::cancelSelection);
  connect(_ui->actionReverse_selected_edges, &QAction::triggered, this,
          &TalipotMainWindow::reverseSelectedEdges);
  connect(_ui->actionMake_selection_a_graph, &QAction::triggered, this,
          &TalipotMainWindow::make_graph);
  connect(_ui->actionSelect_All, &QAction::triggered, [this] { selectAll(); });
  connect(_ui->actionSelect_All_Nodes, &QAction::triggered, this,
          &TalipotMainWindow::selectAllNodes);
  connect(_ui->actionSelect_All_Edges, &QAction::triggered, this,
          &TalipotMainWindow::selectAllEdges);
  connect(_ui->actionUndo, &QAction::triggered, this, &TalipotMainWindow::undo);
  connect(_ui->actionRedo, &QAction::triggered, this, &TalipotMainWindow::redo);
  connect(_ui->actionCut, &QAction::triggered, this, &TalipotMainWindow::cut);
  connect(_ui->actionPaste, &QAction::triggered, this, &TalipotMainWindow::paste);
  connect(_ui->actionCopy, &QAction::triggered, [this] { copy(); });
  connect(_ui->actionGroup_elements, &QAction::triggered, this, &TalipotMainWindow::group);
  connect(_ui->actionCreate_sub_graph, &QAction::triggered, [this] { createSubGraph(); });
  connect(_ui->actionClone_sub_graph, &QAction::triggered, this, &TalipotMainWindow::cloneSubGraph);
  connect(_ui->actionCreate_empty_sub_graph, &QAction::triggered, this,
          &TalipotMainWindow::addEmptySubGraph);
  connect(_ui->actionImport_CSV, &QAction::triggered, this, &TalipotMainWindow::CSVImport);
  connect(_ui->actionAlgorithms, &QAction::triggered, this, &TalipotMainWindow::findPlugins);
  connect(_ui->actionNew_graph, &QAction::triggered, this, &TalipotMainWindow::addNewGraph);
  connect(_ui->actionNewProject, &QAction::triggered, this, &TalipotMainWindow::newProject);
  connect(_ui->actionPreferences, &QAction::triggered, this, &TalipotMainWindow::openPreferences);
  connect(_ui->graphsButton, &QAbstractButton::clicked, this,
          &TalipotMainWindow::dockButtonClicked);
  connect(_ui->algorithmsButton, &QAbstractButton::clicked, this,
          &TalipotMainWindow::dockButtonClicked);
  connect(_ui->searchButton, &QAbstractButton::clicked, this,
          &TalipotMainWindow::dockButtonClicked);
  connect(_ui->pythonButton, &QAbstractButton::clicked, this,
          &TalipotMainWindow::dockButtonClicked);
  connect(_ui->workspace, &Workspace::importGraphRequest, [this] { importGraph(); });
  connect(_ui->action_Close_All, &QAction::triggered, _ui->workspace, &Workspace::closeAll);
  connect(_ui->actionColor_scales_management, &QAction::triggered, this,
          &TalipotMainWindow::displayColorScalesDialog);
  connect(_ui->exportButton, &QAbstractButton::clicked, [this] { exportGraph(); });

  connect(_ui->actionPlugins_Center, &QAction::triggered, this,
          &TalipotMainWindow::showPluginsCenter);
  connect(_ui->actionAbout_us, &QAction::triggered, this, &TalipotMainWindow::showAboutDialog);

  auto showDockWidget = [this](QToolButton *button) {
    return [this, button]() { showHideDockWidget(button, true); };
  };

  connect(_ui->actionGraphs, &QAction::triggered, this, showDockWidget(_ui->graphsButton));
  connect(_ui->actionAlgorithms, &QAction::triggered, this, showDockWidget(_ui->algorithmsButton));
  connect(_ui->actionSearch, &QAction::triggered, this, showDockWidget(_ui->searchButton));
  connect(_ui->actionPython_IDE, &QAction::triggered, this, showDockWidget(_ui->pythonButton));

  _ui->actionShowUserDocumentation->setVisible(false);
  if (QFile(tlpStringToQString(tlp::TalipotShareDir) +
            "../doc/talipot/talipot-python/html/index.html")
          .exists()) {
    connect(_ui->actionShowPythonDocumentation, &QAction::triggered, this,
            &TalipotMainWindow::showPythonDocumentation);
  } else {
    _ui->actionShowPythonDocumentation->setVisible(false);
  }

  if (QFile(tlpStringToQString(tlp::TalipotShareDir) + "../doc/talipot/doxygen/html/index.html")
          .exists()) {
    connect(_ui->actionShowAPIDocumentation, &QAction::triggered, this,
            &TalipotMainWindow::showAPIDocumentation);
  } else {
    _ui->actionShowAPIDocumentation->setVisible(false);
  }

  connect(_project, &Project::projectFileChanged, this, &TalipotMainWindow::projectFileChanged);
  connect(this, &TalipotMainWindow::resetWindowTitle, [this] { projectFileChanged(); });

  // set a default position and size that works seamlessly on all platforms
  move(0, 0);
  QSize desktopSize = qApp->primaryScreen()->availableGeometry().size();
  resize(desktopSize * 0.9);
  _ui->workspace->setMinimumWidth(0.5 * desktopSize.width());
  _ui->mainSplitter->setSizes({int(0.29 * width()), int(0.71 * width())});
}

void TalipotMainWindow::buildRecentDocumentsMenu() {
  _ui->menuOpen_recent_file->clear();

  QStringList nonProjectFiles;

  for (const QString &file : Settings::recentDocuments()) {
    if (!QFileInfo(file).exists() || !talipotCanOpenFile(file)) {
      continue;
    }

    if (file.endsWith(".tlpx")) {
      QAction *action =
          _ui->menuOpen_recent_file->addAction(FontIcon::icon(MaterialDesignIcons::Archive), file,
                                               this, &TalipotMainWindow::openRecentFile);
      action->setData(file);
    } else {
      nonProjectFiles.append(file);
    }
  }

  _ui->menuOpen_recent_file->addSeparator();

  for (const QString &file : nonProjectFiles) {
    QAction *action =
        _ui->menuOpen_recent_file->addAction(FontIcon::icon(MaterialDesignIcons::FileOutline), file,
                                             this, &TalipotMainWindow::openRecentFile);
    action->setData(file);
  }
  _ui->menuOpen_recent_file->setEnabled(!_ui->menuOpen_recent_file->isEmpty());
}

void TalipotMainWindow::addRecentDocument(const QString &path) {
  if (!talipotCanOpenFile(path)) {
    return;
  }
  Settings::addToRecentDocuments(path);
  Settings::instance().sync();
  buildRecentDocumentsMenu();
}

void TalipotMainWindow::updateLogIconsAndCounters() {
  TalipotLogger::LogType logType = _logger->getLastLogType();
  QFrame *logIconCounterFrame = nullptr;
  QLabel *logIconLabel = nullptr;
  QLabel *logCounterLabel = nullptr;

  if (logType == TalipotLogger::Info) {
    logIconCounterFrame = _ui->loggerFrameInfo;
    logIconLabel = _ui->loggerIconInfo;
    logCounterLabel = _ui->loggerMessageInfo;
  } else if (logType == TalipotLogger::Warning) {
    logIconCounterFrame = _ui->loggerFrameWarning;
    logIconLabel = _ui->loggerIconWarning;
    logCounterLabel = _ui->loggerMessageWarning;
  } else if (logType == TalipotLogger::Error) {
    logIconCounterFrame = _ui->loggerFrameError;
    logIconLabel = _ui->loggerIconError;
    logCounterLabel = _ui->loggerMessageError;
  } else {
    logIconCounterFrame = _ui->loggerFramePython;
    logIconLabel = _ui->loggerIconPython;
    logCounterLabel = _ui->loggerMessagePython;
  }

  logIconCounterFrame->setVisible(true);
  logIconLabel->setPixmap(_logger->icon(logType).pixmap(QSize(16, 16)));
  logIconLabel->setToolTip("Click here to show/hide the message log window");
  logCounterLabel->setText(QString::number(_logger->countByType(logType)));
  logCounterLabel->setToolTip("Click here to show/hide the message log window");
}

void TalipotMainWindow::log(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  _logger->log(type, context, msg);
  updateLogIconsAndCounters();
}

TalipotMainWindow::~TalipotMainWindow() {

  qInstallMessageHandler(nullptr);

  // ensure all loaded graphs are deleted
  for (auto *graph : _graphs->graphs()) {
    delete graph;
  }

  delete _project;
  delete _ui;
}

bool TalipotMainWindow::terminated() {

  if (_pythonIDE->isScriptRunning()) {
    _pythonIDE->pauseCurrentScript();
    QString message("A Python script is currently running.\nDo you really want to exit ?");
    QMessageBox::StandardButton answer =
        QMessageBox::question(this, "Exit ?", message, QMessageBox::Yes | QMessageBox::No);

    if (answer == QMessageBox::No) {
      _pythonIDE->executeCurrentScript();
      return false;
    }
  }

  if (_graphs->needsSaving() || isWindowModified()) {
    QString message("The project has been modified (loaded graphs or Python files opened in the "
                    "IDE).\nDo you want to save your changes?");
    QMessageBox::StandardButton answer = QMessageBox::question(
        this, "Save project ?", message,
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel | QMessageBox::Escape);

    if (answer == QMessageBox::Cancel) {
      if (_pythonIDE->isScriptRunning()) {
        _pythonIDE->executeCurrentScript();
      }
      return false;
    } else if (answer == QMessageBox::Yes) {
      _pythonIDE->stopCurrentScript();
      _pythonIDE->savePythonFilesAndWriteToProject(true);
      return save();
    }
  }

  _pythonIDE->hide();
  _pythonIDE->stopCurrentScript();

  // force workspace and views destruction here to avoid hanging on exit
  // when linking against QtWebEngine binaries provided by qt.io
  _ui->workspace->closeAll();
  return true;
}

void TalipotMainWindow::logCleared() {
  _ui->loggerMessageInfo->clear();
  _ui->loggerIconInfo->clear();
  _ui->loggerFrameInfo->setVisible(false);
  _ui->loggerFrameWarning->setVisible(false);
  _ui->loggerFrameError->setVisible(false);
  _ui->loggerFramePython->setVisible(false);
}

void TalipotMainWindow::findPlugins() {
  _ui->algorithmRunner->findPlugins();
}

bool TalipotMainWindow::eventFilter(QObject *obj, QEvent *ev) {
  if (ev->type() == QEvent::DragEnter) {
    auto *dragEvent = static_cast<QDragEnterEvent *>(ev);

    if (dragEvent->mimeData()->hasUrls()) {
      dragEvent->accept();
    }
  }

  if (ev->type() == QEvent::Drop) {
    auto *dropEvent = static_cast<QDropEvent *>(ev);

    for (const QUrl &url : dropEvent->mimeData()->urls()) {
      open(url.toLocalFile());
    }
  }

  if (obj == _ui->loggerFrame && ev->type() == QEvent::MouseButtonPress)
    showHideLogger();

  if (obj == this && _logger &&
      (ev->type() == QEvent::Resize || ev->type() == QEvent::Move ||
       ev->type() == QEvent::WindowStateChange)) {

    if (_logger->anchored()) {
      resetLoggerDialogPosition();
    }
  }

  return false;
}

void TalipotMainWindow::showLogger() {
  _logger->show();

  static bool firstTime = true;

  if (firstTime) {
    resetLoggerDialogPosition();
    firstTime = false;
  }
}

void TalipotMainWindow::showHideLogger() {

  if (!_logger->isVisible()) {
    showLogger();
  } else {
    _logger->hide();
  }
}

void TalipotMainWindow::redrawPanels(bool center) {
  _ui->workspace->redrawPanels(center);
}

void TalipotMainWindow::start(const QString &inputFilePath) {

  // these ui initializations are needed here
  // in case of a call to showStartPanels in the open method
  _ui->graphHierarchiesEditor->setModel(_graphs);
  _ui->workspace->setModel(_graphs);
  _ui->searchWidget->setModel(_graphs);

  _pythonIDE->setGraphsModel(_graphs);
  connect(_pythonIDE, &PythonIDE::anchoredRequest, this, &TalipotMainWindow::anchoredPythonIDE);
  tlp::PluginsManager::instance().addListener(this);
  QTimer::singleShot(100, this, &TalipotMainWindow::initPythonIDE);

  if (!inputFilePath.isEmpty() && QFileInfo(inputFilePath).exists()) {
    open(inputFilePath);
  } else {
    projectFileChanged();
  }

  connect(_ui->sidebarButton, &QAbstractButton::clicked, [this] { showHideSideBar(); });
  connect(_ui->menubarButton, &QAbstractButton::clicked, this, &TalipotMainWindow::showHideMenuBar);

#if defined(Q_OS_MAC)
  // menu bar cannot be hidden on MacOS
  _ui->menubarButton->setEnabled(false);
#endif

  // fill menu with recent documents
  buildRecentDocumentsMenu();

  logCleared();
}

tlp::GraphHierarchiesModel *TalipotMainWindow::model() const {
  return _graphs;
}

void TalipotMainWindow::exportGraph(Graph *g) {
  if (g == nullptr) {
    g = _graphs->currentGraph();
  }

  if (g == nullptr) {
    return;
  }

  static QString exportFile;
  ExportWizard wizard(g, exportFile, this);
  wizard.setWindowTitle(QString("Exporting graph \"") + tlpStringToQString(g->getName()) + '"');

  if (wizard.exec() != QDialog::Accepted || wizard.algorithm().isEmpty() ||
      wizard.outputFile().isEmpty()) {
    return;
  }

  std::string filename = QStringToTlpString(exportFile = wizard.outputFile());
  std::string exportPluginName = QStringToTlpString(wizard.algorithm());

  DataSet data = wizard.parameters();
  PluginProgress *prg = progress(NoProgressOption);
  prg->setTitle(exportPluginName);
  // take time before run
  QTime start = QTime::currentTime();
  bool result = tlp::saveGraph(g, filename, prg, &data);

  if (!result) {
    qDetailedMessageBox(QMessageBox::Critical, "Export error",
                        QString("<b>%1</b> export module failed to export graph.")
                            .arg(tlp::tlpStringToQString(exportPluginName)),
                        tlp::tlpStringToQString(prg->getError()), this);
  } else {
    // log export plugin call
    if (Settings::logPluginCall() != Settings::NoLog) {
      std::stringstream log;
      log << exportPluginName.c_str() << " - " << data.toString().c_str();

      if (Settings::logPluginCall() == Settings::LogCallWithExecutionTime) {
        log << ": " << start.msecsTo(QTime::currentTime()) << "ms";
      }

      qDebug() << log.str().c_str();
    }

    addRecentDocument(wizard.outputFile());
  }

  delete prg;
}

void TalipotMainWindow::saveGraphHierarchyInTlpFile(Graph *g) {
  if (g == nullptr) {
    g = _graphs->currentGraph();
  }

  if (g == nullptr) {
    return;
  }

  static QString savedFile;
  QString filter("TLP format (*.tlp *.tlp.gz *.tlpz);;TLPB format (*.tlpb *.tlpb.gz *.tlpbz)");
  QString filename = QFileDialog::getSaveFileName(this, tr("Save graph hierarchy in tlp/tlpb file"),
                                                  savedFile, filter);

  if (!filename.isEmpty()) {
    bool result = tlp::saveGraph(g, tlp::QStringToTlpString(filename));

    if (!result) {
      QMessageBox::critical(this, "Save error", "Failed to save graph hierarchy");
    } else {
      savedFile = filename;
      addRecentDocument(filename);
    }
  }
}

void TalipotMainWindow::importGraph(const std::string &module, DataSet &data) {
  Graph *g;

  if (!module.empty()) {
    PluginProgress *prg = progress(ProgressOptions(IsStoppable | IsCancellable));
    prg->setTitle(module);
    // take time before run
    QTime start = QTime::currentTime();
    g = tlp::importGraph(module, data, prg);

    if (g == nullptr) {
      qDetailedMessageBox(QMessageBox::Critical, "Import error",
                          QString("<b>%1</b> import module failed to import graph.")
                              .arg(tlp::tlpStringToQString(module)),
                          tlp::tlpStringToQString(prg->getError()), this);
      delete prg;
      return;
    }

    delete prg;

    // log import plugin call
    if (Settings::logPluginCall() != Settings::NoLog) {
      std::stringstream log;
      log << module.c_str() << " import - " << data.toString().c_str();

      if (Settings::logPluginCall() == Settings::LogCallWithExecutionTime) {
        log << ": " << start.msecsTo(QTime::currentTime()) << "ms";
      }

      qDebug() << log.str().c_str();
    }

    if (g->getName().empty()) {
      g->setName(module);
      g->setAttribute("Import algorithm", module);
      g->setAttribute("Import parameters", data.toString());
    }
  } else {
    g = tlp::newGraph();
  }

  projectFileChanged();
  _graphs->addGraph(g);
  std::string fileName;

  if (data.get("file::filename", fileName))
    // set current directory to the directory of the loaded file
    // to ensure a correct loading of the associated texture files if any
    QDir::setCurrent(QFileInfo(tlpStringToQString(fileName)).absolutePath());

  applyDefaultLayout(g);
  showStartPanels(g);
}

void TalipotMainWindow::importGraph() {
  ImportWizard wizard(this);

  if (wizard.exec() == QDialog::Accepted) {
    DataSet data = wizard.parameters();
    importGraph(tlp::QStringToTlpString(wizard.algorithm()), data);
  }
}

void TalipotMainWindow::createPanel(tlp::Graph *g) {
  if (_graphs->empty()) {
    return;
  }

  PanelSelectionWizard wizard(_graphs, this);

  if (g != nullptr) {
    wizard.setSelectedGraph(g);
  } else {
    wizard.setSelectedGraph(_graphs->currentGraph());
  }

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

void TalipotMainWindow::panelFocused(tlp::View *view) {
  disconnect(view, &View::graphSet, this, &TalipotMainWindow::focusedPanelGraphSet);

  if (!_ui->graphHierarchiesEditor->synchronized())
    return;

  connect(view, &View::graphSet, this, &TalipotMainWindow::focusedPanelGraphSet);
  focusedPanelGraphSet(view->graph());
}

void TalipotMainWindow::changeSynchronization(bool s) {
  _ui->workspace->setFocusedPanelHighlighting(s);
}

void TalipotMainWindow::focusedPanelGraphSet(Graph *g) {
  _graphs->setCurrentGraph(g);
}

void TalipotMainWindow::focusedPanelSynchronized() {
  _ui->workspace->setGraphForFocusedPanel(_graphs->currentGraph());
}

bool TalipotMainWindow::save() {
  return saveAs(_project->projectFile());
}

bool TalipotMainWindow::saveAs(const QString &path) {
  if (_graphs->empty()) {
    return false;
  }

  if (path.isEmpty()) {
    QString path =
        QFileDialog::getSaveFileName(this, "Save project", QString(), "Talipot Project (*.tlpx)");

    if (!path.isEmpty()) {
      if (!path.endsWith(".tlpx")) {
        path += ".tlpx";
      }

      _project->setProjectFile(path);
      return saveAs(path);
    }

    return false;
  }

  SimplePluginProgressDialog progress(this);
  progress.showPreview(false);
  progress.show();
  QMap<Graph *, QString> rootIds = _graphs->writeProject(_project, &progress);
  _ui->workspace->writeProject(_project, rootIds, &progress);
  _pythonIDE->savePythonFilesAndWriteToProject();
  bool ret = _project->write(path, &progress);

  if (ret) {
    addRecentDocument(path);
  }

  return ret;
}

void TalipotMainWindow::open(QString fileName) {

  QMap<std::string, std::string> modules;

  std::string filters("Talipot project (*.tlpx);;");
  std::string filterAny("Any supported format (");

  for (const auto &pluginName : PluginsManager::availablePlugins<ImportModule>()) {
    const auto &importPlugin =
        static_cast<const ImportModule &>(PluginsManager::pluginInformation(pluginName));
    std::string currentFilter;

    for (const auto &ext : importPlugin.allFileExtensions()) {

      if (ext.empty())
        continue;

      filterAny += "*." + ext + " ";
      currentFilter += "*." + ext + " ";

      modules[ext] = pluginName;
    }

    if (!currentFilter.empty())
      filters += pluginName + "(" + currentFilter + ");;";
  }

  filterAny += " *.tlpx);;";

  filters += "All files (*)";
  filters.insert(0, filterAny);

  if (fileName.isEmpty()) { // If open() was called without a parameter, open the file dialog
    fileName =
        QFileDialog::getOpenFileName(this, tr("Open graph"), _lastOpenLocation, filters.c_str());
  }

  if (!fileName.isEmpty()) {
    QFileInfo fileInfo(fileName);

    _lastOpenLocation = fileInfo.absolutePath();

    if (fileName.endsWith(".tlpx")) {
      openProjectFile(fileName);
      addRecentDocument(QFileInfo(fileName).absoluteFilePath());
      return;
    }

    for (const std::string &extension : modules.keys()) {
      if (fileName.endsWith(QString::fromStdString(extension))) {
        DataSet params;
        params.set("file::filename", QStringToTlpString(fileName));
        addRecentDocument(fileName);
        importGraph(modules[extension], params);
        break;
      }
    }
  }
}

void TalipotMainWindow::openProjectFile(const QString &path) {
  cleanProject();
  PluginProgress *prg = progress(NoProgressOption);
  if (_project->openProjectFile(path, prg)) {
    QMap<QString, tlp::Graph *> rootIds = _graphs->readProject(_project, prg);
    _ui->workspace->readProject(_project, rootIds, prg);
    QTimer::singleShot(100, this, &TalipotMainWindow::initPythonIDE);
  } else {
    _project->clearProject();
    QMessageBox::critical(this,
                          QString("Error while loading project ").append(_project->projectFile()),
                          QString("The Talipot project file is probably corrupted:<br>") +
                              tlpStringToQString(prg->getError()));
  }
  delete prg;
}

void TalipotMainWindow::initPythonIDE() {
  _pythonIDE->setProject(_project);
  _pythonIDE->setAnchored(Settings::pythonIDEAnchored());
  if (Settings::pythonIDEAnchored()) {
    _pythonIDE->setVisible(false);
    _pythonIDE->setParent(nullptr);
    _ui->docksWidget->addWidget(_pythonIDE);
    _ui->pythonButton->setCheckable(true);
  } else {
    auto desktopSize = qApp->primaryScreen()->availableGeometry().size();
    _pythonIDE->resize(desktopSize.width() * 0.3, desktopSize.height() * 0.7);
    _pythonIDE->move(desktopSize.width() / 2 - _pythonIDE->width() / 2,
                     desktopSize.height() / 2 - _pythonIDE->height() / 2);
  }
}

void TalipotMainWindow::anchoredPythonIDE(bool anchored) {
  _ui->pythonButton->setCheckable(anchored);
  Settings::setPythonIDEAnchored(anchored);
  if (anchored) {
    _pythonIDE->hide();
    _pythonIDE->setParent(nullptr);
    _ui->docksWidget->addWidget(_pythonIDE);
    showHideDockWidget(_ui->pythonButton, true);
  } else {
    showHideDockWidget(_ui->graphsButton, true);
    _ui->docksWidget->removeWidget(_pythonIDE);
    _pythonIDE->setParent(nullptr);
    auto desktopSize = qApp->primaryScreen()->availableGeometry().size();
    _pythonIDE->resize(desktopSize.width() * 0.3, desktopSize.height() * 0.7);
    _pythonIDE->show();
  }
  _pythonIDE->setAnchored(anchored);
  showPythonIDE();
}

void TalipotMainWindow::deleteSelectedElementsFromRootGraph() {
  deleteSelectedElements(true);
}

void TalipotMainWindow::clearGraph() {
  if (QMessageBox::question(this, "Clear graph content",
                            "Do you really want to remove all nodes and edges from the current "
                            "graph. This action cannot be undone",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    _graphs->currentGraph()->clear();
  }
}

void TalipotMainWindow::deleteSelectedElements(bool fromRoot) {
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

void TalipotMainWindow::invertSelection() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->reverse(graph);
  Observable::unholdObservers();
}

void TalipotMainWindow::reverseSelectedEdges() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->reverseEdgeDirection(graph);
  graph->popIfNoUpdates();
  Observable::unholdObservers();
}

void TalipotMainWindow::cancelSelection() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->setAllNodeValue(false, graph);
  selection->setAllEdgeValue(false, graph);
  graph->popIfNoUpdates();
  Observable::unholdObservers();
}

void TalipotMainWindow::selectAll(bool nodes, bool edges) {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();
  tlp::BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  graph->push();
  selection->setAllNodeValue(false);
  selection->setAllEdgeValue(false);

  if (nodes) {
    selection->setAllNodeValue(true, graph);
  }

  if (edges) {
    selection->setAllEdgeValue(true, graph);
  }

  Observable::unholdObservers();
}

void TalipotMainWindow::selectAllEdges() {
  selectAll(false, true);
}

void TalipotMainWindow::selectAllNodes() {
  selectAll(true, false);
}

void TalipotMainWindow::undo() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();

  if (graph != nullptr) {
    graph->pop();
  }

  Observable::unholdObservers();

  for (auto *v : _ui->workspace->panels()) {
    if (v->graph() == graph)
      v->undoCallback();
  }
}

void TalipotMainWindow::redo() {
  Observable::holdObservers();
  tlp::Graph *graph = _graphs->currentGraph();

  if (graph != nullptr) {
    graph->unpop();
  }

  Observable::unholdObservers();

  for (auto *v : _ui->workspace->panels()) {
    if (v->graph() == graph)
      v->undoCallback();
  }
}

void TalipotMainWindow::cut() {
  copy(_graphs->currentGraph(), true);
}

void TalipotMainWindow::paste() {
  if (_graphs->currentGraph() == nullptr) {
    return;
  }

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

void TalipotMainWindow::copy() {
  copy(_graphs->currentGraph());
}

void TalipotMainWindow::copy(Graph *g, bool deleteAfter) {
  if (g == nullptr) {
    return;
  }

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

void TalipotMainWindow::group() {
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

  if (!changeGraph) {
    return;
  }

  for (auto *v : _ui->workspace->panels()) {
    if (v->graph() == graph->getRoot())
      v->setGraph(graph);
  }
}

void TalipotMainWindow::make_graph() {
  Graph *graph = _graphs->currentGraph();
  makeSelectionGraph(_graphs->currentGraph(), graph->getBooleanProperty("viewSelection"));
}

Graph *TalipotMainWindow::createSubGraph(Graph *graph) {
  if (graph == nullptr) {
    return nullptr;
  }

  graph->push();
  Observable::holdObservers();
  BooleanProperty *selection = graph->getBooleanProperty("viewSelection");
  makeSelectionGraph(graph, selection);
  Graph *result = graph->addSubGraph(selection, "selection subgraph");
  Observable::unholdObservers();
  return result;
}

void TalipotMainWindow::createSubGraph() {
  createSubGraph(_graphs->currentGraph());
}

void TalipotMainWindow::cloneSubGraph() {
  if (_graphs->currentGraph() == nullptr) {
    return;
  }

  tlp::BooleanProperty prop(_graphs->currentGraph());
  prop.setAllNodeValue(true);
  prop.setAllEdgeValue(true);
  _graphs->currentGraph()->push();
  _graphs->currentGraph()->addSubGraph(&prop, "clone subgraph");
}

void TalipotMainWindow::addEmptySubGraph() {
  if (_graphs->currentGraph() == nullptr) {
    return;
  }

  _graphs->currentGraph()->push();
  _graphs->currentGraph()->addSubGraph(nullptr, "empty subgraph");
}

void TalipotMainWindow::currentGraphChanged(Graph *graph) {
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
  _ui->searchButton->setEnabled(enabled);
  _ui->algorithmsButton->setEnabled(enabled);
  _ui->graphsButton->setEnabled(enabled);
  _ui->undoButton->setEnabled(enabled);
  _ui->redoButton->setEnabled(enabled);
  _ui->exportButton->setEnabled(enabled);
  _ui->actionSave_Project->setEnabled(enabled);
  _ui->actionSave_Project_as->setEnabled(enabled);
  _ui->actionGraphs->setEnabled(enabled);
  _ui->actionAlgorithms->setEnabled(enabled);
  _ui->actionSearch->setEnabled(enabled);

  if (graph == nullptr) {
    _ui->workspace->switchToStartupMode();
    _ui->searchButton->setChecked(false);
    _ui->actionSave_Project->setEnabled(false);
    _ui->actionSave_Project_as->setEnabled(false);
  } else {
    _ui->workspace->setGraphForFocusedPanel(graph);
  }

  if (_graphs->empty()) {
    _pythonIDE->clearPythonCodeEditors();
    _pythonIDE->hide();
    _ui->pythonButton->setEnabled(false);
    _ui->actionPython_IDE->setEnabled(false);
  } else {
    _ui->pythonButton->setEnabled(true);
    _ui->actionPython_IDE->setEnabled(true);
  }
}

void TalipotMainWindow::CSVImport() {
  bool mustDeleteGraph = false;

  if (_graphs->empty()) {
    _graphs->addGraph(tlp::newGraph());
    mustDeleteGraph = true;
  }

  Graph *g = _graphs->currentGraph();

  if (g == nullptr) {
    return;
  }

  CSVImportWizard wizard(this);

  if (mustDeleteGraph) {
    wizard.setWindowTitle("Import CSV data into a new graph");
    wizard.setButtonText(QWizard::FinishButton, QString("Import into a new graph"));
  } else {
    wizard.setWindowTitle(QString("Import CSV data into current graph: ") + g->getName().c_str());
    wizard.setButtonText(QWizard::FinishButton, QString("Import into current graph"));
  }

  // get the number of line displayed in the logger
  uint nbLogsBefore = _logger->countByType(TalipotLogger::Error);
  nbLogsBefore += _logger->countByType(TalipotLogger::Warning);

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
    uint nbLogsAfter = _logger->countByType(TalipotLogger::Error);
    nbLogsAfter += _logger->countByType(TalipotLogger::Warning);
    applyDefaultLayout(g);
    bool openPanels = true;

    for (auto *v : _ui->workspace->panels()) {
      if (v->graph() == g) {
        openPanels = false;
        break;
      }
    }

    if (openPanels) {
      showStartPanels(g);
    } else {
      centerPanelsForGraph(g);
    }

    unsigned nb_error = nbLogsAfter - nbLogsBefore;
    if ((nb_error == 1) &&
        (QMessageBox::question(this, "CSV parse error",
                               "When parsing your CSV file,<br/> one error has been "
                               "encountered.<br/>Do you want to see it?",
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)) {
      showLogger();
    }
    if ((nb_error > 1) &&
        (QMessageBox::question(this, "CSV parse errors",
                               QString("When parsing your CSV file,<br/> %1 errors have been "
                                       "encountered.<br/>Do you want to see them?")
                                   .arg(nb_error),
                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)) {
      showLogger();
    }

    g->popIfNoUpdates();
  }
}

void TalipotMainWindow::showStartPanels(Graph *g) {
  if (!Settings::displayDefaultViews()) {
    return;
  }

  // expose mode is not safe to add a new panel
  // so hide it if needed
  _ui->workspace->hideExposeMode();
  View *firstPanel = nullptr;

  for (const auto *panelName : {"Node Link Diagram", "Spreadsheet"}) {
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
  _ui->workspace->switchToSplitHorizontalMode();
}

void TalipotMainWindow::applyDefaultLayout(Graph *g) {
  Observable::holdObservers();
  LayoutProperty *viewLayout = g->getLayoutProperty("viewLayout");

  if (!viewLayout->hasNonDefaultValuatedNodes(g)) {
    std::string str;
    std::string layoutAlgorithm = "FM^3 (OGDF)";
    if (TreeTest::isTree(g)) {
      layoutAlgorithm = "Bubble Tree";
    }
    g->applyPropertyAlgorithm(layoutAlgorithm, viewLayout, str);
  }

  Observable::unholdObservers();
}

void TalipotMainWindow::centerPanelsForGraph(tlp::Graph *g, bool graphChanged, bool onlyGlView) {
  for (auto *v : _ui->workspace->panels()) {
    if ((v->graph() == g) && (!onlyGlView || dynamic_cast<tlp::GlView *>(v)))
      v->centerView(graphChanged);
  }
}

void TalipotMainWindow::closePanelsForGraph(tlp::Graph *g) {
  list<View *> viewsToDelete;

  for (auto *v : _ui->workspace->panels()) {
    if (v->graph() == g || g->isDescendantGraph(v->graph()))
      viewsToDelete.push_back(v);
  }

  if (!viewsToDelete.empty()) {
    // expose mode is not safe when deleting a panel
    // so hide it first
    _ui->workspace->hideExposeMode();
    for (auto *v : viewsToDelete) {
      _ui->workspace->delView(v);
    }
  }
}

bool TalipotMainWindow::setGlViewPropertiesForGraph(
    tlp::Graph *g, const std::map<std::string, tlp::PropertyInterface *> &propsMap) {
  bool result = false;

  for (auto *v : _ui->workspace->panels()) {
    auto *glView = dynamic_cast<tlp::GlView *>(v);

    if (v->graph() == g && glView != nullptr) {
      if (glView->glWidget()->inputData()->installProperties(propsMap))
        result = true;
    }
  }

  return result;
}

void TalipotMainWindow::openPreferences() {
  PreferencesDialog dlg(_ui->mainWidget);
  dlg.readSettings();

  if (dlg.exec() == QDialog::Accepted) {
    dlg.writeSettings();
  }
}

void TalipotMainWindow::setAutoCenterPanelsOnDraw(bool f) {
  _ui->workspace->setAutoCenterPanelsOnDraw(f);
}

void TalipotMainWindow::pluginsListChanged() {
  _ui->algorithmRunner->refreshPluginsList();
}

void TalipotMainWindow::addNewGraph() {
  Graph *g = tlp::newGraph();
  _graphs->addGraph(g);
  showStartPanels(g);
}

void TalipotMainWindow::newProject() {
  cleanProject();
  _project->setProjectFile("");
}

void TalipotMainWindow::openRecentFile() {
  auto *action = static_cast<QAction *>(sender());
  open(action->data().toString());
}

void TalipotMainWindow::treatEvent(const tlp::Event &ev) {
  if (dynamic_cast<const tlp::PluginEvent *>(&ev)) {
    pluginsListChanged();
  }
}

void TalipotMainWindow::showPythonIDE() {
  if (!_pythonIDE->isAnchored()) {
    if (!_pythonIDE->isVisible() || _pythonIDE->windowState() == Qt::WindowMinimized) {
      _pythonIDE->show();
      _pythonIDE->raise();
      _pythonIDE->setWindowState((_pythonIDE->windowState() & ~Qt::WindowMinimized) |
                                 Qt::WindowActive);
    } else {
      _pythonIDE->setWindowState(Qt::WindowMinimized);
    }
  } else {
    _pythonIDE->setVisible(true);
  }
}

void TalipotMainWindow::showUserDocumentation() {
  QDesktopServices::openUrl(QUrl::fromLocalFile(tlpStringToQString(tlp::TalipotShareDir) +
                                                "../doc/talipot/talipot-user/html/index.html"));
}

void TalipotMainWindow::showPythonDocumentation() {
  QDesktopServices::openUrl(QUrl::fromLocalFile(tlpStringToQString(tlp::TalipotShareDir) +
                                                "../doc/talipot/talipot-python/html/index.html"));
}

void TalipotMainWindow::showAPIDocumentation() {
  QDesktopServices::openUrl(QUrl::fromLocalFile(tlpStringToQString(tlp::TalipotShareDir) +
                                                "../doc/talipot/doxygen/html/index.html"));
}

void TalipotMainWindow::showHideSideBar(bool forceShow) {
  bool newState = forceShow || !_ui->docksWidget->isVisible();
  _ui->docksWidget->setVisible(newState);
  _ui->sidebarButton->setToolTip(newState ? "Hide Sidebar" : "Show Sidebar");
  _ui->sidebarButton->setChecked(newState);

  if (_logger->anchored()) {
    resetLoggerDialogPosition();
  }

  static const QMap<QWidget *, QToolButton *> widgetToButton = {
      {_ui->docksWidget->widget(0), _ui->graphsButton},
      {_ui->docksWidget->widget(1), _ui->algorithmsButton},
      {_ui->docksWidget->widget(2), _ui->searchButton},
      {_pythonIDE, _ui->pythonButton}};

  if (!newState) {
    for (auto *button : widgetToButton.values()) {
      button->setChecked(false);
    }
  } else {
    for (auto *widget : widgetToButton.keys()) {
      widgetToButton[widget]->setChecked(_ui->docksWidget->currentWidget() == widget);
    }
  }
}

void TalipotMainWindow::showHideMenuBar() {
  if (menuBar()->isVisible()) {
    menuBar()->setVisible(false);
    _ui->menubarButton->setToolTip("Show Menubar");
  } else {
    menuBar()->setVisible(true);
    _ui->menubarButton->setToolTip("Hide Menubar");
  }
}

void TalipotMainWindow::displayColorScalesDialog() {
  _colorScalesDialog->show();
}

void TalipotMainWindow::showAboutDialog() {
  AboutDialog aboutDialog(this);
  aboutDialog.adjustSize();
  aboutDialog.setFixedSize(aboutDialog.size());
  aboutDialog.exec();
}

void TalipotMainWindow::dockButtonClicked() {
  showHideDockWidget(static_cast<QToolButton *>(sender()));
}

void TalipotMainWindow::showHideDockWidget(QToolButton *button, bool forceShow) {
  static const QVector<QToolButton *> dockButtons = {_ui->graphsButton, _ui->algorithmsButton,
                                                     _ui->searchButton, _ui->pythonButton};
  bool newState = forceShow || button->isChecked();
  for (auto *dockButton : dockButtons) {
    if (dockButton != button) {
      if (dockButton == _ui->pythonButton && _pythonIDE->isAnchored()) {
        dockButton->setChecked(false);
      } else if (_pythonIDE->isAnchored()) {
        dockButton->setChecked(false);
      }
    }
  }

  if (button == _ui->pythonButton && !_pythonIDE->isAnchored()) {
    showPythonIDE();
  } else {
    if (!_ui->docksWidget->isVisible() && !newState) {
      button->setChecked(true);
    } else {
      button->setChecked(newState);
    }
    _ui->docksWidget->setCurrentIndex(dockButtons.indexOf(button));
    showHideSideBar(newState);
  }
}

void TalipotMainWindow::resetLoggerDialogPosition() {
  QPoint pos = mapToGlobal(_ui->exportButton->pos());
  pos.setX(pos.x() + _ui->loggerFrame->width());

  if (_logger->isVisible()) {
    _logger->showNormal();
  }

  // extend the logger frame width until reaching the right side of the main window
  _logger->setGeometry(pos.x(), pos.y(), width() - _ui->loggerFrame->width(),
                       mapToGlobal(QPoint(0, 0)).y() + height() - pos.y() - 2);
}

PluginProgress *TalipotMainWindow::progress(ProgressOptions options) {
  auto *dlg = new SimplePluginProgressDialog(this);
  dlg->setWindowIcon(windowIcon());
  dlg->showPreview(options.testFlag(IsPreviewable));
  dlg->setCancelButtonVisible(options.testFlag(IsCancellable));
  dlg->setStopButtonVisible(options.testFlag(IsStoppable));
  dlg->show();
  QApplication::processEvents();
  return dlg;
}

void TalipotMainWindow::closeEvent(QCloseEvent *event) {
  if (terminated()) {
    QMainWindow::closeEvent(event);
  } else {
    event->ignore();
  }
}

void TalipotMainWindow::projectFileChanged(const QString &projectFile) {
  QString wTitle(_title);

  if (!_project->name().isEmpty()) {
    wTitle += QString(" - ") + _project->name();
  } else if (!_project->projectFile().isEmpty()) {
    wTitle += QString(" - ") + _project->projectFile();
  } else if (!projectFile.isEmpty()) {
    wTitle += QString(" - ") + projectFile;
  } else { // All graphs are deleted, so clear project.
    _project->clearProject();
    wTitle += QString(" - unsaved project");
  }

  wTitle += "[*]"; // placeholder for window modification
#ifndef NDEBUG
  wTitle += " - [ Debug mode ]";
#endif
  setWindowTitle(wTitle);
  setWindowModified(false);
}

void TalipotMainWindow::cleanProject() {
  if (!_graphs->empty()) {
    terminated();
    // ensure the opened views and interactors get deleted before the loaded graphs
    // to avoid possible segfaults
    _ui->workspace->closeAll();

    // ensure all loaded graphs are deleted
    for (Graph *graph : _graphs->graphs()) {
      delete graph;
    }

    _pythonIDE->clearPythonCodeEditors();
  }
  _graphs->clear(_project);
  _project->clearProject();
}

void TalipotMainWindow::showFullScreen(bool f) {
  if (f) {
    _maximized = isMaximized();
    QMainWindow::showFullScreen();
    _ui->fullscreenButton->setToolTip("Exit fullscreen");
  } else {
    showNormal();

    if (_maximized) {
      showMaximized();
    }
    _ui->fullscreenButton->setToolTip("Enter fullscreen");
  }
}

void TalipotMainWindow::showPluginsCenter() {
  _pluginsCenter->setWindowIcon(windowIcon());
  _pluginsCenter->show();
}

Workspace *TalipotMainWindow::workspace() const {
  return _ui->workspace;
}
