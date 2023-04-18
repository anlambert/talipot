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

%ModuleHeaderCode
typedef std::vector<tlp::edge> VectorEdgeType;
%End

typedef std::vector<tlp::edge> VectorEdgeType;

namespace tlp {

class SimpleTest   {
%TypeHeaderCode
#include <talipot/SimpleTest.h>
%End

public:

  static bool isSimple(tlp::Graph *graph, bool directed = false);
%Docstring
tlp.SimpleTest.isSimple(graph,directed = false)

Returns :const:`True` if the graph is simple (i.e. it contains no loops or parallel edges),
:const:`False` otherwise.

:param graph:
   the graph on which to perform the simple test

:type graph:
   :class:`tlp.Graph`

:param directed:
   Indicates if the graph should be considered as directed or not

:type directed:
   boolean

:rtype:
   boolean
%End

//===========================================================================================

  static std::vector<tlp::edge> makeSimple(tlp::Graph* graph, bool directed = false) ;
%Docstring
tlp.SimpleTest.makeSimple(graph,directed = false)

Makes the graph simple by removing self loops and parallel edges if any. Returns a list of removed
edges.

:param graph:
   the graph to make simple

:type graph:
   :class:`tlp.Graph`

:param directed:
   Indicates if the graph should be considered as directed or not

:type directed:
   boolean

:rtype:
   list of :class:`tlp.edge`
%End

//===========================================================================================

  static std::pair<VectorEdgeType, VectorEdgeType>
  getLoopsAndParallelEdges(tlp::Graph *graph, bool directed = false) /TypeHint="Tuple[List[tlp.edge], List[tlp.edge]]"/;
%Docstring
tlp.SimpleTest.getLoopsAndParallelEdges(graph, bool directed = false)

Performs a simple test and returns found loops and parallel edges.

:param graph:
   the graph on which to perform the simple test

:type graph:
   :class:`tlp.Graph`

:param directed:
   Indicates if the graph should be considered as directed or not

:type directed:
   boolean

:rtype:
   (list of :class:`tlp.edge`, list of :class:`tlp.edge`)
%End

private:

  SimpleTest();

};

};
