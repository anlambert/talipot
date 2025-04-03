/**
 *
 * Copyright (C) 2021-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Algorithm.h>
#include <talipot/Graph.h>
#include <talipot/PluginLibraryLoader.h>
#ifndef NDEBUG
#include <talipot/PluginLoaderTxt.h>
#endif
#include <talipot/PluginsManager.h>
#include <talipot/TlpTools.h>

#include <CrashHandler.h>

using namespace std;
using namespace tlp;

DataSet getDefaultPluginParameters(const std::string &pluginName, Graph *graph = nullptr) {
    DataSet result;
    const ParameterDescriptionList &parameters = PluginsManager::getPluginParameters(pluginName);
    parameters.buildDefaultDataSet(result, graph);
    return result;
}

int main() {

    CrashHandler::install();

    string talipotBuildDir = TALIPOT_BUILD_DIR;

    initTalipotLib();

    vector<string> pluginDirs = {"clustering", "colors",    "general", "import", "layout",
                                 "metric",     "selection", "sizes",   "string", "test"};

    for (const auto &pluginDir : pluginDirs) {
        PluginLibraryLoader::loadPluginsFromDir(talipotBuildDir + "/plugins/" + pluginDir);
    }

    const string importPlugin = "Planar Graph";
    DataSet importParams = getDefaultPluginParameters(importPlugin);

    for (const auto &plugin : PluginsManager::availablePlugins<Algorithm>()) {
        Graph *graph = tlp::importGraph(importPlugin, importParams);
        DataSet params = getDefaultPluginParameters(plugin, graph);
        string errMsg;
        graph->applyAlgorithm(plugin, errMsg, &params);
        delete graph;
    }

    return EXIT_SUCCESS;
}
