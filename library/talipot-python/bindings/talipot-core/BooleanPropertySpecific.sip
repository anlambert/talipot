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

  void reverse(const tlp::Graph* graph = NULL);
%Docstring
tlp.BooleanProperty.reverse(graph = None)

Reverses all values associated to graph elements, i.e :const:`True` => :const:`False`,
:const:`False` => :const:`True`.

:param graph:
   a subgraph can be given in parameter, in that case reverse only values of elements belonging to
   that subgraph

:type graph:
   :class:`tlp.Graph`
%End

//===========================================================================================

  void reverseEdgeDirection(tlp::Graph* graph = NULL);
%Docstring
tlp.BooleanProperty.reverseEdgeDirection(graph = None)

Reverses all the direction of edges in the attached graph which are set to :const:`True` in this
boolean property.

:param graph:
   a subgraph can be given in parameter, in that case reverse only the direction of edges belonging
   to it

:type graph:
   :class:`tlp.Graph`
%End

