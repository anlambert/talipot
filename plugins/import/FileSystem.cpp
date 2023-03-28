/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/PluginHeaders.h>
#include <talipot/TlpQtTools.h>
#include <talipot/ViewSettings.h>
#include <talipot/FontAwesome.h>

#include <QStack>
#include <QDir>
#include <QDateTime>

using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // directory
    "The directory to scan recursively.",

    // icons
    "If true, set icons as node shapes according to file mime types.",

    // tree layout
    "If true, apply the 'Bubble Tree' layout algorithm on the imported graph.",

    // directory color
    "This parameter indicates the color used to display directories.",

    // other color
    "This parameter indicates the color used to display other files.",

    // hidden files
    "If true, also include hidden files.",

    // symlinks
    "If true, follow symlinks on Unix (including Mac OS X) or .lnk file on Windows."};

static const char *commonTextFilesExtArray[] = {"log", "msg", "odt", "pages", "rtf", "json",
                                                "tex", "txt", "wpd", "wps",   "srt", "nfo"};
static const std::vector<std::string> commonTextFilesExt(
    commonTextFilesExtArray,
    commonTextFilesExtArray + sizeof(commonTextFilesExtArray) / sizeof(commonTextFilesExtArray[0]));

static const char *commonAudioFilesExtArray[] = {"aif", "iff", "m3u", "m4a", "mid", "mp3",
                                                 "mpa", "ogg", "ra",  "wav", "wma", "flac"};
static const std::vector<std::string>
    commonAudioFilesExt(commonAudioFilesExtArray,
                        commonAudioFilesExtArray +
                            sizeof(commonAudioFilesExtArray) / sizeof(commonAudioFilesExtArray[0]));

static const char *commonVideoFilesExtArray[] = {"3g2", "3gp", "asf", "asx", "avi",
                                                 "flv", "m4v", "mkv", "mov", "mp4",
                                                 "mpg", "rm",  "swf", "vob", "wmv"};
static const std::vector<std::string>
    commonVideoFilesExt(commonVideoFilesExtArray,
                        commonVideoFilesExtArray +
                            sizeof(commonVideoFilesExtArray) / sizeof(commonVideoFilesExtArray[0]));

static const char *commonImageFilesExtArray[] = {"bmp", "dds",      "gif", "jpg", "jpeg", "png",
                                                 "psd", "pspimage", "tga", "thm", "tif",  "tiff",
                                                 "yuv", "ai",       "eps", "ps",  "svg"};
static const std::vector<std::string>
    commonImageFilesExt(commonImageFilesExtArray,
                        commonImageFilesExtArray +
                            sizeof(commonImageFilesExtArray) / sizeof(commonImageFilesExtArray[0]));

static const char *commonArchiveFilesExtArray[] = {
    "7z", "cbr", "deb", "gz", "pkg", "rar", "rpm", "sitx", "tar", "zip", "zipx", "bz2", "lzma"};
static const std::vector<std::string>
    commonArchiveFilesExt(commonArchiveFilesExtArray,
                          commonArchiveFilesExtArray + sizeof(commonArchiveFilesExtArray) /
                                                           sizeof(commonArchiveFilesExtArray[0]));

static const char *commonDevFilesExtArray[] = {
    "c",       "cc",        "class", "cpp", "cs",   "dtd", "fla", "h",   "hh",
    "hpp",     "java",      "lua",   "m",   "pl",   "py",  "sh",  "sln", "swift",
    "vcxproj", "xcodeproj", "css",   "js",  "html", "xml", "htm", "php", "xhtml"};

static const std::vector<std::string> commonDevFilesExt(commonDevFilesExtArray,
                                                        commonDevFilesExtArray +
                                                            sizeof(commonDevFilesExtArray) /
                                                                sizeof(commonDevFilesExtArray[0]));

static tlp::DataSet getDefaultAlgorithmParameters(const std::string &algoName, tlp::Graph *graph) {
  tlp::DataSet result;
  const tlp::ParameterDescriptionList &parameters =
      tlp::PluginsManager::getPluginParameters(algoName);
  parameters.buildDefaultDataSet(result, graph);
  return result;
}

/// Imports a tree representation of a file system directory.
/** This plugin enables to capture in a tree the full hierarchy of
 *  of a given file system directory
 */
