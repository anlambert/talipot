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

#ifndef TALIPOT_PLUGIN_LOADER_H
#define TALIPOT_PLUGIN_LOADER_H

#include <list>
#include <string>
#include <talipot/WithDependency.h>

namespace tlp {

class Plugin;

/**
 * @ingroup Plugins
 *
 * @brief A callback class when loading plugins into Talipot
 *
 * This interface can be subclassed and passed to the tlp::PluginLibraryLoader to implement custom
 * event handling when loading plugins into Talipot
 *
 * @see tlp::PluginLibraryLoader
 * @see tlp::PluginLoaderTxt
 */
struct TLP_SCOPE PluginLoader {
    virtual ~PluginLoader() = default;

    /**
     * @brief Called when starting to load plugins into a given directory
     * @param path The absolute path of the directory
     */
    virtual void start(const std::string &path) = 0;

    /**
     * @brief Indicates the number of files to be loaded
     * @param int the number of files
     */
    virtual void numberOfFiles(int) {}

    /**
     * @brief Indicates that a new file is being loaded
     * @param filename The absolute path of the file
     */
    virtual void loading(const std::string &filename) = 0;

    /**
     * @brief Indicates that a plugin has been loaded successfully
     * @param info The Plugin object that has just been loaded
     * @param dependencies The plugin dependencies
     *
     * @see tlp::Dependency
     */
    virtual void loaded(const Plugin *info, const std::list<Dependency> &dependencies) = 0;

    /**
     * @brief Indicates that an error occurred when trying to load a file.
     * @param filename The absolute path of where the error occurred.
     * @param errormsg A human-readable error message.
     */
    virtual void aborted(const std::string &filename, const std::string &errormsg) = 0;

    /**
     * @brief Indicates that a file has finished being loaded
     * @param state true if the file was loaded successfully
     * @param msg An additional human-readable message about the load state
     */
    virtual void finished(bool state, const std::string &msg) = 0;
};
}
#endif // TALIPOT_PLUGIN_LOADER_H
