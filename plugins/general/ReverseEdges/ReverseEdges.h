/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef REVERSE_EDGES_H
#define REVERSE_EDGES_H

#include <talipot/Algorithm.h>

class ReverseEdges : public tlp::Algorithm {
  public:
    PLUGININFORMATION(
        "Reverse edges", "Ludwig Fiolka", "10/10/2012",
        "Reverse selected edges of the graph (or all if no selection property is given).", "0.1",
        "Topology Update")

    ReverseEdges(tlp::PluginContext *context);

    bool run() override;
};

#endif // REVERSE_EDGES_H
