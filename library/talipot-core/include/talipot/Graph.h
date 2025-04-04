/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GRAPH_H
#define TALIPOT_GRAPH_H

#include <iostream>
#include <functional>
#include <set>
#include <string>
#include <vector>
#include <talipot/hash.h>

#include <climits>
#include <talipot/config.h>
#include <talipot/DataSet.h>
#include <talipot/Node.h>
#include <talipot/Edge.h>
#include <talipot/Observable.h>
#include <talipot/PropertyProxy.h>

namespace tlp {

class PropertyInterface;
class BooleanProperty;
class ColorProperty;
class DoubleProperty;
class IntegerProperty;
class GraphProperty;
class LayoutProperty;
class SizeProperty;
class StringProperty;
class BooleanVectorProperty;
class ColorVectorProperty;
class DoubleVectorProperty;
class IntegerVectorProperty;
class CoordVectorProperty;
class SizeVectorProperty;
class StringVectorProperty;
class PluginProgress;
template <class C>
struct Iterator;

/**
 * @enum This Enum describes the possible types of an element of the graph.
 *
 * It is used in functions that can return an edge or a node, to distinguish between the two cases.
 **/
enum class ElementType {
  /** This element describes a node **/
  NODE = 0,
  /** This element describes an edge **/
  EDGE = 1
};

/**
 * @ingroup Graph
 * @brief Loads a graph from a file
 *
 * This function loads a graph serialized in a file through one of the available
 * Talipot import plugins.
 *
 * The selection of the import plugin is based on the provided filename extension.
 * The graph file formats that can be imported from a standard Talipot installation
 * are: TLP (*.tlp), TLP Binary (*.tlpb), TLP JSON (*.json), Gephi (*.gexf),
 * Pajek (*.net, *.paj), GML (*.gml), Graphviz (*.dot) and UCINET (*.txt).
 *
 * Apart for the Gephi and Graphviz formats, the function can also import a graph
 * file compressed with zlib (filename must be suffixed by ".gz" or "z") or Zstandard
 * (filename must be suffixed by ".zst" or "zst").
 *
 * The import will fail if the selected import plugin is not loaded.
 *
 * As a fallback, the function uses the "TLP Import" import plugin
 * (always loaded as it is contained in the talipot-core library).
 *
 * If the import fails (no such file, parse error, ...) nullptr is returned.
 *
 * @param filename path to a graph file to import
 * @param progress to report the progress of the operation
 * @param graph optional graph to import the data into (useful to import data into a
 * subgraph)
 * @return the imported graph or nullptr if the import failed
 **/
TLP_SCOPE Graph *loadGraph(const std::string &filename, tlp::PluginProgress *progress = nullptr,
                           tlp::Graph *graph = nullptr);

/**
 * @ingroup Graph
 * @brief Saves the corresponding graph to a file
 *
 * This function serializes the corresponding graph and all its subgraphs
 * (depending on the format) to a file through the available Talipot export plugins.
 *
 * The selection of the export plugin is based on the provided filename extension.
 * The file formats a graph can be exported to from a standard Talipot installation
 * are: TLP (*.tlp), TLP Binary (*.tlpb), TLP JSON (*.json), GML (*.gml) and CSV (*.csv).
 *
 * The function can also export to a file compressed using zlib (filename must be
 * suffixed by ".gz" or "z") or Zstandard (filename must be suffixed by ".zst" or "zst").
 *
 * The export will fail if the selected export plugin is not loaded.
 *
 * As a fallback, this function uses the "TLP Export" export plugin
 * (always loaded as it is contained into the talipot-core library).
 *
 * @param graph the graph to save
 * @param filename the file to save the graph to
 * @param progress to report the progress of the operation
 * @param parameters parameters to pass to the export plugin (e.g. additional data,
 * options for the format)
 * @return whether the export was successful or not
 **/
TLP_SCOPE bool saveGraph(Graph *graph, const std::string &filename,
                         tlp::PluginProgress *progress = nullptr,
                         tlp::DataSet *parameters = nullptr);

/**
 * @ingroup Graph
 * @brief Exports a graph using the specified export plugin with parameters
 *
 * You determine the destination, whether by using a file stream or any other
 * stream type (string stream for instance).
 *
 * @param graph the graph to export
 * @param os the stream to export data into (can be a standard ostream, an ofstream,
 * or even a compressed ostream)
 * @param format The format to use to export the graph
 * @param parameters parameters to pass to the export plugin (e.g. additional data,
 * options for the format)
 * @param progress a PluginProgress to report the progress of the operation
 * @return whether the export was successful or not
 **/
TLP_SCOPE bool exportGraph(Graph *graph, std::ostream &os, const std::string &format,
                           DataSet &parameters, PluginProgress *progress = nullptr);

/**
 * @ingroup Graph
 * @brief Imports a graph using the specified import plugin with parameters
 *
 * If no graph is passed, then a new graph will be created.
 * You can pass a graph in order to import data into it.
 *
 * Returns the graph with imported data, or nullptr if the import failed.
 * In this case, the PluginProgress should have an error that can be displayed.
 *
 * @param format the format to use to import the graph
 * @param parameters the parameters to pass to the import plugin (file to read, ...)
 * @param progress a PluginProgress to report the progress of the operation
 * @param graph optional graph to import the data into (useful to import data into a subgraph)
 * @return the graph containing the imported data, or nullptr in case of failure
 **/
TLP_SCOPE Graph *importGraph(const std::string &format, DataSet &parameters,
                             PluginProgress *progress = nullptr, Graph *graph = nullptr);

/**
 * @ingroup Graph
 * @brief Creates a new, empty graph.
 *
 * This is a simple method factory to create a Graph implementation (remember, Graph is only an
 *interface).
 *
 * This is the recommended way to create a new Graph.
 *
 * @return :Graph* A new, empty graph.
 **/
TLP_SCOPE Graph *newGraph();

/**
 * @ingroup Graph
 * Appends the selected part of the graph inG (properties, nodes and edges) into the graph outG.
 * If no selection is done (inSel=nullptr), the whole inG graph is appended.
 * The output selection is used to select the appended nodes & edges
 * \warning The input selection is extended to all selected edge ends.
 */
TLP_SCOPE void copyToGraph(Graph *outG, const Graph *inG, BooleanProperty *inSelection = nullptr,
                           BooleanProperty *outSelection = nullptr);

/**
 * @ingroup Graph
 * Removes the selected part of the graph ioG (properties values, nodes and edges).
 * If no selection is done (inSel=nullptr), the whole graph is reset to default value.
 * \warning The selection is extended to all selected edge ends.
 */
TLP_SCOPE void removeFromGraph(Graph *ioG, BooleanProperty *inSelection = nullptr);

/**
 * @ingroup Graph
 * Gets an iterator over the root graphs. That is all the currently existing graphs which have been
 * created using the tlp::newGraph, tlp::loadGraph or tlp::importGraph functions and are the root
 * graphs of an existing graph hierarchy.
 * @return An iterator over all the root graphs. The caller of this function is responsible of the
 * deletion of the returned iterator.
 */
TLP_SCOPE Iterator<Graph *> *getRootGraphs();

/**
 * @ingroup Graph
 * The class Graph is the interface of a Graph in the Talipot Library.
 *
 * There are a few principles to know when working with a Talipot Graph.
 *
 * @chapter Directed
 * Every edge is directed in a Talipot Graph.
 * You can choose to ignore this, but every edge has a source and destination.
 *
 * @chapter Inheritance
 *
 * Subgraphs inherit from their parent graph.
 * This is true of nodes and edges; every node and edge in a subgraph also exists in each of its
 *parent graphs.
 * This is also true of properties; every property in a graph exist in all of its subgraphs, except
 *if it has been replaced
 * by a local property.
 *
 * For instance, if you have the following graph hierarchy:
 * root
 *  / \
 * A   B
 *
 * Every node in A is in root, and every node in B is in root.
 * Nodes can be in A and root but not B; or in B and root but not A.
 *
 * For instance, imagine a graph. You want to compare it to its Delaunay triangulation.
 * You need to create a subgraph that is a clone of the original (say this is A) to keep the
 *original graph,
 * and another copy (say this one is B) on which you will perform the delaunay triangulation.
 *
 * B will have none of the original edges, and A will have only the original edges.
 *
 * As for properties; let's imagine the same graph hierarchy.
 * You want to compare two different layouts on the same graph.
 * You need to create two clone subgraphs, on each you make the 'viewLayout' property local.
 * This results in A and B having different values for the layout, but everything else in common.
 * You then can apply two different algorithms on A and B (e.g. Bubble Tree and Tree Radial).
 *
 * @chapter Meta Nodes
 * A meta node is a node representing a subgraph of the current graph.
 *
 * @chapter Undo Redo
 * The Talipot Graph object supports for undo and redo of modifications.
 *The operations affect the whole graph hierarchy, and cannot be limited to a subgraph.
 *
 */
class TLP_SCOPE Graph : public Observable {

