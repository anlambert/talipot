/**
 *
 * Copyright (C) 2022-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <cstring>

#include <cgraph.h>
#include <gvc.h>

#include <talipot/Coord.h>
#include <talipot/Graph.h>
#include <talipot/LayoutProperty.h>

#include <talipot/hash.h>

using namespace std;
using namespace tlp;

extern bool getCoordFromGraphvizPos(Coord &outCoord, const string &inValue);
extern bool getCoordsFromGraphvizPos(vector<Coord> &outCoords, const string &inValue);

bool applyGraphvizLayout(Graph *graph, LayoutProperty *result, const string &layoutName,
                         PluginProgress *pluginProgress = nullptr) {
  flat_hash_map<node, Agnode_t *> nodesMap;
  flat_hash_map<edge, Agedge_t *> edgesMap;
  Agraph_t *G = agopen(const_cast<char *>("graph"), Agdirected, 0);
  Agsym_t *width = agattr(G, AGNODE, const_cast<char *>("width"), const_cast<char *>(""));
  Agsym_t *height = agattr(G, AGNODE, const_cast<char *>("height"), const_cast<char *>(""));
  for (auto n : graph->nodes()) {
    nodesMap[n] = agnode(G, const_cast<char *>(to_string(n.id).c_str()), 1);
    Size size = (*graph)["viewSize"][n];
    agxset(nodesMap[n], width, const_cast<char *>(to_string(size[0]).c_str()));
    agxset(nodesMap[n], height, const_cast<char *>(to_string(size[1]).c_str()));
  }
  for (auto e : graph->edges()) {
    const auto &[src, tgt] = graph->ends(e);
    edgesMap[e] =
        agedge(G, nodesMap[src], nodesMap[tgt], const_cast<char *>(to_string(e.id).c_str()), 1);
  }

  // to prevent a crash with old graphviz versions
  agattr(G, AGNODE, const_cast<char *>("label"), const_cast<char *>(""));

  bool ret = false;
  GVC_t *gvc = gvContext();
  if (gvLayout(gvc, G, layoutName.c_str()) == 0 && gvRender(gvc, G, "dot", 0) == 0) {
    ret = true;
    Agsym_t *pos = agattr(G, AGNODE, const_cast<char *>("pos"), 0);
    Coord c;
    for (auto n : graph->nodes()) {
      string posValue = agxget(nodesMap[n], pos);
      if (getCoordFromGraphvizPos(c, posValue)) {
        result->setNodeValue(n, c);
      }
    }
    pos = agattr(G, AGEDGE, const_cast<char *>("pos"), 0);
    vector<Coord> bends;
    for (auto e : graph->edges()) {
      string posValue = agxget(edgesMap[e], pos);
      if (getCoordsFromGraphvizPos(bends, posValue)) {
        result->setEdgeValue(e, bends);
      }
    }
    gvFreeLayout(gvc, G);
  } else if (pluginProgress && aglasterr()) {
    pluginProgress->setError(aglasterr());
  }

  agclose(G);
  gvFreeContext(gvc);

  return ret;
}
