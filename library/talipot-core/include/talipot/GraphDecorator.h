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

#ifndef TALIPOT_GRAPH_DECORATOR_H
#define TALIPOT_GRAPH_DECORATOR_H

#include <talipot/Graph.h>

namespace tlp {

class TLP_SCOPE GraphDecorator : public Graph {
public:
  GraphDecorator(Graph *s);
  ~GraphDecorator() override;

  void clear() override;
  Graph *addSubGraph(BooleanProperty *selection = nullptr,
                     const std::string &name = "unnamed") override;
  void delSubGraph(Graph *s) override;
  void delAllSubGraphs(Graph *s) override;
  Graph *getSuperGraph() const override;
  void setSuperGraph(Graph *s) override;
  Graph *getRoot() const override;
  Iterator<Graph *> *getSubGraphs() const override;
  const std::vector<Graph *> &subGraphs() const override;
  bool isSubGraph(const Graph *sg) const override;
  bool isDescendantGraph(const Graph *sg) const override;
  Graph *getSubGraph(uint id) const override;
  Graph *getSubGraph(const std::string &name) const override;
  Graph *getDescendantGraph(uint id) const override;
  Graph *getDescendantGraph(const std::string &name) const override;
  uint numberOfSubGraphs() const override;
  uint numberOfDescendantGraphs() const override;

  //============================================================
  node source(const edge e) const override;
  void setSource(const edge, const node) override;
  node target(const edge e) const override;
  void setTarget(const edge, const node) override;
  node opposite(const edge e, const node n) const override;
  edge existEdge(const node n, const node m, bool directed = true) const override;
  bool hasEdge(const node source, const node target, bool directed = true) const override;
  std::vector<edge> getEdges(const node source, const node target,
                             bool directed = true) const override;
  const std::pair<node, node> &ends(const edge e) const override;
  void setEnds(const edge, const node, const node) override;
  void reverse(const edge e) override;
  uint deg(const node n) const override;
  uint indeg(const node n) const override;
  uint outdeg(const node n) const override;
  Graph *getNodeMetaInfo(const node n) const override;

  //============================================================
  node createMetaNode(const std::vector<node> &nodes, bool multiEdges = true,
                      bool delAllEdge = true) override;
  void createMetaNodes(Iterator<Graph *> *itS, Graph *quotientGraph,
                       std::vector<node> &metaNodes) override;
  node createMetaNode(Graph *subGraph, bool multiEdges = true, bool delAllEdge = true) override;

  //============================================================
  node getOneNode() const override;
  node getRandomNode() const override;
  node getInNode(const node n, uint i) const override;
  node getOutNode(const node n, uint i) const override;
  edge getOneEdge() const override;
  edge getRandomEdge() const override;

  uint numberOfNodes() const override;
  uint numberOfEdges() const override;
  bool isEmpty() const override;

  //=========================================================================

  void reserveNodes(uint nbNodes) override;
  void reserveEdges(uint nbEdges) override;

  //============================================================
  bool isElement(const node n) const override;
  bool isMetaNode(const node n) const override;
  bool isElement(const edge e) const override;
  bool isMetaEdge(const edge e) const override;
  node addNode() override;
  std::vector<node> addNodes(uint nb) override;
  void addNode(const node) override;
  void addNodes(const std::vector<node> &nodes) override;
  edge addEdge(const node n1, const node n2) override;
  std::vector<edge> addEdges(const std::vector<std::pair<node, node>> &edges) override;
  void addEdge(const edge) override;
  void addEdges(const std::vector<edge> &edges) override;
  void delNode(const tlp::node n, bool deleteInAllGraphs = false) override;
  void delNodes(const std::vector<node> &nodes, bool deleteInAllGraphs) override;
  void delEdge(const tlp::edge e, bool deleteInAllGraphs = false) override;
  void delEdges(const std::vector<edge> &edges, bool deleteInAllGraphs = false) override;
  void setEdgeOrder(const node n, const std::vector<edge> &s) override;
  void swapEdgeOrder(const node n, const edge e1, const edge e2) override;

