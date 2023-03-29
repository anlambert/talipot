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

#include "talipot/QuaZIPFacade.h"

#include <QDir>

#include <talipot/SimplePluginProgress.h>
#include <talipot/TlpQtTools.h>

#include <quazipfile.h>

void copy(QIODevice &in, QIODevice &out) {
  const size_t size = 40960;
  char buffer[size];
  int cnt;

  while ((cnt = in.read(buffer, size))) {
    out.write(buffer, cnt);
  }

  in.close();
  out.close();
}

bool zipDirContent(QDir &currentDir, QuaZip &archive, const QString &archivePath,
                   tlp::PluginProgress *progress) {
  QFileInfoList entries = currentDir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                                       QDir::Hidden | QDir::AllDirs | QDir::Files,
                                                   QDir::DirsFirst);
  progress->setComment(
      tlp::QStringToTlpString("Compressing directory " + currentDir.absolutePath()));
  int i = 0;
  progress->progress(i, entries.size());

  for (const QFileInfo &info : entries) {
    progress->progress(i++, entries.size());

    if (info.isDir()) { // Recurse in directories if they are different from . and ..
      QDir childDir(info.absoluteFilePath());
      QFileInfo childInfo(childDir.absolutePath());
      zipDirContent(childDir, archive, archivePath + childInfo.fileName() + "/", progress);
    }

    else {
      QuaZipFile outFile(&archive);
      QuaZipNewInfo newFileInfo(archivePath + info.fileName(), info.absoluteFilePath());
      newFileInfo.externalAttr = 0x81fd0000;
      QFile inFile(info.absoluteFilePath());

      if (!outFile.open(QIODevice::WriteOnly, newFileInfo) || !inFile.open(QIODevice::ReadOnly)) {
        return false;
      }

      copy(inFile, outFile);

      if (outFile.getZipError() != UNZ_OK) {
        return false;
      }
    }
  }

  return true;
}

bool QuaZIPFacade::zipDir(const QString &rootPath, const QString &archivePath,
                          tlp::PluginProgress *progress) {
  QFileInfo rootInfo(rootPath);

  if (!rootInfo.exists() || !rootInfo.isDir()) {
    return false;
  }

  QDir rootDir(rootPath);

  QuaZip archive(archivePath);

  if (!archive.open(QuaZip::mdCreate)) {
    return false;
  }

  bool deleteProgress = false;

  if (!progress) {
    progress = new tlp::SimplePluginProgress;
    deleteProgress = true;
  }

  bool result = zipDirContent(rootDir, archive, "", progress);
  archive.close();

  if (deleteProgress) {
    delete progress;
  }

  return result;
}

bool QuaZIPFacade::unzip(const QString &rootPath, const QString &archivePath,
                         tlp::PluginProgress *progress) {

  QFileInfo rootPathInfo(rootPath);

  if (rootPathInfo.exists() && !rootPathInfo.isDir()) {
    progress->setError("Root path does not exists or is not a dir");
    return false;
  }

  QDir rootDir(rootPath);

  if (!rootDir.exists() && !rootDir.mkpath(rootPath)) {
    progress->setError("Could not create root path");
    return false;
  }

  QFile archiveFile(archivePath);

  if (!archiveFile.exists()) {
    progress->setError(tlp::QStringToTlpString("No such file: " + archivePath));
    return false;
  }

  QuaZip archive(archivePath);

  if (!archive.open(QuaZip::mdUnzip)) {
    progress->setError("Could not open archive");
    return false;
  }

  bool deleteProgress = false;

  if (!progress) {
    progress = new tlp::SimplePluginProgress;
    deleteProgress = true;
  }

  progress->setComment(tlp::QStringToTlpString("Uncompressing archive " + archivePath));
  int i = 0, n = archive.getEntriesCount();
  progress->progress(i, n);

  for (bool readMore = archive.goToFirstFile(); readMore; readMore = archive.goToNextFile()) {
    progress->progress(i++, n);

    QuaZipFile inFile(&archive);
    QuaZipFileInfo inInfo;
    inFile.getFileInfo(&inInfo);

    QFileInfo outInfo(rootDir.absoluteFilePath(inInfo.name));
    rootDir.mkpath(outInfo.absolutePath());

    QFile outFile(outInfo.absoluteFilePath());

    if (!outFile.open(QIODevice::WriteOnly) || !inFile.open(QIODevice::ReadOnly)) {
      progress->setError("Could not write in folder or could not read from file");
      return false;
    }

    copy(inFile, outFile);
  }

  if (deleteProgress) {
    delete progress;
  }

  return true;
}
