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
#include <talipot/Delaunay.h>

using namespace std;

static bool delaunayTriangulation(tlp::Graph *graph, bool simplicesSubGraphs, bool originalClone) {
    tlp::NodeVectorProperty<tlp::Coord> points(graph);
    points.copyFromProperty(graph->getLayoutProperty("viewLayout"));

    vector<pair<uint, uint>> edges;
    vector<vector<uint>> simplices;
    bool ret = tlp::delaunayTriangulation(points, edges, simplices);

    if (ret) {

        if (originalClone) {
            graph->addCloneSubGraph("Original graph");
        }

        tlp::Graph *delaunaySg = graph->addSubGraph("Delaunay");
        const std::vector<tlp::node> &nodes = graph->nodes();
        delaunaySg->addNodes(nodes);

        for (const auto &[src, tgt] : edges) {
            delaunaySg->addEdge(nodes[src], nodes[tgt]);
        }

        if (simplicesSubGraphs) {
            for (size_t i = 0; i < simplices.size(); ++i) {
                vector<tlp::node> sNodes(simplices[i].size());
                tlp::TLP_PARALLEL_MAP_INDICES(sNodes.size(),
                                              [&](uint j) { sNodes[j] = nodes[simplices[i][j]]; });

                ostringstream oss;
                oss << (simplices[i].size() == 3 ? "triangle " : "tetrahedron ") << i;
                tlp::Graph *simplexSg = delaunaySg->inducedSubGraph(sNodes);
                simplexSg->setName(oss.str());
            }
        }
    }

    return ret;
}

static constexpr std::string_view paramHelp[] = {
    // simplices
    "If true, a subgraph will be added for each computed simplex (a triangle in 2d, a tetrahedron "
    "in 3d).",
    // original clone
    "If true, a clone subgraph named 'Original graph' will be first added."};

class DelaunayTriangulation : public tlp::Algorithm {

  public:
    DelaunayTriangulation(tlp::PluginContext *context) : Algorithm(context) {
        addInParameter<bool>("simplices", paramHelp[0].data(), "false");
        addInParameter<bool>("original clone", paramHelp[1].data(), "true");
    }

    PLUGININFORMATION(
        "Delaunay triangulation", "Antoine Lambert", "",
        "Performs a Delaunay triangulation, in considering the positions of the graph "
        "nodes as a set of points. The building of simplices (triangles in 2D or "
        "tetrahedrons in 3D) consists in adding edges between adjacent nodes.",
        "1.1", "Triangulation")

    bool run() override {
        if (graph->isEmpty()) {
            return true;
        }

        bool simplicesSg = false;
        bool originalClone = true;

        if (dataSet) {
            dataSet->get("simplices", simplicesSg);
            dataSet->get("original clone", originalClone);
        }

        bool ret = delaunayTriangulation(graph, simplicesSg, originalClone);

        return ret;
    }
};

PLUGIN(DelaunayTriangulation)
