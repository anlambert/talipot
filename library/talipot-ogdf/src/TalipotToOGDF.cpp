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

#include <talipot/TalipotToOGDF.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/NumericProperty.h>
#include <talipot/SimpleTest.h>

using namespace std;
using namespace tlp;

TalipotToOGDF::TalipotToOGDF(Graph *g, bool importEdgeBends) : talipotGraph(g) {

  // needed to initialize some ogdfAttributes fields
  long attributes =
      // x, y, width, height fields
      ogdf::GraphAttributes::nodeGraphics |
      // bends field
      ogdf::GraphAttributes::edgeGraphics |
      // doubleWeight field
      ogdf::GraphAttributes::edgeDoubleWeight |
      // weight field
      ogdf::GraphAttributes::nodeWeight |
      // z coordinate
      ogdf::GraphAttributes::threeD;

  ogdfGraphAttributes = ogdf::GraphAttributes(ogdfGraph, attributes);

  SizeProperty *sizeProp = talipotGraph->getSizeProperty("viewSize");
  LayoutProperty *layoutProp = talipotGraph->getLayoutProperty("viewLayout");

  for (auto nTlp : talipotGraph->nodes()) {
    ogdf::node nOGDF = ogdfGraph.newNode();
    ogdfNodes[nTlp] = nOGDF;
    const Coord &c = layoutProp->getNodeValue(nTlp);
    ogdfGraphAttributes.x(nOGDF) = c.getX();
    ogdfGraphAttributes.y(nOGDF) = c.getY();
    ogdfGraphAttributes.z(nOGDF) = c.getZ();
    const Size &s = sizeProp->getNodeValue(nTlp);
    ogdfGraphAttributes.width(nOGDF) = s.getW();
    ogdfGraphAttributes.height(nOGDF) = s.getH();
  }

  for (auto eTlp : talipotGraph->edges()) {
    const auto &[srcTlp, tgtTlp] = talipotGraph->ends(eTlp);
    ogdf::edge eOGDF = ogdfGraph.newEdge(ogdfNodes[srcTlp], ogdfNodes[tgtTlp]);
    ogdfEdges[eTlp] = eOGDF;

    if (importEdgeBends) {

      const vector<Coord> &v = layoutProp->getEdgeValue(eTlp);
      ogdf::DPolyline bends;

      for (const auto &coord : v) {
        bends.pushBack(ogdf::DPoint(coord.getX(), coord.getY()));
      }

      ogdfGraphAttributes.bends(eOGDF) = bends;
    }

    ogdfGraphAttributes.doubleWeight(eOGDF) = 1.0;
  }
}

Coord TalipotToOGDF::getNodeCoordFromOGDFGraphAttr(node nTlp) {
  ogdf::node n = ogdfNodes[nTlp];

  double x = ogdfGraphAttributes.x(n);
  double y = ogdfGraphAttributes.y(n);
  double z = ogdfGraphAttributes.has(ogdf::GraphAttributes::threeD) ? ogdfGraphAttributes.z(n) : 0;

  return Coord(x, y, z);
}

vector<Coord> TalipotToOGDF::getEdgeCoordFromOGDFGraphAttr(edge eTlp) {
  vector<Coord> v;
  if (ogdfEdges.contains(eTlp)) {

    ogdf::edge e = ogdfEdges[eTlp];

    for (const auto &bend : ogdfGraphAttributes.bends(e)) {
      v.push_back(Coord(bend.m_x, bend.m_y));
    }
  }
  return v;
}

void TalipotToOGDF::copyTlpNumericPropertyToOGDFEdgeLength(NumericProperty *metric) {
  if (!metric) {
    return;
  }

  for (auto eTlp : talipotGraph->edges()) {
    ogdfGraphAttributes.doubleWeight(ogdfEdges[eTlp]) = metric->getEdgeDoubleValue(eTlp);
  }
}

void TalipotToOGDF::copyTlpNodeSizeToOGDF(SizeProperty *size) {
  if (!size) {
    return;
  }

  for (auto nTlp : talipotGraph->nodes()) {
    const Size &s = size->getNodeValue(nTlp);
    ogdf::node nOGDF = ogdfNodes[nTlp];
    ogdfGraphAttributes.width(nOGDF) = s.getW();
    ogdfGraphAttributes.height(nOGDF) = s.getH();
  }

  for (auto eTlp : talipotGraph->edges()) {
    const auto &[srcTlp, tgtTlp] = talipotGraph->ends(eTlp);
    ogdf::node srcOGDF = ogdfNodes[srcTlp];
    const Size &sSrc = size->getNodeValue(srcTlp);
    ogdf::node tgtOGDF = ogdfNodes[tgtTlp];
    const Size &sTgt = size->getNodeValue(tgtTlp);

    ogdf::edge eOGDF = ogdfEdges[eTlp];
    ogdfGraphAttributes.doubleWeight(eOGDF) =
        ogdfGraphAttributes.doubleWeight(eOGDF) + sSrc.getW() / 2. + sTgt.getW() / 2. - 1.;
  }
}

void TalipotToOGDF::copyTlpNumericPropertyToOGDFNodeWeight(NumericProperty *metric) {
  if (!metric) {
    return;
  }

  for (auto nTlp : talipotGraph->nodes()) {
    ogdfGraphAttributes.weight(ogdfNodes[nTlp]) = int(metric->getNodeDoubleValue(nTlp));
  }
}

void TalipotToOGDF::makeOGDFGraphSimple() {
  auto [loops, parallelEdges] = SimpleTest::getLoopsAndParallelEdges(talipotGraph);

  for (auto e : loops) {
    ogdfGraph.delEdge(ogdfEdges[e]);
    ogdfEdges.erase(e);
  }

  for (auto e : parallelEdges) {
    auto [src, tgt] = talipotGraph->ends(e);
    auto ee = talipotGraph->existEdge(src, tgt, false);
    // an edge can be a loop and a parallel one
    if (ogdfEdges.contains(e)) {
      ogdfGraph.delEdge(ogdfEdges[e]);
      ogdfEdges[e] = ogdfEdges[ee];
    }
  }
}

void TalipotToOGDF::enable3DLayout(bool enable3D) {
  if (enable3D) {
    ogdfGraphAttributes.addAttributes(ogdf::GraphAttributes::threeD);
  } else {
    ogdfGraphAttributes.destroyAttributes(ogdf::GraphAttributes::threeD);
  }
}