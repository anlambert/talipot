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

#ifndef TALIPOT_ALGORITHM_H
#define TALIPOT_ALGORITHM_H

#include <list>
#include <string>
#include <talipot/Plugin.h>
#include <talipot/PluginContext.h>
#include <talipot/MaterialDesignIcons.h>

namespace tlp {

static const std::string ALGORITHM_CATEGORY = "Algorithm";

class PluginProgress;
class Graph;
class DataSet;

/**
 * @ingroup Plugins
 * @brief This abstract class describes a basic algorithm plugin.
 *
 * It inherits on WithParameter and WithDependency for convenience.
 * Basic functionality consists in checking the algorithm can run on the current Graph (e.g. is the
 * graph simple ?), running the algorithm and resetting the algorithm to re-apply it.
 * The algorithm can and should report progress and which task it is performing if it is decomposed
 * in multiple phases (e.g. layouting the graph, coloring it, ...).
 */
class Algorithm : public tlp::Plugin {
  public:
    /**
     * @brief Constructs an algorithm and initializes members from the AlgorithmContext.
     *
     * @param context The context this algorithm runs in, containing the graph, a DataSet for the
     * parameters, and a PluginProgress
     * to give feedback to the user about the tasks the algorithm is performing.
     */
    Algorithm(const PluginContext *context)
        : graph(nullptr), pluginProgress(nullptr), dataSet(nullptr) {
        if (context != nullptr) {
            const auto *algorithmContext = static_cast<const AlgorithmContext *>(context);
            graph = algorithmContext->graph;
            pluginProgress = algorithmContext->pluginProgress;
            dataSet = algorithmContext->dataSet;
        }
    }
    ~Algorithm() override = default;

    std::string icon() const override {
        return MaterialDesignIcons::Cogs;
    }
    /**
     * @brief Runs the algorithm.
     * It is a good practice to report progress through the PluginProgress, Even if your algorithm
     *is very fast. Keep in mind that Tulip can handle very large graphs. The PluginProgress should
     *also be used to report errors, if any.
     *
     * @return bool Whether the algorithm execution was successful or not.
     **/
    virtual bool run() = 0;

    std::string category() const override {
        return ALGORITHM_CATEGORY;
    }
    /**
     * @brief Checks whether the algorithm can be applied on this graph or not.
     * If not, the reason why should be reported through the PluginProgress.
     *
     * @param errorMessage A string whose value will be modified to an error message, if the check
     *fails.
     * @return bool Whether the plug-in can run on this Graph.
     **/
    virtual bool check(std::string &) {
        return true;
    }
    /**
     * @brief The graph this algorithm will be run on. Retrieved from the context at construction.
     */
    Graph *graph;
    /**
     * @brief A PluginProgress to give feedback to the user, retrieved from the context. It can be a
     * nullptr pointer, so use with caution.
     */
    PluginProgress *pluginProgress;
    /**
     * @brief A DataSet containing parameters for this algorithm, if any. Retrieved from the context
     * at construction.
     */
    DataSet *dataSet;
};
}
#endif // TALIPOT_ALGORITHM_H
