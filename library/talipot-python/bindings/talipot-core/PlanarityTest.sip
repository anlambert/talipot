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

class PlanarityTest {
%TypeHeaderCode
#include <talipot/PlanarityTest.h>
%End

public:

  static bool isPlanar(tlp::Graph *graph);
%Docstring
tlp.PlanarityTest.isPlanar(graph)

Returns :const:`True` if the graph is planar (i.e. the graph can be drawn in such a way that no
edges cross each other), :const:`False` otherwise.

:param graph:
   the graph on which to perform the planarity test

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

//===========================================================================================

  static bool isPlanarEmbedding(tlp::Graph *graph);
%Docstring
tlp.PlanarityTest.isPlanarEmbedding(graph)

Returns :const:`True` if the current embedding of the graph is planar, :const:`False` otherwise.

:param graph:
   the graph on which to perform the planar embedding test

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

//===========================================================================================

  static bool planarEmbedding(tlp::Graph *graph);
%Docstring
tlp.PlanarityTest.planarEmbedding(graph)

Changes the order of edges around the nodes in order to make the embedding planar (the graph must be
planar for that). Returns :const:`True` if the graph is planar, :const:`False` otherwise.

:param graph:
   the graph on which to compute a planar embedding

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

//===========================================================================================

  static std::list<tlp::edge> getObstructionsEdges(tlp::Graph *graph);
%Docstring
tlp.PlanarityTest.getObstructionsEdges(graph)

Returns a list of edges that prevents to make the graph planar (ie. part of the minor of K3,3 or
K5).

:param graph:
   the graph on which to compute the obstruction edges

:type graph:
   :class:`tlp.Graph`

:rtype:
   list of :class:`tlp.edge`
%End

private :

  PlanarityTest();
};

};
