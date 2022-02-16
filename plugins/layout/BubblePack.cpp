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
#include <talipot/PluginHeaders.h>

class BubblePack : public tlp::LayoutAlgorithm {
public:
  PLUGININFORMATION("Bubble Pack", "D.Auber", "01/10/2012", "Stable", "1.0", "Tree")
  BubblePack(const tlp::PluginContext *context);
  ~BubblePack() override;
  bool run() override;

private:
  double computeRelativePosition(tlp::node n,
                                 tlp::NodeVectorProperty<tlp::Vec4f> &relativePosition);
  void calcLayout(tlp::node n, tlp::Vec2f pos,
                  tlp::NodeVectorProperty<tlp::Vec4f> &relativePosition);

  tlp::Graph *tree;
  tlp::SizeProperty *nodeSize;
  bool nAlgo;
};

PLUGIN(BubblePack)

using namespace std;
using namespace tlp;

struct greaterRadius {
  const std::vector<double> &radius;
  greaterRadius(const std::vector<double> &r) : radius(r) {}
  bool operator()(unsigned i1, unsigned i2) const {
    return radius[i1] > radius[i2];
  }
};

struct lessRadius {
  const std::vector<double> &radius;
  lessRadius(const std::vector<double> &r) : radius(r) {}
  bool operator()(unsigned i1, unsigned i2) const {
    return radius[i1] < radius[i2];
  }
};

double BubblePack::computeRelativePosition(tlp::node n,
                                           NodeVectorProperty<Vec4f> &relativePosition) {

  Size centralNodeSize = nodeSize->getNodeValue(n);
  centralNodeSize[2] = 0.; // remove z-coordinates because the drawing is 2D
  double sizeFather = std::max(centralNodeSize[0], centralNodeSize[1]) / 2.;

  if (sizeFather < 1E-5) {
    sizeFather = 0.1;
  }

  uint outdeg = tree->outdeg(n);

  /**
   * Special case if the node is a leaf.
   */
  if (outdeg == 0) {
    return sizeFather + 1.; // minimum spacing
  }

  /**
   * Recursive call to obtain the set of radius of the children of n
   */
  std::vector<double> realCircleRadius(outdeg);

  int i = 0;
  for (auto ni : tree->getOutNodes(n)) {
    realCircleRadius[i] = computeRelativePosition(ni, relativePosition);
    ++i;
  }

  /**
   * Pack Circles
   */
  vector<Circled> circles(outdeg);
  double angle = 2. * M_PI; // start position
  double bestAngle = angle;

  {
    std::vector<unsigned> index(outdeg);

    for (uint i = 0; i < outdeg; ++i) {
      index[i] = i;
    }

    sort(index.begin(), index.end(), lessRadius(realCircleRadius));

    vector<Circled> placed;

    if (index.size() > 3) {
      double alpha = 0;
      double curRad = sizeFather;
      bool sens = true;

      for (uint i = 0; i < index.size(); ++i) {
        double radius = realCircleRadius[index[i]];
        double crad = radius + curRad + 0.01;
        double calpha;

        if (sens) {
          calpha = alpha + radius / crad;
        } else {
          calpha = alpha - radius / crad;
        }

        Circled tmp(crad * cos(calpha), crad * sin(calpha), radius);
        bool reject = false;

        for (const auto &circle : placed) {
          if (circle.dist(tmp) < circle.radius + circle.radius) {
            reject = true;
            break;
          }
        }

        if (reject) {
          curRad += radius + 0.01;
          sens = !sens;
          --i;
          continue;
        }

        double newalpha;

        if (sens) {
          newalpha = alpha + 2.2 * radius / crad;
        } else {
          newalpha = alpha - 2.2 * radius / crad;
        }

        Vec2f v0(crad * cos(calpha), crad * sin(calpha));
        Vec2f v1(crad * cos(newalpha), crad * sin(newalpha));

        while (v0.dist(v1) < radius) {
          if (sens) {
            newalpha += 0.01;
          } else {
            newalpha -= 0.01;
          }

          v1 = Vec2f(crad * cos(newalpha), crad * sin(newalpha));
        }

        alpha = newalpha;
        circles[index[i]] = tmp;
        placed.push_back(circles[index[i]]);
      }
    } else { //        if (false) //polyo packing
      for (uint i : index) {
        double radius = realCircleRadius[i];
        double bestRadius = FLT_MAX;
        int discret = ceil(2. * (sizeFather + radius) * M_PI) + 3;
        angle += M_PI / 3.;

        TLP_PARALLEL_MAP_INDICES(discret, [&](uint j) {
          float _angle = float(j) * 2. * M_PI / float(discret) + angle;
          double spiralRadius = sizeFather + radius + 1E-3;
          Circled tmp(spiralRadius * cos(_angle), spiralRadius * sin(_angle), radius);
          bool restart = true;

          // int restcnt = 0;
          while (restart) {
            // restcnt += 1;
            restart = false;

            for (const auto &circle : placed) {
              if (circle.dist(tmp) < circle.radius + tmp.radius) {
                spiralRadius =
                    std::max(spiralRadius, double(circle.norm()) + circle.radius + radius + 1E-3);
                // spiralRadius += 0.01;
                tmp = Circled(spiralRadius * cos(_angle), spiralRadius * sin(_angle), radius);
                // restart = true;
              }
            }
          }

          TLP_LOCK_SECTION(GOODCIRCLE) {
            if (spiralRadius < bestRadius) {
              bestRadius = spiralRadius;
              bestAngle = _angle;
            }
          }
          TLP_UNLOCK_SECTION(GOODCIRCLE);
        });

        circles[i][0] = bestRadius * cos(bestAngle);
        circles[i][1] = bestRadius * sin(bestAngle);
        circles[i].radius = radius;
        placed.push_back(circles[i]);
      }
    }
  }
  circles.push_back(Circled(0., 0., sizeFather));
  Circled circleH;

  if (circles.size() > 2000) { // Stack overflow when number of circles exceed 2k
    circleH = tlp::lazyEnclosingCircle(circles);
  } else {
    circleH = tlp::enclosingCircle(circles);
  }

  /*
   * Set relative position of all children
   * according to the center of the enclosing circle
   */
  Iterator<node> *itN = tree->getOutNodes(n);

  for (uint i = 0; i < outdeg; ++i) {
    Vec4f &relPos = relativePosition[itN->next()];
    Circled &circle = circles[i];
    relPos[0] = circle[0] - circleH[0];
    relPos[1] = circle[1] - circleH[1];
  }

  delete itN;

  Vec4f &relPos = relativePosition[n];
  relPos[2] = -circleH[0];
  relPos[3] = -circleH[1];

  return circleH.radius + 1.;
}

