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

#ifndef TALIPOT_IMPORT_MODULE_H
#define TALIPOT_IMPORT_MODULE_H

#include <talipot/Plugin.h>
#include <talipot/MaterialDesignIcons.h>

#include <list>
#include <string>

namespace tlp {

static const std::string IMPORT_CATEGORY = "Import";

class PluginProgress;
class Graph;
class DataSet;

/**
 * @addtogroup Plugins
 * @brief Base class for import plug-ins.
 **/
class TLP_SCOPE ImportModule : public tlp::Plugin {
public:
  ImportModule(const tlp::PluginContext *context) {
    if (context != nullptr) {
      const auto *algoritmContext = static_cast<const tlp::AlgorithmContext *>(context);
      graph = algoritmContext->graph;
      pluginProgress = algoritmContext->pluginProgress;
      dataSet = algoritmContext->dataSet;
    }
  }

  /**
   * @brief Gets the extensions of the file formats the plugin can import.
   * e.g. a TLP import would return 'tlp'.
   *
   * @return the list of file extensions the plugin can import.
   **/
  virtual std::list<std::string> fileExtensions() const {
    return {};
  }

  /**
   * @brief Gets the extensions of the gzipped file formats this plugin can import.
   * e.g. a TLP import would return 'tlp.gz and tlpz'.
   *
   * @return the list of gzipped file extensions the plugin can import.
   **/
  virtual std::list<std::string> gzipFileExtensions() const {
    std::list<std::string> gzipExtensions;
    for (const auto &ext : fileExtensions()) {
      for (const std::string gzExt : {".gz", "z"}) {
        gzipExtensions.push_back(ext + gzExt);
      }
    }
    return gzipExtensions;
  }

  virtual std::list<std::string> zstdFileExtensions() const {
    std::list<std::string> zstdExtensions;
    for (const auto &ext : fileExtensions()) {
      for (const std::string zstExt : {".zst", "zst"}) {
        zstdExtensions.push_back(ext + zstExt);
      }
    }
    return zstdExtensions;
  }

  /**
   * @brief Gets all the extensions (normal and gzipped) of the file formats this plugin can import.
   *
   * @return the list of file extensions the plugin can import.
   **/
  std::list<std::string> allFileExtensions() const {
    std::list<std::string> zext(gzipFileExtensions());
    std::list<std::string> zstext(zstdFileExtensions());
    std::list<std::string> ext(fileExtensions());
    ext.splice(ext.end(), zext);
    ext.splice(ext.end(), zstext);
    return ext;
  }

  std::string category() const override {
    return IMPORT_CATEGORY;
  }

  std::string icon() const override {
    return MaterialDesignIcons::Import;
  }

  /**
   * @brief The import operations should take place here.
   *
   * @return bool Whether the import was successful or not.
   **/
  virtual bool importGraph() = 0;

  /**
   * @brief The Graph in which to write the data to import.
   **/
  Graph *graph;

  /**
   * @brief A means to report progress to the user.
   **/
  PluginProgress *pluginProgress;

  /**
   * @brief A container for the parameters of this import plug-in.
   **/
  DataSet *dataSet;

protected:
  struct InputData {
    InputData(std::istream *is = nullptr, size_t size = 0, const std::string &filename = "")
        : is(is), size(size), filename(filename) {}
    bool valid() const {
      return is.get() != nullptr && !is->fail();
    }
    std::unique_ptr<std::istream> is;
    size_t size;
    std::string filename;
  };

  InputData getInputData() const;
};
}
#endif // TALIPOT_IMPORT_MODULE_H
