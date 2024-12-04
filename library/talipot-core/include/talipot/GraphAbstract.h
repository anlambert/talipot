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

#ifndef TALIPOT_GRAPH_ABSTRACT_H
#define TALIPOT_GRAPH_ABSTRACT_H

#include <set>
#include <vector>
#include <talipot/Graph.h>
#include <talipot/DataSet.h>

namespace tlp {

template <class C>
struct Iterator;
class PropertyManager;
class GraphProperty;

/// Abstract class for default graph operations.
class TLP_SCOPE GraphAbstract : public Graph {
  friend class PropertyManager;
  DataSet attributes;
  Graph *supergraph;
  Graph *const root;
  std::vector<Graph *> subgraphs;
  Graph *subGraphToKeep;
  // pointer to root viewMetaGraph property
  GraphProperty *metaGraphProperty;

protected:
  GraphAbstract(Graph *supergraph, uint id = 0);

public:
  ~GraphAbstract() override;
  void clear() override;
  // use to enforce subgraph id
  Graph *addSubGraph(uint id, BooleanProperty *selection = nullptr,
                     const std::string &name = "unnamed");
  Graph *addSubGraph(BooleanProperty *selection = nullptr,
                     const std::string &name = "unnamed") override {
    return addSubGraph(0, selection, name);
  }
  void delSubGraph(Graph *) override;
  void delAllSubGraphs(Graph *) override;
  Graph *getSuperGraph() const override {
    return supergraph;
  }
  Graph *getRoot() const override {
    return root;
  }
  Iterator<Graph *> *getSubGraphs() const override;
  const std::vector<Graph *> &subGraphs() const override {
    return subgraphs;
  }
  bool isSubGraph(const Graph *sg) const override;
  bool isDescendantGraph(const Graph *sg) const override;
  Graph *getSubGraph(uint id) const override;
  Graph *getSubGraph(const std::string &name) const override;
  Graph *getDescendantGraph(uint id) const override;
  Graph *getDescendantGraph(const std::string &name) const override;
  Graph *getNthSubGraph(uint n) const override;
  uint numberOfSubGraphs() const override {
    return subgraphs.size();
  }
  uint numberOfDescendantGraphs() const override;

  //=======================================
  bool isMetaNode(const node) const override;
  Graph *getNodeMetaInfo(const node) const override;
  void delNodes(const std::vector<node> &nodes, bool deleteInAllGraphs = false) override;
  bool isMetaEdge(const edge) const override;
  Iterator<edge> *getEdgeMetaInfo(const edge) const override;
  void delEdges(const std::vector<edge> &edges, bool deleteInAllGraphs = false) override;
  //=======================================
  node getOneNode() const override;
  node getRandomNode() const override;
  node getInNode(const node, uint) const override;
  node getOutNode(const node, uint) const override;
  edge getOneEdge() const override;
  edge getRandomEdge() const override;
  //========================================
  bool existProperty(const std::string &) const override;
  bool existLocalProperty(const std::string &) const override;
  void delLocalProperty(const std::string &) override;
  void addLocalProperty(const std::string &name, PropertyInterface *prop) override;
  Iterator<std::string> *getLocalProperties() const override;
  Iterator<std::string> *getInheritedProperties() const override;
  Iterator<std::string> *getProperties() const override;
  Iterator<PropertyInterface *> *getLocalObjectProperties() const override;
  Iterator<PropertyInterface *> *getInheritedObjectProperties() const override;
  Iterator<PropertyInterface *> *getObjectProperties() const override;
  PropertyInterface *getProperty(const std::string &) const override;

  // to get viewMetaGraph property
  GraphProperty *getMetaGraphProperty();

  void setName(const std::string &name) override;
  std::string getName() const override;

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

protected:
  DataSet &getNonConstAttributes() override {
    return attributes;
  }
  void setSuperGraph(Graph *) override;
  PropertyManager *propertyContainer;
  const std::set<edge> &getReferencedEdges(const edge) const;

  bool renameLocalProperty(PropertyInterface *prop, const std::string &newName) override;

  // internally used to deal with sub graph deletion
  void clearSubGraphs() override;
  void removeSubGraph(Graph *) override;
  void restoreSubGraph(Graph *) override;
  void setSubGraphToKeep(Graph *) override;

private:
  void delAllSubGraphs();
  // notification of addition/deletion of inherited properties
  void notifyBeforeAddInheritedProperty(const std::string &prop);
  void notifyAddInheritedProperty(const std::string &prop);
  void notifyBeforeDelInheritedProperty(const std::string &prop);
  void notifyAfterDelInheritedProperty(const std::string &prop);
  // notification of property renaming
  void notifyBeforeRenameLocalProperty(PropertyInterface *prop, const std::string &newName);
  void notifyAfterRenameLocalProperty(PropertyInterface *prop, const std::string &oldName);
};
}
#endif // TALIPOT_GRAPH_ABSTRACT_H
