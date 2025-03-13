Talipot
=======

Graphs play an important role in many research areas, such as
biology, microelectronics, social sciences, data mining, and
computer science. Talipot is an Information Visualization framework
dedicated to the analysis and visualization of such relational data.

The main features provided by Talipot are the following ones:

* **Creation and manipulation of graphs** :
  Talipot provides an efficient graph data structure for storing
  large and complex networks.
  It is also one of the few that offer the possibility to
  efficiently define and navigate graph hierarchies or cluster
  trees (nested subgraphs).

* **Storage of data on graph elements** :
  Talipot allows to associate different kind of serializable data
  (boolean, integer, float, string, ...) and visual attributes
  (layout, color, size, ...) to graph elements. All these data can
  be easily accessed from the Talipot graph data structure facilitating
  the development of algorithms.

* **Application of algorithms of different types on graph** :
  Talipot has been designed to be easily extensible and provides a variety
  of graph algorithms (layout, metric, clustering, ...) implemented as C++
  plugins. All these algorithms can be called from Python.
  As Talipot is dedicated to graph visualization, it is provided with
  numerous state of the art graph layout algorithms but also a bridge
  to the Open Graph Drawing Framework.
