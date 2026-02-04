/**
 *
 * Copyright (C) 2019-2026  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/Project.h"

#include <QMetaProperty>
#include <QXmlStreamWriter>

#include <talipot/SimplePluginProgress.h>
#include <talipot/QuaZIPFacade.h>
#include <talipot/TlpQtTools.h>

#define DATA_DIR_NAME "data"
#define INFO_FILE_NAME "project.xml"
#define TLPPROJ_VERSION "1.0"

namespace tlp {

Project::Project(QTemporaryDir *path) : _rootDir(path) {}

Project::~Project() {
  delete _rootDir;
}

Project *Project::newProject() {
  auto *tempdir = new QTemporaryDir();
  bool dirOk = tempdir->isValid() && QDir(tempdir->path()).mkdir(DATA_DIR_NAME);

  if (!dirOk) {
    std::string err =
        "Failed to create a temporary path " + tlp::QStringToTlpString(tempdir->path());
    err += ": " + tlp::QStringToTlpString(tempdir->errorString());

    tlp::error() << err << std::endl;
    delete tempdir;
    return nullptr;
  }

  return new Project(tempdir);
}

bool Project::openProjectFile(const QString &file, tlp::PluginProgress *progress) {

  QFileInfo fileInfo(file);

  if (!fileInfo.exists()) {
    progress->setError("File " + tlp::QStringToTlpString(file) + " not found");
    return false;
  }

  if (fileInfo.isDir()) {
    progress->setError(tlp::QStringToTlpString(file) + " is a directory, not a regular file");
    return false;
  }

  bool deleteProgress = false;

  if (!progress) {
    progress = new tlp::SimplePluginProgress;
    deleteProgress = true;
  }

  if (!QuaZIPFacade::unzip(rootDir(), file, progress)) {
    progress->setError("Failed to unzip project.");

    if (deleteProgress) {
      delete progress;
    }

    return false;
  }

  readMetaInfo();

  if (deleteProgress) {
    delete progress;
  }

  _projectFile = file;
  emit projectFileChanged(file);
  return true;
}

Project *Project::openProject(const QString &file, tlp::PluginProgress *progress) {
  Project *project = Project::newProject();

  if (project != nullptr) {
    if (!project->openProjectFile(file, progress)) {
      return nullptr;
    }
  }
  return project;
}

bool Project::write(const QString &file, tlp::PluginProgress *progress) {
  bool deleteProgress = false;

  if (!progress) {
    progress = new tlp::SimplePluginProgress;
    deleteProgress = true;
  }

  if (!writeMetaInfo()) {
    progress->setError("Failed to save meta-information.");
    return false;
  }

  if (!QuaZIPFacade::zipDir(rootDir(), file)) {
    progress->setError("Failed to zip project.");
    return false;
  }

  if (deleteProgress) {
    delete progress;
  }

  _projectFile = file;
  emit projectFileChanged(file);
  return true;
}

// Project *Project::restoreProject(const QString &path) {
//  Project *project = new Project(path);
//  project->_isValid = project->readMetaInfo();
//  return project;
//}

// ==============================
//      FILES MANIPULATION
// ==============================
QStringList Project::entryList(const QString &relativePath, const QStringList &nameFilters,
                               QDir::Filters filters, QDir::SortFlags sort) {
  QString path(toAbsolutePath(relativePath));
  QFileInfo info(path);

  if (!info.exists() || !info.isDir()) {
    return QStringList();
  }

  QDir dir(path);
  return dir.entryList(nameFilters, filters, sort);
}

QStringList Project::entryList(const QString &relativePath, QDir::Filters filters,
                               QDir::SortFlags sort) {
  QString path(toAbsolutePath(relativePath));
  QFileInfo info(path);

  if (!info.exists() || !info.isDir()) {
    return QStringList();
  }

  QDir dir(path);
  return dir.entryList(filters, sort);
}

bool Project::isDir(const QString &path) {
  QFileInfo info(toAbsolutePath(path));
  return info.isDir();
}

bool Project::mkpath(const QString &path) {
  return QDir(rootDir()).mkpath(toAbsolutePath(path));
}

bool Project::exists(const QString &path) {
  return QFileInfo(toAbsolutePath(path)).exists();
}

bool Project::touch(const QString &path) {
  QFile f(toAbsolutePath(path));
  bool result = f.open(QIODevice::WriteOnly);
  f.close();
  return result;
}

bool Project::removeFile(const QString &path) {
  return QFile::remove(toAbsolutePath(path));
}

bool Project::removeDir(const QString &path) {
  QDir dir(toAbsolutePath(path));
  return dir.rmdir(dir.absolutePath());
}

bool Project::removeAllDir(const QString &path) {
  QDir dir(toAbsolutePath(path));
  return dir.removeRecursively();
}

bool Project::copy(const QString &source, const QString &destination) {
  return QFile::copy(source, toAbsolutePath(destination));
}

std::fstream *Project::stdFileStream(const QString &path, std::ios_base::openmode mode) {
  QString filePath(toAbsolutePath(path));
  auto *result = new std::fstream();
  result->open(QStringToTlpString(filePath).c_str(), mode);

  if (!result->is_open()) {
    delete result;
    result = nullptr;
  }

  return result;
}

QIODevice *Project::fileStream(const QString &path, QIODevice::OpenMode mode) {
  auto *result = new QFile(toAbsolutePath(path));
  if (result->open(mode)) {
    return result;
  } else {
    delete result;
    return nullptr;
  }
}

QString Project::absoluteRootPath() const {
  return QDir(rootDir()).absolutePath();
}

// ==============================
//      META-INFORMATION
// ==============================
QString Project::name() const {
  return _name;
}

void Project::setName(const QString &n) {
  _name = n;
}

QString Project::description() const {
  return _description;
}

void Project::setDescription(const QString &d) {
  _description = d;
}

QString Project::author() const {
  return _author;
}

void Project::setAuthor(const QString &a) {
  _author = a;
}

QString Project::version() const {
  return TLPPROJ_VERSION;
}

bool Project::writeMetaInfo() {
  QFile out(QDir(rootDir()).absoluteFilePath(INFO_FILE_NAME));

  if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }

  QXmlStreamWriter doc(&out);
  doc.setAutoFormatting(true);
  doc.writeStartElement("talipotproject");
  doc.writeAttribute("version", TLPPROJ_VERSION);

  const QMetaObject *mo = metaObject();

  for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
    QMetaProperty prop(mo->property(i));

    if (QString(prop.name()) == "objectName") {
      continue;
    }

    doc.writeTextElement(prop.name(), property(prop.name()).toString());
  }

  doc.writeEndDocument();
  out.close();
  return !doc.hasError();
}

bool Project::readMetaInfo() {
  QFile in(QDir(rootDir()).absoluteFilePath(INFO_FILE_NAME));

  if (!in.open(QIODevice::ReadOnly)) {
    return false;
  }

  QXmlStreamReader doc(&in);

  if (doc.hasError()) {
    in.close();
    tlp::debug() << "Error opening xml meta information file: "
                 << QStringToTlpString(doc.errorString()) << std::endl;
    return false;
  }

  while (!doc.atEnd()) {
    if (doc.readNextStartElement()) {
      if (doc.hasError()) {
        tlp::debug() << "Error reading xml meta information: "
                     << QStringToTlpString(doc.errorString()) << std::endl;
        in.close();
        return false;
      }

      std::string name = QStringToTlpString(doc.name().toString());

      if (property(name.c_str()).isValid()) {
        setProperty(name.c_str(), doc.readElementText());
      }
    }
  }

  in.close();
  return true;
}

QString Project::toAbsolutePath(const QString &relativePath) {
  QString path(relativePath);

  if (relativePath.startsWith("/")) {
    path = path.remove(0, 1);
  }

  return QDir(rootDir() + "/" + QString(DATA_DIR_NAME)).absoluteFilePath(path);
}

bool Project::clearProject() {
  QFileInfo pathInfo(QDir(rootDir()).absolutePath());

  if (!pathInfo.isDir() || !pathInfo.exists()) {
    return false;
  }

  QDir dir(pathInfo.absoluteFilePath());
  QFileInfoList entries(dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
                                              QDir::AllDirs | QDir::Files,
                                          QDir::DirsFirst));

  for (const QFileInfo &info : entries) {
    bool result = true;
    if (info.isDir()) {
      QDir dird(info.absoluteFilePath());
      result = dird.removeRecursively();
    } else {
      result = dir.remove(info.absoluteFilePath());
    }

    if (!result) {
      return false;
    }
  }
  return true;
}

void Project::setProjectFile(const QString &projectFile) {
  _projectFile = projectFile;
  emit projectFileChanged(projectFile);
}
}
