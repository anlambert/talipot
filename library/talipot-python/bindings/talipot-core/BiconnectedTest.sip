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

class BiconnectedTest {
%TypeHeaderCode
#include <talipot/BiconnectedTest.h>
%End

public:

  static bool isBiconnected(tlp::Graph *graph);
%Docstring
tlp.BiconnectedTest.isBiconnected(graph)

Returns :const:`True` if the graph is biconnected (ie. one must remove at least two nodes in order
to disconnect the graph), :const:`False` otherwise.

:param graph:
   the graph on which to perform the biconnected test

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

//========================================================================================

  static std::vector<tlp::edge> makeBiconnected(tlp::Graph *graph);
%Docstring
tlp.BiconnectedTest.makeBiconnected(graph)

If the graph is not biconnected, adds edges in order to make the graph biconnected. Returns a list
of added edges.

:param graph:
   the graph to make biconnected.

:type graph:
   :class:`tlp.Graph`

:rtype:
   list of :class:`tlp.edge`
%End

private :

  BiconnectedTest();
};

};