  friend class GraphAbstract;
  friend class GraphUpdatesRecorder;
  friend class GraphDecorator;
  friend class PropertyManager;
  friend class PropertyInterface;

public:
  Graph() : id(0) {}
  ~Graph() override = default;

  /**
   * @brief Applies an algorithm plugin, identified by its name.
   * Algorithm plugins are subclasses of the tlp::Algorithm interface.
   * Parameters are transmitted to the algorithm through the DataSet.
   * To determine a plugin's parameters, you can either:
   *
   * * refer to its documentation
   *
   * * use buildDefaultDataSet on the plugin object if you have an instance of it
   *
   * * call getPluginParameters() with the name of the plugin on the PluginsManager
   *
   *
   * If an error occurs, a message describing the error should be stored in errorMessage.
   *
   * @param algorithm The algorithm to apply.
   * @param errorMessage A string that will be modified to contain an error message if an error
   *occurs.
   * @param parameters The parameters of the algorithm. Defaults to nullptr.
   * @param progress A PluginProgress to report the progress of the operation, as well as the final
   *state. Defaults to nullptr.
   * @return bool Whether the algorithm applied successfully or not. If not, check the error
   *message.
   **/
  bool applyAlgorithm(const std::string &algorithm, std::string &errorMessage,
                      DataSet *parameters = nullptr, PluginProgress *progress = nullptr);

  //=========================================================================
  // Graph hierarchy access and building
  //=========================================================================

  /**
   * @brief Removes all nodes, edges and subgraphs from this graph.
   *
   * Contrarily to creating a new Graph, this keeps attributes and properties.
   *
   * @return void
   **/
  virtual void clear() = 0;

  /**
   * @brief Creates and returns a new subgraph of this graph.
   *
   * If a BooleanProperty is provided, only nodes and edges for which it is true will be added to
   *the subgraph.
   * If none is provided, then the subgraph will be empty.
   *
   * @param selection The elements to add to the new subgraph. Defaults to nullptr.
   * @param name The name of the newly created subgraph. Defaults to "unnamed".
   * @return :Graph* The newly created subgraph.
   **/
  virtual Graph *addSubGraph(BooleanProperty *selection = nullptr,
                             const std::string &name = "unnamed") = 0;

  /**
   * @brief Creates and returns a new named subgraph of this graph.
   *
   * @param name The name of the newly created subgraph.
   * @return :Graph* The newly created subgraph.
   **/
  Graph *addSubGraph(const std::string &name);

  /**
   * @brief Creates and returns a subgraph that contains all the elements of this graph.
   *
   * @param name The name of the newly created subgraph. Defaults to "unnamed".
   * @param addSibling if true the clone subgraph will be a sibling of this graph, if false (the
   *default) it will be a subgraph of this graph
   * @param addSiblingProperties if true the local properties will be cloned into the sibling of
   *this graph, if false (the default) the local properties will not be cloned
   * @return :Graph* The newly created clone subgraph. nullptr will be returned if addSibling is set
   *to
   *true and this graph is a root graph.
   **/
  virtual Graph *addCloneSubGraph(const std::string &name = "unnamed", bool addSibling = false,
                                  bool addSiblingProperties = false);

  /**
   * @brief Creates and returns a new subgraph of the graph induced by a vector of nodes.
   *
   * Every node contained in the given vector is added to the subgraph.
   * Every edge connecting any two nodes in the set of given nodes is also added.
   * @param nodes The nodes to add to the subgraph. All the edges between these nodes are added too.
   * @param parentSubGraph If provided, is used as parent graph for the newly created subgraph
   * instead of the graph this method is called on.
   * @param name The name of the newly created subgraph.
   * @return The newly created subgraph.
   */
  Graph *inducedSubGraph(const std::vector<node> &nodes, Graph *parentSubGraph = nullptr,
                         const std::string &name = "unnamed");

  /**
   * @brief Creates and returns a new subgraph of the graph induced by a selection of nodes and
   * edges.
   *
   * Every node contained in the selection is added to the subgraph.
   * Every edge and its source and target node contained in the selection is added to the subgraph.
   * Every edge connecting any two nodes in the resulting set of nodes is also added.
   * @param selection a selection of nodes and edges.
   * @param parentSubGraph If provided, is used as parent graph for the newly created subgraph
   * instead of the graph this method is called on.
   * @param name The name of the newly created subgraph.
   * @return The newly created subgraph.
   */
  Graph *inducedSubGraph(BooleanProperty *selection, Graph *parentSubGraph = nullptr,
                         const std::string &name = "unnamed");

  /**
   * @brief Deletes a subgraph of this graph.
   * All subgraphs of the removed graph are re-parented to this graph.
   * For instance, with a graph hierarchy as follows :
   * root
   * / \
   * A  B
   *    /|\
   *   C D E
   *
   * @code root->delSubGraph(B);
   * would result in the following hierarchy:
   *  root
   * / | \\
   * A C D E
   *
   * @param graph The subgraph to delete.
   *
   * @see delAllSubGraphs() if you want to remove all descendants of a graph.
   */
  virtual void delSubGraph(Graph *graph) = 0;

  /**
   * @brief Deletes a subgraph of this graph and all of its subgraphs.
   ** For instance, with a graph hierarchy as follows :
   * root
   * / \
   * A  B
   *    /|\
   *   C D E
   *
   * @codeline root->delSubGraph(B); @endcode
   * would result in the following hierarchy:
   *  root
   *   |
   *   A
   *
   * @param graph The subgraph to delete.
   * @see delSubGraph() if you want to keep the descendants of the subgraph to remove.
   */
  virtual void delAllSubGraphs(Graph *graph = nullptr) = 0;

  /**
   * @brief Returns the parent of the graph. If called on the root graph, it returns itself.
   * @return The parent of this graph (or itself if it is the root graph).
   * @see getRoot() to directly retrieve the root graph.
   */
  virtual Graph *getSuperGraph() const = 0;

  /**
   * @brief Gets the root graph of the graph hierarchy.
   * @return The root graph of the graph hierarchy.
   */
  virtual Graph *getRoot() const = 0;

  /**
   * @brief Sets the parent of a graph.
   * @warning ONLY USE IF YOU KNOW EXACTLY WHAT YOU ARE DOING.
   */
  virtual void setSuperGraph(Graph *) = 0;

  /**
   * @brief Gets an iterator over all the subgraphs of the graph.
   * For instance, in the following graph hierarchy:
   ** root
   * / \
   * A  B
   *    /|\
   *   C D E
   *
   * @codeline root->getSubGraphs(); @endcode
   * Will return an iterator over A and B, but not C, D and E.
   * @return An iterator over this graph's direct subgraphs.
   */
  virtual Iterator<Graph *> *getSubGraphs() const = 0;

  /**
   * @brief Return a const reference on the vector of subgraphs of the graph
   * It is the fastest way to access to subgraphs, Iterators are 25% slower.
   * @remark o(1)
   */
  virtual const std::vector<Graph *> &subGraphs() const = 0;

  /**
   * @brief This method returns the nth subgraph.
   * Since subgraphs order cannot be ensured in every implementation, this method should be
   equivalent to:
   * @code
    int i=0;
    Iterator<Graph *> *it = g->getSubGraphs();
    while (it->hasNext()) {
      Graph *result = it->next();
      if (i++ == n) {
        delete it;
        return result;
      }
    }
    delete it;
    return nullptr;
   * @endcode
   * @param n the index of the subgraph to retrieve.
   * @return The n-th subgraph.
   */
  virtual Graph *getNthSubGraph(uint n) const;

  /**
   * @brief Return the number of direct subgraphs.
   * For instance, in the following graph hierarchy:
   * root
   * / \
   * A  B
   *    /|\
   *   C D E
   *
   * @codeline root->numberOfSubGraphs(); @endcode
   * Will return 2.
   * @return The number of direct subgraphs.
   * @see numberOfDescendantGraphs() to count in the whole hierarchy.
   */
  virtual uint numberOfSubGraphs() const = 0;

  /**
   * @brief Return the number of descendant subgraphs.
   * For instance, in the following graph hierarchy:
   * root
   * / \
   * A  B
   *    /|\
   *   C D E
   *
   * @codeline root->numberOfSubGraphs(); @endcode
   * Will return 5.
   * @return The number of descendants subgraphs.
   * @see numberOfSubGraphs() to count only direct subgraphs.
   */
  virtual uint numberOfDescendantGraphs() const = 0;

  /**
   * @brief Indicates if the graph argument is a direct subgraph.
   * @param subGraph The graph to check is a subgraph of this graph.
   * @return Whether subGraph is a direct subgraph of this graph.
   * @see isDescendantGraph() to search in the whole hierarchy.
   */
  virtual bool isSubGraph(const Graph *subGraph) const = 0;

  /**
   * @brief Indicates if the graph argument is a descendant of this graph.
   * @param subGraph The graph to check is a descendant of this graph.
   * @return Whether subGraph is a descendant of this graph.
   * @see isSubGraph to search only in direct subgraphs.
   */
  virtual bool isDescendantGraph(const Graph *subGraph) const = 0;

