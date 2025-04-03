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

#ifndef RANDOM_H
#define RANDOM_H

#include <talipot/PluginHeaders.h>

/// Random.h - An implementation of a random 3D layout.
/** This plugin is an implementation of a random 3D layout
 *
 *  <b>HISTORY</b>
 *
 *  01/12/99 Version 0.0.1: Initial release
 *
 *
 *  \note This algorithm works on general graphs. \n
 *  Let n be the number of nodes, the algorithm complexity is in O(n).
 *
 *
 *  \author David Auber University Bordeaux I France, Email : auber@talipot-software.com
 */
class Random : public tlp::LayoutAlgorithm {
  public:
    PLUGININFORMATION("Random layout", "David Auber", "01/12/1999",
                      "The positions of the graph nodes are randomly selected.", "1.1", "Basic")
    Random(const tlp::PluginContext *context);

    bool run() override;
};

#endif // RANDOM_H