void BubblePack::calcLayout(tlp::node n, Vec2f pos, NodeVectorProperty<Vec4f> &relativePosition) {
  /*
   * Make the recursive call, to place the children of n.
   */
  Vec4f &relPos = relativePosition[n];
  Vec2f shift(relPos[2], relPos[3]);
  result->setNodeValue(n, Coord(pos + shift, 0));
  for (auto ni : tree->getOutNodes(n)) {
    Vec4f &relPos = relativePosition[ni];
    Vec2f relat(relPos[0], relPos[1]);
    calcLayout(ni, pos + relat, relativePosition);
  }
}

static constexpr std::string_view paramHelp[] = {
    // complexity
    "This parameter enables to choose the complexity of the algorithm, true = o(nlog(n)) / false = "
    "o(n)",

    // node size
    "This parameter defines the property used for node's sizes."};

BubblePack::BubblePack(const tlp::PluginContext *context) : LayoutAlgorithm(context) {
  addInParameter<bool>("complexity", paramHelp[0].data(), "true");
  addInParameter<SizeProperty>("node size", paramHelp[1].data(), "viewSize");
  addDependency("Connected Components Packing", "1.0");
}

BubblePack::~BubblePack() = default;

bool BubblePack::run() {
  if (pluginProgress) {
    pluginProgress->showPreview(false);
  }

  if (!ConnectedTest::isConnected(graph)) {
    // for each component draw
    string err;
    auto components = ConnectedTest::computeConnectedComponents(graph);

    for (const auto &component : components) {
      Graph *tmp = graph;
      // apply "Bubble Pack" on the subgraph induced
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

  if (!((dataSet != nullptr) && dataSet->get("node size", nodeSize))) {
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

  tree = TreeTest::computeTree(graph, pluginProgress);

  if (pluginProgress && pluginProgress->state() != ProgressState::TLP_CONTINUE) {
    TreeTest::cleanComputedTree(graph, tree);
    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  }

  node startNode = tree->getSource();
  assert(startNode.isValid());
  NodeVectorProperty<Vec4f> relativePosition(graph);
  computeRelativePosition(startNode, relativePosition);
  calcLayout(startNode, Vec2f(0, 0), relativePosition);

  TreeTest::cleanComputedTree(graph, tree);

  return true;
}
