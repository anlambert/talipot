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

#include <talipot/Circle.h>

#include "ConeTreeExtended.h"
#include "DatasetTools.h"

PLUGIN(ConeTreeExtended)

using namespace std;
using namespace tlp;

//===============================================================
float sqr(float x) {
  return x * x;
}
//===============================================================
float minRadius(float radius1, float alpha1, float radius2, float alpha2) {
  return sqrt(sqr(radius1 + radius2) /
              (sqr(cos(alpha1) - cos(alpha2)) + sqr(sin(alpha1) - sin(alpha2))));
}
//===============================================================
void ConeTreeExtended::computeLayerSize(tlp::node n, uint level) {
  if (levelSize.size() < level + 1) {
    levelSize.push_back(0);
  }

  levelSize[level] = std::max(levelSize[level], nodeSize->getNodeValue(n)[1]);
  for (auto on : tree->getOutNodes(n)) {
    computeLayerSize(on, level + 1);
  }
}
//===============================================================
void ConeTreeExtended::computeYCoodinates(tlp::node root) {
  levelSize.clear();
  yCoordinates.clear();
  computeLayerSize(root, 0);
  yCoordinates.resize(levelSize.size());
  yCoordinates[0] = 0;

  for (uint i = 1; i < levelSize.size(); ++i) {
    yCoordinates[i] =
        yCoordinates[i - 1] + levelSize[i] / 2.0f + levelSize[i - 1] / 2.0f + spaceBetweenLevels;
  }
}
//===============================================================
double ConeTreeExtended::treePlace3D(tlp::node n, flat_hash_map<tlp::node, double> *posRelX,
                                     flat_hash_map<tlp::node, double> *posRelY) {
  (*posRelX)[n] = 0;
  (*posRelY)[n] = 0;

  auto outdeg = tree->outdeg(n);
  if (outdeg == 0) {
    const Coord &tmp = Coord(nodeSize->getNodeValue(n));
    return sqrt(tmp[0] * tmp[0] + tmp[2] * tmp[2]) / 2.0;
  }

  if (outdeg == 1) {
    Iterator<node> *itN = tree->getOutNodes(n);
    node itn = itN->next();
    delete itN;
    return treePlace3D(itn, posRelX, posRelY);
  }

  double sumRadius = 0;
  double maxRadius = 0;
  float newRadius;

  vector<double> subCircleRadius(outdeg);
  Iterator<node> *itN = tree->getOutNodes(n);

  for (int i = 0; itN->hasNext(); ++i) {
    node itn = itN->next();
    subCircleRadius[i] = treePlace3D(itn, posRelX, posRelY);
    sumRadius += 2 * subCircleRadius[i];
    maxRadius = std::max(maxRadius, subCircleRadius[i]);
  }

  delete itN;

  double radius = sumRadius / (2 * M_PI);

  // treat angles
  vector<double> vangles(subCircleRadius.size());
  double angle = 0;
  vangles[0] = 0;

  for (uint i = 1; i < subCircleRadius.size(); ++i) {
    angle += (subCircleRadius[i - 1] + subCircleRadius[i]) / radius;
    vangles[i] = angle;
  }

  // compute new radius
  newRadius = 0;

  for (uint i = 0; i < subCircleRadius.size() - 1; ++i) {
    for (uint j = i + 1; j < subCircleRadius.size(); ++j) {
      newRadius = std::max(newRadius, minRadius(float(subCircleRadius[i]), float(vangles[i]),
                                                float(subCircleRadius[j]), float(vangles[j])));
    }
  }

  if (newRadius == 0) {
    newRadius = float(radius);
  }

  // compute Circle Hull
  vector<Circlef> circles(subCircleRadius.size());

  for (uint i = 0; i < subCircleRadius.size(); ++i) {
    circles[i][0] = newRadius * float(cos(vangles[i]));
    circles[i][1] = newRadius * float(sin(vangles[i]));
    circles[i].radius = float(subCircleRadius[i]);
  }

  tlp::Circlef circleH = tlp::enclosingCircle(circles);

  // Place relative position
  itN = tree->getOutNodes(n);

  for (uint i = 0; i < subCircleRadius.size(); ++i) {
    node itn = itN->next();
    (*posRelX)[itn] = newRadius * cos(vangles[i]) - circleH[0];
    (*posRelY)[itn] = newRadius * sin(vangles[i]) - circleH[1];
  }

  delete itN;
  return circleH.radius;
}
//===============================================================
void ConeTreeExtended::calcLayout(tlp::node n, flat_hash_map<tlp::node, double> *px,
                                  flat_hash_map<tlp::node, double> *py, double x, double y,
                                  int level) {
  result->setNodeValue(
      n, Coord(float(x + (*px)[n]), -float(yCoordinates[level]), float(y + (*py)[n])));
  for (auto itn : tree->getOutNodes(n)) {
    calcLayout(itn, px, py, x + (*px)[n], y + (*py)[n], level + 1);
  }
}
//===============================================================
static constexpr std::string_view paramHelp[] = {
    // orientation
    "This parameter enables to choose the orientation of the drawing.",

    // space between levels
    "This parameter enables to add extra spacing between the different levels of the tree"};
