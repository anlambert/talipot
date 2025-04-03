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

#ifndef CIRCULAR_H
#define CIRCULAR_H

#include <talipot/PluginHeaders.h>

/**
 * This plugin is an implementation of a circular layout that takes
 * node size into account.  It manages size of nodes and use a standard dfs
 * for ordering nodes or search the maximum length cycle.
 *
 * HISTORY:
 *
 * 25/11/2004 version 0.0.2: size and order
 * 01/12/1999 Version 0.0.1: Initial release
 *
 *
 * NOTES:
 *
 * This work on general graphs.
 * Let n be the number of nodes, the algorithm complexity is in O(n). If the search_cycle is
 * enabled,
 * the complexity is exponential (NP-Complete problem !).
 *
 * AUTHORS:
 *
 * David Auber and Romain Bourqui University Bordeaux I France: Email:auber@labri.fr
 * Daniel Archambault The University of British Columbia:
 * Email:  archam@cs.ubc.ca
 */
class Circular : public tlp::LayoutAlgorithm {
  public:
    PLUGININFORMATION(
        "Circular", "David Auber/ Daniel Archambault", "25/11/2004",
        "Implements a circular layout that takes node size into account.<br/>"
        "It manages size of nodes and use a standard dfs for ordering nodes or search "
        "the maximum length cycle.",
        "1.1", "Basic")
    Circular(const tlp::PluginContext *context);
    bool run() override;
};

#endif // CIRCULAR_H
