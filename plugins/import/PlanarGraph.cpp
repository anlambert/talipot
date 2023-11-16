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

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // nodes
    "Number of nodes in the final graph."};

namespace {
struct Triangle {
  Triangle(node a, node b, node c) : a(a), b(b), c(c) {}
  node a, b, c;
};
} // namespace
//=============================================================

/// Planar Graph - Import of a planar graph
/** This plugin enables to create a planar graph
 *
 *  User can specify the number of nodes.
 */
class PlanarGraph : public ImportModule {
public:
  PLUGININFORMATION("Planar Graph", "Auber", "25/06/2005",
                    "Imports a new randomly generated planar graph.", "1.0", "Graph")
  PlanarGraph(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "30");
  }
  ~PlanarGraph() override = default;

  bool importGraph() override {
    uint nbNodes = 30;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nbNodes);
    }

    if (nbNodes < 3) {
      nbNodes = 3;
    }

    // initialize a random sequence according the given seed
    tlp::initRandomSequence();

    LayoutProperty *newLayout = graph->getLocalLayoutProperty("viewLayout");
    SizeProperty *newSize = graph->getLocalSizeProperty("viewSize");
    newSize->setAllNodeValue(Size(1.0, 1.0, 1.0));

    // reserve graph needed elts
    graph->reserveNodes(nbNodes);
    graph->reserveEdges(3 * (nbNodes - 2));

    vector<Triangle> faces;
    Triangle f(graph->addNode(), graph->addNode(), graph->addNode());
    faces.push_back(f);
    graph->addEdge(f.a, f.b);
    graph->addEdge(f.b, f.c);
    graph->addEdge(f.c, f.a);
    float val = nbNodes;
    newLayout->setNodeValue(f.a, Coord(-val, -val, 0));
    newLayout->setNodeValue(f.b, Coord(0, val, 0));
    newLayout->setNodeValue(f.c, Coord(val, -val, 0));
    uint nb = 3;

    while (nb < nbNodes) {
      // choose a Triangle randomly
      uint i = randomNumber(faces.size() - 1);
      Triangle f = faces[i];
      node n = graph->addNode();
      Coord tmp = newLayout->getNodeValue(f.a) + newLayout->getNodeValue(f.b) +
                  newLayout->getNodeValue(f.c);
      tmp /= 3.0;
      newLayout->setNodeValue(n, tmp);

      // Split the triangle in three part
      graph->addEdge(n, f.a);
      graph->addEdge(n, f.b);
      graph->addEdge(n, f.c);
      // add the three new Triangle, remove the old one(replace)
      Triangle f1(f.a, f.b, n);
      Triangle f2(f.b, f.c, n);
      Triangle f3(f.c, f.a, n);
      faces[i] = f1;
      faces.push_back(f2);
      faces.push_back(f3);
      ++nb;
    }

    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  }
};

PLUGIN(PlanarGraph)
