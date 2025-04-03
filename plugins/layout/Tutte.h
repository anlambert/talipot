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

#ifndef TUTTE_H
#define TUTTE_H

#include <talipot/PluginHeaders.h>

/** This plugin is an implementation of the Tutte layout for 3-Connected graph
 *  algorithm first published as:
 *
 *  W.T. Tutte,
 *  "How to Draw a Graph",
 *  "Proc. London Math. Soc.",
 *  "1963",
 *  pages 743-768.
 *
 */
class Tutte : public tlp::LayoutAlgorithm {
  public:
    PLUGININFORMATION(
        "3-Connected (Tutte)", "David Auber", "06/11/2002",
        "Implements the Tutte layout for 3-Connected graph algorithm first published as:<br/>"
        "<b>How to Draw a Graph</b>, W.T. Tutte, Proc. London Math. Soc. pages 743--768 (1963).",
        "1.0", "Planar")
    Tutte(const tlp::PluginContext *context);
    ~Tutte() override;

    bool run() override;
    bool check(std::string &) override;
};

#endif // TUTTE_H
