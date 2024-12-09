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

// This plugin is an implementation of the connected component packing
// algorithm published as:
//
// Freivalds Karlis, Dogrusoz Ugur and Kikusts Paulis, \n
// "Disconnected Graph Layout and the Polyomino Packing Approach", \n
// In Proceeding GD '01 Revised Papers from the 9th International Symposium on Graph Drawing, \n
//
// The implementation is freely inspired from the one that can be found in the GraphViz project
// (http://www.graphviz.org/).
//
// Author: Antoine LAMBERT, University of Bordeaux 1, FR: Email: antoine.lambert@labri.fr
// Version 1.0: May 2011

#include <talipot/DrawingTools.h>
#include <talipot/ParametricCurves.h>
#include <talipot/ViewSettings.h>

#include "DatasetTools.h"

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // coordinates
    "Input layout of nodes and edges.",

    // rotation
    "Input rotation of nodes on z-axis",

    // margin
    "The minimum margin between each pair of nodes in the resulting packed layout.",

    // increment
    "The polyomino packing tries to find a place where the next polyomino will fit by following a "
    "square."
    "If there is no place where the polyomino fits, the square gets bigger and every place gets "
    "tried again."};

struct Polyomino {
  std::vector<node> *ccNodes;    // the connected nodes associated to that polyomino
  int perim;                     // the perimeter value of the polyomino
  std::vector<tlp::Vec2i> cells; // the cells of the grid representing the polyomino
  tlp::BoundingBox ccBB;         // the bounding box of the connected nodes
  tlp::Vec2i newPlace;

  Polyomino(std::vector<node> *nodes, tlp::BoundingBox &bb) : ccNodes(nodes), perim(0), ccBB(bb) {}
};

class PolyominoPacking : public tlp::LayoutAlgorithm {

public:
  PLUGININFORMATION(
      "Connected Components Packing (Polyomino)", "Antoine Lambert", "05/05/11",
      "Implements the connected component packing algorithm published as:<br/>"
      "<b>Disconnected Graph Layout and the Polyomino Packing Approach</b>, Freivalds Karlis, "
      "Dogrusoz Ugur and Kikusts Paulis, "
      "Graph Drawing '01 Revised Papers from the 9th International Symposium on Graph Drawing.",
      "1.0", "Misc")
  PolyominoPacking(const tlp::PluginContext *context);

  ~PolyominoPacking() override;

  bool run() override;

private:
  int computeGridStep();
  void genPolyomino(Polyomino &poly, LayoutProperty *layout, SizeProperty *size);
  void fillEdge(tlp::edge e, const tlp::Vec2i &p, std::vector<tlp::Vec2i> &cells, int dx, int dy,
                LayoutProperty *layout);
  void fillLine(const tlp::Coord &p, const tlp::Coord &q, std::vector<tlp::Vec2i> &cells);
  bool polyominoFits(Polyomino &info, int x, int y);
  void placePolyomino(int i, Polyomino &info);

  uint margin;
  uint bndIncrement;

  std::vector<Polyomino> polyominos;

  int gridStepSize;

  flat_hash_map<tlp::Vec2i, bool> pointsSet;

  tlp::IntegerProperty *shape;
};

#define C 100

class polyPerimOrdering {

public:
  polyPerimOrdering() = default;

  bool operator()(const Polyomino &ci1, const Polyomino &ci2) const {
    return ci1.perim > ci2.perim;
  }
};

PLUGIN(PolyominoPacking)

PolyominoPacking::PolyominoPacking(const PluginContext *context) : LayoutAlgorithm(context) {
  addInParameter<LayoutProperty>("coordinates", paramHelp[0].data(), "viewLayout");
  addNodeSizePropertyParameter(this);
  addInParameter<DoubleProperty>("rotation", paramHelp[1].data(), "viewRotation");
  addInParameter<uint>("margin", paramHelp[2].data(), "1");
  addInParameter<uint>("increment", paramHelp[3].data(), "1");
}

PolyominoPacking::~PolyominoPacking() = default;

