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

  int getNodeMin(const tlp::Graph *sg=0);
%Docstring
tlp.IntegerProperty.getNodeMin(subgraph=None)

Returns the minimum value on the nodes for that integer property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the minimum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   integer
%End

//===========================================================================================

  int getNodeMax(const tlp::Graph *sg=0);
%Docstring
tlp.IntegerProperty.getNodeMax(subgraph=None)

Returns the maximum value on the nodes for that integer property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the maximum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   integer
%End

//===========================================================================================

  int getEdgeMin(const tlp::Graph *sg=0);
%Docstring
tlp.IntegerProperty.getEdgeMin(subgraph=None)

Returns the minimum value on the edges for that integer property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the minimum value on the edges
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   integer
%End

//===========================================================================================

  int getEdgeMax(const tlp::Graph *sg=0);
%Docstring
tlp.IntegerProperty.getEdgeMax(subgraph=None)

Returns the maximum value on the edges for that integer property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the maximum value on the edges
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   integer
%End

private:

  void treatEvent(const tlp::Event&);
