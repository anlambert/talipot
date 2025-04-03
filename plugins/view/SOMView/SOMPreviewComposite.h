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

#ifndef SOM_PREVIEW_COMPOSITE_H
#define SOM_PREVIEW_COMPOSITE_H

#include <talipot/GlComposite.h>
#include <talipot/GlLabel.h>
#include <talipot/GlRect.h>
#include "GlLabelledColorScale.h"
#include <SOMMap.h>
#include "SOMMapElement.h"
/**
 * @brief GlComposite used to print SOM preview. Use a SOMMapElement to print the SOM and add other
 * information like property name or color scale and min and max value.
 */
namespace tlp {

class SOMPreviewComposite : public GlComposite {
  public:
    /**
     *@brief Constructor for the SOMPreviewComposite.
     * @param position The bottom right corner of the preview.
     * @param size The size of the composite
     * @param propertyName The name of the displayed properties.
     * @param colorProperty The property containing colors for each element in the grid.
     * @param map The SOM grid.
     * @param colorScale The color scale to use in the preview.
     * @param minValue The minimum value label.
     * @param maxValue The maximum value label.
     **/
    SOMPreviewComposite(tlp::Coord position, tlp::Size size, const std::string &propertyName,
                        tlp::ColorProperty *colorProperty, SOMMap *map, tlp::ColorScale *colorScale,
                        double minValue, double maxValue);
    ~SOMPreviewComposite() override;

    std::string getPropertyName() const {
        return propertyName;
    }

    /**
     * Change the color of the frame.
     * @param color the new color.
     */
    void setFrameColor(tlp::Color color);

    /**
     * Change the colors of the som representation with those in the given property;
     * @param newColor The new colors.
     */
    void updateColors(ColorProperty *newColor);

    /**
     * @brief Test is the entity is an internal element of the SOM.
     */
    bool isElement(GlEntity *entity);

  protected:
    tlp::Size computeAspectRatio(uint width, uint height, float maxWidth, float maxHeight);

    void buildMainComposite(const std::string &colorPropertyName, SOMMap *map, tlp::Coord topLeft,
                            tlp::Size elementSize);

    tlp::GlLabel *label;
    tlp::GlRect *frame;
    SOMMapElement *mapComposite;
    std::string propertyName;
    GlLabelledColorScale *lColorScale;
    tlp::Coord currentPosition;
};
}
#endif // SOM_PREVIEW_COMPOSITE_H
