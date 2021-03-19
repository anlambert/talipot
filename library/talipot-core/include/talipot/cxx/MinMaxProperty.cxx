/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
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
    tlp::Graph *graph, const std::string &name, typename NodeType::RealType nodeMin,
    typename NodeType::RealType nodeMax, typename EdgeType::RealType edgeMin,
    typename EdgeType::RealType edgeMax)
    : AbstractProperty<NodeType, EdgeType, PropType>(graph, name), _nodeMin(nodeMin),
      _nodeMax(nodeMax), _edgeMin(edgeMin), _edgeMax(edgeMax), _needGraphListener(false) {}

template <typename NodeType, typename EdgeType, typename PropType>
typename tlp::StoredType<typename NodeType::RealType>::ReturnedConstValue
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getNodeMin(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  unsigned int graphId = graph->getId();

  if (const auto it = _minMaxNode.find(graphId); it == _minMaxNode.end()) {
    computeMinMaxNode(graph);
    return _minMaxNode[graphId].first;
  } else {
    return it->second.first;
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
typename tlp::StoredType<typename NodeType::RealType>::ReturnedConstValue
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getNodeMax(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  unsigned int graphId = graph->getId();

  if (const auto it = _minMaxNode.find(graphId); it == _minMaxNode.end()) {
    computeMinMaxNode(graph);
    return _minMaxNode[graphId].second;
  } else {
    return it->second.second;
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
typename tlp::StoredType<typename EdgeType::RealType>::ReturnedConstValue
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getEdgeMin(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  unsigned int graphId = graph->getId();

  if (const auto it = _minMaxEdge.find(graphId); it == _minMaxEdge.end()) {
    computeMinMaxEdge(graph);
    return _minMaxEdge[graphId].first;
  } else {
    return it->second.first;
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
typename tlp::StoredType<typename EdgeType::RealType>::ReturnedConstValue
tlp::MinMaxProperty<NodeType, EdgeType, PropType>::getEdgeMax(const tlp::Graph *graph) {
  if (!graph) {
    graph = this->PropType::graph;
  }

  unsigned int graphId = graph->getId();

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
      const auto &tmp = this->getNodeValue(n);
      minN = std::min(minN, tmp);
      maxN = std::max(maxN, tmp);
    }
  }

  if (maxN < minN) {
    maxN = minN = AbstractProperty<NodeType, EdgeType, PropType>::nodeDefaultValue;
  }

  unsigned int sgi = graph->getId();

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
      const auto &tmp = this->getEdgeValue(ite);
      minE = std::min(minE, tmp);
      maxE = std::max(maxE, tmp);
    }
  }

  if (maxE < minE) {
    maxE = minE = AbstractProperty<NodeType, EdgeType, PropType>::edgeDefaultValue;
  }

  unsigned int sgi = graph->getId();

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
  //

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
    tlp::node n,
    typename tlp::StoredType<typename NodeType::RealType>::ReturnedConstValue newValue) {

  if (!_minMaxNode.empty()) {
    const auto &oldV = this->getNodeValue(n);

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
    tlp::edge e,
    typename tlp::StoredType<typename EdgeType::RealType>::ReturnedConstValue newValue) {

  if (!_minMaxEdge.empty()) {
    const auto &oldV = this->getEdgeValue(e);

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
    typename tlp::StoredType<typename NodeType::RealType>::ReturnedConstValue newValue) {

  // loop on subgraph min/max
  for (const auto &[graphId, minMax] : _minMaxNode) {
    _minMaxNode[graphId] = {newValue, newValue};
  }
}

template <typename NodeType, typename EdgeType, typename PropType>
void tlp::MinMaxProperty<NodeType, EdgeType, PropType>::updateAllEdgesValues(
    typename tlp::StoredType<typename EdgeType::RealType>::ReturnedConstValue newValue) {
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
    case GraphEvent::TLP_ADD_NODE:
      removeListenersAndClearNodeMap();
      break;

    case GraphEvent::TLP_DEL_NODE: {
      unsigned int sgi = graph->getId();

      if (const auto it = _minMaxNode.find(sgi); it != _minMaxNode.end()) {
        const auto &oldV = this->getNodeValue(graphEvent->getNode());

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

    case GraphEvent::TLP_ADD_EDGE:
      removeListenersAndClearEdgeMap();
      break;

    case GraphEvent::TLP_DEL_EDGE: {
      unsigned int sgi = graph->getId();

      if (const auto it = _minMaxEdge.find(sgi); it != _minMaxEdge.end()) {
        const auto &oldV = this->getEdgeValue(graphEvent->getEdge());

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
      // we don't care about the rest
      break;
    }
  }
}
