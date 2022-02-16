/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Circle.h>
#include "BubbleTree.h"
#include "DatasetTools.h"

PLUGIN(BubbleTree)

using namespace std;
using namespace tlp;

struct greaterRadius {
  const std::vector<double> &radius;
  greaterRadius(const std::vector<double> &r) : radius(r) {}
  bool operator()(unsigned i1, unsigned i2) const {
    return radius[i1] > radius[i2];
  }
};

double BubbleTree::computeRelativePosition(tlp::node n,
                                           NodeVectorProperty<Vec5d> &relativePosition) {

  Size tmpSizeFather = nodeSize->getNodeValue(n);
  tmpSizeFather[2] = 0.; // remove z-coordinates because the drawing is 2D
  double sizeFather = tmpSizeFather.norm() / 2.;

  if (sizeFather < 1E-5) {
    sizeFather = 1.;
  }

  double sizeVirtualNode = 1.;

  if (tree->indeg(n) == 0) {
    sizeVirtualNode = 0.;
  }

  /*
   * Initialize node position
   */
  relativePosition[n][0] = 0.;
  tlp::Vec5d &rPos = relativePosition[n];
  rPos[1] = 0.;

  /*
   * Special case if the node is a leaf.
   */
  if (tree->outdeg(n) == 0) {
    rPos[2] = 0.;
    rPos[3] = 0.;
    Size tmpSizeNode = nodeSize->getNodeValue(n);
    tmpSizeNode[2] = 0.;
    return (rPos[4] = tmpSizeNode.norm() / 2.);
  }

  /*
   * Recursive call to obtain the set of radius of the children of n
   * A node is dynamically inserted in the neighborhood of n in order to
   * reserve space for the connection of the father of n
   */
  uint Nc = tree->outdeg(n) + 1;
  vector<double> angularSector(Nc);
  std::vector<double> realCircleRadius(Nc);
  realCircleRadius[0] = sizeVirtualNode;
  double sumRadius = sizeVirtualNode;

  Iterator<node> *itN = tree->getOutNodes(n);

  for (uint i = 1; itN->hasNext(); ++i) {
    node itn = itN->next();
    realCircleRadius[i] = computeRelativePosition(itn, relativePosition);
    sumRadius += realCircleRadius[i];
  }

  delete itN;

  double resolution = 0.;

  if (nAlgo) {
    std::vector<double> subCircleRadius(Nc);
    subCircleRadius[0] = realCircleRadius[0];
    double maxRadius = sizeVirtualNode;
    uint maxRadiusIndex = 0;

    for (uint i = 0; i < Nc; ++i) {
      subCircleRadius[i] = realCircleRadius[i];

      if (maxRadius < subCircleRadius[i]) {
        maxRadius = subCircleRadius[i];
        maxRadiusIndex = i;
      }
    }

    if (maxRadius > (sumRadius / 2.)) {
      double ratio;

      if (sumRadius - maxRadius > 1E-5) {
        ratio = maxRadius / (sumRadius - maxRadius);
      } else {
        ratio = 1.;
      }

      for (uint i = 0; i < Nc; ++i) {
        if (i != maxRadiusIndex) {
          subCircleRadius[i] *= ratio;
        }
      }

      sumRadius = 2. * maxRadius;
    }

    for (uint i = 0; i < Nc; ++i) {
      angularSector[i] = 2. * M_PI * subCircleRadius[i] / sumRadius;
    }
  } else {
    resolution = 2. * M_PI;
    std::vector<unsigned> index(Nc);

    for (unsigned i = 0; i < Nc; ++i) {
      index[i] = i;
    }

    sort(index.begin(), index.end(), greaterRadius(realCircleRadius));
    auto i = index.begin();

    for (; i != index.end(); ++i) {
      double radius = realCircleRadius[*i];
      double angleMax = 2. * asin(radius / (radius + sizeFather));
      double angle = radius * resolution / sumRadius;

      if (angle > angleMax) {
        angularSector[*i] = angleMax;
        sumRadius -= radius;
        resolution -= angleMax;
      } else {
        break;
      }
    }

    if (i != index.end()) {
      for (; i != index.end(); ++i) {
        double radius = realCircleRadius[*i];
        double angle = radius * resolution / sumRadius;
        angularSector[*i] = angle;
      }

      resolution = 0.;
    } else {
      resolution /= Nc;
    }
  }

  double angle = 0.;
  vector<Circled> circles(Nc);

  for (uint i = 0; i < Nc; ++i) {
    double packRadius;

    if (fabs(sin(angularSector[i])) > 1E-05) {
      packRadius = realCircleRadius[i] / sin(angularSector[i] / 2.);
    } else {
      packRadius = 0.;
    }

    packRadius = std::max(packRadius, sizeFather + realCircleRadius[i]);

    if (i > 0) {
      angle += (angularSector[i - 1] + angularSector[i]) / 2. + resolution;
    }

    circles[i][0] = packRadius * cos(angle);
    circles[i][1] = packRadius * sin(angle);
    circles[i].radius = realCircleRadius[i];
  }

  Circled circleH = tlp::enclosingCircle(circles);
  rPos[2] = -circleH[0];
  rPos[3] = -circleH[1];
  rPos[4] = sqrt(circleH.radius * circleH.radius - circleH[1] * circleH[1]) - fabs(circleH[0]);
  /*
   * Set relative position of all children
   * according to the center of the enclosing circle
   */
  itN = tree->getOutNodes(n);

  for (uint i = 1; i < Nc; ++i) {
    node itn = itN->next();
    relativePosition[itn][0] = circles[i][0] - circleH[0];
    relativePosition[itn][1] = circles[i][1] - circleH[1];
  }

  delete itN;
  return circleH.radius;
}

