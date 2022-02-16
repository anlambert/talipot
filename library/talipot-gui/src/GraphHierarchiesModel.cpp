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

#include "talipot/GraphHierarchiesModel.h"

#include <QFont>
#include <QCryptographicHash>

#include <talipot/MetaTypes.h>
#include <talipot/Project.h>
#include <talipot/Settings.h>
#include <talipot/Mimes.h>
#include <talipot/GraphNeedsSavingObserver.h>
#include <talipot/TlpQtTools.h>
#include <talipot/StableIterator.h>
#include <talipot/GlTextureManager.h>

using namespace std;
using namespace tlp;

#define NAME_SECTION 0
#define ID_SECTION 1
#define NODES_SECTION 2
#define EDGES_SECTION 3

// Serialization
static QString GRAPHS_PATH("/graphs/");
static QString TEXTURES_PATH("/textures/");
static QString FONTS_PATH("/fonts/");

static void copyTextureFileInProject(const QString &textureFilePath, tlp::Project *project,
                                     QStringList &projectTexturesFolders,
                                     QStringList &projectTexturesFiles) {
  // We use QCryptographicHash to generate a MD5 sum for each value in the viewTexture property.
  // Each texture is copied to the following project path : /textures/<md5_sum>/<texture filename> .
  // It enables to copy textures with the same filename but located in different folders.
  QCryptographicHash hasher(QCryptographicHash::Md5);
  QFileInfo fileInfo(textureFilePath);

  // If the texture file path exists, we copy the texture in the project
  if (fileInfo.exists()) {
    // Generate a MD5 sum from the absolute texture file path
    hasher.reset();
    hasher.addData(textureFilePath.toUtf8());
    // Compute texture folder and texture file path in the project
    QString textureProjectFolder = TEXTURES_PATH + hasher.result().toHex() + "/";
    QString textureProjectFile = textureProjectFolder + fileInfo.fileName();

    if (!projectTexturesFiles.contains(textureProjectFile)) {
      // The texture file may have not already been copied in the project,
      // so create its folder if needed and copy the file
      if (!project->exists(textureProjectFolder)) {
        project->mkpath(textureProjectFolder);
      }

      project->copy(fileInfo.absoluteFilePath(), textureProjectFile);
    } else {
      // The texture file has already been copied in the project,
      // recopy the file as it may have changed and remove texture folder and project path
      // from the lists of folders and files to remove afterwards
      project->copy(fileInfo.absoluteFilePath(), textureProjectFile);
      projectTexturesFiles.removeAll(textureProjectFile);
      projectTexturesFolders.removeAll(textureProjectFolder);
    }
  }
}

// Function for copying nodes and edges textures in the project to make it portable across computers
static void writeTextureFilesInProject(const QList<tlp::Graph *> &graphs, tlp::Project *project,
                                       tlp::PluginProgress *progress) {
  if (progress) {
    progress->progress(0, 0);
    progress->setComment(
        "Writing texture files into project to ensure its portability across computers ...");
  }

  QStringList projectTexturesFolders;
  QStringList projectTexturesFiles;

  // gather list of texture folders already present in the project
  for (const QString &textureFolder :
       project->entryList(TEXTURES_PATH, QDir::Dirs | QDir::NoDotAndDotDot)) {
    projectTexturesFolders.append(TEXTURES_PATH + textureFolder);
  }

  // gather list of texture file paths already present in the project
  for (const QString &textureFolder : projectTexturesFolders) {
    for (const QString &textureFile : project->entryList(textureFolder, QDir::Files)) {
      projectTexturesFiles.append(textureFolder + "/" + textureFile);
    }
  }

  for (auto *g : graphs) {
    // Process the viewTexture default node value
    StringProperty *viewTexture = g->getStringProperty("viewTexture");
    copyTextureFileInProject(tlpStringToQString(viewTexture->getNodeDefaultValue()), project,
                             projectTexturesFolders, projectTexturesFiles);

    // Process the non default valuated nodes in the viewTexture property
    for (auto n : viewTexture->getNonDefaultValuatedNodes()) {
      copyTextureFileInProject(tlpStringToQString(viewTexture->getNodeValue(n)), project,
                               projectTexturesFolders, projectTexturesFiles);
    }

    // Process the viewTexture default edge value
    copyTextureFileInProject(tlpStringToQString(viewTexture->getEdgeDefaultValue()), project,
                             projectTexturesFolders, projectTexturesFiles);

    // Process the non default valuated nodes in the viewTexture property
    for (auto e : viewTexture->getNonDefaultValuatedEdges()) {
      copyTextureFileInProject(tlpStringToQString(viewTexture->getEdgeValue(e)), project,
                               projectTexturesFolders, projectTexturesFiles);
    }
  }

  // Previously copied textures are not used anymore in the project,
  // so remove associated files and folders
  for (const QString &textureProjectFileToRemove : projectTexturesFiles) {
    project->removeFile(textureProjectFileToRemove);
  }

  for (const QString &textureProjectFolderToRemove : projectTexturesFolders) {
    project->removeDir(textureProjectFolderToRemove);
  }
}

