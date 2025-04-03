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

#ifndef TALIPOT_GL_GRID_H
#define TALIPOT_GL_GRID_H

#include <talipot/Coord.h>
#include <talipot/Color.h>
#include <talipot/Size.h>

#include <talipot/GlEntity.h>

namespace tlp {
/**
 * @ingroup OpenGL
 * @brief General class used to render grids as GlEntity.
 *
 */
class TLP_GL_SCOPE GlGrid : public GlEntity {

  public:
    /**
     * @brief Constructor
     * @warning Don't use this constructor : see other constructor
     */
    GlGrid() = default;

    /**
     * @brief Constructor
     *
     * @param frontTopLeft The minimum of the bounding box of the grid.
     * @param backBottomRight The maximum of the bounding box of the grid.
     * @param cell The size of a single cell of the grid.
     * @param color The color of the lines of the grid.
     * @param displays The dimensions of the grid to display.
     * @param hollowGrid Indicates if the grid should be hollowed.
     */
    GlGrid(const Coord &frontTopLeft, const Coord &backBottomRight, const Size &cell,
           const Color &color, bool displays[3]);
    /**
     * @brief Virtual function used to draw the grid.
     */
    void draw(float lod, Camera *camera) override;
    /**
     * @brief Accessor in reading to the dimensions to display.
     */
    void getDisplayDim(bool displayDim[3]) const;
    /**
     * @brief Accessor in writing to the dimensions to display.
     */
    void setDisplayDim(bool displayDim[3]);

    /**
     * @brief Translate entity
     */
    void translate(const Coord &move) override;

    /**
     * @brief Function to export data in outString (in XML format)
     */
    void getXML(std::string &outString) override;

    /**
     * @brief Function to set data with inString (in XML format)
     */
    void setWithXML(const std::string &inString, uint &currentPosition) override;

  protected:
    bool displayDim[3]; /**< Which dimensions should be displayed ? Note that only one dimension at
                           most should be disabled*/
    bool hollowGrid;    /**< Should the grid be hollowed ?*/
    Coord frontTopLeft; /**< Front top left point of the grid, the "minimum" */
    Coord backBottomRight; /**< Back bottom right point of the grid, the "maximum" */
    Color color;           /**< The color of the grid */
    Size cell;             /**< The size of a cell of the grid */
};
}
#endif // TALIPOT_GL_GRID_H
