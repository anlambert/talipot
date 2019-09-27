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
#ifndef Tulip_SpanningDagSelection_H
#define Tulip_SpanningDagSelection_H

#include <tulip/BooleanProperty.h>
#include "../utils/PluginNames.h"

/** \addtogroup selection */

/**
 * This selection plugin enables to find a subgraph of G that is acyclic.
 *
 *  \author David Auber, LaBRI University of Bordeaux, France:
 *   auber@labri.fr
 */
class SpanningDagSelection : public tlp::BooleanAlgorithm {

public:
  PLUGININFORMATION(tlp::SelectionAlgorithm::SpanningDagSelection, "David Auber", "01/12/1999",
                    "Selects an acyclic subgraph of a graph.", "1.0", "Selection")
  SpanningDagSelection(const tlp::PluginContext *context);
  bool run() override;
};

#endif
