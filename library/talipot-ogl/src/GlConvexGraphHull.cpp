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

#include <talipot/GlConvexGraphHull.h>

#include <talipot/Graph.h>
#include <talipot/DrawingTools.h>
#include <talipot/GlComplexPolygon.h>
#include <talipot/GlComposite.h>
using namespace std;

namespace tlp {

int GlConvexGraphHull::bezierValue = 1;

GlConvexGraphHull::GlConvexGraphHull(GlComposite *parent, const std::string &name,
                                     const tlp::Color &fcolor, Graph *graph, LayoutProperty *layout,
                                     SizeProperty *size, DoubleProperty *rotation)
    : _parent(parent), _name(name), _fcolor(fcolor), _polygon(nullptr), graph(graph),
      _layout(layout), _size(size), _rotation(rotation) {
    assert(graph);

    updateHull();
}

GlConvexGraphHull::~GlConvexGraphHull() {
    delete _polygon;
}

void GlConvexGraphHull::updateHull(LayoutProperty *layout, SizeProperty *size,
                                   DoubleProperty *rotation) {
    bool visible = !_polygon || _polygon->isVisible();

    if (_polygon) {
        _parent->deleteGlEntity(_polygon);
        delete _polygon;
        _polygon = nullptr;
    }

    if (layout) {
        _layout = layout;
    }

    if (size) {
        _size = size;
    }

    if (rotation) {
        _rotation = rotation;
    }

    if (!graph->isEmpty()) {
        _polygon =
            new GlComplexPolygon(computeConvexHull(graph, _layout, _size, _rotation, nullptr),
                                 _fcolor, GlConvexGraphHull::bezierValue);
        _polygon->setVisible(visible);
        _parent->addGlEntity(_polygon, _name);
    }
}

bool GlConvexGraphHull::isVisible() {
    return _polygon && _polygon->isVisible();
}

void GlConvexGraphHull::setVisible(bool visible) {
    if (_polygon) {
        _polygon->setVisible(visible);
    }
}
}
