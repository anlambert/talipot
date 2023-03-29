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

#ifndef TALIPOT_QUAZIP_FACADE_H
#define TALIPOT_QUAZIP_FACADE_H

#include <talipot/config.h>

namespace tlp {
class PluginProgress;
}

class QString;

/**
  @brief This class is a facade to the QuaZIP libraries features.
  It provides convenience methods allowing to manipulate the QuaZIP library on a higher level.
  QuaZIPFacade contains only static methods, as it should only be used as a toolbox.
  */
class TLP_QT_SCOPE QuaZIPFacade {

public:
  /**
    @brief Compress (ZIPs) a directory into an archive.
    This method compress the content of a directory (without taking the top directory) into an
    archive.
    @param rootPath The absolute path of the directory to compress.
    @param archivePath The absolute file path of the ZIP file to create.
    @param progress A progress handler
    @warning If an archive already exists with that name, it will be overwritten.
    @return true if compression process went successfully
    */
  static bool zipDir(const QString &rootPath, const QString &archivePath,
                     tlp::PluginProgress *progress = nullptr);

  /**
    @brief Uncompress (un-ZIPs) an archive into a directory.
    Files contained in the archive will be stored inside the specified directory. Making it the root
    path of the uncompressed files.
    @param rootPath The absolute path of the directory where files should be written.
    @param archivePath The absolute file path of the ZIP file to uncompress.
    @param progress A progress handler
    @warning If some files already exist in the target directory, they will be overwritten.
    @return true if uncompression process went successfully
    */
  static bool unzip(const QString &rootPath, const QString &archivePath,
                    tlp::PluginProgress *progress = nullptr);
};

#endif // TALIPOT_QUAZIP_FACADE_H