// Function for restoring nodes and edges textures possibly bundled in the project if the original
// texture files are not present on the computer loading the project
static void restoreTextureFilesFromProject(tlp::Graph *g, tlp::Project *project,
                                           tlp::PluginProgress *progress) {
  if (progress) {
    progress->progress(0, 0);
    progress->setComment("Checking if texture files can be restored from project if needed ...");
  }

  // We use QCryptographicHash to generate a MD5 sum for each value in the viewTexture property.
  // It enables to copy textures with the same filename but located in different folders.
  // Each texture may have been copied to the following project path : /textures/<md5_sum>/<texture
  // filename> .
  QCryptographicHash hasher(QCryptographicHash::Md5);

  StringProperty *viewTexture = g->getStringProperty("viewTexture");

  // Process the nodes first

  // Get the default node texture file
  QString defaultNodeTextureFile(tlpStringToQString(viewTexture->getNodeDefaultValue()));
  QFileInfo defaultNodeTextureFileInfo(defaultNodeTextureFile);
  // Backup non default valuated node values in the viewTexture property
  // as they will be removed by the possible call to setAllNodeValue afterwards
  QMap<node, QString> nonDefaultValuatedNodes;

  for (auto n : viewTexture->getNonDefaultValuatedNodes()) {
    nonDefaultValuatedNodes[n] = tlpStringToQString(viewTexture->getNodeValue(n));
  }

  // Generate a MD5 sum from the absolute texture file path
  hasher.reset();
  hasher.addData(defaultNodeTextureFile.toUtf8());
  // Compute texture file path in the project
  QString textureProjectFile =
      TEXTURES_PATH + hasher.result().toHex() + "/" + defaultNodeTextureFileInfo.fileName();

  // If the original texture file is not present in the computer but is present in the project
  // change the value of the default node texture path in the viewTexture property
  if (!defaultNodeTextureFileInfo.exists() && project->exists(textureProjectFile)) {
    viewTexture->setAllNodeValue(QStringToTlpString(project->toAbsolutePath(textureProjectFile)));
  } else if (defaultNodeTextureFileInfo.exists()) {
    viewTexture->setAllNodeValue(QStringToTlpString(defaultNodeTextureFileInfo.absoluteFilePath()));
  } else if (defaultNodeTextureFile.startsWith("http")) {
    viewTexture->setAllNodeValue(QStringToTlpString(defaultNodeTextureFile));
  }

  // Iterate once again on non default valuated nodes
  // Get a stable iterator on non default valuated nodes as their value can be reset to the
  // default one by the possible call to setAllNodeValue
  for (auto n : stableIterator(viewTexture->getNonDefaultValuatedNodes())) {
    // Get the node texture file previously backuped
    const QString &textureFile = nonDefaultValuatedNodes[n];
    QFileInfo fileInfo(textureFile);
    // Generate a MD5 sum from the absolute texture file path
    hasher.reset();
    hasher.addData(textureFile.toUtf8());
    // Compute texture file path in the project
    QString textureProjectFile =
        TEXTURES_PATH + hasher.result().toHex() + "/" + fileInfo.fileName();

    // If the original texture file is not present in the computer but is present in the project
    // change the texture path for the node in the viewTexture property
    if (!fileInfo.exists() && project->exists(textureProjectFile)) {
      viewTexture->setNodeValue(n, QStringToTlpString(project->toAbsolutePath(textureProjectFile)));
    } else if (fileInfo.exists()) {
      viewTexture->setNodeValue(n, QStringToTlpString(fileInfo.absoluteFilePath()));
    } else if (textureFile.startsWith("http")) {
      viewTexture->setNodeValue(n, QStringToTlpString(textureFile));
    }
  }

  // Apply the same process for edges

  // Get the default edge texture file
  QString defaultEdgeTextureFile(tlpStringToQString(viewTexture->getEdgeDefaultValue()));
  QFileInfo defaultEdgeTextureFileInfo(defaultEdgeTextureFile);
  // Backup non default valuated edge values in the viewTexture property
  // as they will be removed by the possible call to setAllEdgeValue afterwards
  QMap<edge, QString> nonDefaultValuatedEdges;

  for (auto e : viewTexture->getNonDefaultValuatedEdges()) {
    nonDefaultValuatedEdges[e] = tlpStringToQString(viewTexture->getEdgeValue(e));
  }

  // Generate a MD5 sum from the absolute texture file path
  hasher.reset();
  hasher.addData(defaultEdgeTextureFile.toUtf8());
  // Compute texture file path in the project
  textureProjectFile =
      TEXTURES_PATH + hasher.result().toHex() + "/" + defaultEdgeTextureFileInfo.fileName();

  // If the original texture file is not present in the computer but is present in the project
  // change the value of the default edge texture path in the viewTexture property
  if (!defaultEdgeTextureFileInfo.exists() && project->exists(textureProjectFile)) {
    viewTexture->setAllEdgeValue(QStringToTlpString(project->toAbsolutePath(textureProjectFile)));
  } else if (defaultEdgeTextureFileInfo.exists()) {
    viewTexture->setAllEdgeValue(QStringToTlpString(defaultEdgeTextureFileInfo.absoluteFilePath()));
  } else if (defaultEdgeTextureFile.startsWith("http")) {
    viewTexture->setAllEdgeValue(QStringToTlpString(defaultEdgeTextureFile));
  }

  // Iterate once again on non default valuated edges
  // Get a stable iterator on non default valuated edges as their value can be reset to the default
  // one by the possible call to setAllEdgeValue afterwards
  for (auto e : stableIterator(viewTexture->getNonDefaultValuatedEdges())) {
    // Get the node texture file previously backuped
    const QString &textureFile = nonDefaultValuatedEdges[e];
    QFileInfo fileInfo(textureFile);
    // Generate a MD5 sum from the absolute texture file path
    hasher.reset();
    hasher.addData(textureFile.toUtf8());
    // Compute texture file path in the project
    QString textureProjectFile =
        TEXTURES_PATH + hasher.result().toHex() + "/" + fileInfo.fileName();

    // If the original texture file is not present in the computer but is present in the project
    // change the texture path for the edge in the viewTexture property
    if (!fileInfo.exists() && project->exists(textureProjectFile)) {
      viewTexture->setEdgeValue(e, QStringToTlpString(project->toAbsolutePath(textureProjectFile)));
    } else if (fileInfo.exists()) {
      viewTexture->setEdgeValue(e, QStringToTlpString(fileInfo.absoluteFilePath()));
    } else if (textureFile.startsWith("http")) {
      viewTexture->setEdgeValue(e, QStringToTlpString(textureFile));
    }
  }
}

