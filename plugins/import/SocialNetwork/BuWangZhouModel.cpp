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

#include <talipot/ImportModule.h>
#include <talipot/Graph.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // nodes
    "Number of nodes.",
    // nodes
    "Number of node types.",
    // m
    "Number of edges added for each new node."};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * Shouliang Bu, Bing-Hong Wang, Tao Zhou.
 * Gaining scale-free and high clustering complex networks.
 * Physica A, 374, 864-868, 2007.
 *
 * The model indicates that a probability must be assign to every type of node
 * (such the sum is equal to 1), but this implementation assign the same value
 * as the article states that this give small world, scale-free networks.
 *
 */
class BuWangZhouModel : public ImportModule {
public:
  PLUGININFORMATION("Bu Wang Zhou Model", "sallaberry", "20/06/2011",
                    "Randomly generates a scale-free graph using the model described "
                    "in<br/>Shouliang Bu, Bing-Hong Wang, Tao Zhou.<br/><b>Gaining scale-free and "
                    "high clustering complex networks.</b><br/>Physica A, 374, 864--868, 2007.",
                    "1.0", "Social network")
  BuWangZhouModel(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "200");
    addInParameter<uint>("types of nodes", paramHelp[1].data(), "3");
    addInParameter<uint>("m", paramHelp[2].data(), "2");
  }

  bool importGraph() override {
    uint nb_nodes = 200;
    uint types_of_nodes = 3;
    uint m = 2;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nb_nodes);
      dataSet->get("types of nodes", types_of_nodes);
      dataSet->get("m", m);
    }

    // check arguments
    if (types_of_nodes > nb_nodes) {
      pluginProgress->setError(
          "The number of node types cannot be greater than the number of nodes");
      return false;
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    vector<vector<node>> nodes(types_of_nodes);
    graph->reserveNodes(nb_nodes);

    // In the paper, there are 3 types starting from a triangle without telling if the whole graph
    // or a cycle has to be taken into account.
    for (uint i = 0; i < types_of_nodes; ++i) {
      nodes[i].push_back(graph->addNode());

      for (unsigned j = 0; j < i; ++j) {
        graph->addEdge(nodes[j][0], nodes[i][0]);
      }
    }

    graph->addEdge(nodes[types_of_nodes - 1][0], nodes[0][0]);

    uint random_type, random_node;
    double pr, k_sum, pr_sum;

    for (uint i = 0; i < nb_nodes - types_of_nodes; ++i) {
      nodes[i % types_of_nodes].push_back(graph->addNode());

      for (uint j = 0; j < m; ++j) {

        // Random type
        do {
          random_type = tlp::randomNumber(types_of_nodes - 1);
        } while (random_type == i % types_of_nodes);

        // Random node
        k_sum = 0;
        pr_sum = 0;

        for (random_node = 0; random_node < nodes[random_type].size(); random_node++) {
          k_sum += graph->deg(nodes[random_type][random_node]);
        }

        pr = tlp::randomNumber();
        random_node = 0;

        while (pr_sum < pr && nodes[random_type].size() > (random_node + 1)) {
          pr_sum += graph->deg(nodes[random_type][random_node]) / k_sum;
          random_node++;
        }

        graph->addEdge(nodes[i % types_of_nodes].back(), nodes[random_type][random_node]);
      }

      if (i % 100 == 0) {
        if (pluginProgress->progress(i, nb_nodes - types_of_nodes) != ProgressState::TLP_CONTINUE) {
          return pluginProgress->state() != ProgressState::TLP_CANCEL;
        }
      }
    }

    return true;
  }
};

PLUGIN(BuWangZhouModel)
