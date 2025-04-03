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

#ifndef LIN_LOG_ALGORITHM_H
#define LIN_LOG_ALGORITHM_H

/// An implementation of the LinLog layout algorithm
/** This plugin is an implementation of the LinLog
 *  algorithm first published as:
 *
 * Andreas Noack. \n
 * "Energy Models for Graph Clustering", \n
 * in Journal of Graph \n
 * Algorithms and Applications 11(2):453-480, 2007
 * http://www-sst.informatik.tu-cottbus.de/~an/Publications/JGAA2007.pdf
 *
 * The code of the plugin has been translated and adapted
 * from A. Noack's java implementation of the LinLog algorithm
 */
#include <talipot/PluginHeaders.h>

class LinLogAlgorithm : public tlp::LayoutAlgorithm {

  public:
    PLUGININFORMATION("LinLog", "B. Renoust", "11/02/13",
                      "Implements the LinLog layout algorithm, an energy model layout algorithm, "
                      "first published as:<br/>"
                      "<b>Energy Models for Graph Clustering</b>, Andreas Noack., "
                      "Journal of Graph Algorithms and Applications 11(2):453-480, 2007.",
                      "1.0", "Force Directed");

    LinLogAlgorithm(const tlp::PluginContext *context);

    ~LinLogAlgorithm() override;

    bool run() override;
};

#endif // LIN_LOG_ALGORITHM_H