static void copyFontFileInProject(const string &fontName, tlp::Project *project,
                                  QStringList &projectFontFiles) {
  const Font &font = Font::fromName(fontName);
  if (font.fontName() == Font::defaultFont().fontName()) {
    return;
  }

  QFileInfo fileInfo(tlpStringToQString(font.fontFile()));
  if (!fileInfo.exists() || projectFontFiles.contains(fileInfo.fileName())) {
    return;
  }

  if (!project->exists(FONTS_PATH)) {
    project->mkpath(FONTS_PATH);
  }

  project->copy(fileInfo.absoluteFilePath(), FONTS_PATH + fileInfo.fileName());
  projectFontFiles.append(fileInfo.fileName());
}

// Function for copying nodes and edges fonts in the project to make it portable across computers
static void writeFontFilesInProject(const QList<tlp::Graph *> &graphs, tlp::Project *project,
                                    tlp::PluginProgress *progress) {
  if (progress) {
    progress->progress(0, 0);
    progress->setComment(
        "Writing font files into project to ensure its portability across computers ...");
  }

  QStringList projectFontFiles;

  // gather list of font filenames already present in the project
  for (const QString &fontFile : project->entryList(FONTS_PATH, QDir::Files)) {
    projectFontFiles.append(fontFile);
  }

  for (auto *g : graphs) {
    // Process the viewFont default node value
    StringProperty *viewFont = g->getStringProperty("viewFont");
    copyFontFileInProject(viewFont->getNodeDefaultValue(), project, projectFontFiles);

    // Process the non default valuated nodes in the viewFont property
    for (auto n : viewFont->getNonDefaultValuatedNodes()) {
      copyFontFileInProject(viewFont->getNodeValue(n), project, projectFontFiles);
    }

    // Process the viewFont default edge value
    copyFontFileInProject(viewFont->getEdgeDefaultValue(), project, projectFontFiles);

    // Process the non default valuated nodes in the viewFont property
    for (auto e : viewFont->getNonDefaultValuatedEdges()) {
      copyFontFileInProject(viewFont->getEdgeValue(e), project, projectFontFiles);
    }
  }
}

