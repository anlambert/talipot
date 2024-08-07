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

class SelfLoops {
%Docstring
The tlp.SelfLoops class represents a self loop that has been replaced by two nodes and three edges.
The method :meth:`tlp.AcyclicTest.makeAcyclic` returns a list of this type.

It contains the following variables :

   -  old (:class:`tlp.edge`) : the self loop edge that has been replaced
   -  n1 (:class:`tlp.node`) : the first node added
   -  n2 (:class:`tlp.node`) : the second node added
   -  e1 (:class:`tlp.edge`) : the edge added between the node at the extremity of the loop and n1
   -  e2 (:class:`tlp.edge`) : the edge added between n1 and n2
   -  e3 (:class:`tlp.edge`) : the edge added between the node at the extremity of the loop and n2
%End

%TypeHeaderCode
#include <talipot/AcyclicTest.h>
%End

public:

  SelfLoops(tlp::node n1, tlp::node n2, tlp::edge e1, tlp::edge e2, tlp::edge e3, tlp::edge old);

  tlp::node n1;
  tlp::node n2;
  tlp::edge e1;
  tlp::edge e2;
  tlp::edge e3;
  tlp::edge old;
};

class AcyclicTest {
%TypeHeaderCode
#include <talipot/AcyclicTest.h>
%End
public:

  static bool isAcyclic(const tlp::Graph *graph);
%Docstring
tlp.AcyclicTest.isAcyclic(graph)

Returns :const:`True` if the graph is acyclic, :const:`False` if not. The result is cached (ie. the
next call with the same graph is done in O(1) time)

:param graph:
   the graph on which to perform the acyclic test

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

//===========================================================================================

   static void makeAcyclic(tlp::Graph* graph, std::vector<tlp::edge> &reversed /Out/, std::vector<tlp::SelfLoops> &selfLoops /Out/) /TypeHint="Tuple[List[tlp.edge], List[tlp.SelfLoops]]"/;
%Docstring
tlp.AcyclicTest.makeAcyclic(graph)

Makes the graph acyclic, by reversing edge direction (feedback arc set problem). If there is self
loops, a new node is added with two edges that points to it. Returns a tuple containing whose first
member is a list of reversed edges and second member is a list of transformed self loops.

:param graph:
   the graph to make acyclic

:type graph:
   :class:`tlp.Graph`

:rtype:
   (list of :class:`tlp.edge`, list of :class:`tlp.SelfLoops`)
%End

//===========================================================================================

   static bool acyclicTest(const tlp::Graph *graph, std::vector<tlp::edge> *obstructionEdges /Out/) /TypeHint="Tuple[bool, List[tlp.edge]]"/;
%Docstring
tlp.AcyclicTest.acyclicTest(graph)

Returns a tuple whose first member is a boolean indicating if the graph is acyclic or not and second
member is a list of edges that create cycle (when the graph is not acyclic).

:param graph:
   the graph on which to perform the acyclic test

:type graph:
   :class:`tlp.Graph`

:rtype:
   (boolean, list of :class:`tlp.edge`)
%End

private :

  AcyclicTest();
};

};