  /**
   * @brief Returns a pointer on the subgraph with the corresponding id
   * or nullptr if there is no subgraph with that id.
   * @param id The id of the subgraph to retrieve.
   * @return A subgraph of the given id, or null if no such subgraph exists on this graph.
   * @see getDescendantGraph(uint) to search in the whole hierarchy.
   */
  virtual Graph *getSubGraph(uint id) const = 0;

  /**
   * @brief Returns a pointer on the subgraph with the corresponding name
   * or nullptr if there is no subgraph with that name.
   * @param name The name of the subgraph to retrieve.
   * @return A Graph named name, or nullptr if no such subgraph exists on this graph.
   * @see getDescendantGraph(const std::string &) to search in the whole hierarchy.
   */
  virtual Graph *getSubGraph(const std::string &name) const = 0;

  /**
   * @brief Returns a pointer on the descendant with the corresponding id
   * or nullptr if there is no descendant  with that id.
   * @param id The id of the descendant graph to retrieve.
   * @return A graph with the given id, or nullptr if no such graph exists in this graph's
   * descendants.
   * @see getSubGraph(uint) to search only in direct subgraphs.
   */
  virtual Graph *getDescendantGraph(uint id) const = 0;

  /**
   * @brief Returns a pointer on the first descendant graph with the corresponding name
   * or nullptr if there is no descendant graph with that name.
   * @param name The name of the descendant graph to look for.
   * @return A graph named name, or nullptr if there is no such graph in this graph's descendants.
   * @see getSubGraph(const std::string &) to search only in direct subgraphs.
   */
  virtual Graph *getDescendantGraph(const std::string &name) const = 0;

  /**
   * @brief Gets an iterator over all the descendant subgraphs of the graph.
   * For instance, in the following graph hierarchy:
   ** root
   * / \
   * A  B
   *    /|\
   *   C D E
   *
   * @codeline root->getSubGraphs(); @endcode
   * Will return an iterator over A B, C, D and E.
   * @return An iterator over this graph's descendant subgraphs.
   */
  Iterator<Graph *> *getDescendantGraphs() const;

  //==============================================================================
  // Modification of the graph structure
  //==============================================================================
  /**
   * @brief Adds a new node in the graph and returns it. This node is also added in all
   * the ancestor graphs.
   * @return The newly added node.
   * @see addNodes() if you want to add more than one node.
   */
  virtual node addNode() = 0;

  /**
   * @brief Adds new nodes in the graph and returns them in the addedNodes vector.
   * The new nodes are also added in all the ancestor graphs.
   *
   * @param nbNodes The number of nodes to add.
   * @return The newly added nodes in a vector.
   * @see addNode() to add a single node.
   */
  virtual std::vector<node> addNodes(uint nbNodes) = 0;

  /**
   * @brief Adds an existing node in the graph. This node is also added in all the ancestor graphs.
   * This node must exists in the graph hierarchy (which means it must exist in the root graph).
   * You cannot add a node to the root graph this way (as it must already be an element of the root
   * graph).
   * @warning Using this method on the root graph will display a warning on the console.
   *
   * @param n The node to add to a subgraph. This node must exist in the root graph.
   * @see addNode() to add a new node to a graph.
   */
  virtual void addNode(const node n) = 0;

  /**
   * @brief Adds existing nodes in the graph. The nodes are also added in all the ancestor graphs.
   * as with addNode(const tlp::node), the nodes must exist in the graph hierarchy and thus exist in
   the root graph,
   * and node cannot be added this way to the root graph.

   * @warning Using this method on the root graph will display a warning on the console.
   * @warning The graph takes ownership of the Iterator.
   *
   * @param nodes An iterator over nodes to add to this subgraph. The graph takes ownership
   of this iterator.
   */
  void addNodes(Iterator<node> *nodes);

  /**
  * @brief Adds existing nodes in the graph. The nodes are also added in all the ancestor graphs.
  * as with addNode(const tlp::node), the nodes must exist in the graph hierarchy and thus exist in
  the root graph,
  * and nodes cannot be added this way to the root graph.

  * @warning Using this method on the root graph will display a warning on the console.
  *
  * @param nodes a vector of nodes to add to this subgraph.
  */
  virtual void addNodes(const std::vector<node> &nodes) = 0;

  /**
   * @brief Deletes a node in the graph.
   * This node is also removed in the subgraphs hierarchy of the current graph.
   * @param n The node to delete.
   * @param deleteInAllGraphs Whether to delete in all its parent graphs or only in this graph. By
   * default only removes in the current graph.
   * @see delNodes() to remove multiple nodes.
   */
  virtual void delNode(const node n, bool deleteInAllGraphs = false) = 0;

  /**
   * @brief Deletes nodes in the graph.
   * These nodes are also removed in the subgraphs hierarchy of the current graph.
   * @warning the graph takes ownership of the Iterator.
   * @param it The nodes to delete.
   * @param deleteInAllGraphs Whether to delete in all its parent graphs or only in this graph. By
   * default only removes in the current graph.
   * @see delNode() to remove a single node.
   */
  void delNodes(Iterator<node> *it, bool deleteInAllGraphs = false);

  /**
   * @brief Deletes nodes in the graph.
   * These nodes are also removed in the subgraphs hierarchy of the current graph.
   * @warning the graph does not take ownership of the Iterator.
   * @param nodes a vector of the nodes to delete.
   * @param deleteInAllGraphs Whether to delete in all its parent graphs or only in this graph. By
   * default only removes in the current graph.
   * @see delNode() to remove a single node.
   */
  virtual void delNodes(const std::vector<node> &nodes, bool deleteInAllGraphs = false) = 0;

  /**
   * @brief Adds a new edge in the graph
   * This edge is also added in all the super-graph of the graph.
   * @param source The source of the edge.
   * @param target The target of the edge.
   * @return The newly added edge.
   * @see addEdges() to add multiple edges at once.
   */
  virtual edge addEdge(const node source, const node target) = 0;

  /**
   * @brief Adds new edges in the graph and returns them in the addedEdges vector.
   * The new edges are also added in all graph ancestors.
   *
   * @warning If the edges vector contains a node that does not belong to this graph,
   * undefined behavior will ensue.
   * @param edges A vector describing between which nodes to add edges.
   * The first element of the pair is the source, the second is the destination.
   * @return The newly added edges in a vector.
   *
   */
  virtual std::vector<edge> addEdges(const std::vector<std::pair<node, node>> &edges) = 0;

  /**
   * @brief Adds an existing edge in the graph. This edge is also added in all
   * the ancestor graphs.
   * The edge must be an element of the graph hierarchy, thus it must be
   * an element of the root graph.
   * @warning Using this method on the root graph will display a warning on the console.
   * @param e The edge to add to this subgraph.
   * @see addEgdes() to add more than one edge at once.
   * @see addNode() to add nodes.
   */
  virtual void addEdge(const edge e) = 0;

  /**
   * @brief Adds existing edges in the graph. The edges are also added in all
   * the ancestor graphs.
   * The added edges must be elements of the graph hierarchy,
   * thus they must be elements of the root graph.
   * @warning Using this method on the root graph will display a warning on the console.
   * @warning The graph takes ownership of the iterator.
   * @param edges An iterator over edges to add to this subgraph. The graph takes ownership
   of this iterator.
   */
  void addEdges(Iterator<edge> *edges);

  /**
   * @brief Adds existing edges in the graph. The edges are also added in all
   * the ancestor graphs.
   * The added edges must be elements of the graph hierarchy,
   * thus they must be elements of the root graph.
   * @warning Using this method on the root graph will display a warning on the console.
   * @param edges a vector of the edges to add on this subgraph.
   */
  virtual void addEdges(const std::vector<edge> &edges) = 0;

  /**
   * @brief Deletes an edge in the graph. The edge is also removed in
   * the subgraphs hierarchy.
   * The ordering of remaining edges is preserved.
   * @param e The edge to delete.
   * @param deleteInAllGraphs Whether to delete in all its parent graphs or only in this graph. By
   * default only removes in the current graph.
   */
  virtual void delEdge(const edge e, bool deleteInAllGraphs = false) = 0;

  /**
   * @brief Deletes edges in the graph. These edges are also removed in the subgraphs hierarchy.
   * The ordering of remaining edges is preserved.
   * @warning The graph takes ownership of the Iterator.
   * @param itE
   * @param deleteInAllGraphs  Whether to delete in all its parent graphs or only in this graph. By
   * default only removes in the current graph.
   */
  void delEdges(Iterator<edge> *itE, bool deleteInAllGraphs = false);

  /**
   * @brief Deletes edges in the graph. These edges are also removed in the subgraphs hierarchy.
   * The ordering of remaining edges is preserved.
   * @warning The graph does not take ownership of the Iterator.
   * @param edges a vector of the edges to delete
   * @param deleteInAllGraphs  Whether to delete in all its parent graphs or only in this graph. By
   * default only removes in the current graph.
   */
  virtual void delEdges(const std::vector<edge> &edges, bool deleteInAllGraphs = false) = 0;

