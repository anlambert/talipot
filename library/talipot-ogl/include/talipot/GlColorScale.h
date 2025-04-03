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

#ifndef TALIPOT_GL_COLOR_SCALE_H
#define TALIPOT_GL_COLOR_SCALE_H

#include <talipot/Observable.h>
#include <talipot/GlEntity.h>
#include <talipot/Color.h>

namespace tlp {

class ColorScale;
class GlPolyQuad;

class TLP_GL_SCOPE GlColorScale : public GlEntity, public Observable {

  public:
    enum Orientation { Horizontal, Vertical };

    GlColorScale(ColorScale *colorScale, const Coord &baseCoord, const float length,
                 const float thickness, Orientation orientation);

    ~GlColorScale() override;

    /**
     * @brief Compute the color corresponding to the position in the color scale.
     * The orientation of the scale define the coordinate used to compute the color (if the
     *orientation is horizontal use only the X coordinate).
     * If the position is outside of the entity coordinates returns the nearest extremity value.
     **/
    Color getColorAtPos(Coord pos);

    void draw(float lod, Camera *camera) override;

    void translate(const Coord &move) override;

    Coord getBaseCoord() const {
        return baseCoord;
    }

    float getThickness() const {
        return thickness;
    }

    float getLength() const {
        return length;
    }

    GlPolyQuad *getColorScalePolyQuad() const {
        return colorScalePolyQuad;
    }

    void setColorScale(ColorScale *scale);

    ColorScale *getColorScale() {
        return colorScale;
    }

    void getXML(std::string &) override {}

    void setWithXML(const std::string &, uint &) override {}

  protected:
    void treatEvent(const Event &) override;

  private:
    void updateDrawing();

    ColorScale *colorScale;
    Coord baseCoord;
    float length, thickness;
    GlPolyQuad *colorScalePolyQuad;
    Orientation orientation;
};
}

#endif // TALIPOT_GL_COLOR_SCALE_H
