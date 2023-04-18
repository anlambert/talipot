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

namespace tlp {
class TriconnectedTest {
%TypeHeaderCode
#include <talipot/TriconnectedTest.h>
%End
public:

  static bool isTriconnected(tlp::Graph *graph);
%Docstring
tlp.TriconnectedTest.isTriconnected(graph)

Returns :const:`True` if the graph is triconnected (i.e. a connected graph such that deleting any
two nodes (and incident edges) results in a graph that is still connected), :const:`False`
otherwise.

:param graph:
   the graph on which to perform the triconnected test

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

private :

  TriconnectedTest();

};

};

