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

#include "DegreeMetric.h"

#include <talipot/StringCollection.h>
#include <talipot/GraphMeasure.h>

PLUGIN(DegreeMetric)

using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // type
    "Type of degree to compute (in/out/inout).",

    // metric
    "The weighted degree of a node is the sum of weights of "
    "all its in/out/inout edges. "
    "If no metric is specified, using a uniform metric value of 1 for all edges "
    "returns the usual degree for nodes (number of neighbors).",

    // norm
    "If true, the measure is normalized in the following way."
    "<ul><li>Unweighted case: m(n) = deg(n) / (#V - 1)</li> "
    "<li>Weighted case: m(n) = deg_w(n) / [(sum(e_w)/#E)(#V - 1)] </li></ul>"

};

#define DEGREE_TYPE "type"
#define DEGREE_TYPES "InOut;In;Out;"
//==============================================================================
DegreeMetric::DegreeMetric(const tlp::PluginContext *context) : DoubleAlgorithm(context) {
  addInParameter<StringCollection>(DEGREE_TYPE, paramHelp[0].data(), DEGREE_TYPES, true,
                                   "<b>InOut</b> <br> <b>In</b> <br> <b>Out</b>");
  addInParameter<NumericProperty *>("metric", paramHelp[1].data(), "", false);
  addInParameter<bool>("norm", paramHelp[2].data(), "false", false);
}
//==================================================================
bool DegreeMetric::run() {
  StringCollection degreeTypes(DEGREE_TYPES);
  degreeTypes.setCurrent(0);
  NumericProperty *weights = nullptr;
  bool norm = false;

  if (dataSet != nullptr) {
    dataSet->get(DEGREE_TYPE, degreeTypes);
    dataSet->get("metric", weights);
    dataSet->get("norm", norm);
  }

  NodeVectorProperty<double> deg(graph);
  degree(graph, deg, static_cast<EdgeType>(degreeTypes.getCurrent()), weights, norm);
  deg.copyToProperty(result);

  return true;
}
//==================================================================
bool DegreeMetric::check(std::string &errorMsg) {
  // check weights validity if it exists
  DoubleProperty *weights = nullptr;

  if (dataSet != nullptr) {
    dataSet->get("metric", weights);

    if (weights && !weights->getEdgeDefaultValue() && !weights->hasNonDefaultValuatedEdges()) {
      errorMsg = "Cannot compute a weighted degree with a null weight value\nfor all edges";
      return false;
    }
  }

  return true;
}
