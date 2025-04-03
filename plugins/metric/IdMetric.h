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

#ifndef ID_METRIC_H
#define ID_METRIC_H

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

/** This plugins assigns id to nodes/edges or both. The ids are those used internally by Talipot.
 *
 */
class IdMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Id", "David Auber", "06/04/2000", "Assigns their id to nodes and edges.",
                      "1.1", "Misc")
    IdMetric(const tlp::PluginContext *context);
    bool run() override;
};

#endif // ID_METRIC_H