#define ORIENTATION "vertical;horizontal;"
//===============================================================
ConeTreeExtended::ConeTreeExtended(const tlp::PluginContext *context)
    : LayoutAlgorithm(context), spaceBetweenLevels(10) {
  addNodeSizePropertyParameter(this);
  addInParameter<StringCollection>("orientation", paramHelp[0].data(), ORIENTATION, true,
                                   "<b>vertical</b> <br> <b>horizontal</b>");
  addInParameter<float>("space between levels", paramHelp[1].data(), "1.0");
}
//===============================================================
ConeTreeExtended::~ConeTreeExtended() = default;
//===============================================================
bool ConeTreeExtended::run() {
  nodeSize = nullptr;
  string orientation = "vertical";

  if (dataSet != nullptr) {
    getNodeSizePropertyParameter(dataSet, nodeSize);
    StringCollection tmp;

    if (dataSet->get("orientation", tmp)) {
      orientation = tmp.getCurrentString();
    }

    dataSet->get("space between levels", spaceBetweenLevels);
    // no negative spacing
    spaceBetweenLevels = std::max(spaceBetweenLevels, 0.f);
  }

  if (!nodeSize) {
    nodeSize = graph->getSizeProperty("viewSize");
  }

  //=========================================================
  // rotate size if needed
  // will be undone at then end
  if (orientation == "horizontal") {
    for (auto n : graph->nodes()) {
      const Size &tmp = nodeSize->getNodeValue(n);
      nodeSize->setNodeValue(n, Size(tmp[1], tmp[0], tmp[2]));
    }
  }

  //===========================================================
  result->setAllEdgeValue(vector<Coord>(0));

  if (pluginProgress) {
    pluginProgress->showPreview(false);
  }

  tree = TreeTest::computeTree(graph, pluginProgress);

  if (pluginProgress && pluginProgress->state() != ProgressState::TLP_CONTINUE) {
    TreeTest::cleanComputedTree(graph, tree);
    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  }

  node root = tree->getSource();
  assert(root.isValid());
  flat_hash_map<node, double> posX;
  flat_hash_map<node, double> posY;
  treePlace3D(root, &posX, &posY);
  computeYCoodinates(root);
  calcLayout(root, &posX, &posY, 0, 0, 0);

  // rotate layout and size
  if (orientation == "horizontal") {
    for (auto n : graph->nodes()) {
      // if not in talipot gui, ensure cleanup
      LayoutProperty *elementLayout;

      if (!graph->getAttribute("viewLayout", elementLayout)) {
        const Size &tmp = nodeSize->getNodeValue(n);
        nodeSize->setNodeValue(n, Size(tmp[1], tmp[0], tmp[2]));
      }

      const Coord &tmpC = result->getNodeValue(n);

      result->setNodeValue(n, Coord(-tmpC[1], tmpC[0], tmpC[2]));
    }
  }

  TreeTest::cleanComputedTree(graph, tree);

  return true;
}
