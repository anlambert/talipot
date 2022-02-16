/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Graph.h>
#include <talipot/Coord.h>

template <typename NodeType, typename EdgeType, typename PropType>
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::MinMaxProperty(
    tlp::Graph *graph, const std::string &name, REAL_TYPE(NodeType) nodeMin,
    REAL_TYPE(NodeType) nodeMax, REAL_TYPE(EdgeType) edgeMin, REAL_TYPE(EdgeType) edgeMax)
    : AbstractProperty<NodeType, EdgeType, PropType>(graph, name), _nodeMin(nodeMin),
      _nodeMax(nodeMax), _edgeMin(edgeMin), _edgeMax(edgeMax), _needGraphListener(false) {}

template <typename NodeType, typename EdgeType, typename PropType>
TYPE_CONST_REFERENCE(NodeType)
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getNodeMin(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  uint graphId = graph->getId();

  if (const auto it = _minMaxNode.find(graphId); it == _minMaxNode.end()) {
    computeMinMaxNode(graph);
    return _minMaxNode[graphId].first;
  } else {
    return it->second.first;
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
TYPE_CONST_REFERENCE(NodeType)
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getNodeMax(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  uint graphId = graph->getId();

  if (const auto it = _minMaxNode.find(graphId); it == _minMaxNode.end()) {
    computeMinMaxNode(graph);
    return _minMaxNode[graphId].second;
  } else {
    return it->second.second;
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
TYPE_CONST_REFERENCE(EdgeType)
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getEdgeMin(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  uint graphId = graph->getId();

  if (const auto it = _minMaxEdge.find(graphId); it == _minMaxEdge.end()) {
    computeMinMaxEdge(graph);
    return _minMaxEdge[graphId].first;
  } else {
    return it->second.first;
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
TYPE_CONST_REFERENCE(EdgeType)
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getEdgeMax(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  uint graphId = graph->getId();

  if (const auto it = _minMaxEdge.find(graphId); it == _minMaxEdge.end()) {
    computeMinMaxEdge(graph);
    return _minMaxEdge[graphId].second;
  } else {
    return it->second.second;
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
MINMAX_PAIR(NodeType)
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::computeMinMaxNode(const Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  auto maxN = _nodeMin, minN = _nodeMax;

  if (AbstractProperty<NodeType, EdgeType, PropType>::hasNonDefaultValuatedNodes(graph)) {
    for (auto n : graph->nodes()) {
      TYPE_CONST_REFERENCE(NodeType) tmp = this->getNodeValue(n);
      minN = std::min(minN, tmp);
      maxN = std::max(maxN, tmp);
    }
  }

  if (maxN < minN) {
    maxN = minN = AbstractProperty<NodeType, EdgeType, PropType>::nodeDefaultValue;
  }

  uint sgi = graph->getId();

  // graph observation is now delayed
  // until we need to do some minmax computation
  // this will minimize the graph loading
  if (_minMaxNode.find(sgi) == _minMaxNode.end() && _minMaxEdge.find(sgi) == _minMaxEdge.end()) {
    // launch graph hierarchy observation
    graph->addListener(this);
  }

  return _minMaxNode[sgi] = {minN, maxN};
}

template <typename NodeType, typename EdgeType, typename PropType>
MINMAX_PAIR(EdgeType)
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::computeMinMaxEdge(const Graph *graph) {
  auto maxE = _edgeMin, minE = _edgeMax;

  if (AbstractProperty<NodeType, EdgeType, PropType>::hasNonDefaultValuatedEdges(graph)) {
    for (auto ite : graph->edges()) {
      TYPE_CONST_REFERENCE(EdgeType) tmp = this->getEdgeValue(ite);
      minE = std::min(minE, tmp);
      maxE = std::max(maxE, tmp);
    }
  }

  if (maxE < minE) {
    maxE = minE = AbstractProperty<NodeType, EdgeType, PropType>::edgeDefaultValue;
  }

  uint sgi = graph->getId();

  // graph observation is now delayed
  // until we need to do some minmax computation
  // this will minimize the graph loading time
  if (_minMaxNode.find(sgi) == _minMaxNode.end() && _minMaxEdge.find(sgi) == _minMaxEdge.end()) {
    // launch graph hierarchy observation
    graph->addListener(this);
  }

  return _minMaxEdge[sgi] = {minE, maxE};
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::removeListenersAndClearNodeMap() {
  // we need to clear one of our map
  // this will invalidate some minmax computations
  // so the graphs corresponding to these cleared minmax computations
  // may not have to be longer observed if they have no validated
  // minmax computation in the other map

  // loop to remove unneeded graph observation
  // it is the case if minmax computation
  for (const auto &[graphId, minMax] : _minMaxNode) {

    if (const auto itg = _minMaxEdge.find(graphId); itg == _minMaxEdge.end()) {
      // no computation in the other map
      // we can stop observing the current graph
      Graph *g = (PropType::graph->getId() == graphId)
                     ? (_needGraphListener ? nullptr : PropType::graph)
                     : PropType::graph->getDescendantGraph(graphId);

      if (g) {
        g->removeListener(this);
      }
    }
  }

  // finally clear the map
  _minMaxNode.clear();
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::removeListenersAndClearEdgeMap() {
  // we need to clear one of our map
  // this will invalidate some minmax computations
  // so the graphs corresponding to these cleared minmax computations
  // may not have to be longer observed if they have no validated
  // minmax computation in the other map

  // loop to remove unneeded graph observation
  // it is the case if minmax computation
  for (const auto &[graphId, minMax] : _minMaxEdge) {

    if (const auto itg = _minMaxNode.find(graphId); itg == _minMaxNode.end()) {
      // no computation in the other map
      // we can stop observing the current graph
      Graph *g = (PropType::graph->getId() == graphId)
                     ? (_needGraphListener ? nullptr : PropType::graph)
                     : PropType::graph->getDescendantGraph(graphId);

      if (g) {
        g->removeListener(this);
      }
    }
  }

  // finally clear the map
  _minMaxEdge.clear();
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::updateNodeValue(
    tlp::node n, TYPE_CONST_REFERENCE(NodeType) newValue) {

  if (!_minMaxNode.empty()) {
    TYPE_CONST_REFERENCE(NodeType) oldV = this->getNodeValue(n);

    if (newValue != oldV) {
      // loop on subgraph min/max
      for (const auto &[graphId, minMax] : _minMaxNode) {
        // if min/max is ok for the current subgraph
        // check if min or max has to be updated
        const auto &[minV, maxV] = minMax;

        // check if min or max has to be updated
        if ((newValue < minV) || (newValue > maxV) || (oldV == minV) || (oldV == maxV)) {
          removeListenersAndClearNodeMap();
          break;
        }
      }
    }
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::updateEdgeValue(
    tlp::edge e, TYPE_CONST_REFERENCE(EdgeType) newValue) {

  if (!_minMaxEdge.empty()) {
    TYPE_CONST_REFERENCE(EdgeType) oldV = this->getEdgeValue(e);

    if (newValue != oldV) {
      // loop on subgraph min/max
      for (const auto &[graphId, minMax] : _minMaxEdge) {
        // if min/max is ok for the current subgraph
        // check if min or max has to be updated
        const auto &[minV, maxV] = minMax;

        // check if min or max has to be updated
        if ((newValue < minV) || (newValue > maxV) || (oldV == minV) || (oldV == maxV)) {
          removeListenersAndClearEdgeMap();
          break;
        }
      }
    }
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::updateAllNodesValues(
    TYPE_CONST_REFERENCE(NodeType) newValue) {
  // loop on subgraph min/max
  for (const auto &[graphId, minMax] : _minMaxNode) {
    _minMaxNode[graphId] = {newValue, newValue};
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::updateAllEdgesValues(
    TYPE_CONST_REFERENCE(EdgeType) newValue) {
  // loop on subgraph min/max
  for (const auto &[graphId, minMax] : _minMaxEdge) {
    _minMaxEdge[graphId] = {newValue, newValue};
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::treatEvent(const tlp::Event &ev) {
  const auto *graphEvent = dynamic_cast<const tlp::GraphEvent *>(&ev);

  if (graphEvent) {
    tlp::Graph *graph = graphEvent->getGraph();

    switch (graphEvent->getType()) {
    case GraphEventType::TLP_ADD_NODE:
      removeListenersAndClearNodeMap();
      break;

    case GraphEventType::TLP_DEL_NODE: {
      uint sgi = graph->getId();

      if (const auto it = _minMaxNode.find(sgi); it != _minMaxNode.end()) {
        TYPE_CONST_REFERENCE(NodeType) oldV = this->getNodeValue(graphEvent->getNode());

        // check if min or max has to be updated
        if ((oldV == it->second.first) || (oldV == it->second.second)) {
          _minMaxNode.erase(it);

          if ((_minMaxEdge.find(sgi) == _minMaxEdge.end()) &&
              (!_needGraphListener || (graph != PropType::graph))) {
            // graph observation is no longer needed
            graph->removeListener(this);
          }
        }
      }

      break;
    }

    case GraphEventType::TLP_ADD_EDGE:
      removeListenersAndClearEdgeMap();
      break;

    case GraphEventType::TLP_DEL_EDGE: {
      uint sgi = graph->getId();

      if (const auto it = _minMaxEdge.find(sgi); it != _minMaxEdge.end()) {
        TYPE_CONST_REFERENCE(EdgeType) oldV = this->getEdgeValue(graphEvent->getEdge());

        // check if min or max has to be updated
        if ((oldV == it->second.first) || (oldV == it->second.second)) {
          _minMaxEdge.erase(it);

          if ((_minMaxNode.find(sgi) == _minMaxNode.end()) &&
              (!_needGraphListener || (graph != PropType::graph))) {
            // graph observation is no longer needed
            graph->removeListener(this);
          }
        }
      }

      break;
    }

    default:
      break;
    }
  }
}
