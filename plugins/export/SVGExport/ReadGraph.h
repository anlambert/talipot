
#ifndef READ_GRAPH_H
#define READ_GRAPH_H

/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

class ExportInterface;

namespace tlp {
class Graph;
class DataSet;
class PluginProgress;
} // namespace tlp

// Director
struct ReadGraph {
  static bool readGraph(tlp::Graph *graph, tlp::DataSet *ds, tlp::PluginProgress *pp,
                        ExportInterface &exportint);
};

#endif // READ_GRAPH_H
