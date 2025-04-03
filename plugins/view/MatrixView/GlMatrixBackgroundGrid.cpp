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

#include "GlMatrixBackgroundGrid.h"

#include <talipot/GlLine.h>

#include "MatrixView.h"

using namespace std;

namespace tlp {

GlMatrixBackgroundGrid::GlMatrixBackgroundGrid(MatrixView *view) : _view(view) {}

BoundingBox GlMatrixBackgroundGrid::getBoundingBox() {
    int N = _view->graph()->numberOfNodes();
    BoundingBox result;
    result.expand(Coord(0, 0, 0));
    result.expand(Coord(1 + N, -1 - N, 0));
    return result;
}

void GlMatrixBackgroundGrid::draw(float lod, tlp::Camera *camera) {
    const Vec4i &viewPort = camera->getViewport();

    Coord topLeft = camera->viewportTo3DWorld(Coord(viewPort[0] + viewPort[2], viewPort[1], 0)),
          bottomRight = camera->viewportTo3DWorld(Coord(viewPort[0], viewPort[1] + viewPort[3], 0));

    GridDisplayMode mode = _view->gridDisplayMode();

    if (mode == SHOW_NEVER || (mode == SHOW_ON_ZOOM && abs(bottomRight[0] - topLeft[0]) > 50)) {
        return;
    }

    int N = _view->graph()->numberOfNodes();
    double startX = max<double>(0.5, floor(topLeft[0]) - 0.5),
           startY = min<double>(-0.5, ceil(topLeft[1]) + 0.5),
           endX = min<double>(0.5 + N, ceil(bottomRight[0]) + 0.5),
           endY = max<double>(-0.5 - N, floor(bottomRight[1]) - 0.5);

    for (double x = startX; x <= endX; ++x) {
        vector<Coord> points(2);
        points[0] = Coord(x, startY, 0);
        points[1] = Coord(x, endY, 0);
        vector<Color> colors(2);
        colors[0] = Color(0, 0, 0);
        colors[1] = Color(0, 0, 0);
        GlLine line(points, colors);
        line.draw(lod, camera);
    }

    for (double y = startY; y >= endY; --y) {
        vector<Coord> points(2);
        points[0] = Coord(startX, y, 0);
        points[1] = Coord(endX, y, 0);
        vector<Color> colors(2);
        colors[0] = Color(0, 0, 0);
        colors[1] = Color(0, 0, 0);
        GlLine line(points, colors);
        line.draw(lod, camera);
    }
}
}