void BubbleTree::calcLayout2(tlp::node n, tlp::Vec5d &nrPos,
                             NodeVectorProperty<Vec5d> &relativePosition,
                             const tlp::Vec3d &enclosingCircleCenter,
                             const tlp::Vec3d &originNodePosition) {
  /*
   * Make rotation around the center of the enclosing circle in order to align :
   * the virtual node, the enclosing circle' center and the grand father of the node.
   */
  Vec3d bend, zeta, zetaOriginal;
  bend.fill(0.);
  bend[0] = nrPos[4];

  zeta[0] = nrPos[2];
  zeta[1] = nrPos[3];
  zeta[2] = 0.;
  zetaOriginal = zeta;

  Vec3d vect, vect3;
  vect = originNodePosition - enclosingCircleCenter;
  vect /= vect.norm();
  vect3 = zeta + bend;
  vect3 /= vect3.norm();

  double cosAlpha, sinAlpha;
  cosAlpha = (vect3.dotProduct(vect));
  sinAlpha = (vect ^ vect3)[2];

  Vec3d rot1, rot2;
  rot1[0] = cosAlpha;
  rot1[1] = -sinAlpha;
  rot1[2] = 0.;
  rot2[0] = sinAlpha;
  rot2[1] = cosAlpha;
  rot2[2] = 0.;
  zeta = rot1 * zeta[0] + rot2 * zeta[1];

  result->setNodeValue(n, Coord(float(enclosingCircleCenter[0] + zeta[0]),
                                float(enclosingCircleCenter[1] + zeta[1]), 0.));

  /*
   * Place bend on edge to prevent overlapping
   */
  if (tree->outdeg(n) > 0) {
    bend += zetaOriginal;
    bend = rot1 * bend[0] + rot2 * bend[1];
    bend += enclosingCircleCenter;
    Vec3d a = enclosingCircleCenter + zeta - bend;
    Vec3d b = originNodePosition - bend;
    a /= a.norm();
    b /= b.norm();

    if ((1. - fabs(a.dotProduct(b))) > 1E-5) {
      Iterator<edge> *itE = tree->getInEdges(n);
      edge ite = itE->next();
      delete itE;
      vector<Coord> tmp(1);
      tmp[0] = Coord(float(bend[0]), float(bend[1]), 0.);
      result->setEdgeValue(ite, tmp);
    }
  }

  /*
   * Make the recursive call, to place the children of n.
   */
  Iterator<node> *it = tree->getOutNodes(n);

  while (it->hasNext()) {
    node itn = it->next();
    tlp::Vec5d &rPos = relativePosition[itn];
    Vec3d newpos;
    newpos[0] = rPos[0];
    newpos[1] = rPos[1];
    newpos[2] = 0.;
    newpos = rot1 * newpos[0] + rot2 * newpos[1];
    newpos += enclosingCircleCenter;
    calcLayout2(itn, rPos, relativePosition, newpos, enclosingCircleCenter + zeta);
  }

  delete it;
}

