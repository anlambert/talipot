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

#ifndef TALIPOT_BUILD_CORE_ONLY
#include <QApplication>
#endif

#include <talipot/PluginLoaderTxt.h>
#include <talipot/PluginLibraryLoader.h>
#include <talipot/PluginsManager.h>

#include <CrashHandler.h>

using namespace std;
using namespace tlp;

// Custom loader to catch if there was some issues
// when loading plugins
class PluginLoaderTest : public PluginLoaderTxt {
  public:
    PluginLoaderTest() : allPluginsLoaded(true) {}

    void aborted(const string &filename, const string &errormsg) override {
        const string &libName = PluginLibraryLoader::getCurrentPluginFileName();
        // plugins may be loaded twice because it may exist an other version
        // of the plugins in a CMakeFiles sub dir (/CMakeRelink.dir)
        // So set the failure flag only if the plugin was not found
        // under the CMakeFiles dir
        if (libName.find("CMakeFiles") == string::npos) {
            allPluginsLoaded = false;
        }
        PluginLoaderTxt::aborted(filename, errormsg);
    }

    bool allPluginsLoaded;
};

// Simple test that will try to load all compiled plugins from the Talipot trunk
// in order to detect possible issues.
// The test will then try to create and destroy an instance of each plugin.
// This can be helpful to catch possible segfaults or memory leaks.
int main(int argc, char **argv) {

    CrashHandler::install();

#ifndef TALIPOT_BUILD_CORE_ONLY
    // we need to create a QApplication as some plugins (view, interactor)
    // need one to load correctly
    QApplication app(argc, argv);
#endif

    string talipotPluginsDir = TALIPOT_PLUGINS_DIR;

    if (talipotPluginsDir.empty() && argc > 1) {
        talipotPluginsDir = argv[1];
    }

    initTalipotLib();

    // load all plugins from the Talipot build folder
    PluginLoaderTest pLoader;
    PluginLibraryLoader::loadPluginsFromDir(talipotPluginsDir, &pLoader);

    // create an instance of each of them, then destroy it
    auto pluginNames = PluginsManager::availablePlugins();
    for (const auto &pluginName : pluginNames) {
        Plugin *plugin = PluginsManager::getPluginObject(pluginName);
        delete plugin;
    }

    // test is successful if there was no plugin loading issue
    if (pLoader.allPluginsLoaded) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
