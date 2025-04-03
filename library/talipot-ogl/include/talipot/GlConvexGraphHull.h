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

#ifndef TALIPOT_GL_CONVEX_GRAPH_HULL_H
#define TALIPOT_GL_CONVEX_GRAPH_HULL_H

#include <talipot/Color.h>
#include <talipot/config.h>

namespace tlp {

class GlComposite;
class GlEntity;
class Camera;
class Graph;
class LayoutProperty;
class SizeProperty;
class DoubleProperty;
class GlComplexPolygon;

/**
 * @brief This class draws a convex hull around a graph.
 *
 * Given a graph, this class computes the various control points of a convex hull that embraces the
 * nodes and edges present into the graph.
 * If this entity is translated using the translate method, every element of the linked graph will
 * be translated as well.
 *
 * @warning The graph or any of the properties linked to a GlConvexGraphHull should never be deleted
 * before the entity. Such a thing should be
 * reported to the user in debug mode, raising an assertion.
 */
class TLP_GL_SCOPE GlConvexGraphHull {
  public:
    /**
     * @brief ...
     *
     * @param parent ...
     * @param name ...
     * @param fcolor The color used to fill the hull.
     * @param graph The graph whose elements should be inside the hull.
     * @param layout The property used to layout the elements in the graph.
     * @param size The property defining the graph's elements' sizes.
     * @param rotation The property defining the graph's elements' rotation.
     **/
    GlConvexGraphHull(GlComposite *parent, const std::string &name, const tlp::Color &fcolor,
                      Graph *graph, LayoutProperty *layout, SizeProperty *size,
                      DoubleProperty *rotation);

    ~GlConvexGraphHull();

    /**
     * Recomputes the whole Hull
     */
    void updateHull(LayoutProperty *layout = nullptr, SizeProperty *size = nullptr,
                    DoubleProperty *rotation = nullptr);

    void setVisible(bool visible);
    bool isVisible();

  private:
    GlComposite *_parent;
    std::string _name;
    Color _fcolor;
    GlComplexPolygon *_polygon;
    Graph *graph;
    LayoutProperty *_layout;
    SizeProperty *_size;
    DoubleProperty *_rotation;
    static int bezierValue;
};
}

#endif // TALIPOT_GL_CONVEX_GRAPH_HULL_H
