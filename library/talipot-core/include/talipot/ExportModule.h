/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_EXPORT_MODULE_H
#define TALIPOT_EXPORT_MODULE_H

#include <iostream>
#include <talipot/Plugin.h>
#include <talipot/Algorithm.h>
#include <talipot/MaterialDesignIcons.h>

namespace tlp {

static const std::string EXPORT_CATEGORY = "Export";

class Graph;
class DataSet;
class PluginProgress;

/**
 * @ingroup Plugins
 * @brief The ExportModule class
 */
class ExportModule : public tlp::Plugin {
  public:
    ExportModule(const tlp::PluginContext *context) {
        if (context != nullptr) {
            const auto *algoritmContext = static_cast<const tlp::AlgorithmContext *>(context);
            graph = algoritmContext->graph;
            pluginProgress = algoritmContext->pluginProgress;
            dataSet = algoritmContext->dataSet;
        }
    }

    ~ExportModule() override = default;

    std::string category() const override {
        return EXPORT_CATEGORY;
    }

    std::string icon() const override {
        return MaterialDesignIcons::Export;
    }

    /**
     * @brief Gets the extension of the file format this export plugin saves to.
     * e.g. a GML export would return 'gml'.
     *
     * @return :string the extension that this export module saves to.
     **/
    virtual std::string fileExtension() const = 0;

    /**
     * @brief Gets a list of the extensions file format when compressed with gzip this export plugin
     * saves to.
     *
     * @return :string the extension that this export module saves to.
     **/
    virtual std::list<std::string> gzipFileExtensions() const {
        std::list<std::string> gzipExtensions;
        for (const std::string gzExt : {".gz", "z"}) {
            gzipExtensions.push_back(fileExtension() + gzExt);
        }
        return gzipExtensions;
    }

    virtual std::list<std::string> zstdFileExtensions() const {
        std::list<std::string> zstdExtensions;
        for (const std::string zstExt : {".zst", "zst"}) {
            zstdExtensions.push_back(fileExtension() + zstExt);
        }
        return zstdExtensions;
    }

    /**
     * @brief Gets a list of all extensions file format (normal and gzipped) this export plugin
     * saves to.
     *
     * @return the list of file extensions this export plugin saves to.
     */
    std::list<std::string> allFileExtensions() const {
        std::list<std::string> ext(gzipFileExtensions());
        std::list<std::string> zstext(zstdFileExtensions());
        ext.push_back(fileExtension());
        ext.splice(ext.end(), zstext);
        return ext;
    }

    /**
     * @brief The export operations should take place here.
     * @param the output stream
     * @return bool Whether the export was successful or not.
     **/
    virtual bool exportGraph(std::ostream &os) = 0;

    /** It is the root graph*/
    Graph *graph;

    PluginProgress *pluginProgress;
    DataSet *dataSet;
};
}
#endif // TALIPOT_EXPORT_MODULE_H
