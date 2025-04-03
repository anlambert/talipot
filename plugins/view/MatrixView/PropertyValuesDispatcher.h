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

#ifndef PROPERTY_VALUES_DISPATCHER_H
#define PROPERTY_VALUES_DISPATCHER_H

#include <QHash>

#include <set>
#include <string>

#include <talipot/Observable.h>
#include <talipot/Graph.h>

namespace tlp {
class IntegerVectorProperty;
class BooleanProperty;
class IntegerProperty;

class PropertyValuesDispatcher : public Observable {
  public:
    PropertyValuesDispatcher(tlp::Graph *source, tlp::Graph *target,
                             const std::set<std::string> &sourceToTargetProperties,
                             const std::set<std::string> &targetToSourceProperties,
                             tlp::IntegerVectorProperty *_graphEntitiesToDisplayedNodes,
                             tlp::BooleanProperty *_displayedNodesAreNodes,
                             tlp::IntegerProperty *displayedNodesToGraphEntities,
                             tlp::IntegerProperty *displayedEdgesToGraphEdges,
                             QHash<tlp::edge, tlp::edge> &edgesMap);
    void afterSetNodeValue(tlp::PropertyInterface *, const tlp::node);
    void afterSetEdgeValue(tlp::PropertyInterface *, const tlp::edge);
    void afterSetAllNodeValue(tlp::PropertyInterface *);
    void afterSetAllEdgeValue(tlp::PropertyInterface *);
    void addLocalProperty(tlp::Graph *, const std::string &);
    void treatEvent(const tlp::Event &) override;

  private:
    tlp::Graph *_source;
    tlp::Graph *_target;
    tlp::IntegerVectorProperty *_graphEntitiesToDisplayedNodes;
    tlp::BooleanProperty *_displayedNodesAreNodes;
    tlp::IntegerProperty *_displayedNodesToGraphEntities;
    tlp::IntegerProperty *_displayedEdgesToGraphEdges;
    QHash<tlp::edge, tlp::edge> &_edgesMap;
    std::set<std::string> _sourceToTargetProperties;
    std::set<std::string> _targetToSourceProperties;
    bool _modifying;
};
}
#endif // PROPERTY_VALUES_DISPATCHER_H
