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

#include <talipot/SizeProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/VectorProperty.h>
#include <talipot/PropertyAlgorithm.h>
#include <talipot/TreeTest.h>

using namespace std;
using namespace tlp;

class OrthoTree : public tlp::LayoutAlgorithm {

    uint nodeSpacing;
    uint layerSpacing;
    tlp::SizeProperty *size;
    tlp::Graph *tree;

    void computeVerticalSize(const tlp::node n, tlp::NodeVectorProperty<double> &verticalSize);
    void computeLayout(const tlp::node n, tlp::NodeVectorProperty<double> &verticalSize);

  public:
    PLUGININFORMATION("OrthoTree", "Romain Bourqui", "20/02/2012", "Orthogonal Tree", "1.0", "Tree")

    OrthoTree(const tlp::PluginContext *context);

    bool run() override;
};

PLUGIN(OrthoTree)

//================================================================================
static constexpr std::string_view paramHelp[] = {
    // layer spacing
    "Define the spacing between two successive layers",
    // node spacing
    "Define the spacing between two nodes"};

OrthoTree::OrthoTree(const tlp::PluginContext *context)
    : tlp::LayoutAlgorithm(context), nodeSpacing(4), layerSpacing(10), size(nullptr) {
    addInParameter<uint>("layer spacing", paramHelp[0].data(), "10", true);
    addInParameter<uint>("node spacing", paramHelp[1].data(), "4", true);
}

void OrthoTree::computeVerticalSize(const node n, NodeVectorProperty<double> &verticalSize) {
    if (tree->outdeg(n) == 0) {
        verticalSize[n] = size->getNodeValue(n)[1];
    } else {
        double s = 0.;
        for (auto u : tree->getOutNodes(n)) {
            computeVerticalSize(u, verticalSize);
            s += verticalSize[u];
        }

        if (tree->outdeg(n) > 1) {
            s += nodeSpacing * (graph->outdeg(n) - 1);
        }

        verticalSize[n] = s;
    }
}

void OrthoTree::computeLayout(const node n, NodeVectorProperty<double> &verticalSize) {
    Coord cn = result->getNodeValue(n);
    double prev = 0.;
    for (auto e : tree->getOutEdges(n)) {
        node u = tree->opposite(e, n);
        Coord c = cn;
        c[0] += layerSpacing;
        c[1] -= prev;

        prev += verticalSize[u] + nodeSpacing;
        result->setNodeValue(u, c);

        Coord bend = {cn[0], c[1]};
        vector<Coord> bends(1);
        bends[0] = bend;
        result->setEdgeValue(e, bends);
        computeLayout(u, verticalSize);
    }
}

bool OrthoTree::run() {
    layerSpacing = 10;
    nodeSpacing = 4;

    if (dataSet != nullptr) {
        dataSet->get("layer spacing", layerSpacing);
        dataSet->get("node spacing", nodeSpacing);
    }

    tree = TreeTest::computeTree(graph, pluginProgress);

    NodeVectorProperty<double> verticalSize(tree);
    size = graph->getSizeProperty("viewSize");

    verticalSize.setAll(0);

    node root = tree->getSource();
    assert(root.isValid());

    computeVerticalSize(root, verticalSize);

    result->setAllNodeValue(Coord(0, 0, 0));
    result->setAllEdgeValue(vector<Coord>(0));
    computeLayout(root, verticalSize);

    TreeTest::cleanComputedTree(graph, tree);

    return true;
}
