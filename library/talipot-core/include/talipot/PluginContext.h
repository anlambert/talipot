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

#ifndef TALIPOT_PLUGIN_CONTEXT_H
#define TALIPOT_PLUGIN_CONTEXT_H

#include <talipot/DataSet.h>

namespace tlp {

class Graph;
class PropertyInterface;
class PluginProgress;

/**
 * @ingroup Plugins
 * @brief Contains runtime parameters for a plugin.
 *
 * tlp::PluginContext is a data structure containing a set of parameters required by a plugin.
 * Each tlp::Plugin subclass has its associated tlp::PluginContext subclass that contains specific
 * parameters related to the Plugin.
 * For instance, the tlp::AlgorithmContext (related to tlp::Algorithm) contains the Graph on which
 * the algorithm should be run
 * In essence, you will never have to access to a plugin's context since the base class constructor
 * will copy its members into protected members of the plugin interface.
 */
class PluginContext {
  public:
    // Required for dynamic casting
    virtual ~PluginContext() = default;
};

/**
 * @ingroup Plugins
 * @brief Parameters structure for a tlp::Algorithm
 *
 * This class contains data required for a tlp::Algorithm to run. Each of its member is copied into
 * a protected member at the object construction.
 */
class AlgorithmContext : public tlp::PluginContext {
  public:
    /**
     * @brief The pointer to the tlp::Graph on which the algorithm will be run.
     */
    Graph *graph;

    /**
     * @brief Input parameters set by the user when running the plugin.
     * @see tlp::DataSet
     * @see tlp::WithDependency
     */
    DataSet *dataSet;

    /**
     * @brief A progress handler to notify the user about the progress state of the algorithm when
     * run.
     * @see tlp::PluginProgress
     */
    PluginProgress *pluginProgress;

    AlgorithmContext(tlp::Graph *graph = nullptr, tlp::DataSet *dataSet = nullptr,
                     tlp::PluginProgress *progress = nullptr)
        : graph(graph), dataSet(dataSet), pluginProgress(progress) {}
    ~AlgorithmContext() override = default;
};
}
#endif // TALIPOT_PLUGIN_CONTEXT_H