  /**
   * @brief Sets the order of the edges around a node.
   * This operation ensures that adjacent edges of a node will
   * be ordered as they are in the vector of edges given in parameter.
   *
   * This can be useful if you want to make sure you retrieve the edges in a specific order when
   * iterating upon them.
   * @param n The node whose edges to order.
   * @param edges The edges, in the order you want them.
   */
  virtual void setEdgeOrder(const node n, const std::vector<edge> &edges) = 0;

  /**
   * @brief Swaps two edges in the adjacency list of a node.
   * @param n The node on whoch to swap the edges order.
   * @param e1 The first edge, that will take the second edge's position.
   * @param e2 The second edge, that will take the first edge's position.
   */
  virtual void swapEdgeOrder(const node n, const edge e1, const edge e2) = 0;

  /**
   * @brief Sets the source of an edge to be the given node.
   * @param e The edge to change the source of.
   * @param source The new source of the edge.
   */
  virtual void setSource(const edge e, const node source) = 0;

  /**
   * @brief Sets the target of an edge to be the given node.
   * @param e The edge to change the target of.
   * @param target The new target of the edge.
   */
  virtual void setTarget(const edge e, const node target) = 0;

  /**
   * @brief Sets both the source and the target of an edge.
   * @param e The edge to set the source and target of.
   * @param source The new source of the edge.
   * @param target The new target of the edge.
   */
  virtual void setEnds(const edge e, const node source, const node target) = 0;

  /**
   * @brief Reverses the direction of an edge, the source becomes the target and the target
   *  becomes the source.
   * @warning The ordering is global to the entire graph hierarchy. Thus, by changing
   *  the ordering of a graph you change the ordering of the hierarchy.
   * @param e The edge top reverse.
   */
  virtual void reverse(const edge e) = 0;
  // Attempts to reserve enough space to store nodes.
  // Only defined on root graph.
  virtual void reserveNodes(uint nbNodes) = 0;
  // Attempts to reserve enough space to store edges.
  // Only defined on root graph.
  virtual void reserveEdges(uint nbEdges) = 0;
  //================================================================================
  // Iterators on the graph structure.
  //================================================================================
  /**
   * @brief Finds the first node whose input degree equals 0.
   *
   * @return tlp::node The first encountered node with input degree of 0, or an invalid node if none
   *was found.
   **/
  virtual tlp::node getSource() const;

  /**
   * @brief Finds the first node whose output degree equals 0.
   *
   * @return tlp::node The first encountered node with output degree of 0, or an invalid node if
   *none was found.
   **/
  virtual tlp::node getSink() const;

  /**
   * @brief Returns the first node in the graph.
   *
   */
  virtual node getOneNode() const = 0;

  /**
   * @brief Returns a random node in the graph.
   *
   */
  virtual node getRandomNode() const = 0;

  /**
   * @brief Return a const reference on the vector of nodes of the graph
   * It is the fastest way to access to nodes, Iterators are 25% slower.
   * @remark o(1)
   */
  virtual const std::vector<node> &nodes() const = 0;

  /**
   * @brief Return the position of a node in the vector of nodes of the graph
   * @param n The node for which the position is requested
   */
  virtual uint nodePos(const node n) const = 0;

  /**
   * @brief Gets an iterator over this graph's nodes.
   * @return An iterator over all the nodes of this graph.
   * @see getInNodes()
   * @see getOutNodes()
   * @see getInOutNodes()
   * @see getEdges()
   */
  virtual Iterator<node> *getNodes() const = 0;

  /**
   * @brief Gets the i-th node in the input nodes of a given node.
   * An input node 'in' of a node 'N' is the source of an edge going from
   * 'in' to 'N'. e.g.
   * @code
   * node in = graph->addNode();
   * node N = graph->addNode();
   * graph->addEdge(in, N);
   * //in == graph->getInNode(N, 1);
   * @endcode
   *
   * If you have 5 input nodes on a node N, then
   * @codeline graph->getInNode(2); @endcode
   * will return the second of those nodes.
   * It will ignore the output nodes of this node.
   * @param n The node to get an input node of.
   * @param i The index of the input node to get.
   * @return The i-th input node of the given node.
   * @see getInNodes()
   * @see getInEdges()
   */
  virtual node getInNode(const node n, uint i) const = 0;

  /**
   * @brief Gets an iterator over the input nodes of a node.
   * @param n The node to get the input nodes of.
   * @return An iterator over the input nodes of a node.
   * @see getInNode()
   * @see getInOutNodes()
   * @see getInEdges()
   */
  virtual Iterator<node> *getInNodes(const node n) const = 0;

  /**
   * @brief Gets the i-th node in the output nodes of a given node.
   * An output node 'out' of a node 'N' is the target of an edge going from
   * 'N' to 'out'. e.g.
   * @code
   * node N = graph->addNode();
   * node out = graph->addNode();
   * graph->addEdge(N, out);
   * //out == graph->getOutNode(N, 1);
   * @endcode
   *
   * If you have 5 output nodes on a node N, then
   * @codeline graph->getOutNode(2); @endcode
   * will return the second of those nodes.
   * It will ignore the input nodes of this node.
   * @param n The node to get an output node of.
   * @param i The index of the output node to get.
   * @return The i-th output node of the given node.
   * @see getOutNodes()
   * @see getOutEdges()
   */
  virtual node getOutNode(const node n, uint i) const = 0;

  /**
   * @brief Gets an iterator over the output nodes of a node.
   * @param n The node to get the output nodes of.
   * @return An iterator over the output nodes of a node.
   * @see getOutNode()
   * @see getInOutNodes()
   * @see getOutEdges()
   */
  virtual Iterator<node> *getOutNodes(const node n) const = 0;

  /**
   * @brief Gets an iterator over the neighbors of a given node.
   * @param n The node to retrieve the neighbors of.
   * @return An iterator over the node's neighbors.
   */
  virtual Iterator<node> *getInOutNodes(const node n) const = 0;

  /**
   * @brief Gets an iterator performing a breadth-first search on the graph.
   * @param root The node from whom to start the BFS. If not provided, the root
   * node will be assigned to a source node in the graph (node with input degree equals to 0).
   * If there is no source node in the graph, a random node will be picked.
   * @param directed if true only follow output edges, follow all edges otherwise
   * @return A vector of graph nodes in the BFS order.
   */
  virtual std::vector<node> bfs(const node root = node(), bool directed = false) const = 0;

  /**
   * @brief Gets an iterator performing a breadth-first search on the graph.
   * @param root The node from whom to start the BFS. If not provided, the root
   * node will be assigned to a source node in the graph (node with input degree equals to 0).
   * If there is no source node in the graph, a random node will be picked.
   * @param directed if true only follow output edges, follow all edges otherwise
   * @return A vector of graph edges in the BFS order.
   */
  virtual std::vector<edge> bfsEdges(const node root = node(), bool directed = false) const = 0;

  /**
   * @brief Gets an iterator performing a depth-first search on the graph.
   * @param root The node from whom to start the DFS. If not provided, the root
   * node will be assigned to a source node in the graph (node with input degree equals to 0).
   * If there is no source node in the graph, a random node will be picked.
   * @param directed if true only follow output edges, follow all edges otherwise
   * @return A vector of graph nodes in the DFS order.
   */
  virtual std::vector<node> dfs(const node root = node(), bool directed = false) const = 0;

  /**
   * @brief Gets an iterator performing a depth-first search on the graph.
   * @param root The node from whom to start the DFS. If not provided, the root
   * node will be assigned to a source node in the graph (node with input degree equals to 0).
   * If there is no source node in the graph, a random node will be picked.
   * @param directed if true only follow output edges, follow all edges otherwise
   * @return A vector of graph edges in the DFS order.
   */
  virtual std::vector<edge> dfsEdges(const node root = node(), bool directed = false) const = 0;

  /**
   * @brief Gets the underlying graph of a meta node.
   * @param metaNode The metanode.
   * @return The Graph pointed to by the metanode.
   * @see getEdgeMetaInfo()
   */
  virtual Graph *getNodeMetaInfo(const node metaNode) const = 0;

  /**
   * @brief Return a const reference on the vector of edges of the graph
   * It is the fastest way to access to edges, Iterators are 25% slower.
   * @remark o(1)
   */
  virtual const std::vector<edge> &edges() const = 0;

  /**
   * @brief Return the position of an edge in the vector of edges of the graph
   * @param e The edge for which the position is requested
   */
  virtual uint edgePos(const edge e) const = 0;

  /**
   * @brief Get an iterator over all the graph's edges.
   * @return An iterator over all the graph's edges.
   * @see getInEdges()
   * @see getOutEdges()
   * @see getInOutEdges()
   */
  virtual Iterator<edge> *getEdges() const = 0;

