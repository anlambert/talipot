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

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

/** This plugin assigns random values to nodes or edges or both.
 * The values are between 0 and 1.
 */
class RandomMetric : public tlp::DoubleAlgorithm {

  public:
    PLUGININFORMATION("Random metric", "David Auber", "04/10/2001",
                      "Assigns random values to nodes and edges.", "1.1", "Misc")
    RandomMetric(const tlp::PluginContext *context);
    bool run() override;
};
#endif // RANDOM_H
