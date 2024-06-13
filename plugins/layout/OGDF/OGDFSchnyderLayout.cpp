/**
 *
 * Copyright (C) 2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/planarlayout/SchnyderLayout.h>

#include <talipot/StringCollection.h>

#include "OGDFPlanarLayoutBase.h"

#define ELT_COMBINATORIAL_OBJECTS "combinatorial objects"
#define ELT_COMBINATORIAL_OBJECTS_LIST "VerticesMinusDepth;Faces"
static const std::vector<ogdf::SchnyderLayout::CombinatorialObjects> combinatorialObjects = {
    ogdf::SchnyderLayout::CombinatorialObjects::VerticesMinusDepth,
    ogdf::SchnyderLayout::CombinatorialObjects::Faces};

static constexpr std::string_view paramHelp[] = {
    // combinatorial objects
    "Each node in a Schnyder wood splits the graph into three regions. The barycentric coordinates "
    "of the nodes are given by the count of combinatorial objects in these regions.",
};

static const char *combinatorialObjectsValuesDescription =
    "<b>VerticesMinusDepth</b> <i>(Count the number of vertices in each region i and subtract "
    "the depth of the (i-1)-path of the node. The grid layout size is (n - 2) × (n - "
    "2).)</i><br>"
    "<b>Faces</b> <i>(Count the number of faces in each region i. The grid layout size is (2n - 5) "
    "× (2n - 5).)</i><br>";

class OGDFSchnyderLayout : public OGDFPlanarLayoutBase {

public:
  PLUGININFORMATION(
      "Schnyder (OGDF)", "Antoine Lambert", "O6/2024",
      "This algorithm draws a planar graph G straight-line without crossings. G (with |V| ≥ 3) "
      "must not contain self-loops or multiple edges. The algorithm runs in three phases. In the "
      "first phase, the graph is augmented by adding new artificial edges to get a triangulated "
      "plane graph. Then, a partition of the set of interior edges in three trees (also called "
      "Schnyder trees) with special orientation properties is derived. In the third step, the "
      "actual coordinates are computed.",
      "1.0", "Planar")
  OGDFSchnyderLayout(const tlp::PluginContext *context)
      : OGDFPlanarLayoutBase(context, tlp::getOGDFLayoutModule<ogdf::SchnyderLayout>(context)),
        schnyderLayout(static_cast<ogdf::SchnyderLayout *>(ogdfLayoutAlgo)) {
    addInParameter<tlp::StringCollection>(ELT_COMBINATORIAL_OBJECTS, paramHelp[0].data(),
                                          ELT_COMBINATORIAL_OBJECTS_LIST, true,
                                          combinatorialObjectsValuesDescription);
  }

  void beforeCall() override {
    tlp::StringCollection sc;
    if (dataSet->get(ELT_COMBINATORIAL_OBJECTS, sc)) {
      schnyderLayout->setCombinatorialObjects(combinatorialObjects[sc.getCurrent()]);
    }
    tlpToOGDF->makeOGDFGraphSimple();
  }

private:
  ogdf::SchnyderLayout *schnyderLayout;
};

PLUGIN(OGDFSchnyderLayout)