static void restoreFontsFromProject(tlp::Project *project, tlp::PluginProgress *progress) {
  if (!project->exists(FONTS_PATH)) {
    return;
  }
  if (progress) {
    progress->progress(0, 0);
    progress->setComment("Restoring fonts from project ...");
  }
  auto fonts = Font::addFontsFromDir(QStringToTlpString(project->toAbsolutePath(FONTS_PATH)));
  for (const auto &font : fonts) {
    addFontToQFontDatabase(font);
  }
}

static void removeFontsFromProject(tlp::Project *project) {
  if (!project->exists(FONTS_PATH)) {
    return;
  }
  QString fontsPath = project->toAbsolutePath(FONTS_PATH);
  for (const QString &fontFile : project->entryList(FONTS_PATH, QDir::Files)) {
    string fontFilePath = QStringToTlpString(fontsPath + "/" + fontFile);
    removeFontFromQFontDatabase(fontFilePath);
    Font::removeFont(fontFilePath);
  }
}

GraphHierarchiesModel::GraphHierarchiesModel(QObject *parent)
    : Model(parent), _currentGraph(nullptr) {}

GraphHierarchiesModel::GraphHierarchiesModel(const GraphHierarchiesModel &copy)
    : Model(copy.QObject::parent()), tlp::Observable() {
  for (auto *g : copy) {
    addGraph(g);
  }

  _currentGraph = nullptr;
}

GraphHierarchiesModel::~GraphHierarchiesModel() {
  qDeleteAll(_saveNeeded);
}

// Cache related methods
QModelIndex GraphHierarchiesModel::indexOf(const tlp::Graph *g) {
  if (g == nullptr) {
    return QModelIndex();
  }

  QModelIndex result = _indexCache[g];

  // ensure result is valid and points on an existing row
  if (!result.isValid() || (result.row() > (size() - 1))) {
    result = forceGraphIndex(const_cast<Graph *>(g));
  }

  return result;
}

QModelIndex GraphHierarchiesModel::forceGraphIndex(Graph *g) {
  if (g == nullptr) {
    return QModelIndex();
  }

  QModelIndex result;

  if (g->getRoot() == g) { // Peculiar case for root graphs
    result = createIndex(_graphs.indexOf(g), 0, g);
    _indexCache[g] = result;
  } else {
    Graph *parent = g->getSuperGraph();
    uint n = 0;

    for (n = 0; n < parent->numberOfSubGraphs(); ++n) {
      if (parent->getNthSubGraph(n) == g) {
        break;
      }
    }

    result = createIndex(n, 0, g);
    _indexCache[g] = result;
  }

  return result;
}

bool GraphHierarchiesModel::needsSaving() {
  bool saveNeeded = false;

  for (GraphNeedsSavingObserver *observer : _saveNeeded) {
    saveNeeded = saveNeeded || observer->needsSaving();
  }

  return saveNeeded;
}

