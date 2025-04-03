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

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // depth
    "Depth of the tree.",

    // degree
    "The tree's degree.",

    // tree layout
    "If true, the generated tree is drawn with the 'Tree Leaf' layout algorithm."};

/// Complete Tree - Import of a complete tree
/** This plugin enables to create a complete tree
 *
 *  User can specify the depth and the degree of the tree.
 */
class CompleteTree : public ImportModule {
  public:
    PLUGININFORMATION("Complete Tree", "Auber", "08/09/2002", "Imports a new complete tree.", "1.1",
                      "Graph")
    CompleteTree(tlp::PluginContext *context) : ImportModule(context) {
        addInParameter<uint>("depth", paramHelp[0].data(), "5");
        addInParameter<uint>("degree", paramHelp[1].data(), "2");
        addInParameter<bool>("tree layout", paramHelp[2].data(), "false");
        addDependency("Tree Leaf", "1.0");
    }
    ~CompleteTree() override = default;

    bool importGraph() override {
        uint degree = 2;
        uint depth = 5;
        bool needLayout = false;

        if (dataSet != nullptr) {
            dataSet->get("depth", depth);
            dataSet->get("degree", degree);
            dataSet->get("tree layout", needLayout);
        }

        // reserve enough memory for nodes/edges to add
        uint total = 0, previous = 1;

        for (uint i = 0; i < depth; ++i) {
            previous *= degree;
            total += previous;
        }

        graph->reserveEdges(total);
        graph->addNodes(total + 1);
        const vector<node> &nodes = graph->nodes();

        uint current = 0;
        uint nextChild = 1;

        while (total) {
            node n = nodes[current];

            for (uint i = 0; i < degree; ++i, ++nextChild, --total) {
                graph->addEdge(n, nodes[nextChild]);
            }

            ++current;
        }

        if (needLayout) {
            // apply Tree Leaf
            string errMsg;
            LayoutProperty *layout = graph->getLayoutProperty("viewLayout");
            return graph->applyPropertyAlgorithm("Tree Leaf", layout, errMsg, nullptr,
                                                 pluginProgress);
        }

        return true;
    }
};

PLUGIN(CompleteTree)