class FileSystem : public tlp::ImportModule {
public:
  PLUGININFORMATION("File System Directory", "Auber", "16/12/2002",
                    "Imports a tree representation of a file system directory.", "2.2", "Misc")
  FileSystem(tlp::PluginContext *context)
      : ImportModule(context), _absolutePaths(nullptr), _baseNames(nullptr), _createdDates(nullptr),
        _fileNames(nullptr), _isDir(nullptr), _isExecutable(nullptr), _isReadable(nullptr),
        _isSymlink(nullptr), _isWritable(nullptr), _lastModifiedDates(nullptr),
        _lastReadDates(nullptr), _owners(nullptr), _permissions(nullptr), _suffixes(nullptr),
        _sizes(nullptr), _fontIcon(nullptr), _useIcons(true), _treeLayout(true),
        dirColor(255, 255, 127, 128) {
    addInParameter<std::string>("dir::directory", paramHelp[0].data(), "");
    addInParameter<bool>("include hidden files", paramHelp[5].data(), "true");
    addInParameter<bool>("follow symlinks", paramHelp[6].data(), "true");
    addInParameter<bool>("icons", paramHelp[1].data(), "true");
    addInParameter<bool>("tree layout", paramHelp[2].data(), "true");
    addInParameter<tlp::Color>("directory color", paramHelp[3].data(), "(255, 255, 127, 255)");
    addInParameter<tlp::Color>("other color", paramHelp[4].data(), "(85, 170, 255, 255)");
  }