QMap<QString, tlp::Graph *> GraphHierarchiesModel::readProject(tlp::Project *project,
                                                               tlp::PluginProgress *progress) {
  QMap<QString, tlp::Graph *> rootIds;

  for (const QString &entry :
       project->entryList(GRAPHS_PATH, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) {
    QString filename;
    for (const QString path : {"/graph.tlp", "/graph.tlp.zst", "/graph.tlpb", "/graph.tlpb.zst"}) {
      filename = GRAPHS_PATH + entry + path;
      if (!project->exists(filename)) {
        continue;
      } else {
        break;
      }
    }

    QString absolutePath = project->toAbsolutePath(filename);
    Graph *g = loadGraph(QStringToTlpString(absolutePath), progress);

    if (g) {
      rootIds[entry] = g;
      restoreTextureFilesFromProject(g, project, progress);
      addGraph(g);
    } else {
      // failure when loading a graph
      // so delete already loaded graphs
      for (auto it = rootIds.begin(); it != rootIds.end(); ++it) {
        delete it.value();
      }

      // and clear rootIds
      rootIds.clear();
      break;
    }
  }

  restoreFontsFromProject(project, progress);

  QDir::setCurrent(QFileInfo(project->projectFile()).absolutePath());

  return rootIds;
}
QMap<tlp::Graph *, QString> GraphHierarchiesModel::writeProject(tlp::Project *project,
                                                                tlp::PluginProgress *progress) {
  QMap<tlp::Graph *, QString> rootIds;

  project->removeAllDir(GRAPHS_PATH);
  project->mkpath(GRAPHS_PATH);

  int i = 0;

  for (auto *g : _graphs) {
    rootIds[g] = QString::number(i);
    QString folder = GRAPHS_PATH + "/" + QString::number(i++) + "/";
    project->removeAllDir(folder);
    project->mkpath(folder);

    if (!Settings::isUseTlpbFileFormat()) {
      tlp::saveGraph(g, QStringToTlpString(project->toAbsolutePath(folder + "graph.tlp.zst")),
                     progress);
    } else {
      tlp::saveGraph(g, QStringToTlpString(project->toAbsolutePath(folder + "graph.tlpb.zst")),
                     progress);
    }
  }

  writeTextureFilesInProject(_graphs, project, progress);
  writeFontFilesInProject(_graphs, project, progress);

  for (GraphNeedsSavingObserver *observer : _saveNeeded) {
    observer->saved();
  }

  return rootIds;
}

// Model related
QModelIndex GraphHierarchiesModel::index(int row, int column, const QModelIndex &parent) const {
  if (row < 0) {
    return QModelIndex();
  }

  Graph *g = nullptr;

  if (parent.isValid()) {
    g = static_cast<Graph *>(parent.internalPointer())->getNthSubGraph(row);
  } else if (row < _graphs.size()) {
    g = _graphs[row];
  }

  if (g == nullptr) {
    return QModelIndex();
  }

  return createIndex(row, column, g);
}

QModelIndex GraphHierarchiesModel::parent(const QModelIndex &child) const {
  if (!child.isValid()) {
    return QModelIndex();
  }

  auto *childGraph = static_cast<Graph *>(child.internalPointer());

  if (childGraph == nullptr || _graphs.contains(childGraph) ||
      childGraph->getSuperGraph() == childGraph) {
    return QModelIndex();
  }

  int row = 0;
  Graph *parent = childGraph->getSuperGraph();

  if (_graphs.contains(parent)) {
    row = _graphs.indexOf(parent);
  } else {
    Graph *ancestor = parent->getSuperGraph();

    for (uint i = 0; i < ancestor->numberOfSubGraphs(); i++) {
      if (ancestor->getNthSubGraph(i) == parent) {
        break;
      }

      row++;
    }
  }

  return createIndex(row, 0, parent);
}

int GraphHierarchiesModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return _graphs.size();
  }

  if (parent.column() != 0) {
    return 0;
  }

  auto *parentGraph = static_cast<Graph *>(parent.internalPointer());

  return parentGraph->numberOfSubGraphs();
}

int GraphHierarchiesModel::columnCount(const QModelIndex &) const {
  return 4;
}

bool GraphHierarchiesModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (index.column() == NAME_SECTION) {
    auto *graph = static_cast<Graph *>(index.internalPointer());
    graph->setName(QStringToTlpString(value.toString()));
    return true;
  }

  return QAbstractItemModel::setData(index, value, role);
}

