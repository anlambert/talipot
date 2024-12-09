/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef SOM_MAP_ELEMENT_H
#define SOM_MAP_ELEMENT_H

#include <talipot/hash.h>

#include <talipot/GlComposite.h>
#include <talipot/Size.h>
#include <talipot/Coord.h>
#include <talipot/Node.h>

/**
 * @brief GlComposite used to visualize a SOM.
 * GlComposite used to visualize a SOM. This object take a SOM and the the ColorProperty defining
 * the color of each node of the map and print them.
 * If the SOM is hexagonal kind print hexagonal glyph else print square.
 */
namespace tlp {

class SOMMap;
class ColorProperty;

class SOMMapElement : public tlp::GlComposite {
public:
  SOMMapElement(Coord position, Size size, SOMMap *map, ColorProperty *colorProperty);
  ~SOMMapElement() override;

  tlp::Coord getTopLeftPositionForElement(uint x, uint y);
  tlp::Size getNodeAreaSize();

  /**
   * Change the SOM and its color property.
   * @param map the new SOM
   * @param cp the new color property
   */
  void setData(SOMMap *map, ColorProperty *cp);

  /**
   * Change the colors of the som representation with those in the given property;
   * @param newColor The new colors.
   */
  void updateColors(ColorProperty *newColor);

protected:
  void computeNodeAreaSize();

  float computeMaximizedRadiusForHexagone(uint width, uint height, Size &size);

  void buildMainComposite(Coord topLeft, Size elementSize, SOMMap *map);

  SOMMap *som;

  flat_hash_map<node, GlEntity *> nodesMap;

  Coord position;
  Size size;
  Size nodeAreaSize;
};
}
#endif // SOM_MAP_ELEMENT_H