  /**
   * @brief Returns the first edge in the graph.
   *
   */
  virtual edge getOneEdge() const = 0;

  /**
   * @brief Returns a random edge in the graph.
   *
   */
  virtual edge getRandomEdge() const = 0;

  /**
   * @brief Gets an iterator over the output edges of a node.
   * @param n The node to get the output edges from.
   * @return An iterator over the node's output edges.
   * @see getEdges()
   * @see getOutEdges()
   * @see getInOutEdges()
   */
  virtual Iterator<edge> *getOutEdges(const node n) const = 0;

  /**
   * @brief Gets an iterator over the edges of a node.
   * @param n The node to get the edges from.
   * @return An iterator over the node's edges.
   * @see getEdges()
   * @see getOutEdges()
   * @see getInEdges()
   */
  virtual Iterator<edge> *getInOutEdges(const node n) const = 0;

  /**
   * @brief Gets an iterator over the input edges of a node.
   * @param n The node to get the input edges from.
   * @return An iterator over the node's input edges.
   * @see getEdges()
   * @see getOutEdges()
   * @see getInOutEdges()
   */
  virtual Iterator<edge> *getInEdges(const node n) const = 0;

  /**
   * @brief Gets all input/output edges of a node existing in the root graph
   * @param n The node to get the input/output edges from.
   * @return a const reference to the vector of all edges of a node
   */
  virtual const std::vector<edge> &incidence(const node n) const = 0;

  /**
   * @brief Gets an iterator over the edges composing a meta edge.
   * @param metaEdge The metaEdge to get the real edges of.
   * @return An Iterator over the edges composing the metaEdge.
   * @see getNodeMetaInfo()
   */
  virtual Iterator<edge> *getEdgeMetaInfo(const edge metaEdge) const = 0;

  /**
   * @brief sort the graph elements in ascending order
   * @warning: That operation modify the vector of nodes and the vector of edges
   * and thus devalidate all iterators.
   */
  virtual void sortElts() = 0;

  //================================================================================
  // Graph, nodes and edges information about the graph structure
  //================================================================================
  /**
   * @brief Gets the unique identifier of the graph.
   * @return The unique identifier of this graph.
   */
  uint getId() const {
    return id;
  }

  /**
   * @brief return whether the graph is empty or not.
   * @return true if the graph has no nodes, false if not.
   */
  virtual bool isEmpty() const {
    return nodes().empty();
  }

  /**
   * @brief Gets the number of nodes in this graph.
   * @return The number of nodes in this graph.
   * @see numberOfEdges()
   */
  virtual uint numberOfNodes() const = 0;

  /**
   * @brief Gets the number of edges in this graph.
   * @return The number of edges in this graph.
   * @see numberOfNodes()
   */
  virtual uint numberOfEdges() const = 0;

  /**
   * @param n The node to get the degree of.
   * @return The degree of the given node.
   */
  virtual uint deg(const node n) const = 0;

  /**
   * @brief Get the input degree of a node.
   * @param n The node to get the input degree of.
   * @return The input degree of the given node.
   */
  virtual uint indeg(const node n) const = 0;

  /**
   * @brief Get the output degree of a node.
   * @param n The node to get the output degree of.
   * @return The output degree of the given node.
   */
  virtual uint outdeg(const node n) const = 0;

  /**
   * @brief Gets the source of an edge.
   * @param e The edge to get the source of.
   * @return The source of the given edge.
   */
  virtual node source(const edge e) const = 0;

  /**
   * @brief Gets the target of an edge.
   * @param e The edge to get the target of.
   * @return The target of the given edge.
   */
  virtual node target(const edge e) const = 0;

  /**
   * @brief Gets the source and the target of an edge.
   * @param e The edge to get the ends of.
   * @return A pair whose first element is the source, and second is the target.
   */
  virtual const std::pair<node, node> &ends(const edge e) const = 0;

  /**
   * @brief Gets the opposite node  of n through e.
   * @param e The edge linking the two nodes.
   * @param n The node at one end of e.
   * @return The node at the other end of e.
   */
  virtual node opposite(const edge e, const node n) const = 0;

  /**
   * @brief Checks if an element belongs to this graph.
   * @param n The node to check if it is an element of the graph.
   * @return Whether or not the element belongs to the graph.
   */
  virtual bool isElement(const node n) const = 0;

  /**
   * @brief Checks if a node is a meta node.
   * @param n The node to check if it is a meta node.
   * @return Whether or not the node is a meta node.
   */
  virtual bool isMetaNode(const node n) const = 0;

  /**
   * @brief Checks if an element belongs to this graph.
   * @param e The edge to check if it is an element of the graph.
   * @return Whether or not the element belongs to the graph.
   */
  virtual bool isElement(const edge e) const = 0;

  /**
   * @brief Checks if an edge is a meta edge.
   * @param e The edge to check if it is a meta edge.
   * @return Whether or not the edge is a meta edge.
   */
  virtual bool isMetaEdge(const edge e) const = 0;

  /**
   * @brief Checks if an edge exists between two given nodes.
   * @param source The source of the hypothetical edge.
   * @param target The target of the hypothetical edge.
   * @param directed When set to false edges from target to source are also considered
   * @return true if such an edge exists
   */
  virtual bool hasEdge(const node source, const node target, bool directed = true) const {
    return existEdge(source, target, directed).isValid();
  }

  /**
   * @brief Returns all the edges between two nodes.
   * @param source The source of the hypothetical edges.
   * @param target The target of the hypothetical edges.
   * @param directed When set to false edges from target to source are also considered
   * @return a vector of existing edges
   */
  virtual std::vector<edge> getEdges(const node source, const node target,
                                     bool directed = true) const = 0;

  /**
   * @brief Returns the first edge found between the two given nodes.
   * @warning This function always returns an edge,
   * you need to check if this edge is valid with edge::isValid().
   * @param source The source of the hypothetical edge.
   * @param target The target of the hypothetical edge.
   * @param directed When set to false
   * an edge from target to source may also be returned
   * @return An edge that is only valid if it exists.
   */
  virtual edge existEdge(const node source, const node target, bool directed = true) const = 0;

  //================================================================================
  // Access to the graph attributes and to the node/edge property.
  //================================================================================
  /**
   * @brief Sets the name of the graph.
   * The name does not have to be unique, it is used for convenience.
   * @param name The new name of the graph.
   */
  virtual void setName(const std::string &name) = 0;

  /**
   * @brief Retrieves the name of the graph.
   * @return The name of the graph.
   */
  virtual std::string getName() const = 0;

  /**
   * @brief Gets the attributes of the graph.
   *
   * The attributes contains the name and any user-defined value.
   * @return The attributes of the graph.
   */
  const DataSet &getAttributes() const {
    return const_cast<Graph *>(this)->getNonConstAttributes();
  }

  /**
   * @brief Gets an attribute on the graph.
   * @param name The name of the attribute to set.
   * @param value The value to set.
   * @return Whether the setting of the attribute was successful.
   */
  template <typename ATTRIBUTETYPE>
  bool getAttribute(const std::string &name, ATTRIBUTETYPE &value) const;

  /**
   * @brief Gets a copy of the attribute.
   * @param name The name of the attribute to retrieve.
   * @return A copy of the attribute to retrieve.
   */
  DataType *getAttribute(const std::string &name) const;

  /**
   * @brief Sets an attribute on the graph.
   * @param name The name of the attribute to set.
   * @param value The value to set on this attribute.
   */
  template <typename ATTRIBUTETYPE>
  void setAttribute(const std::string &name, const ATTRIBUTETYPE &value);

  /**
   * @brief Sets an attribute on the graph.
   * @param name The name of the attribute to set.
   * @param value The value to set.
   */
  void setAttribute(const std::string &name, const DataType *value);

  /**
   * @brief Removes an attribute on the graph.
   * @param name The name of the attribute to remove.
   */
  void removeAttribute(const std::string &name) {
    notifyRemoveAttribute(name);
    getNonConstAttributes().remove(name);
  }

  /**
   * @brief Checks if an attribute exists.
   * @param name The name of the attribute to check for.
   * @return Whether the attribute exists.
   */
  bool existAttribute(const std::string &name) const {
    return getAttributes().exists(name);
  }

  /**
   * @brief Adds a property to the graph.
   * The graph takes ownership of the property. If you want to delete it, use
   * Graph::delLocalProperty().
   * @param name The unique identifier of the property.
   * @param prop The property to add.
   */
  virtual void addLocalProperty(const std::string &name, PropertyInterface *prop) = 0;

  /**
   * @brief Gets an existing property.
   * In DEBUG mode an assertion checks the existence of the property.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param name The unique identifier of the property.
   * @return An existing property, or nullptr if no property with the given name exists.
   */
  virtual PropertyInterface *getProperty(const std::string &name) const = 0;