bool PolyominoPacking::run() {

  LayoutProperty *layout = nullptr;
  SizeProperty *size = nullptr;
  DoubleProperty *rotation = nullptr;
  margin = 1;
  bndIncrement = 1;

  if (dataSet != nullptr) {
    dataSet->get("coordinates", layout);
    getNodeSizePropertyParameter(dataSet, size);
    dataSet->get("rotation", rotation);
    dataSet->get("margin", margin);
    dataSet->get("increment", bndIncrement);
  }

  if (pluginProgress) {
    pluginProgress->setComment("Computing connected components ...");
  }

  auto connectedComponents = tlp::ConnectedTest::computeConnectedComponents(graph);

  if (connectedComponents.size() <= 1) {
    for (auto n : graph->nodes()) {
      result->setNodeValue(n, layout->getNodeValue(n));
    }
    for (auto e : graph->edges()) {
      result->setEdgeValue(e, layout->getEdgeValue(e));
    }
    return true;
  }

  shape = graph->getIntegerProperty("viewShape");

  polyominos.reserve(connectedComponents.size());

  for (size_t i = 0; i < connectedComponents.size(); ++i) {
    std::vector<node> &ccNodes = connectedComponents[i];
    uint nbNodes = ccNodes.size();
    std::vector<edge> ccEdges;

    // get edges of current connected component
    for (uint j = 0; j < nbNodes; ++j) {
      for (auto e : graph->getOutEdges(ccNodes[j])) {
        ccEdges.push_back(e);
      }
    }

    BoundingBox ccBB = tlp::computeBoundingBox(graph, ccNodes, ccEdges, layout, size, rotation);
    polyominos.push_back(Polyomino(&ccNodes, ccBB));

    if (pluginProgress && (pluginProgress->progress(i + 1, connectedComponents.size()) !=
                           ProgressState::TLP_CONTINUE)) {
      return pluginProgress->state() != ProgressState::TLP_CANCEL;
    }
  }

  gridStepSize = computeGridStep();

  if (gridStepSize <= 0) {
    return true;
  }

  if (pluginProgress) {
    pluginProgress->setComment("Generating polyominos ...");
    if (pluginProgress->progress(0, polyominos.size()) != ProgressState::TLP_CONTINUE) {
      return pluginProgress->state() != ProgressState::TLP_CANCEL;
    }
  }

  for (size_t i = 0; i < polyominos.size(); ++i) {
    genPolyomino(polyominos[i], layout, size);

    if (pluginProgress &&
        (pluginProgress->progress(i + 1, polyominos.size()) != ProgressState::TLP_CONTINUE)) {
      return pluginProgress->state() != ProgressState::TLP_CANCEL;
    }
  }

  std::sort(polyominos.begin(), polyominos.end(), polyPerimOrdering());

  if (pluginProgress) {
    pluginProgress->setComment("Packing polyominos ...");
    if (pluginProgress->progress(0, polyominos.size()) != ProgressState::TLP_CONTINUE) {
      return pluginProgress->state() != ProgressState::TLP_CANCEL;
    }
  }

  for (size_t i = 0; i < polyominos.size(); ++i) {
    placePolyomino(i, polyominos[i]);

    if (pluginProgress &&
        (pluginProgress->progress(i + 1, polyominos.size()) != ProgressState::TLP_CONTINUE)) {
      return pluginProgress->state() != ProgressState::TLP_CANCEL;
    }
  }

  for (const auto &poly : polyominos) {
    Coord move = Coord(poly.newPlace[0], poly.newPlace[1]);
    const std::vector<node> &ccNodes = *poly.ccNodes;
    uint nbNodes = ccNodes.size();

    for (uint j = 0; j < nbNodes; ++j) {
      node n = ccNodes[j];
      result->setNodeValue(n, layout->getNodeValue(n) + move);
      for (auto e : graph->getOutEdges(n)) {
        const vector<Coord> &bends = layout->getEdgeValue(e);

        if (!bends.empty()) {
          vector<Coord> newBends(bends);
          for (auto &coord : newBends) {
            coord += move;
          }

          result->setEdgeValue(e, newBends);
        }
      }
    }
  }

  return true;
}

