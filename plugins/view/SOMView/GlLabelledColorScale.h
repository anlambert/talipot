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

#ifndef GL_LABELLED_COLOR_SCALE_H
#define GL_LABELLED_COLOR_SCALE_H

#include <talipot/GlComposite.h>
#include <talipot/GlLabel.h>
#include <talipot/GlColorScale.h>
namespace tlp {
class GlLayer;

class GlLabelledColorScale : public tlp::GlComposite {
  public:
    GlLabelledColorScale(tlp::Coord position, tlp::Size size, tlp::ColorScale *colorScale,
                         double minValue = 0, double maxValue = 0, bool ColorScaleAtTop = true);

    ~GlLabelledColorScale() override;

    void setMinValue(double value);
    double getMinValue() const {
        return minValue;
    }
    void setMaxValue(double value);
    double getMaxValue() const {
        return maxValue;
    }

    tlp::Coord getPosition() {
        return position;
    }
    tlp::Size getSize() {
        return size;
    }

    tlp::BoundingBox getColorScaleBoundingBox();

    tlp::GlColorScale *getGlColorScale() {
        return glColorScale;
    }

    void setColorScale(tlp::ColorScale *cs);

    void setPosition(tlp::Coord nPosition);

    void setSize(tlp::Size nSize);

    void addLayerParent(tlp::GlLayer *layer) override;

  protected:
    void buildComposite(tlp::ColorScale *colorScale);
    tlp::Coord position;
    tlp::Size size;
    tlp::GlColorScale *glColorScale;
    tlp::GlLabel *minLabel;
    tlp::GlLabel *maxLabel;
    double minValue;
    double maxValue;
    bool colorScaleAtTop;
};
}
#endif // GL_LABELLED_COLOR_SCALE_H