  const std::vector<node> &nodes() const override;
  uint nodePos(const node) const override;
  Iterator<node> *getNodes() const override;
  Iterator<node> *getInNodes(const node n) const override;
  Iterator<node> *getOutNodes(const node n) const override;
  Iterator<node> *getInOutNodes(const node n) const override;
  std::vector<node> bfs(const node root = node(), bool directed = false) const override;
  std::vector<node> dfs(const node root, bool directed = false) const override;
  std::vector<node> dfs(bool directed = false) const override;
  void dfs(node root, const std::function<bool(const Graph *, node)> &inVisitCallback,
           const std::function<bool(const Graph *, node)> &outVisitCallback,
           bool directed = false) const override;
  void dfs(const std::function<bool(const Graph *, node)> &inVisitCallback,
           const std::function<bool(const Graph *, node)> &outVisitCallback,
           bool directed = false) const override;
  std::vector<edge> bfsEdges(const node root = node(), bool directed = false) const override;
  std::vector<edge> dfsEdges(const node root, bool directed = false) const override;
  std::vector<edge> dfsEdges(bool directed = false) const override;
  const std::vector<edge> &edges() const override;
  uint edgePos(const edge) const override;
  Iterator<edge> *getEdges() const override;
  Iterator<edge> *getOutEdges(const node n) const override;
  Iterator<edge> *getInOutEdges(const node n) const override;
  Iterator<edge> *getInEdges(const node n) const override;
  const std::vector<edge> &incidence(const node n) const override;
  Iterator<edge> *getEdgeMetaInfo(const edge) const override;
  void sortElts() override;
  //============================================================
  template <typename PropertyType>
  PropertyType *getProperty(const std::string &name);
  PropertyInterface *getProperty(const std::string &name) const override;
  bool existProperty(const std::string &name) const override;
  bool existLocalProperty(const std::string &name) const override;
  void delLocalProperty(const std::string &name) override;
  void addLocalProperty(const std::string &name, PropertyInterface *prop) override;
  Iterator<std::string> *getLocalProperties() const override;
  Iterator<std::string> *getInheritedProperties() const override;
  Iterator<std::string> *getProperties() const override;
  Iterator<PropertyInterface *> *getLocalObjectProperties() const override;
  Iterator<PropertyInterface *> *getInheritedObjectProperties() const override;
  Iterator<PropertyInterface *> *getObjectProperties() const override;
  // updates management
  void push(bool unpopAllowed = true,
            std::vector<PropertyInterface *> *propertiesToPreserveOnPop = nullptr) override;
  void pop(bool unpopAllowed = true) override;
  void popIfNoUpdates() override;
  void unpop() override;
  bool canPop() override;
  bool canUnpop() override;
  bool canPopThenUnpop() override;
  //============================================================

  void setName(const std::string &name) override;
  std::string getName() const override;

protected:
  DataSet &getNonConstAttributes() override;
  Graph *graph_component;
  // designed to reassign an id to a previously deleted elt
  // called by GraphUpdatesRecorder
  void restoreNode(node) override;
  void restoreEdge(edge, node source, node target) override;
  // designed to only update own structures
  // used by GraphUpdatesRecorder
  void removeNode(const node) override;
  void removeEdge(const edge) override;
  // internally used to deal with sub graph deletion
  void removeSubGraph(Graph *) override;
  void clearSubGraphs() override;
  void restoreSubGraph(Graph *) override;
  void setSubGraphToKeep(Graph *) override;
  // internally used for property renaming
  bool renameLocalProperty(PropertyInterface *prop, const std::string &newName) override;
};

//====================================================================================
template <typename PropertyType>
PropertyType *GraphDecorator::getProperty(const std::string &name) {
  if (graph_component->existProperty(name)) {
    tlp::PropertyInterface *prop = graph_component->getProperty(name);
    assert(dynamic_cast<PropertyType *>(prop) != nullptr);
    return dynamic_cast<PropertyType *>(prop);
  } else {
    return graph_component->getLocalProperty<PropertyType>(name);
  }
}
}

#endif // TALIPOT_GRAPH_DECORATOR_H