  bool importGraph() override {
    if (dataSet == nullptr) {
      return false;
    }

    std::string rootPathStr;
    dataSet->get("dir::directory", rootPathStr);
    QFileInfo rootInfo(tlpStringToQString(rootPathStr));

    dataSet->get("icons", _useIcons);
    dataSet->get("tree layout", _treeLayout);
    dataSet->get("directory color", dirColor);
    tlp::Color otherColor = {85, 170, 255, 128};
    dataSet->get("other color", otherColor);
    bool hiddenFiles(true);
    dataSet->get("include hidden files", hiddenFiles);
    bool symlinks(true);
    dataSet->get("follow symlinks", symlinks);

    if (!rootInfo.exists()) {
      if (pluginProgress) {
        pluginProgress->setError("Provided directory " +
                                 tlp::QStringToTlpString(rootInfo.absoluteFilePath()) +
                                 " does not exist.");
      }
      return false;
    }

    _absolutePaths = graph->getStringProperty("Absolute paths");
    _baseNames = graph->getStringProperty("Base name");
    _createdDates = graph->getStringProperty("Creation date");
    _fileNames = graph->getStringProperty("File name");
    _isDir = graph->getBooleanProperty("Is directory");
    _isExecutable = graph->getBooleanProperty("Is executable");
    _isReadable = graph->getBooleanProperty("Is readable");
    _isSymlink = graph->getBooleanProperty("Is symbolic link");
    _isWritable = graph->getBooleanProperty("Is writable");
    _lastModifiedDates = graph->getStringProperty("Last modification date");
    _lastReadDates = graph->getStringProperty("Last read date");
    _owners = graph->getStringProperty("Owner");
    _permissions = graph->getIntegerProperty("Permission ID");
    _suffixes = graph->getStringProperty("Suffix");
    _sizes = graph->getDoubleProperty("Size");
    _fontIcon = graph->getStringProperty("viewIcon");

    if (_useIcons) {
      tlp::IntegerProperty *viewShape = graph->getIntegerProperty("viewShape");
      viewShape->setAllNodeValue(tlp::NodeShape::Icon);
      _fontIcon->setAllNodeValue(tlp::FontAwesome::Regular::File);
    }

    tlp::ColorProperty *viewColor = graph->getColorProperty("viewColor");
    viewColor->setAllNodeValue(otherColor);

    tlp::node rootNode = addFileNode(rootInfo, graph);

    if (!rootInfo.isDir()) {
      return true;
    }

    QStack<QPair<QString, tlp::node>> fsStack;
    fsStack.push(QPair<QString, tlp::node>(rootInfo.absoluteFilePath(), rootNode));

    while (!fsStack.empty()) {
      QPair<QString, tlp::node> elem = fsStack.pop();
      QDir currentDir(QDir(elem.first));

      tlp::node parentNode(elem.second);
      QFlags<QDir::Filter> filter =
          QDir::NoDotAndDotDot | QDir::System | QDir::AllDirs | QDir::Files;

      if (hiddenFiles) {
        filter |= QDir::Hidden;
      }

      QFileInfoList entries(currentDir.entryInfoList(filter, QDir::DirsFirst));

      int i = 0;

      pluginProgress->setComment("Reading contents of " +
                                 tlp::QStringToTlpString(currentDir.absolutePath()));
      pluginProgress->progress(i, entries.count());

      for (const auto &fileInfo : entries) {

        // consider files starting with a dot as hidden (for windows platform)
        if (!hiddenFiles && fileInfo.fileName().startsWith(".")) {
          continue;
        }

        tlp::node fileNode = addFileNode(fileInfo, graph);
        graph->addEdge(parentNode, fileNode);

        if (fileInfo.isDir() && (!fileInfo.isSymLink() || symlinks)) {
          fsStack.push_back(QPair<QString, tlp::node>(fileInfo.absoluteFilePath(), fileNode));
        }

        if ((++i % 100) == 0) {
          pluginProgress->progress(i, entries.count());
        }

        if (pluginProgress->state() == ProgressState::TLP_CANCEL) {
          pluginProgress->setError("Import cancelled by user.");
          return false;
        }

        if (pluginProgress->state() == ProgressState::TLP_STOP) {
          break;
        }
      }
    }

    tlp::StringProperty *viewLabel = graph->getStringProperty("viewLabel");
    viewLabel->copy(_fileNames);

    if (_treeLayout) {
      const std::string algoName = "Bubble Tree";
      tlp::DataSet defaultParameters = getDefaultAlgorithmParameters(algoName, graph);
      std::string errMsg;
      tlp::LayoutProperty *viewLayout = graph->getLayoutProperty("viewLayout");
      graph->applyPropertyAlgorithm(algoName, viewLayout, errMsg, &defaultParameters,
                                    pluginProgress);
    }

    return true;
  }

private:
  tlp::node addFileNode(const QFileInfo &info, tlp::Graph *g) {
    tlp::node n = g->addNode();
    _absolutePaths->setNodeValue(n, tlp::QStringToTlpString(info.absoluteFilePath()));
    _baseNames->setNodeValue(n, tlp::QStringToTlpString(info.baseName()));
    _createdDates->setNodeValue(n, tlp::QStringToTlpString(info.birthTime().toString()));
    _fileNames->setNodeValue(n, tlp::QStringToTlpString(info.fileName()));
    _isDir->setNodeValue(n, info.isDir());
    _isExecutable->setNodeValue(n, info.isExecutable());
    _isReadable->setNodeValue(n, info.isReadable());
    _isSymlink->setNodeValue(n, info.isSymLink());
    _isWritable->setNodeValue(n, info.isWritable());
    _lastModifiedDates->setNodeValue(n, tlp::QStringToTlpString(info.lastModified().toString()));
    _lastReadDates->setNodeValue(n, tlp::QStringToTlpString(info.lastRead().toString()));
    _owners->setNodeValue(n, tlp::QStringToTlpString(info.owner()));
    _permissions->setNodeValue(n, int(info.permissions()));
    _suffixes->setNodeValue(n, tlp::QStringToTlpString(info.suffix()));
    _sizes->setNodeValue(n, info.size());

    if (_useIcons) {
      std::string extension = QStringToTlpString(info.suffix());

      if (info.isDir()) {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::Folder);
        tlp::ColorProperty *viewColor = graph->getColorProperty("viewColor");
        viewColor->setNodeValue(n, dirColor);
      } else if (std::find(commonTextFilesExt.begin(), commonTextFilesExt.end(), extension) !=
                 commonTextFilesExt.end()) {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileAlt);
      } else if (std::find(commonArchiveFilesExt.begin(), commonArchiveFilesExt.end(), extension) !=
                 commonArchiveFilesExt.end()) {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileArchive);
      } else if (std::find(commonAudioFilesExt.begin(), commonAudioFilesExt.end(), extension) !=
                 commonAudioFilesExt.end()) {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileAudio);
      } else if (std::find(commonImageFilesExt.begin(), commonImageFilesExt.end(), extension) !=
                 commonImageFilesExt.end()) {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileImage);
      } else if (std::find(commonVideoFilesExt.begin(), commonVideoFilesExt.end(), extension) !=
                 commonVideoFilesExt.end()) {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileVideo);
      } else if (std::find(commonDevFilesExt.begin(), commonDevFilesExt.end(), extension) !=
                 commonDevFilesExt.end()) {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileCode);
      } else if (extension == "pdf") {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FilePdf);
      } else if (extension == "doc" || extension == "docx") {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileWord);
      } else if (extension == "xls" || extension == "xlsx") {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FileExcel);
      } else if (extension == "ppt" || extension == "pptx") {
        _fontIcon->setNodeValue(n, tlp::FontAwesome::Regular::FilePowerpoint);
      }
    }

    return n;
  }

  tlp::StringProperty *_absolutePaths;
  tlp::StringProperty *_baseNames;
  tlp::StringProperty *_createdDates;
  tlp::StringProperty *_fileNames;
  tlp::BooleanProperty *_isDir;
  tlp::BooleanProperty *_isExecutable;
  tlp::BooleanProperty *_isReadable;
  tlp::BooleanProperty *_isSymlink;
  tlp::BooleanProperty *_isWritable;
  tlp::StringProperty *_lastModifiedDates;
  tlp::StringProperty *_lastReadDates;
  tlp::StringProperty *_owners;
  tlp::IntegerProperty *_permissions;
  tlp::StringProperty *_suffixes;
  tlp::DoubleProperty *_sizes;
  tlp::StringProperty *_fontIcon;
  bool _useIcons;
  bool _treeLayout;
  tlp::Color dirColor;
};

PLUGIN(FileSystem)
