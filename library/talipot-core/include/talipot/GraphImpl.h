/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GRAPH_IMPL_H
#define TALIPOT_GRAPH_IMPL_H

#include <set>

#include <vector>
#include <list>
#include <talipot/GraphAbstract.h>
#include <talipot/GraphStorage.h>
#include <talipot/IdManager.h>

namespace tlp {
template <class itType>
struct Iterator;
class GraphView;
class GraphUpdatesRecorder;

/// Implementation of the graph support of the Graph.
class TLP_SCOPE GraphImpl : public GraphAbstract {

    friend class GraphUpdatesRecorder;
    friend class TLPExport;

  public:
    GraphImpl();
    ~GraphImpl() override;
    void clear() override;
    //=========================================================================
    bool isElement(const node n) const override {
        return storage.isElement(n);
    }
    bool isElement(const edge e) const override {
        return storage.isElement(e);
    }
    edge existEdge(const node source, const node target, bool directed = true) const override;
    node addNode() override;
    std::vector<node> addNodes(uint nb) override;
    void addNode(const node) override;
    void addNodes(const std::vector<node> &nodes) override;
    edge addEdge(const node, const node) override;
    std::vector<edge> addEdges(const std::vector<std::pair<node, node>> &edges) override;
    void addEdge(const edge) override;
    void addEdges(const std::vector<edge> &edges) override;
    void delNode(const tlp::node n, bool deleteInAllGraphs = false) override;
    void delEdge(const tlp::edge e, bool deleteInAllGraphs = false) override;
    void setEdgeOrder(const node n, const std::vector<edge> &edges) override {
        assert(isElement(n));
        assert(edges.size() == deg(n));
#ifndef NDEBUG
        for (auto e : edges) {
            assert(isElement(e));
        }
#endif
        storage.setEdgeOrder(n, edges);
    }
    void swapEdgeOrder(const node n, const edge e1, const edge e2) override {
        assert(isElement(n));
        assert(isElement(e1));
        assert(isElement(e2));
        storage.swapEdgeOrder(n, e1, e2);
    }
    //=========================================================================
    const std::vector<node> &nodes() const override {
        return storage.nodes();
    }
    uint nodePos(const node n) const override {
        return storage.nodePos(n);
    }
    Iterator<node> *getNodes() const override;
    Iterator<node> *getInNodes(const node) const override;
    Iterator<node> *getOutNodes(const node) const override;
    Iterator<node> *getInOutNodes(const node) const override;
    const std::vector<edge> &edges() const override {
        return storage.edges();
    }
    uint edgePos(const edge e) const override {
        return storage.edgePos(e);
    }
    Iterator<edge> *getEdges() const override;
    Iterator<edge> *getInEdges(const node) const override;
    Iterator<edge> *getOutEdges(const node) const override;
    Iterator<edge> *getInOutEdges(const node) const override;
    std::vector<edge> getEdges(const node source, const node target,
                               bool directed = true) const override;
    std::vector<edge> getEdges(const node source, const node target, bool directed,
                               const Graph *sg = nullptr) const {
        return storage.getEdges(source, target, directed, sg);
    }
    const std::vector<edge> &incidence(const node n) const override {
        return storage.incidence(n);
    }
    //========================================================================
    uint deg(const node n) const override {
        assert(isElement(n));
        return storage.deg(n);
    }
    uint indeg(const node n) const override {
        assert(isElement(n));
        return storage.indeg(n);
    }
    uint outdeg(const node n) const override {
        assert(isElement(n));
        return storage.outdeg(n);
    }
    //========================================================================
    node source(const edge e) const override {
        assert(isElement(e));
        return storage.source(e);
    }
    node target(const edge e) const override {
        assert(isElement(e));
        return storage.target(e);
    }
    node opposite(const edge e, const node n) const override {
        assert(isElement(e));
        return storage.opposite(e, n);
    }
    const std::pair<node, node> &ends(const edge e) const override {
        return storage.ends(e);
    }
    void setSource(const edge e, const node newSrc) override {
        assert(isElement(e));
        this->setEnds(e, newSrc, node());
    }
    void setTarget(const edge e, const node newTgt) override {
        assert(isElement(e));
        this->setEnds(e, node(), newTgt);
    }
    void setEnds(const edge, const node, const node) override;
    void reverse(const edge) override;
    //=======================================================================
    uint numberOfEdges() const override {
        return storage.numberOfEdges();
    }
    uint numberOfNodes() const override {
        return storage.numberOfNodes();
    }
    void sortElts() override {
        storage.sortElts();
    }
    //=======================================================================
    // updates management
    void push(bool unpopAllowed = true,
              std::vector<PropertyInterface *> *propertiesToPreserveOnPop = nullptr) override;
    void pop(bool unpopAllowed = true) override;
    void popIfNoUpdates() override;
    void unpop() override;
    bool canPop() override;
    bool canUnpop() override;
    bool canPopThenUnpop() override;

    // observer interface
    void treatEvents(const std::vector<Event> &) override;

    // for subgraph id management
    uint getSubGraphId(uint id);
    void freeSubGraphId(uint id);

    // to improve memory allocation
    // attempt to reserve enough space to store nodes/edges
    void reserveNodes(uint nbNodes) override;
    void reserveEdges(uint nbEdges) override;

  protected:
    // designed to reassign an id to a previously deleted elt
    // used by GraphUpdatesRecorder
    void restoreNode(node) override;
    void restoreEdge(edge, node source, node target) override;
    // designed to only update own structures
    // used by GraphUpdatesRecorder
    void removeNode(const node) override;
    void removeEdge(const edge) override;
    // used by PropertyManager
    bool canDeleteProperty(Graph *g, PropertyInterface *prop) override;

  private:
    GraphStorage storage;
    IdManager graphIds;
    std::list<GraphUpdatesRecorder *> previousRecorders;
    std::list<Graph *> observedGraphs;
    std::list<PropertyInterface *> observedProps;
    std::list<GraphUpdatesRecorder *> recorders;

    void observeUpdates(Graph *);
    void unobserveUpdates();
    void delPreviousRecorders();
};
}
#endif // TALIPOT_GRAPH_IMPL_H