QVariant GraphHierarchiesModel::data(const QModelIndex &index, int role) const {

  if (!index.isValid()) {
    return QVariant();
  }

  auto *graph = static_cast<Graph *>(index.internalPointer());

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if (index.column() == NAME_SECTION) {
      return generateName(graph);
    } else if (index.column() == ID_SECTION) {
      return graph->getId();
    } else if (index.column() == NODES_SECTION) {
      return graph->numberOfNodes();
    } else if (index.column() == EDGES_SECTION) {
      return graph->numberOfEdges();
    }
  }

  else if (role == Qt::ToolTipRole) {
    return QString("<table><tr><td>%1</td></tr><tr><td>Id = %2, Nodes = %3, Edges= "
                   "%4</tr></td></table>")
        .arg(generateName(graph))
        .arg(graph->getId())
        .arg(graph->numberOfNodes())
        .arg(graph->numberOfEdges());
  }

  else if (role == GraphRole) {
    return QVariant::fromValue<Graph *>(static_cast<Graph *>(index.internalPointer()));
  }

  else if (role == Qt::TextAlignmentRole && index.column() != NAME_SECTION) {
    return Qt::AlignCenter;

  } else if (role == Qt::FontRole) {
    QFont f;

    if (graph == _currentGraph) {
      f.setBold(true);
    }

    return f;
  }

  return QVariant();
}

QVariant GraphHierarchiesModel::headerData(int section, Qt::Orientation orientation,
                                           int role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      if (section == NAME_SECTION) {
        return "Name";
      } else if (section == ID_SECTION) {
        return "Id";
      } else if (section == NODES_SECTION) {
        return "Nodes";
      } else if (section == EDGES_SECTION) {
        return "Edges";
      }
    }

    else if (role == Qt::TextAlignmentRole && section != NAME_SECTION) {
      return Qt::AlignCenter;
    }
  }

  return Model::headerData(section, orientation, role);
}

Qt::ItemFlags GraphHierarchiesModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags result = QAbstractItemModel::flags(index);

  if (index.column() == 0) {
    result = result | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
  }

  return result;
}

QMimeData *GraphHierarchiesModel::mimeData(const QModelIndexList &indexes) const {
  QSet<Graph *> graphs;

  for (const QModelIndex &index : indexes) {
    auto *g = data(index, GraphRole).value<Graph *>();

    if (g != nullptr) {
      graphs.insert(g);
    }
  }

  auto *result = new GraphMimeType();

  // every current implementation uses a single graph, so we do not have a graphmim with multiple
  // graphs.
  for (auto *g : graphs) {
    result->setGraph(g);
  }

  return result;
}

// Graphs collection
QString GraphHierarchiesModel::generateName(tlp::Graph *graph) const {
  std::string name = graph->getName();

  if (name.empty()) {
    stringstream ss;
    ss << "graph_" << graph->getId();
    name = ss.str();
    graph->setName(name);
  }

  return tlp::tlpStringToQString(name);
}

void GraphHierarchiesModel::setCurrentGraph(tlp::Graph *graph) {

  bool inHierarchy = false;

  for (auto *g : _graphs) {
    if (g->isDescendantGraph(graph) || graph == g) {
      inHierarchy = true;
      break;
    }
  }

  if (!inHierarchy) {
    return;
  }

  Graph *oldGraph = _currentGraph;
  _currentGraph = graph;

  if (oldGraph != nullptr && oldGraph != _currentGraph) {
    QModelIndex oldRow1 = indexOf(oldGraph);
    QModelIndex oldRow2 = createIndex(oldRow1.row(), columnCount() - 1);
    emit dataChanged(oldRow1, oldRow2);
  }

  if (_currentGraph != nullptr) {
    QModelIndex newRow1 = indexOf(_currentGraph);
    QModelIndex newRow2 = createIndex(newRow1.row(), columnCount() - 1);
    emit dataChanged(newRow1, newRow2);
  }

  emit currentGraphChanged(graph);
}

Graph *GraphHierarchiesModel::currentGraph() const {
  return _currentGraph;
}

