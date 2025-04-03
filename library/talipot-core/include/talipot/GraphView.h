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

#ifndef TALIPOT_GRAPH_VIEW_H
#define TALIPOT_GRAPH_VIEW_H

#include <vector>
#include <talipot/GraphAbstract.h>
#include <talipot/GraphImpl.h>
#include <talipot/IdManager.h>
#include <talipot/VectorProperty.h>

namespace tlp {
// used for node management
struct SGraphNodeData {
    uint outDegree = 0;
    std::vector<edge> incidence;

    void inEdgeAdd(edge e) {
        incidence.reserve(incidence.size() + 1);
        incidence.push_back(e);
    }
    void outEdgeAdd(edge e) {
        incidence.reserve(incidence.size() + 1);
        incidence.push_back(e);
        outDegree += 1;
    }
    void inEdgeRemove(edge e) {
        removeEdge(e);
    }
    void outEdgeRemove(edge e) {
        removeEdge(e);
        outDegree -= 1;
    }
    void swapEdgeOrder(edge e1, edge e2) {
        auto ite1 = std::find(incidence.begin(), incidence.end(), e1);
        auto ite2 = std::find(incidence.begin(), incidence.end(), e2);
        std::swap(*ite1, *ite2);
    }

  private:
    void removeEdge(edge e) {
        incidence.erase(std::find(incidence.begin(), incidence.end(), e));
    }
};

/**
 * This class is one of the implementation of the Graph Interface
 * It only filters the elements of its parents.
 */
class GraphView : public GraphAbstract {

    GraphImpl *getRootImpl() const {
        return static_cast<GraphImpl *>(getRoot());
    }

    friend class GraphImpl;

  public:
    GraphView(Graph *supergraph, BooleanProperty *filter, uint id);
    ~GraphView() override;
    //========================================================================
    node addNode() override;
    std::vector<node> addNodes(uint nb) override;
    void addNode(const node) override;
    void addNodes(const std::vector<node> &nodes) override;
    edge addEdge(const node n1, const node n2) override;
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
        _nodeData[n].incidence = edges;
    }
    void swapEdgeOrder(const node n, const edge e1, const edge e2) override {
        assert(isElement(n));
        assert(isElement(e1));
        assert(isElement(e2));
        _nodeData[n].swapEdgeOrder(e1, e2);
    }
    //=========================================================================
    bool isElement(const node n) const override {
        return _nodes.isElement(n);
    }
    bool isElement(const edge e) const override {
        return _edges.isElement(e);
    }
    edge existEdge(const node source, const node target, bool directed) const override;
    uint numberOfNodes() const override {
        return _nodes.size();
    }
    uint numberOfEdges() const override {
        return _edges.size();
    }
    //=========================================================================
    uint deg(const node n) const override {
        assert(isElement(n));
        return _nodeData[n].incidence.size();
    }
    uint indeg(const node n) const override {
        assert(isElement(n));
        const SGraphNodeData &nData = _nodeData[n];
        return nData.incidence.size() - nData.outDegree;
    }
    uint outdeg(const node n) const override {
        assert(isElement(n));
        return _nodeData[n].outDegree;
    }
    //=========================================================================
    node source(const edge e) const override {
        return getRootImpl()->source(e);
    }
    void setSource(const edge e, const node newSrc) override {
        assert(isElement(e));
        getRootImpl()->setEnds(e, newSrc, node());
    }
    node target(const edge e) const override {
        return getRootImpl()->target(e);
    }
    void setTarget(const edge e, const node newTgt) override {
        assert(isElement(e));
        getRootImpl()->setEnds(e, node(), newTgt);
    }
    const std::pair<node, node> &ends(const edge e) const override {
        return getRootImpl()->ends(e);
    }
    void setEnds(const edge e, const node newSrc, const node newTgt) override {
        assert(isElement(e));
        getRootImpl()->setEnds(e, newSrc, newTgt);
    }
    node opposite(const edge e, const node n) const override {
        assert(isElement(e));
        assert(isElement(n));
        return getRootImpl()->opposite(e, n);
    }
    void reverse(const edge e) override {
        assert(isElement(e));
        getRootImpl()->reverse(e);
    }
    //=========================================================================
    const std::vector<node> &nodes() const override {
        return _nodes;
    }
    uint nodePos(const node n) const override {
        return _nodes.getPos(n);
    }
    Iterator<node> *getNodes() const override;
    Iterator<node> *getInNodes(const node) const override;
    Iterator<node> *getOutNodes(const node) const override;
    Iterator<node> *getInOutNodes(const node) const override;
    const std::vector<edge> &edges() const override {
        return _edges;
    }
    uint edgePos(const edge e) const override {
        return _edges.getPos(e);
    }
    Iterator<edge> *getEdges() const override;
    Iterator<edge> *getInEdges(const node) const override;
    Iterator<edge> *getOutEdges(const node) const override;
    Iterator<edge> *getInOutEdges(const node) const override;
    std::vector<edge> getEdges(const node source, const node target,
                               bool directed = true) const override;
    const std::vector<edge> &incidence(const node n) const override {
        return _nodeData[n].incidence;
    }
    void sortElts() override {
        _nodes.sort();
        _edges.sort();
    }
    Graph *getRoot() const override {
        // handle root destruction (see GraphAbstract destructor)
        return id == 0 ? const_cast<GraphView *>(this) : GraphAbstract::getRoot();
    }
    //=========================================================================
    // only implemented on a root graph
    void reserveNodes(uint nbNodes) override;
    void reserveEdges(uint nbEdges) override;
    //=========================================================================
    // updates management
    void push(bool unpopAllowed = true,
              std::vector<PropertyInterface *> *propertiesToPreserveOnPop = nullptr) override;
    void pop(bool unpopAllowed = true) override;
    void popIfNoUpdates() override;
    void unpop() override;
    bool canPop() override;
    bool canUnpop() override;
    bool canPopThenUnpop() override;

  protected:
    // designed to reassign an id to a previously deleted elt
    // used by GraphUpdatesRecorder
    void restoreNode(node) override;
    void restoreEdge(edge, node source, node target) override;
    // designed to only update own structures
    // used by GraphUpdatesRecorder
    void removeNode(const node) override;
    void removeEdge(const edge) override;
    void removeNode(const node n, const std::vector<edge> &edges);
    void removeEdges(const std::vector<edge> &edges);

  private:
    tlp::NodeVectorProperty<SGraphNodeData> _nodeData;
    SGraphIdContainer<node> _nodes;
    SGraphIdContainer<edge> _edges;
    edge addEdgeInternal(edge);
    void reverseInternal(const edge, const node src, const node tgt);
    void setEndsInternal(const edge, node src, node tgt, const node newSrc, const node newTgt);
    void addNodesInternal(const std::vector<node> &nodes);
    void addEdgesInternal(const std::vector<edge> &edges);
};
}
#endif // TALIPOT_GRAPH_VIEW_H