  /**
   * @brief Gets a property on this graph.
   * The name of a property identifies it uniquely.
   * Either there already exists a property with the given name, in which case it is returned.
   * Either no such property exists and it is created.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   * @warning using the wrong template parameter will cause a segmentation fault.
   * @param The unique identifier of the property.
   * @return The property of given name.
   */
  template <typename PropertyType>
  PropertyType *getLocalProperty(const std::string &name);

  /**
   * @brief Gets a property on this graph or one of its ancestors.
   * If the property already exists on the graph or in one of its ancestors, it is returned.
   * Otherwise a new property is created on this graph.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @warning using the wrong propertyType will result in a segmentation fault. Using an invalid
   * property type will always return nullptr.
   * @param name The unique identifier of the property.
   * @return An existing property, or a new one if none exists with the given name.
   */
  template <typename PropertyType>
  PropertyType *getProperty(const std::string &name);

  /**
   * @brief Gets a property on this graph, and this graph only.
   * This forwards the call to the template version of getLocalProperty(), with the correct template
   * parameter deduced from the propertyType parameter.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @warning using the wrong propertyType will result in a segmentation fault. Using an invalid
   * property type will always return nullptr.
   * @param propertyName The unique identifier of the property.
   * @param propertyType A string describing the type of the property.
   * @return The property of given name.
   * @see getLocalProperty().
   */
  PropertyInterface *getLocalProperty(const std::string &propertyName,
                                      const std::string &propertyType);

  /**
   * @brief Gets a property on this graph or one of its ancestors.
   * This forwards the call to the template version of getProperty(), with the correct template
   * parameter deduced from the propertyType parameter.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @warning using the wrong propertyType will result in a segmentation fault. Using an invalid
   * property type will always return nullptr.
   * @param propertyName The unique identifier of the property.
   * @param propertyType A string describing the type of the property.
   * @return The property of given name.
   * @see getProperty().
   */
  PropertyInterface *getProperty(const std::string &propertyName, const std::string &propertyType);

  /**
   * @brief Checks if a property exists in this graph or one of its ancestors.
   * @param The unique identifier of the property.
   * @return Whether a property with the given name exists.
   */
  virtual bool existProperty(const std::string &name) const = 0;

  /**
   * @brief Checks if a property exists in this graph.
   * @param The unique identifier of the property.
   * @return Whether a property with the given name exists.
   */
  virtual bool existLocalProperty(const std::string &name) const = 0;

  /**
   * @brief Removes and deletes a property from this graph.
   * The property is removed from the graph's property pool, meaning its name can now be used by
   * another property.
   * The object is deleted and the memory freed.
   * @param name The unique identifier of the property.
   */
  virtual void delLocalProperty(const std::string &name) = 0;

  /**
   * @brief Gets an iterator over the names of the local properties of this graph.
   * @return An iterator over this graph's properties names.
   */
  virtual Iterator<std::string> *getLocalProperties() const = 0;

  /**
   * @brief Gets an iterator over the local properties of this graph.
   * @return An iterator over this graph's properties.
   */
  virtual Iterator<PropertyInterface *> *getLocalObjectProperties() const = 0;

  /**
   * @brief Gets an iterator over the names of the properties inherited from this graph's ancestors,
   * excluding this graph's local properties.
   * @return An iterator over the names of the properties this graph inherited.
   */
  virtual Iterator<std::string> *getInheritedProperties() const = 0;

  /**
   * @brief Gets an iterator over the properties inherited from this graph's ancestors,
   * excluding this graph's local properties.
   * @return An iterator over the properties this graph inherited.
   */
  virtual Iterator<PropertyInterface *> *getInheritedObjectProperties() const = 0;

  /**
   * @brief Gets an iterator over the names of all the properties attached to this graph,
   * whether they are local or inherited.
   * @return An iterator over the names of all the properties attached to this graph.
   */
  virtual Iterator<std::string> *getProperties() const = 0;

  /**
   * @brief Gets an iterator over the properties attached to this graph,
   * whether they are local or inherited.
   * @return An iterator over all of the properties attached to this graph.
   */
  virtual Iterator<PropertyInterface *> *getObjectProperties() const = 0;

  /**
   * @brief Gets a boolean property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the boolean property.
   * @return The boolean property of given name.
   */
  BooleanProperty *getLocalBooleanProperty(const std::string &propertyName);

  /**
   * @brief Gets a boolean property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the boolean property.
   * @return The boolean property of given name.
   */
  BooleanProperty *getBooleanProperty(const std::string &propertyName);

  /**
   * @brief Gets a color property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the color property.
   * @return The color property of given name.
   */
  ColorProperty *getLocalColorProperty(const std::string &propertyName);

  /**
   * @brief Gets a color property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the color property.
   * @return The color property of given name.
   */
  ColorProperty *getColorProperty(const std::string &propertyName);

  /**
   * @brief Gets a double property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the double property.
   * @return The double property of given name.
   */
  DoubleProperty *getLocalDoubleProperty(const std::string &propertyName);

  /**
   * @brief Gets a double property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the double property.
   * @return The double property of given name.
   */
  DoubleProperty *getDoubleProperty(const std::string &propertyName);

  /**
   * @brief Gets a graph property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the graph property.
   * @return The graph property of given name.
   */
  GraphProperty *getLocalGraphProperty(const std::string &propertyName);

  /**
   * @brief Gets a graph property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the graph property.
   * @return The graph property of given name.
   */
  GraphProperty *getGraphProperty(const std::string &propertyName);

  /**
   * @brief Gets an integer property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the integer property.
   * @return The integer property of given name.
   */
  IntegerProperty *getLocalIntegerProperty(const std::string &propertyName);

  /**
   * @brief Gets an integer property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the integer property.
   * @return The integer property of given name.
   */
  IntegerProperty *getIntegerProperty(const std::string &propertyName);

  /**
   * @brief Gets a layout property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the layout property.
   * @return The layout property of given name.
   */
  LayoutProperty *getLocalLayoutProperty(const std::string &propertyName);

  /**
   * @brief Gets a layout property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the layout property.
   * @return The layout property of given name.
   */
  LayoutProperty *getLayoutProperty(const std::string &propertyName);

  /**
   * @brief Gets a size property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the size property.
   * @return The size property of given name.
   */
  SizeProperty *getLocalSizeProperty(const std::string &propertyName);

  /**
   * @brief Gets a size property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the size property.
   * @return The size property of given name.
   */
  SizeProperty *getSizeProperty(const std::string &propertyName);

  /**
   * @brief Gets a string property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the string property.
   * @return The string property of given name.
   */
  StringProperty *getLocalStringProperty(const std::string &propertyName);

  /**
   * @brief Gets a string property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the string property.
   * @return The string property of given name.
   */
  StringProperty *getStringProperty(const std::string &propertyName);

  /**
   * @brief Gets a boolean vector property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the boolean vector property.
   * @return The boolean vector property of given name.
   */
  BooleanVectorProperty *getLocalBooleanVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a boolean vector property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the boolean vector property.
   * @return The boolean vector property of given name.
   */
  BooleanVectorProperty *getBooleanVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a color vector property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the color vector property.
   * @return The color vector property of given name.
   */
  ColorVectorProperty *getLocalColorVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a color vector property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the color vector property.
   * @return The color vector property of given name.
   */
  ColorVectorProperty *getColorVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a double vector property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the double vector property.
   * @return The double vector property of given name.
   */
  DoubleVectorProperty *getLocalDoubleVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a double vector property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the double vector property.
   * @return The double vector property of given name.
   */
  DoubleVectorProperty *getDoubleVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets an integer vector property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the integer vector property.
   * @return The integer vector property of given name.
   */
  IntegerVectorProperty *getLocalIntegerVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets an integer vector property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the integer vector property.
   * @return The integer vector property of given name.
   */
  IntegerVectorProperty *getIntegerVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a coord vector property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the coord vector property.
   * @return The coord vector property of given name.
   */
  CoordVectorProperty *getLocalCoordVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a coord vector property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the coord vector property.
   * @return The coord vector property of given name.
   */
  CoordVectorProperty *getCoordVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a size vector property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the size vector property.
   * @return The size vector property of given name.
   */
  SizeVectorProperty *getLocalSizeVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a size vector property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the size vector property.
   * @return The size vector property of given name.
   */
  SizeVectorProperty *getSizeVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a string vector property on this graph, and this graph only.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the string vector property.
   * @return The string vector property of given name.
   */
  StringVectorProperty *getLocalStringVectorProperty(const std::string &propertyName);

  /**
   * @brief Gets a string vector property on this graph or one of its ancestors.
   *
   * The graph keeps ownership of the property, if you wish to remove it from the graph use
   * Graph::delLocalProperty().
   *
   * @param propertyName The unique identifier of the string vector property.
   * @return The string vector property of given name.
   */
  StringVectorProperty *getStringVectorProperty(const std::string &propertyName);