int PolyominoPacking::computeGridStep() {
  double a = C * polyominos.size() - 1.0;
  double b = 0.0;
  double c = 0.0;

  for (const auto &polyomino : polyominos) {
    const BoundingBox &ccBB = polyomino.ccBB;
    double W = ccBB[1][0] - ccBB[0][0] + 2 * margin;
    double H = ccBB[1][1] - ccBB[0][1] + 2 * margin;
    b -= (W + H);
    c -= (W * H);
  }

  double d = b * b - 4.0 * a * c;

  if (d < 0) {
    return -1;
  }

  double r = sqrt(d);
  double l1 = (-b + r) / (2 * a);
  int root = int(l1);

  if (root == 0) {
    root = 1;
  }

  return root;
}

inline int grid(float x, int s) {
  return int(ceil(x / s));
}

template <typename T>
inline T cval(T val, int size) {
  return (val >= 0) ? (val / size) : (((val + 1) / size) - 1);
}

template <typename T>
static T cell(const T &p, int gridStep) {
  T ret;
  ret[0] = cval(p[0], gridStep);
  ret[1] = cval(p[1], gridStep);
  return ret;
}

inline Vec2i vec3fToVec2i(const Vec3f &c) {
  return Vec2i(int(rint(c[0])), int(rint(c[1])));
}

void PolyominoPacking::genPolyomino(Polyomino &poly, LayoutProperty *layout, SizeProperty *size) {

  const BoundingBox &ccBB = poly.ccBB;
  const std::vector<node> &ccNodes = *poly.ccNodes;

  int dx = -rint(ccBB[0][0]);
  int dy = -rint(ccBB[0][1]);

  uint nbNodes = ccNodes.size();

  for (uint i = 0; i < nbNodes; ++i) {
    node n = ccNodes[i];
    const Coord &nodeCoord = layout->getNodeValue(n);
    const Size &nodeSize = size->getNodeValue(n);
    Vec2i point = vec3fToVec2i(nodeCoord);
    point[0] += dx;
    point[1] += dy;
    Vec2i s2;
    s2[0] = margin + nodeSize[0] / 2;
    s2[1] = margin + nodeSize[1] / 2;
    Vec2i LL = point - s2;
    Vec2i UR = point + s2;
    LL = cell(LL, gridStepSize);
    UR = cell(UR, gridStepSize);

    for (int x = LL[0]; x <= UR[0]; ++x) {
      for (int y = LL[1]; y <= UR[1]; ++y) {
        Vec2i cellCoord;
        cellCoord[0] = x;
        cellCoord[1] = y;
        poly.cells.push_back(cellCoord);
      }
    }

    point = cell(point, gridStepSize);
    for (auto e : graph->getOutEdges(n)) {
      fillEdge(e, point, poly.cells, dx, dy, layout);
    }
  }

  int W = grid(ccBB[1][0] - ccBB[0][0] + 2 * margin, gridStepSize);
  int H = grid(ccBB[1][1] - ccBB[0][1] + 2 * margin, gridStepSize);
  poly.perim = W + H;
}

void PolyominoPacking::fillEdge(edge e, const Vec2i &p, std::vector<Vec2i> &cells, int dx, int dy,
                                LayoutProperty *layout) {

  Coord pf = Coord(p[0], p[1]);
  const auto &[src, tgt] = graph->ends(e);
  const Coord &srcCoord = layout->getNodeValue(src);
  Coord tgtCoord = layout->getNodeValue(tgt);
  const std::vector<Coord> &bends = layout->getEdgeValue(e);

  if (bends.empty()) {
    tgtCoord += Coord(dx, dy);
    tgtCoord = cell(tgtCoord, gridStepSize);
    fillLine(pf, tgtCoord, cells);
    return;
  }

  std::vector<Coord> newBends;
  auto eShape = shape->getEdgeValue(e);

  if (eShape == EdgeShape::Polyline) {
    newBends = bends;
  } else {
    vector<Coord> controlPoints;
    controlPoints.push_back(srcCoord);
    controlPoints.insert(controlPoints.end(), bends.begin(), bends.end());
    controlPoints.push_back(tgtCoord);
    if (eShape == EdgeShape::BezierCurve) {
      computeBezierPoints(controlPoints, newBends, 20);
    } else if (eShape == EdgeShape::CubicBSplineCurve) {
      if (controlPoints.size() > 3) {
        computeOpenUniformBsplinePoints(controlPoints, newBends, 3, 20);
      } else {
        newBends = controlPoints;
      }
    } else if (eShape == EdgeShape::CatmullRomCurve) {
      computeCatmullRomPoints(controlPoints, newBends, false, 20);
    }
    newBends.erase(newBends.begin());
    newBends.pop_back();
  }

  Coord curSrc = pf;

  for (auto &bend : newBends) {
    bend += Coord(dx, dy);
    bend = cell(bend, gridStepSize);
    fillLine(curSrc, bend, cells);
    curSrc = bend;
  }

  tgtCoord += Coord(dx, dy);
  tgtCoord = cell(tgtCoord, gridStepSize);
  fillLine(curSrc, tgtCoord, cells);
}

