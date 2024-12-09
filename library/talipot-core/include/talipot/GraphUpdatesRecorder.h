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

#ifndef TALIPOT_GRAPH_UPDATES_RECORDER_H
#define TALIPOT_GRAPH_UPDATES_RECORDER_H

#include <string>
#include <set>
#include <talipot/hash.h>
#include <unordered_set>
#include <vector>

#include <talipot/Graph.h>
#include <talipot/MutableContainer.h>

namespace std {
template <>
struct less<tlp::Graph *> {
  size_t operator()(const tlp::Graph *g1, const tlp::Graph *g2) const {
    // the id order corresponds to the creation order
    // so when dealing with a set<Graph*> this will ensure that
    // we encounter a supergraph before its subgraphs
    return g1->getId() < g2->getId();
  }
};
} // namespace std

namespace tlp {
class GraphImpl;
struct GraphStorageIdsMemento;

class GraphUpdatesRecorder : public Observable {
  friend class GraphImpl;
//
#if !defined(NDEBUG)
  bool recordingStopped;
#endif
  bool updatesReverted;
  bool restartAllowed;
  bool newValuesRecorded;
  const bool oldIdsStateRecorded;

  // one 'set' of added nodes per graph
  flat_hash_map<Graph *, std::unordered_set<node>> graphAddedNodes;
  // the whole 'set' of added nodes
  std::unordered_set<node> addedNodes;
  // one 'set' of deleted nodes per graph
  flat_hash_map<Graph *, std::unordered_set<node>> graphDeletedNodes;
  // one 'set' of added edges per graph
  std::map<Graph *, std::unordered_set<edge>> graphAddedEdges;
  // ends of all added edges
  flat_hash_map<edge, std::pair<node, node>> addedEdgesEnds;
  // one 'set' of deleted edges per graph
  std::map<Graph *, std::unordered_set<edge>> graphDeletedEdges;
  // ends of all deleted edges
  flat_hash_map<edge, std::pair<node, node>> deletedEdgesEnds;
  // one set of reverted edges
  std::unordered_set<edge> revertedEdges;
  // source + target per updated edge
  flat_hash_map<edge, std::pair<node, node>> oldEdgesEnds;
  // source + target per updated edge
  flat_hash_map<edge, std::pair<node, node>> newEdgesEnds;
  // one 'set' for old incidences
  flat_hash_map<node, std::vector<edge>> oldIncidences;
  // one 'set' for new incidences
  flat_hash_map<node, std::vector<edge>> newIncidences;

  // copy of nodes/edges id manager state at start time
  const GraphStorageIdsMemento *oldIdsState;
  // copy of nodes/edges id manager state at stop time
  const GraphStorageIdsMemento *newIdsState;

  // one list of (parent graph, added subgraph)
  std::list<std::pair<Graph *, Graph *>> addedSubGraphs;
  // one list of (parent graph, deleted subgraph)
  std::list<std::pair<Graph *, Graph *>> deletedSubGraphs;

  // one set of added properties per graph
  flat_hash_map<Graph *, std::set<PropertyInterface *>> addedProperties;
  // one set of deleted properties per graph
  flat_hash_map<Graph *, std::set<PropertyInterface *>> deletedProperties;
  // one set of old attribute values per graph
  flat_hash_map<Graph *, DataSet> oldAttributeValues;
  // one set of new attribute values per graph
  flat_hash_map<Graph *, DataSet> newAttributeValues;

  // one set of updated addNodes per property
  flat_hash_map<PropertyInterface *, std::set<node>> updatedPropsAddedNodes;

  // one set of updated addEdges per property
  flat_hash_map<PropertyInterface *, std::set<edge>> updatedPropsAddedEdges;

  // the old default node value for each updated property
  flat_hash_map<PropertyInterface *, DataMem *> oldNodeDefaultValues;
  // the new default node value for each updated property
  flat_hash_map<PropertyInterface *, DataMem *> newNodeDefaultValues;
  // the old default edge value for each updated property
  flat_hash_map<PropertyInterface *, DataMem *> oldEdgeDefaultValues;
  // the new default edge value for each updated property
  flat_hash_map<PropertyInterface *, DataMem *> newEdgeDefaultValues;
  // the old name for each renamed property
  flat_hash_map<PropertyInterface *, std::string> renamedProperties;

