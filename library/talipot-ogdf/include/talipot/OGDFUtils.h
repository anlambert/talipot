/**
 *
 * Copyright (C) 2023-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_OGDF_UTILS_H
#define TALIPOT_OGDF_UTILS_H

#include <ogdf/basic/Graph.h>
#include <talipot/Graph.h>

TLP_OGDF_SCOPE tlp::Graph *convertOGDFGraphToTalipotGraph(ogdf::Graph &graph,
                                                          tlp::Graph *tlpGraph = nullptr);

template <typename T>
ogdf::Array<T> vectorToOGDFArray(const std::vector<T> &v) {
  auto a = ogdf::Array<T>(v.size());
  std::copy(v.begin(), v.end(), a.begin());
  return a;
}

#endif // TALIPOT_OGDF_UTILS_H
