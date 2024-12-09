/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/SortIterator.h>

#include "HierarchicalGraph.h"
#include "DatasetTools.h"

PLUGIN(HierarchicalGraph)

using namespace std;
using namespace tlp;

static const int NB_UPDOWN_SWEEP = 4;

//================================================================================

static constexpr std::string_view paramHelp[] = {
    // orientation
    "This parameter enables to choose the orientation of the drawing."};

//================================================================================
#define ORIENTATION "horizontal;vertical;"
//================================================================================
HierarchicalGraph::HierarchicalGraph(const tlp::PluginContext *context) : LayoutAlgorithm(context) {
  addNodeSizePropertyParameter(this);
  addInParameter<StringCollection>("orientation", paramHelp[0].data(), ORIENTATION, true,
                                   "<b>horizontal</b> <br> <b>vertical</b>");
  addSpacingParameters(this);
  addDependency("Hierarchical Tree (R-T Extended)", "1.1");
}
//================================================================================
HierarchicalGraph::~HierarchicalGraph() = default;
//================================================================================
class LessThanEdge {
public:
  DoubleProperty *metric;
  Graph *sg;
  bool operator()(edge e1, edge e2) const {
    return (metric->getNodeValue(sg->source(e1)) < metric->getNodeValue(sg->source(e2)));
  }
};
//================================================================================
void HierarchicalGraph::buildGrid(tlp::Graph *sg) {
  tlp::NodeVectorProperty<uint> levels(sg);
  dagLevel(graph, levels);

  TLP_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
    uint level = levels[i];

    if (level >= grid.size()) {
      grid.resize(level + 1);
    }

    embedding->setNodeValue(n, grid[level].size());
    grid[level].push_back(n);
  });
}
//================================================================================
inline uint HierarchicalGraph::degree(tlp::Graph *sg, tlp::node n, bool directed) {
  return directed ? sg->outdeg(n) : sg->indeg(n);
}
//================================================================================
void HierarchicalGraph::twoLayerCrossReduction(tlp::Graph *sg, uint freeLayer) {
  for (auto n : grid[freeLayer]) {
    double sum = embedding->getNodeValue(n);
    uint deg = 1;
    for (auto itn : sg->getInOutNodes(n)) {
      sum += embedding->getNodeValue(itn);
      ++deg;
    }
    embedding->setNodeValue(n, sum / deg);
  }
}
//================================================================================
// Set initial position using a DFS
void HierarchicalGraph::initCross(tlp::Graph *sg, tlp::node n, tlp::MutableContainer<bool> &visited,
                                  int id) {
  if (visited.get(n.id)) {
    return;
  }

  visited.set(n.id, true);
  embedding->setNodeValue(n, id);
  for (auto it : sg->getOutNodes(n)) {
    initCross(sg, it, visited, id + 1);
  }
}
//================================================================================
// Do layer by layer sweep to reduce crossings in K-Layer graph
void HierarchicalGraph::crossReduction(tlp::Graph *sg) {

  node tmp = sg->addNode();
  embedding->setNodeValue(tmp, 0);
  for (auto it : sg->nodes()) {
    if (sg->outdeg(it) == 0) {
      sg->addEdge(it, tmp);
    }
  }
  grid.push_back(vector<node>(1, tmp));
  {
    MutableContainer<bool> visited;
    visited.setAll(false);
    node root = sg->getSource();
    initCross(sg, root, visited, 1);
  }

  uint maxDepth = grid.size();

  for (uint i = 0; i < maxDepth; ++i) {
    vector<node> &igrid = grid[i];
    stable_sort(igrid.begin(), igrid.end(), lessNode);

    for (uint j = 0; j < igrid.size(); ++j) {
      embedding->setNodeValue(igrid[j], j);
    }
  }

  // Iterations of the sweeping
  for (uint a = 0; a < NB_UPDOWN_SWEEP; ++a) {
    // Up sweeping
    for (int i = maxDepth - 1; i >= 0; --i) {
      twoLayerCrossReduction(graph, i);
    }

    // Down sweeping
    for (uint i = 0; i < maxDepth; ++i) {
      twoLayerCrossReduction(graph, i);
    }
  }

  for (uint i = 0; i < maxDepth; ++i) {
    vector<node> &igrid = grid[i];
    stable_sort(igrid.begin(), igrid.end(), lessNode);

    for (uint j = 0; j < igrid.size(); ++j) {
      embedding->setNodeValue(igrid[j], j);
    }
  }

  sg->delNode(tmp, true);
}
//================================================================================
void HierarchicalGraph::DagLevelSpanningTree(tlp::Graph *sg, tlp::DoubleProperty *embedding) {
  assert(AcyclicTest::isAcyclic(sg));
  LessThanEdge tmpL;
  tmpL.metric = embedding;
  tmpL.sg = sg;
  for (auto n : sg->nodes()) {
    if (sg->indeg(n) > 1) {
      vector<edge> tmpVect;
      for (auto e : sg->getInEdges(n)) {
        tmpVect.push_back(e);
      }
      sort(tmpVect.begin(), tmpVect.end(), tmpL);
      int toKeep = tmpVect.size() / 2;

      for (auto e : tmpVect) {
        if (toKeep != 0) {
          sg->delEdge(e);
        }
        --toKeep;
      }
    }
  }
  assert(TreeTest::isTree(sg));
}
//==============================================================================================================
void HierarchicalGraph::computeEdgeBends(const tlp::Graph *mySGraph, tlp::LayoutProperty &tmpLayout,
                                         const flat_hash_map<tlp::edge, tlp::edge> &replacedEdges,
                                         const std::vector<tlp::edge> &reversedEdges) {
  MutableContainer<bool> isReversed;
  isReversed.setAll(false);

  for (auto e : reversedEdges) {
    isReversed.set(e.id, true);
  }

  for (const auto &it : replacedEdges) {
    edge toUpdate = it.first;
    edge start = it.second;
    edge end = start;
    node tgt;

    // we take the first and last point of the replaced edges
    while ((tgt = graph->target(end)) != graph->target(toUpdate)) {
      Iterator<edge> *itE = mySGraph->getOutEdges(tgt);

      if (!itE->hasNext()) {
        delete itE;
        break;
      }

      end = itE->next();
      delete itE;
    }

    node firstN = graph->target(start);
    node endN = graph->source(end);
    Coord p1, p2;

    if (isReversed.get(toUpdate.id)) {
      p1 = tmpLayout.getNodeValue(endN);
      p2 = tmpLayout.getNodeValue(firstN);
    } else {
      p1 = tmpLayout.getNodeValue(firstN);
      p2 = tmpLayout.getNodeValue(endN);
    }

    LineType::RealType edgeLine;

    if (p1 == p2) {
      edgeLine.push_back(p1);
    } else {
      edgeLine.push_back(p1);
      edgeLine.push_back(p2);
    }

    result->setEdgeValue(toUpdate, edgeLine);
  }
}
//=======================================================================
void HierarchicalGraph::computeSelfLoops(tlp::Graph *mySGraph, tlp::LayoutProperty &tmpLayout,
                                         std::vector<tlp::SelfLoops> &listSelfLoops) {
  // tlp::warning() << "We compute self loops" << endl;
  while (!listSelfLoops.empty()) {
    tlp::SelfLoops tmp = listSelfLoops.back();
    listSelfLoops.pop_back();
    LineType::RealType tmpLCoord;
    const LineType::RealType &edge1 = tmpLayout.getEdgeValue(tmp.e1);
    const LineType::RealType &edge2 = tmpLayout.getEdgeValue(tmp.e2);
    const LineType::RealType &edge3 = tmpLayout.getEdgeValue(tmp.e3);

    for (const auto &c : edge1) {
      tmpLCoord.push_back(c);
    }

    tmpLCoord.push_back(tmpLayout.getNodeValue(tmp.n1));

    for (const auto &c : edge2) {
      tmpLCoord.push_back(c);
    }

    tmpLCoord.push_back(tmpLayout.getNodeValue(tmp.n2));

    for (const auto &c : edge3) {
      tmpLCoord.push_back(c);
    }

    result->setEdgeValue(tmp.old, tmpLCoord);
    mySGraph->delNode(tmp.n1, true);
    mySGraph->delNode(tmp.n2, true);
  }
}
//=======================================================================
bool HierarchicalGraph::run() {
  orientation = "horizontal";
  spacing = 64.0;
  nodeSpacing = 18;
  SizeProperty *nodeSize = nullptr;

  if (dataSet != nullptr) {
    getNodeSizePropertyParameter(dataSet, nodeSize);
    getSpacingParameters(dataSet, nodeSpacing, spacing);
    StringCollection tmp;

    if (dataSet->get("orientation", tmp)) {
      orientation = tmp.getCurrentString();
    }
  }

  if (nodeSize == nullptr) {
    nodeSize = graph->getSizeProperty("viewSize");
  }

  //=========================================================
  // use a temporary rotated size if necessary
  if (orientation == "horizontal") {
    auto *tmpSize = new SizeProperty(graph);

    tmpSize->copy(nodeSize);
    for (auto n : graph->nodes()) {
      const Size &tmp = tmpSize->getNodeValue(n);
      tmpSize->setNodeValue(n, Size(tmp[1], tmp[0], tmp[2]));
    }
    nodeSize = tmpSize;
  }

  // push a temporary graph state (not redoable)
  graph->push(false);
  result->setAllEdgeValue(std::vector<tlp::Coord>());

  //========================================================================
  // Build a clone of this graph
  Graph *mySGraph = graph->addCloneSubGraph("tmp clone");

  // if the graph is not acyclic we reverse edges to make it acyclic
  vector<tlp::SelfLoops> listSelfLoops;
  vector<edge> reversedEdges;
  AcyclicTest::makeAcyclic(mySGraph, reversedEdges, listSelfLoops);

  //========================================================================
  // We add a node and edges to force the dag to have only one source.
  tlp::makeSimpleSource(mySGraph);

  //========================================================================
  list<node> properAddedNodes;
  flat_hash_map<edge, edge> replacedEdges;
  IntegerProperty *edgeLength = nullptr;

  embedding.reset(new DoubleProperty(mySGraph));

  if (!TreeTest::isTree(mySGraph)) {
    // We transform the dag in a proper dag
    edgeLength = new IntegerProperty(mySGraph);
    tlp::makeProperDag(mySGraph, properAddedNodes, replacedEdges, edgeLength);
    // we compute metric for cross reduction
    lessNode.metric = embedding.get();
    buildGrid(mySGraph);
    crossReduction(mySGraph);
    for (auto n : graph->nodes()) {
      vector<edge> order = iteratorVector(
          new SortTargetEdgeIterator(mySGraph->getInOutEdges(n), mySGraph, embedding.get()));
      mySGraph->setEdgeOrder(n, order);
    }
    // We extract a spanning tree from the proper dag.
    DagLevelSpanningTree(mySGraph, embedding.get());
  } else {
    buildGrid(mySGraph);
  }

// We draw the tree using a tree drawing algorithm
#ifndef NDEBUG
  bool resultBool;
#endif
  string erreurMsg;
  LayoutProperty tmpLayout(graph);
  DataSet tmp;
  tmp.set("node size", nodeSize);
  tmp.set("layer spacing", spacing);
  tmp.set("node spacing", nodeSpacing);

  if (edgeLength != nullptr) {
    tmp.set("edge length", edgeLength);
  }

  tmp.set("orthogonal", true);
  StringCollection tmpS("vertical;horizontal;");
  tmpS.setCurrent("vertical");
  tmp.set("orientation", tmpS);
#ifndef NDEBUG
  resultBool =
#endif
      mySGraph->applyPropertyAlgorithm("Hierarchical Tree (R-T Extended)", &tmpLayout, erreurMsg,
                                       &tmp);

  if (edgeLength) {
    delete edgeLength;
  }

  assert(resultBool);

  for (auto n : graph->nodes()) {
    result->setNodeValue(n, tmpLayout.getNodeValue(n));
  }

  computeEdgeBends(graph, tmpLayout, replacedEdges, reversedEdges);
  computeSelfLoops(graph, tmpLayout, listSelfLoops);

  // forget last temporary graph state
  graph->pop();

  // post processing
  // Prevent edge node overlapping
  uint nbGrids = grid.size();
  std::vector<float> levelMaxSize(nbGrids);
  MutableContainer<uint> nodeLevel;

  for (uint i = 0; i < nbGrids; ++i) {
    float levelMax = levelMaxSize[i] = 0;
    std::vector<node> &igrid = grid[i];
    uint nbNodes = igrid.size();

    for (uint j = 0; j < nbNodes; ++j) {
      node n = igrid[j];

      if (graph->isElement(n)) {
        nodeLevel.set(n.id, i);
        const Size &tmp = nodeSize->getNodeValue(n);
        levelMax = levelMaxSize[i] = std::max(levelMax, tmp[1]);
      }
    }
  }

  float spacing_4 = spacing / 4.f;
  for (auto e : graph->edges()) {
    const auto &[src, tgt] = graph->ends(e);

    if (src == tgt) {
      continue;
    }

    uint srcLevel = nodeLevel.get(src.id);
    uint tgtLevel = nodeLevel.get(tgt.id);
    Coord srcPos = result->getNodeValue(src);
    Coord tgtPos = result->getNodeValue(tgt);
    float curSpacing;

    if (srcLevel > tgtLevel) {
      srcPos[1] += (levelMaxSize[srcLevel] / 2.f + spacing_4);
      tgtPos[1] -= (levelMaxSize[tgtLevel] / 2.f + spacing_4);
      curSpacing = spacing / 2.f;
    } else {
      srcPos[1] -= (levelMaxSize[srcLevel] / 2.f + spacing_4);
      tgtPos[1] += (levelMaxSize[tgtLevel] / 2.f + spacing_4);
      curSpacing = -spacing / 2.f;
    }

    const vector<Coord> &old = result->getEdgeValue(e);

    if (old.empty()) {
      vector<Coord> pos(2);
      pos[0] = srcPos;
      pos[1] = tgtPos;
      result->setEdgeValue(e, pos);
    } else {
      vector<Coord> pos(4);
      Coord src2Pos = old.front();
      Coord tgt2Pos = old.back();
      src2Pos[1] = srcPos[1] + curSpacing;
      tgt2Pos[1] = tgtPos[1] - curSpacing;
      pos[0] = srcPos;
      pos[1] = src2Pos;
      pos[2] = tgt2Pos;
      pos[3] = tgtPos;
      result->setEdgeValue(e, pos);
    }
  }

  // post processing align nodes
  for (auto n : graph->nodes()) {
    Coord tmp = result->getNodeValue(n);
    const Size &tmpS = nodeSize->getNodeValue(n);
    tmp[1] -= (levelMaxSize[nodeLevel.get(n.id)] - tmpS[1]) / 2.f;
    result->setNodeValue(n, tmp);
  }

  // rotate layout
  if (orientation == "horizontal") {
    // delete the temporary allocated SizeProperty (see above)
    delete nodeSize;
    for (auto n : graph->nodes()) {
      const Coord &tmpC = result->getNodeValue(n);
      result->setNodeValue(n, Coord(-tmpC[1], tmpC[0], tmpC[2]));
    }
    for (auto e : graph->edges()) {
      const LineType::RealType &tmp = result->getEdgeValue(e);
      LineType::RealType tmp2;

      for (const auto &p : tmp) {
        tmp2.push_back(Coord(-p[1], p[0], p[2]));
      }

      result->setEdgeValue(e, tmp2);
    }
  }

  return true;
}
