/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "PathFinderTools.h"

#include <talipot/GlGraphInputData.h>
#include <talipot/DrawingTools.h>

using namespace std;

namespace tlp {
// Computes the enclosing circle of the elements contained in a boolean property.
Circlef getEnclosingCircle(GlGraphInputData *inputData, BooleanProperty *selection) {
  BoundingBox bbox(computeBoundingBox(inputData->getGraph(), inputData->getElementLayout(),
                                      inputData->getElementSize(), inputData->getElementRotation(),
                                      selection));
  Coord center = bbox.center();
  float norm = (bbox[1] - bbox[0]).norm();
  Circlef result;
  result[0] = center.getX();
  result[1] = center.getY();
  result.radius = norm;
  return result;
}

bool getNodeEnclosingCircle(Circlef &circle, GlGraphInputData *inputData, node n) {
  auto *selection = new BooleanProperty(inputData->getGraph());
  selection->setAllNodeValue(false);
  selection->setNodeValue(n, true);
  circle = getEnclosingCircle(inputData, selection);
  return true;
}

bool getEdgeEnclosingCircle(Circlef &circle, GlGraphInputData *inputData, edge e) {
  auto *selection = new BooleanProperty(inputData->getGraph());
  selection->setAllEdgeValue(false);
  selection->setEdgeValue(e, true);

  if (inputData->getElementLayout()->getEdgeValue(e).empty()) {
    return false;
  }

  circle = getEnclosingCircle(inputData, selection);
  return true;
}
}
