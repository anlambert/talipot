/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/ConnectedTestListener.h>

using namespace std;
using namespace tlp;

void ConnectedTestListener::treatEvent(const Event &evt) {
  const auto *gEvt = dynamic_cast<const GraphEvent *>(&evt);

  if (gEvt) {
    Graph *graph = gEvt->getGraph();

    switch (gEvt->getType()) {
    case GraphEventType::TLP_ADD_NODE:
      resultsBuffer[graph] = false;
      break;

    case GraphEventType::TLP_DEL_NODE:
      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    case GraphEventType::TLP_ADD_EDGE:

      if (resultsBuffer.contains(graph) && resultsBuffer[graph]) {
        return;
      }

      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    case GraphEventType::TLP_DEL_EDGE:

      if (resultsBuffer.contains(graph) && !resultsBuffer[graph]) {
        return;
      }

      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    default:
      // we don't care about other events
      break;
    }
  } else {

    auto *graph = static_cast<Graph *>(evt.sender());

    if (evt.type() == EventType::TLP_DELETE) {
      resultsBuffer.erase(graph);
    }
  }
}
