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

#include <talipot/StlFunctions.h>
#include <talipot/DoubleProperty.h>

bool tlp::LessByMetric::operator()(tlp::node n1, tlp::node n2) {
  return (*metric)[n1] < (*metric)[n2];
}
bool tlp::LessByMetric::operator()(tlp::edge e1, tlp::edge e2) {
  return (*metric)[e1] < (*metric)[e2];
}