void GraphHierarchiesModel::initIndexCache(tlp::Graph *root) {
  int i = 0;
  for (auto *sg : root->subGraphs()) {
    _indexCache[sg] = createIndex(i++, 0, sg);
    initIndexCache(sg);
  }
}

static void addListenerToWholeGraphHierarchy(Graph *root, Observable *listener) {
  for (auto *sg : root->subGraphs()) {
    addListenerToWholeGraphHierarchy(sg, listener);
  }
  root->addListener(listener);
  root->addObserver(listener);
  root->getStringProperty("viewTexture")->addListener(listener);
}

void GraphHierarchiesModel::addGraph(tlp::Graph *graph) {
  if (_graphs.contains(graph) || graph == nullptr) {
    return;
  }

  for (auto *g : _graphs) {
    if (g->isDescendantGraph(graph)) {
      return;
    }
  }

  beginInsertRows(QModelIndex(), rowCount(), rowCount());

  _saveNeeded[graph] = new GraphNeedsSavingObserver(graph, getMainWindow());

  _graphs.push_back(graph);

  if (_graphs.size() == 1) {
    setCurrentGraph(graph);
  }

  endInsertRows();
  initIndexCache(graph);

  // listen events on the whole hierarchy
  // in order to keep track of subgraphs names, number of nodes and edges
  // must be done after the row is inserted
  // to prevent the use of invalid QModelIndex
  addListenerToWholeGraphHierarchy(graph, this);
}

void GraphHierarchiesModel::removeGraph(tlp::Graph *g) {
  if (_graphs.contains(g)) {
    int pos = _graphs.indexOf(g);
    beginRemoveRows(QModelIndex(), pos, pos);
    _graphs.removeAll(g);
    GraphNeedsSavingObserver *s = _saveNeeded.take(g);
    delete s;
    endRemoveRows();

    if (_currentGraph == g) {
      if (_graphs.empty()) {
        _currentGraph = nullptr;
        emit currentGraphChanged(_currentGraph);
      } else {
        setCurrentGraph(_graphs[0]);
      }
    }
  }
}

