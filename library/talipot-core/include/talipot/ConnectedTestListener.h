/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_CONNECTED_TEST_LISTENER_H
#define TALIPOT_CONNECTED_TEST_LISTENER_H

#include <talipot/hash.h>

#include <talipot/Graph.h>
#include <talipot/Observable.h>

namespace tlp {

class ConnectedTestListener : public Observable {
public:
  // override of Observable::treatEvent to remove the cached result for a graph if it is modified.
  void treatEvent(const Event &) override;

  /**
   * @brief Stored results for graphs. When a graph is updated, its entry is removed from the map.
   **/
  flat_hash_map<const Graph *, bool> resultsBuffer;
};

}
#endif // TALIPOT_CONNECTED_TEST_LISTENER_H
