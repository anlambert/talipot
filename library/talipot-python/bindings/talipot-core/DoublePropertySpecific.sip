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

  double getNodeMin(const tlp::Graph *subgraph=0);
%Docstring
tlp.DoubleProperty.getNodeMin(subgraph=None)

Returns the minimum value on the nodes for that double property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the minimum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  double getNodeMax(const tlp::Graph *subgraph=0);
%Docstring
tlp.DoubleProperty.getNodeMax(subgraph=None)

Returns the maximum value on the nodes for that double property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the maximum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  double getEdgeMin(const tlp::Graph *subgraph=0);
%Docstring
tlp.DoubleProperty.getEdgeMin(subgraph=None)

Returns the minimum value on the edges for that double property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the minimum value on the edges
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  double getEdgeMax(const tlp::Graph *subgraph=0);
%Docstring
tlp.DoubleProperty.getEdgeMax(subgraph = None)

Returns the maximum value on the edges for that double property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the maximum value on the edges
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  void uniformQuantification(uint n);
%Docstring
tlp.DoubleProperty.uniformQuantification(n)

Performs an uniform quantification on nodes and edges based on the values stored in that double
property. An uniform quantification tries to separate the nodes and edges in n classes of equivalent
size according to theirs values in increasing order.

:param n:
   the number of desired classes

:type n:
   integer

.. warning::

   All previous values on nodes and edges will be erased and replaced by the id of the class they
   belong to.
%End

//===========================================================================================

  void nodesUniformQuantification(uint n);
%Docstring
tlp.DoubleProperty.nodesUniformQuantification(n)

Performs an uniform quantification on nodes based on the values stored in that double property. An
uniform quantification tries to separate the nodes in n classes of equivalent size according to
theirs values in increasing order.

:param n:
   the number of desired classes

:type n:
   integer

.. warning::

   All previous values on nodes will be erased and replaced by the id of the class they belong to.
%End

//===========================================================================================

  void edgesUniformQuantification(uint n);
%Docstring
tlp.DoubleProperty.edgesUniformQuantification(n)

Performs an uniform quantification on edges based on the values stored in that double property. An
uniform quantification tries to separate the edges in n classes of equivalent size according to
theirs values in increasing order.

:param n:
   the number of desired classes

:type n:
   integer

.. warning::

   All previous values on edges will be erased and replaced by the id of the class they belong to.
%End

private:

  void treatEvent(const tlp::Event&);