// Observation
void GraphHierarchiesModel::treatEvent(const Event &e) {
  const auto *pe = dynamic_cast<const PropertyEvent *>(&e);
  if (pe) {
    // ensure to load textures once they are set and not during graph rendering
    // to avoid recursive repaint when a texture must be downloaded from a HTTP
    // URL (due to QEventLoop use in FileDownloader class implementation)
    string texture;
    auto *viewTexture = static_cast<StringProperty *>(pe->getProperty());
    if (pe->getType() == PropertyEventType::TLP_AFTER_SET_NODE_VALUE) {
      texture = viewTexture->getNodeValue(pe->getNode());
    } else if (pe->getType() == PropertyEventType::TLP_AFTER_SET_EDGE_VALUE) {
      texture = viewTexture->getEdgeValue(pe->getEdge());
    } else if (pe->getType() == PropertyEventType::TLP_AFTER_SET_ALL_NODE_VALUE) {
      texture = viewTexture->getNodeDefaultValue();
    } else if (pe->getType() == PropertyEventType::TLP_AFTER_SET_ALL_EDGE_VALUE) {
      texture = viewTexture->getEdgeDefaultValue();
    }
    if (!texture.empty()) {
      GlTextureManager::loadTexture(texture);
    }
    return;
  }

  auto *g = static_cast<tlp::Graph *>(e.sender());

  if (e.type() == EventType::TLP_DELETE && _graphs.contains(g)) { // A root graph has been deleted
    int pos = _graphs.indexOf(g);
    beginRemoveRows(QModelIndex(), pos, pos);

    _graphs.removeAll(g);
    GraphNeedsSavingObserver *s = _saveNeeded.take(g);
    delete s;

    if (_currentGraph == g) {
      if (_graphs.empty()) {
        _currentGraph = nullptr;
      } else {
        _currentGraph = _graphs[0];
      }

      emit currentGraphChanged(_currentGraph);
    }

    endRemoveRows();
  } else if (e.type() == EventType::TLP_MODIFICATION) {
    const auto *ge = dynamic_cast<const tlp::GraphEvent *>(&e);

    if (!ge) {
      return;
    }

    if (_graphs.contains(ge->getGraph()->getRoot())) {

      if (ge->getType() == GraphEventType::TLP_AFTER_ADD_DESCENDANTGRAPH) {
        // that event must only be treated on a root graph
        if (ge->getGraph() != ge->getGraph()->getRoot()) {
          return;
        }

        const Graph *sg = ge->getSubGraph();

        Graph *parentGraph = sg->getSuperGraph();

#ifndef NDEBUG
        QModelIndex parentIndex = indexOf(parentGraph);

        assert(parentIndex.isValid());
#endif

        // update index cache for subgraphs of parent graph and added subgraphs
        int i = 0;
        for (auto *sg2 : parentGraph->subGraphs()) {
          _indexCache[sg2] = createIndex(i++, 0, sg2);
        }

        i = 0;
        for (auto *sg2 : sg->subGraphs()) {
          _indexCache[sg2] = createIndex(i++, 0, sg2);
        }

        sg->addListener(this);
        sg->addObserver(this);

        // insert the parent graph in the graphs changed set
        // in order to update associated tree views, displaying the graphs hierarchies,
        // when the treatEvents method is called
        _graphsChanged.insert(parentGraph);

      } else if (ge->getType() == GraphEventType::TLP_AFTER_DEL_DESCENDANTGRAPH) {
        // that event must only be treated on a root graph
        if (ge->getGraph() != ge->getGraph()->getRoot()) {
          return;
        }

        const Graph *sg = ge->getSubGraph();

        Graph *parentGraph = sg->getSuperGraph();

        QModelIndex index = indexOf(sg);

        assert(index.isValid());

#ifndef NDEBUG
        QModelIndex parentIndex = indexOf(parentGraph);

        assert(parentIndex.isValid());
#endif

        // update index cache for subgraphs of parent graph
        int i = 0;
        for (auto *sg2 : parentGraph->subGraphs()) {
          if (sg2 != sg) {
            _indexCache[sg2] = createIndex(i++, 0, sg2);
          }
        }

        // prevent dangling pointer to remain in the persistent indexes
        _indexCache.remove(sg);
        changePersistentIndex(index, QModelIndex());

        sg->removeListener(this);
        sg->removeObserver(this);

        // insert the parent graph in the graphs changed set
        // in order to update associated tree views, displaying the graphs hierarchies,
        // when the treatEvents method is called
        _graphsChanged.insert(parentGraph);

        // remove the subgraph from the graphs changed set
        // as no update will be required for it in the associated tree views
        _graphsChanged.remove(sg);

        if (currentGraph() == sg) {
          setCurrentGraph(parentGraph);
          setCurrentGraph(parentGraph);
        }

      } else if (ge->getType() == GraphEventType::TLP_ADD_NODE ||
                 ge->getType() == GraphEventType::TLP_ADD_NODES ||
                 ge->getType() == GraphEventType::TLP_DEL_NODE ||
                 ge->getType() == GraphEventType::TLP_ADD_EDGE ||
                 ge->getType() == GraphEventType::TLP_ADD_EDGES ||
                 ge->getType() == GraphEventType::TLP_DEL_EDGE ||
                 (ge->getType() == GraphEventType::TLP_AFTER_SET_ATTRIBUTE &&
                  ge->getAttributeName() == "name")) {
        const Graph *graph = ge->getGraph();
        // row representing the graph in the associated tree views has to be updated
        _graphsChanged.insert(graph);
      }
    }
  }
}

void GraphHierarchiesModel::treatEvents(const std::vector<tlp::Event> &) {

  if (_graphsChanged.isEmpty()) {
    return;
  }

  // update the rows associated to modified graphs (number of subgraphs/nodes/edges has changed)
  // in the associated tree views

  emit layoutAboutToBeChanged();

  for (const auto *graph : _graphsChanged) {
    QModelIndex graphIndex = indexOf(graph);
    QModelIndex graphEdgesIndex = graphIndex.sibling(graphIndex.row(), EDGES_SECTION);
    emit dataChanged(graphIndex, graphEdgesIndex);
  }

  emit layoutChanged();

  _graphsChanged.clear();
}

void GraphHierarchiesModel::clear(tlp::Project *project) {
  _indexCache.clear();
  _saveNeeded.clear();
  _currentGraph = nullptr;
  removeFontsFromProject(project);
}