void BubbleTree::calcLayout(tlp::node n, NodeVectorProperty<Vec5d> &relativePosition) {
  /*
   * Make the recursive call, to place the children of n.
   */
  result->setNodeValue(n, Coord(0., 0., 0.));
  Iterator<node> *it = tree->getOutNodes(n);

  if (it->hasNext()) {
    tlp::Vec5d &nPos = relativePosition[n];
    double nPos2 = nPos[2];
    double nPos3 = nPos[3];

    while (it->hasNext()) {
      node itn = it->next();
      Vec3d origin, tmp;
      tlp::Vec5d &rPos = relativePosition[itn];
      origin[0] = rPos[0] - nPos2;
      origin[1] = rPos[1] - nPos3;
      origin[2] = 0.;
      tmp.fill(0.);
      calcLayout2(itn, rPos, relativePosition, origin, tmp);
    }
  }

  delete it;
}

static constexpr std::string_view paramHelp[] = {
    // Complexity
    "This parameter enables to choose the complexity of the algorithm."
    "If true, the complexity is O(n.log(n)), if false it is O(n)."};

BubbleTree::BubbleTree(const tlp::PluginContext *context) : LayoutAlgorithm(context) {
  addNodeSizePropertyParameter(this);
  addInParameter<bool>("complexity", paramHelp[0].data(), "true");
  addDependency("Connected Components Packing", "1.0");
  addDependency("Circular", "1.1");
}

BubbleTree::~BubbleTree() = default;

bool BubbleTree::run() {

  if (!ConnectedTest::isConnected(graph)) {
    // for each component draw
    string err;
    auto components = ConnectedTest::computeConnectedComponents(graph);

    for (const auto &component : components) {
      Graph *tmp = graph;
      // apply "Bubble Tree" on the subgraph induced
      // by the current connected component
      graph = graph->inducedSubGraph(component);
      run();
      tmp->delSubGraph(graph);
      // restore current graph
      graph = tmp;
      if (pluginProgress && pluginProgress->state() != ProgressState::TLP_CONTINUE) {
        return pluginProgress->state() != ProgressState::TLP_CANCEL;
      }
    }

    // call connected component packing
    LayoutProperty tmpLayout(graph);
    DataSet tmpdataSet;
    tmpdataSet.set("coordinates", result);
    graph->applyPropertyAlgorithm("Connected Components Packing", &tmpLayout, err, &tmpdataSet,
                                  pluginProgress);
    *result = tmpLayout;
    return true;
  }

  if (!getNodeSizePropertyParameter(dataSet, nodeSize)) {
    if (graph->existProperty("viewSize")) {
      nodeSize = graph->getSizeProperty("viewSize");
    } else {
      nodeSize = graph->getSizeProperty("viewSize");
      nodeSize->setAllNodeValue(Size(1., 1., 1.));
    }
  }

  if (dataSet == nullptr || !dataSet->get("complexity", nAlgo)) {
    nAlgo = true;
  }

  result->setAllEdgeValue(vector<Coord>(0));

  if (pluginProgress) {
    pluginProgress->showPreview(false);
  }

  if (graph->numberOfNodes() == 3 && graph->numberOfEdges() == 3) {
    string err;
    graph->applyPropertyAlgorithm("Circular", result, err, nullptr, pluginProgress);
    return true;
  }

  tree = TreeTest::computeTree(graph, pluginProgress);

  if (pluginProgress && pluginProgress->state() != ProgressState::TLP_CONTINUE) {
    TreeTest::cleanComputedTree(graph, tree);
    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  }

  node startNode = tree->getSource();
  assert(startNode.isValid());
  NodeVectorProperty<Vec5d> relativePosition(graph);
  computeRelativePosition(startNode, relativePosition);
  calcLayout(startNode, relativePosition);

  TreeTest::cleanComputedTree(graph, tree);

  return true;
}