  struct RecordedValues {
    PropertyInterface *values;
    MutableContainer<bool> *recordedNodes;
    MutableContainer<bool> *recordedEdges;

    RecordedValues(PropertyInterface *prop = nullptr, MutableContainer<bool> *rn = nullptr,
                   MutableContainer<bool> *re = nullptr)
        : values(prop), recordedNodes(rn), recordedEdges(re) {}
  };

  // the old nodes/edges values for each updated property
  flat_hash_map<PropertyInterface *, RecordedValues> oldValues;
  // the new node value for each updated property
  flat_hash_map<PropertyInterface *, RecordedValues> newValues;

  // real deletion of deleted objects (properties, sub graphs)
  // during the recording of updates these objects are removed from graph
  // structures but not really 'deleted'
  void deleteDeletedObjects();
  // deletion of recorded values
  void deleteValues(flat_hash_map<PropertyInterface *, RecordedValues> &values);
  // deletion of DataMem default values
  void deleteDefaultValues(flat_hash_map<PropertyInterface *, DataMem *> &values);
  // record of a node's edges container before/after modification
  void recordIncidence(flat_hash_map<node, std::vector<edge>> &, GraphImpl *, node,
                       edge e = edge());
  void recordIncidence(flat_hash_map<node, std::vector<edge>> &, GraphImpl *, node,
                       const std::vector<edge> &, uint);
  // remove an edge from a node's edges container
  void removeFromIncidence(flat_hash_map<node, std::vector<edge>> &, edge, node);

  void removeGraphData(Graph *);

  // record new values for all updated properties
  // restartAllowed must be true
  void recordNewValues(GraphImpl *);
  void recordNewNodeValues(PropertyInterface *p);
  void recordNewEdgeValues(PropertyInterface *p);

  // start of recording (push)
  void startRecording(GraphImpl *);
  // end of recording
  // push an other recorder or pop this one
  void stopRecording(Graph *);
  // restart of recording (unpop)
  void restartRecording(Graph *);
  // perform undo/redo updates
  void doUpdates(GraphImpl *, bool undo);
  // check for recorded updates
  bool hasUpdates();
  // remove a property from the observed ones
  // only if nothing is yet recorded for that property
  bool dontObserveProperty(PropertyInterface *);
  // check if the property is newly added or deleted
  bool isAddedOrDeletedProperty(Graph *, PropertyInterface *);

public:
  GraphUpdatesRecorder(bool allowRestart = true,
                       const GraphStorageIdsMemento *prevIdsMemento = nullptr);
  ~GraphUpdatesRecorder() override;

  // old GraphObserver interface
  // graphAddedNodes
  void addNode(Graph *g, const node n);

  // graphAddedEdges
  void addEdge(Graph *g, const edge e);

  void addEdges(Graph *g, uint nbAddedEdges);

  // graphDeletedNodes
  void delNode(Graph *g, const node n);

  // graphDeletedEdges
  void delEdge(Graph *g, const edge e);

  // revertedEdges
  void reverseEdge(Graph *g, const edge e);

  // oldEdgesEnds
  void beforeSetEnds(Graph *g, const edge e);

  // newEdgesEnds
  void afterSetEnds(Graph *g, const edge e);

  // addedSubGraphs
  void addSubGraph(Graph *g, Graph *sg);

  // deletedSubGraphs
  void delSubGraph(Graph *g, Graph *sg);

  // addedProperties
  void addLocalProperty(Graph *g, const std::string &name);

  // deletedProperties
  void delLocalProperty(Graph *g, const std::string &name);

  // beforeSetAttribute
  void beforeSetAttribute(Graph *g, const std::string &name);

  // removeAttribute
  void removeAttribute(Graph *g, const std::string &name);

  // old PropertyObserver Interface
  // oldValues
  void beforeSetNodeValue(PropertyInterface *p, const node n);

  // oldNodeDefaultValues
  void beforeSetAllNodeValue(PropertyInterface *p);

  // oldValues
  void beforeSetEdgeValue(PropertyInterface *p, const edge e);

  // oldEdgeDefaultValues
  void beforeSetAllEdgeValue(PropertyInterface *p);

  // renamedProperties
  void propertyRenamed(PropertyInterface *p);

protected:
  // override Observable::treatEvent
  void treatEvent(const Event &ev) override;
};
}

#endif // TALIPOT_GRAPH_UPDATES_RECORDER_H