void PolyominoPacking::fillLine(const Coord &p, const Coord &q, std::vector<Vec2i> &cells) {
  int x1 = rint(p[0]);
  int y1 = rint(p[1]);
  int x2 = rint(q[0]);
  int y2 = rint(q[1]);

  int dx = x2 - x1;
  int ax = abs(dx) << 1;
  int sx = dx < 0 ? -1 : 1;
  int dy = y2 - y1;
  int ay = abs(dy) << 1;
  int sy = dy < 0 ? -1 : 1;

  int x = x1;
  int y = y1;

  if (ax > ay) {
    int d = ay - (ax >> 1);

    while (true) {
      Vec2i cell;
      cell[0] = x;
      cell[1] = y;
      cells.push_back(cell);

      if (x == x2) {
        return;
      }

      if (d >= 0) {
        y += sy;
        d -= ax;
      }

      x += sx;
      d += ay;
    }
  } else {
    int d = ax - (ay >> 1);

    while (true) {
      Vec2i cell;
      cell[0] = x;
      cell[1] = y;
      cells.push_back(cell);

      if (y == y2) {
        return;
      }

      if (d >= 0) {
        x += sx;
        d -= ay;
      }

      y += sy;
      d += ax;
    }
  }
}

bool PolyominoPacking::polyominoFits(Polyomino &poly, int x, int y) {
  std::vector<Vec2i> &cells = poly.cells;
  const BoundingBox &ccBB = poly.ccBB;

  for (auto cell : cells) {
    cell[0] += x;
    cell[1] += y;

    if (pointsSet.contains(cell)) {
      return false;
    }
  }

  Vec2i LL = vec3fToVec2i(ccBB[0]);
  poly.newPlace[0] = gridStepSize * x - LL[0];
  poly.newPlace[1] = gridStepSize * y - LL[1];

  for (auto cell : cells) {
    cell[0] += x;
    cell[1] += y;
    pointsSet[cell] = true;
  }

  return true;
}

void PolyominoPacking::placePolyomino(int i, Polyomino &poly) {
  int x = 0, y = 0;
  int W = 0, H = 0;

  const BoundingBox &ccBB = poly.ccBB;

  if (i == 0) {
    W = grid(ccBB[1][0] - ccBB[0][0] + 2 * margin, gridStepSize);
    H = grid(ccBB[1][1] - ccBB[0][1] + 2 * margin, gridStepSize);

    if (polyominoFits(poly, -W / 2, -H / 2)) {
      return;
    }
  }

  if (polyominoFits(poly, 0, 0)) {
    return;
  }

  W = ceil(ccBB[1][0] - ccBB[0][0]);
  H = ceil(ccBB[1][1] - ccBB[0][1]);

  if (W >= H) {
    for (int bnd = 1;; bnd += bndIncrement) {
      x = 0;
      y = -bnd;

      for (; x < bnd; ++x) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; y < bnd; ++y) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; x > -bnd; --x) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; y > -bnd; --y) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; x < 0; ++x) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }
    }
  } else {
    for (int bnd = 1;; bnd += bndIncrement) {
      y = 0;
      x = -bnd;

      for (; y > -bnd; --y) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; x < bnd; ++x) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; y < bnd; ++y) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; x > -bnd; --x) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }

      for (; y > 0; --y) {
        if (polyominoFits(poly, x, y)) {
          return;
        }
      }
    }
  }
}
