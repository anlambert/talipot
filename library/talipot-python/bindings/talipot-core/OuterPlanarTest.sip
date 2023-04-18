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

class OuterPlanarTest {
%TypeHeaderCode
#include <talipot/OuterPlanarTest.h>
%End

public:

  static bool isOuterPlanar(tlp::Graph *graph);
%Docstring
tlp.OuterPlanarTest.isOuterPlanar(graph)

Returns :const:`True` if the graph is outerplanar (i.e. a graph with an embedding in the plane such
that all vertices belong to the unbounded face of the embedding), :const:`False` otherwise.

:param graph:
   the graph on which to perform the outerplanarity test

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

private:

  OuterPlanarTest();

};

};

