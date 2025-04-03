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

#ifndef PIXEL_ORIENTED_OVERVIEW_H
#define PIXEL_ORIENTED_OVERVIEW_H

#include "POLIB/GraphDimension.h"
#include "POLIB/PixelOrientedMediator.h"

#include <talipot/GlEntity.h>
#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/Graph.h>
#include <talipot/GlRect.h>
#include <talipot/GlWidget.h>
#include <talipot/GlLabel.h>

#include <string>

namespace tlp {

class SizeProperty;
class LayoutProperty;

class PixelOrientedOverview : public GlComposite {

  public:
    PixelOrientedOverview(GraphDimension *data, PixelOrientedMediator *pixelOrientedMediator,
                          Coord blCornerPos, const std::string &dimName,
                          const Color &backgroundColor, const Color &textColor);

    ~PixelOrientedOverview() override;

    GraphDimension *getData() const {
        return data;
    }
    std::string getDimensionName() const {
        return dimName;
    }
    LayoutProperty *getPixelViewLayout() const {
        return pixelLayout;
    }
    SizeProperty *getPixelViewSize() const {
        return pixelSize;
    }

    void setBLCorner(const Coord &blCorner);

    void computePixelView(GlWidget *glWidget = nullptr);

    bool overviewGenerated() const {
        return overviewGen;
    }

    void setBackgroundColor(const Color &color) {
        backgroundColor = color;
    }
    void setTextColor(const Color &color) {
        textColor = color;
    }

  private:
    void computeBoundingBox() {
        GlBoundingBoxSceneVisitor glBBSV(nullptr);
        acceptVisitor(&glBBSV);
        boundingBox = glBBSV.getBoundingBox();
    }

    GraphDimension *data;
    PixelOrientedMediator *pixelOrientedMediator;

    GlGraph *glGraph;
    LayoutProperty *pixelLayout;
    SizeProperty *pixelSize;

    Coord blCornerPos;
    std::string textureName;
    std::string dimName;

    GlLabel *clickLabel;
    GlRect *backgroundRect;
    GlLabel *overviewLabel;
    GlRect *frame;
    GlRect *frame2;

    int overviewId;
    static int overviewCpt;

    bool overviewGen;

    Color backgroundColor, textColor;
};
}
#endif // PIXEL_ORIENTED_OVERVIEW_H