  /**
   * @brief Runs a plugin on the graph, whose result is a property.
   *
   * @param algorithm The name of the plugin to run.
   * @param result The property in which to store the computed nodes/edges associated values. All
   * previous values will be erased.
   * @param errorMessage Stores the error message if the plugin fails.
   * @param parameters The parameters of the algorithm. Some algorithms use this DataSet to output
   * @param progress A PluginProgress to report the progress of the operation, as well as the final
   * state. Defaults to nullptr.
   * some additional information.
   * @return Whether the plugin applied successfully or not. If not, check the error message.
   *
   * @see PluginsManager::getPluginParameters() to retrieve the list of default parameters for the
   * plugin.
   */
  bool applyPropertyAlgorithm(const std::string &algorithm, PropertyInterface *result,
                              std::string &errorMessage, DataSet *parameters = nullptr,
                              PluginProgress *progress = nullptr);

  // updates management
  /**
   * @brief Saves the current state of the whole graph hierarchy and allows to revert to this state
   * later on, using pop().
   * All modifications except those altering the ordering of the edges will be undone.
   *
   * This allows to undo/redo modifications on a graph.
   * This is mostly useful from a user interface point of view, but some algorithms use this
   * mechanism to clean up before finishing.
   * For instance:
   * @code
   * Graph* graph = tlp::newGraph();
   * DoubleProperty* prop = graph->getDoubleProperty("metric");
   * string errorMessage;
   *
   * //our super metric stuff we want to kee
   * DoubleProperty* superProperty = graph->getDoubleProperty("superStuff");
   * vector<PropertyInterface*> propertiesToKeep;
   * propertiesToKeep.push_back(superProperty);
   *
   *
   * //apply some metric
   * graph->applyPropertyAlgorithm("Degree", prop, errorMessage);
   *
   * // save this state to be able to revert to it later
   * //however we do not want to allow to unpop(), which would go forward again to the state where
   * prop contains 'Depth'.
   * //this saves some memory.
   * //Also we always want to keep the value of our super property, so we pass it in the collection
   * of properties to leave unaffected by the pop().
   * graph->push(false, propertiesToKeep);
   *
   * //compute the quality of this metric, or whatever makes sense
   * int degreeQuality = prop->getMax();
   *
   * //compute another metric
   * graph->applyPropertyAlgorithm("Depth", prop, errorMessage);
   *
   * //compute our secret metric, that depends on depth
   * graph->applyPropertyAlgorithm("MySuperSecretAlgorithm", superProperty, errorMessage);
   *
   * //compute its quality
   * int depthQuality = prop->getMax();
   *
   * //if the degree was better, revert back to the state where its contents were in prop.
   * if(degreeQuality > depthQuality) {
   *    //this does not affect superProperty, as we told the system not to consider it when
   * recording modifications to potentially revert.
   *    graph->pop();
   * }
   *
   * //do some stuff using our high quality metric
   * ColorProperty* color = graph->getProperty("viewColor");
   * graph->applyPropertyAlgorithm("Color Mapping", color, errorMessage);
   *
   * @endcode
   *
   * @param unpopAllowed Whether or not to allow to re-do the modifications once they are undone.
   * @param propertiesToPreserveOnPop A collection of properties whose state to preserve when using
   * pop().
   * @see pop()
   * @see popIfNoUpdates()
   * @see unpop()
   * @see canPop()
   * @see canUnPop()
   * @see canPopThenUnPop()
   */
  virtual void push(bool unpopAllowed = true,
                    std::vector<PropertyInterface *> *propertiesToPreserveOnPop = nullptr) = 0;

  /**
   * @brief Undoes modifications and reverts the whole graph hierarchy back to a previous state.
   *
   * @param unpopAllowed Whether or not it is possible to redo what will be undoe by this call.
   */
  virtual void pop(bool unpopAllowed = true) = 0;

  /**
   * @brief abort last push if no updates have been recorded
   */
  virtual void popIfNoUpdates() = 0;

  /**
   * @brief Re-perform actions that were undone using pop().
   *
   * For instance:
   * @code
   * DoubleProperty* prop = graph->getDoubleProperty("metric");
   * string errorMessage;
   *
   * //apply some metric
   * graph->applyPropertyAlgorithm("Degree", prop, errorMessage);
   *
   * // save this state to be able to revert to it later
   * graph->push();
   *
   * //compute the quality of this metric, or whatever makes sense
   * int degreeQuality = prop->getMax();
   *
   * //compute another metric
   * graph->applyPropertyAlgorithm("Depth", prop, errorMessage);
   *
   * //compute its quality
   * int depthQuality = prop->getMax();
   *
   * //if the degree was better, revert back to the state where its contents were in prop.
   * if(degreeQuality > depthQuality) {
   *    graph->pop();
   * }
   *
   * ...
   *
   * //revert back to the depth for some reason.
   * graph->unpop();
   * @endcode
   */
  virtual void unpop() = 0;

  /**
   * @brief Checks if there is a state to revert to.
   * @return Whether there was a previous call to push() that was not yet pop()'ed.
   */
  virtual bool canPop() = 0;

  /**
   * @brief Checks if the last undone modifications can be redone.
   * @return Whether it is possible to re-do modifications that have been undone by pop().
   */
  virtual bool canUnpop() = 0;

  /**
   * @brief Checks if it is possible to call pop() and then unPop(), to undo then re-do
   * modifications.
   * @return Whether it is possible to undo and then redo.
   */
  virtual bool canPopThenUnpop() = 0;

  // meta nodes management
  /**
   * @brief Creates a meta-node from a vector of nodes.
   * Every edges from any node in the vector to another node of the graph will be replaced with meta
   * edges
   * from the meta node to the other nodes.
   * @warning This method will fail when called on the root graph.
   *
   * @param nodes The vector of nodes to put into the meta node.
   * @param multiEdges Whether a meta edge should be created for each underlying edge.
   * @param delAllEdge Whether the underlying edges will be removed from the whole hierarchy.
   * @return The newly created meta node.
   */
  virtual node createMetaNode(const std::vector<node> &nodes, bool multiEdges = true,
                              bool delAllEdge = true);

  /**
   *  @brief Populates a quotient graph with one meta node
   * for each iterated graph.
   *
   * @param itS a Graph iterator, (typically a subgraph iterator)
   * @param quotientGraph the graph that will contain the meta nodes
   * @param metaNodes will contains all the added meta nodes after the call
   *
   */
  virtual void createMetaNodes(Iterator<Graph *> *itS, Graph *quotientGraph,
                               std::vector<node> &metaNodes);
  /**
   * @brief Closes an existing subgraph into a metanode.  Edges from nodes
   * in the subgraph to nodes outside the subgraph are replaced with
   * edges from the metanode to the nodes outside the subgraph.
   * @warning this method will fail when called on the root graph.
   *
   * @param subGraph an existing subgraph
   * @param multiEdges indicates if a meta edge will be created for each underlying edge
   * @param delAllEdge indicates if the underlying edges will be removed from the entire hierarchy
   */
  virtual node createMetaNode(Graph *subGraph, bool multiEdges = true, bool delAllEdge = true);

  /**
   * @brief Opens a metanode and replaces all edges between that
   * meta node and other nodes in the graph.
   *
   * @warning this method will fail when called on the root graph.
   *
   * @param n The meta node to open.
   * @param updateProperties If set to true, open meta node will update inner nodes layout, color,
   * size, etc
   */
  void openMetaNode(node n, bool updateProperties = true);

  PropertyProxy operator[](const std::string &propertyName) {
    return PropertyProxy(this, propertyName);
  }

protected:
  virtual DataSet &getNonConstAttributes() = 0;
  // designed to reassign an id to a previously deleted elt
  // used by GraphUpdatesRecorder
  virtual void restoreNode(node) = 0;
  virtual void restoreEdge(edge, node source, node target) = 0;
  // designed to only update own structures
  // used by GraphUpdatesRecorder
  virtual void removeNode(const node) = 0;
  virtual void removeEdge(const edge) = 0;

  // to check if a property can be deleted
  // used by PropertyManager
  virtual bool canDeleteProperty(Graph *g, PropertyInterface *prop) {
    return getRoot()->canDeleteProperty(g, prop);
  }

  // local property renaming
  // can failed if a property with the same name already exists
  virtual bool renameLocalProperty(PropertyInterface *prop, const std::string &newName) = 0;

  // internally used to deal with sub graph deletion
  virtual void removeSubGraph(Graph *) = 0;
  virtual void clearSubGraphs() = 0;
  virtual void restoreSubGraph(Graph *) = 0;
  virtual void setSubGraphToKeep(Graph *) = 0;

