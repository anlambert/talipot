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

#ifndef TALIPOT_GL_COMPOSITE_HIERARCHY_MANAGER_H
#define TALIPOT_GL_COMPOSITE_HIERARCHY_MANAGER_H

#include <string>
#include <map>

#include <talipot/DataSet.h>
#include <talipot/Observable.h>
#include <talipot/GlComposite.h>

namespace tlp {

class GlConvexGraphHull;

struct node;
class Color;
class LayoutProperty;
class DoubleProperty;
class SizeProperty;
struct ConvexHullItem;
class Graph;
class GlLayer;

/**
 * Create a GlComposite item for each and every graph and subgraph.
 * This class observes the graph to update said hierarchy when a subgraph is added or deleted.
 */
class TLP_QT_SCOPE GlCompositeHierarchyManager : private Observable {

  public:
    GlCompositeHierarchyManager(Graph *graph, GlLayer *layer, const std::string &layerName,
                                LayoutProperty *layout, SizeProperty *size,
                                DoubleProperty *rotation, bool visible = false,
                                const std::string &namingProperty = "name",
                                const std::string &subCompositeSuffix = " sub-hulls");

    void setGraph(tlp::Graph *graph);
    DataSet getData();
    void setData(const DataSet &dataSet);

    void createComposite();

    void setVisible(bool visible);
    bool isVisible() const;

  protected:
    void treatEvents(const std::vector<Event> &events) override;
    void treatEvent(const Event &) override;

  private:
    const tlp::Color getColor();
    int _currentColor;

    /**
     * Create the hierarchy of ConvexHullItem
     */
    void buildComposite(tlp::Graph *current, tlp::GlComposite *composite);

    Graph *_graph;
    GlLayer *_layer;
    GlComposite *_composite;
    LayoutProperty *_layout;
    SizeProperty *_size;
    DoubleProperty *_rotation;
    std::vector<Color> _fillColors;
    std::string _layerName;
    bool _isVisible;
    const std::string _subCompositesSuffix;
    const std::string _nameAttribute;

    const static std::string temporaryPropertyValue;
    /**
     * This map contains the composite that contains the associated graph's hull.
     */
    std::map<tlp::Graph *, std::pair<tlp::GlComposite *, GlConvexGraphHull *>> _graphsComposites;
};

class GlHierarchyMainComposite : public GlComposite {
  public:
    GlHierarchyMainComposite(GlCompositeHierarchyManager *manager);
    void setVisible(bool visible) override;

  private:
    GlCompositeHierarchyManager *_manager;
};
}

#endif // TALIPOT_GL_COMPOSITE_HIERARCHY_MANAGER_H