  // for notification of GraphObserver
  void notifyAddNode(const node n);
  void notifyAddNode(Graph *, const node n) {
    notifyAddNode(n);
  }
  void notifyAddEdge(const edge e);
  void notifyAddEdge(Graph *, const edge e) {
    notifyAddEdge(e);
  }
  void notifyBeforeSetEnds(const edge e);
  void notifyBeforeSetEnds(Graph *, const edge e) {
    notifyBeforeSetEnds(e);
  }
  void notifyAfterSetEnds(const edge e);
  void notifyAfterSetEnds(Graph *, const edge e) {
    notifyAfterSetEnds(e);
  }
  void notifyDelNode(const node n);
  void notifyDelNode(Graph *, const node n) {
    notifyDelNode(n);
  }
  void notifyDelEdge(const edge e);
  void notifyDelEdge(Graph *, const edge e) {
    notifyDelEdge(e);
  }
  void notifyReverseEdge(const edge e);
  void notifyReverseEdge(Graph *, const edge e) {
    notifyReverseEdge(e);
  }
  void notifyBeforeAddSubGraph(const Graph *);
  void notifyAfterAddSubGraph(const Graph *);
  void notifyBeforeAddSubGraph(Graph *, const Graph *sg) {
    notifyBeforeAddSubGraph(sg);
  }
  void notifyAfterAddSubGraph(Graph *, const Graph *sg) {
    notifyAfterAddSubGraph(sg);
  }
  void notifyBeforeDelSubGraph(const Graph *);
  void notifyAfterDelSubGraph(const Graph *);
  void notifyBeforeDelSubGraph(Graph *, const Graph *sg) {
    notifyBeforeDelSubGraph(sg);
  }
  void notifyAfterDelSubGraph(Graph *, const Graph *sg) {
    notifyAfterDelSubGraph(sg);
  }

  void notifyBeforeAddDescendantGraph(const Graph *);
  void notifyAfterAddDescendantGraph(const Graph *);
  void notifyBeforeDelDescendantGraph(const Graph *);
  void notifyAfterDelDescendantGraph(const Graph *);

  void notifyBeforeAddLocalProperty(const std::string &);
  void notifyAddLocalProperty(const std::string &);
  void notifyAddLocalProperty(Graph *, const std::string &name) {
    notifyAddLocalProperty(name);
  }
  void notifyBeforeDelLocalProperty(const std::string &);
  void notifyAfterDelLocalProperty(const std::string &);
  void notifyDelLocalProperty(Graph *, const std::string &name) {
    notifyBeforeDelLocalProperty(name);
  }
  void notifyBeforeSetAttribute(const std::string &);
  void notifyBeforeSetAttribute(Graph *, const std::string &name) {
    notifyBeforeSetAttribute(name);
  }
  void notifyAfterSetAttribute(const std::string &);
  void notifyAfterSetAttribute(Graph *, const std::string &name) {
    notifyAfterSetAttribute(name);
  }
  void notifyRemoveAttribute(const std::string &);
  void notifyRemoveAttribute(Graph *, const std::string &name) {
    notifyRemoveAttribute(name);
  }
  void notifyDestroy();
  void notifyDestroy(Graph *) {
    notifyDestroy();
  }

  uint id;
  flat_hash_map<std::string, tlp::PropertyInterface *> circularCalls;
};

enum class GraphEventType {
  TLP_ADD_NODE = 0,
  TLP_DEL_NODE = 1,
  TLP_ADD_EDGE = 2,
  TLP_DEL_EDGE = 3,
  TLP_REVERSE_EDGE = 4,
  TLP_BEFORE_SET_ENDS = 5,
  TLP_AFTER_SET_ENDS = 6,
  TLP_ADD_NODES = 7,
  TLP_ADD_EDGES = 8,
  TLP_BEFORE_ADD_DESCENDANTGRAPH = 9,
  TLP_AFTER_ADD_DESCENDANTGRAPH = 10,
  TLP_BEFORE_DEL_DESCENDANTGRAPH = 11,
  TLP_AFTER_DEL_DESCENDANTGRAPH = 12,
  TLP_BEFORE_ADD_SUBGRAPH = 13,
  TLP_AFTER_ADD_SUBGRAPH = 14,
  TLP_BEFORE_DEL_SUBGRAPH = 15,
  TLP_AFTER_DEL_SUBGRAPH = 16,
  TLP_ADD_LOCAL_PROPERTY = 17,
  TLP_BEFORE_DEL_LOCAL_PROPERTY = 18,
  TLP_AFTER_DEL_LOCAL_PROPERTY = 19,
  TLP_ADD_INHERITED_PROPERTY = 20,
  TLP_BEFORE_DEL_INHERITED_PROPERTY = 21,
  TLP_AFTER_DEL_INHERITED_PROPERTY = 22,
  TLP_BEFORE_RENAME_LOCAL_PROPERTY = 23,
  TLP_AFTER_RENAME_LOCAL_PROPERTY = 24,
  TLP_BEFORE_SET_ATTRIBUTE = 25,
  TLP_AFTER_SET_ATTRIBUTE = 26,
  TLP_REMOVE_ATTRIBUTE = 27,
  TLP_BEFORE_ADD_LOCAL_PROPERTY = 28,
  TLP_BEFORE_ADD_INHERITED_PROPERTY = 29
};

/**
 * @ingroup Observation
 * Event class for specific events on Graph
 **/
class TLP_SCOPE GraphEvent : public Event {
public:
  // constructor for node/edge/nodes/edges events
  GraphEvent(const Graph &g, GraphEventType graphEvtType, uint id,
             EventType evtType = EventType::TLP_MODIFICATION)
      : Event(g, evtType), evtType(graphEvtType) {
    if (graphEvtType == GraphEventType::TLP_ADD_NODES ||
        graphEvtType == GraphEventType::TLP_ADD_EDGES) {
      info.nbElts = id;
    } else {
      info.eltId = id;
    }

    vectInfos.addedNodes = nullptr;
  }
  // constructor for subgraph events
  GraphEvent(const Graph &g, GraphEventType graphEvtType, const Graph *sg)
      : Event(g, EventType::TLP_MODIFICATION), evtType(graphEvtType) {
    info.subGraph = sg;
    vectInfos.addedNodes = nullptr;
  }

  // constructor for attribute/property events
  GraphEvent(const Graph &g, GraphEventType graphEvtType, const std::string &str,
             EventType evtType = EventType::TLP_MODIFICATION)
      : Event(g, evtType), evtType(graphEvtType) {
    info.name = new std::string(str);
    vectInfos.addedNodes = nullptr;
  }

  // constructor for rename property events
  GraphEvent(const Graph &g, GraphEventType graphEvtType, PropertyInterface *prop,
             const std::string &newName)
      : Event(g, EventType::TLP_MODIFICATION), evtType(graphEvtType) {
    info.renamedProp = new std::pair<PropertyInterface *, std::string>(prop, newName);
    vectInfos.addedNodes = nullptr;
  }

  ~GraphEvent() override;

  Graph *getGraph() const {
    return static_cast<Graph *>(sender());
  }

  node getNode() const {
    assert(evtType < GraphEventType::TLP_ADD_EDGE);
    return node(info.eltId);
  }

  edge getEdge() const {
    assert(evtType > GraphEventType::TLP_DEL_NODE && evtType < GraphEventType::TLP_ADD_NODES);
    return edge(info.eltId);
  }

  const std::vector<node> &getNodes() const;

  uint getNumberOfNodes() const {
    assert(evtType == GraphEventType::TLP_ADD_NODES);
    return info.nbElts;
  }

  const std::vector<edge> &getEdges() const;

  uint getNumberOfEdges() const {
    assert(evtType == GraphEventType::TLP_ADD_EDGES);
    return info.nbElts;
  }

  const Graph *getSubGraph() const {
    assert(evtType > GraphEventType::TLP_ADD_EDGES &&
           evtType < GraphEventType::TLP_ADD_LOCAL_PROPERTY);
    return info.subGraph;
  }

  const std::string &getAttributeName() const {
    assert(evtType > GraphEventType::TLP_AFTER_DEL_INHERITED_PROPERTY);
    return *(info.name);
  }

  const std::string &getPropertyName() const;

  PropertyInterface *getProperty() const {
    assert(evtType == GraphEventType::TLP_BEFORE_RENAME_LOCAL_PROPERTY ||
           evtType == GraphEventType::TLP_AFTER_RENAME_LOCAL_PROPERTY);
    return info.renamedProp->first;
  }

  const std::string &getPropertyNewName() const {
    assert(evtType == GraphEventType::TLP_BEFORE_RENAME_LOCAL_PROPERTY);
    return info.renamedProp->second;
  }

  const std::string &getPropertyOldName() const {
    assert(evtType == GraphEventType::TLP_AFTER_RENAME_LOCAL_PROPERTY);
    return info.renamedProp->second;
  }

  GraphEventType getType() const {
    return evtType;
  }

protected:
  GraphEventType evtType;
  union {
    uint eltId;
    const Graph *subGraph;
    std::string *name;
    uint nbElts;
    std::pair<PropertyInterface *, std::string> *renamedProp;
  } info;
  union {
    std::vector<node> *addedNodes;
    std::vector<edge> *addedEdges;
  } vectInfos;
};
}

/// Print the graph (only nodes and edges) in ostream, in the TLP format
TLP_SCOPE std::ostream &operator<<(std::ostream &, const tlp::Graph *);

#include "cxx/Graph.cxx"
#endif // TALIPOT_GRAPH_H
