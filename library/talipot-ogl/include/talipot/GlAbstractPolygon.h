/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_ABSTRACT_POLYGON_H
#define TALIPOT_GL_ABSTRACT_POLYGON_H

#include <vector>

#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/Size.h>
#include <talipot/GlSimpleEntity.h>

#include <talipot/OpenGlIncludes.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief class to create a abstract polygon
 * @warning You don't have to use this class, it's only a base class for some others entities
 */
class TLP_GL_SCOPE GlAbstractPolygon : public GlSimpleEntity {
public:
  ///@cond DOXYGEN_HIDDEN

  /**
   * Constructor
   */
  GlAbstractPolygon();
  /**
   * Default empty destructor
   */
  ~GlAbstractPolygon() override;

  enum PolygonMode { POLYGON = 0, QUAD_STRIP = 1 };

  /**
   * Get the polygon mode (see PolygonMode enum)
   */
  PolygonMode getPolygonMode();

  /**
   * Set the polygon mode (see PolygonMode enum)
   */
  void setPolygonMode(PolygonMode mode);

  /**
   * Get if the polygon is filled or not
   */
  bool getFillMode();

  /**
   * Set if the polygon is filled or not
   */
  void setFillMode(const bool);

  /**
   * Get if the polygon is outlined or not
   */
  bool getOutlineMode();

  /**
   * Set if the polygon is outlined or not
   */
  void setOutlineMode(const bool);

  /**
   * Get if the polygon use light or not
   */
  bool getLightingMode();

  /**
   * Set if the polygon use light or not
   */
  void setLightingMode(const bool);

  /**
   * Get the ith color used to filling the polygon
   */
  Color getFillColor(unsigned int i);

  /**
   * Set the ith color used to filling the polygon
   */
  void setFillColor(unsigned int i, const Color &color);

  ///@endcond

  /**
   * @brief Set color used to filling the whole polygon
   */
  void setFillColor(const Color &color);

  ///@cond DOXYGEN_HIDDEN

  /**
   * Get the ith color used to outlining the polygon
   */
  Color getOutlineColor(unsigned int i);

  /**
   * Set the ith color used to outlining the polygon
   */
  void setOutlineColor(unsigned int i, const Color &color);

  ///@endcond

  /**
   * @brief Set the color used to outlining the whole polygon
   */
  void setOutlineColor(const Color &color);

  /**
   * @brief Get the textureName
   */
  std::string getTextureName();

  /**
   * @brief Set the textureName
   */
  void setTextureName(const std::string &name);

  /**
   * @brief Get the outline size
   */
  float getOutlineSize();

  /**
   * @brief Set the outline size
   */
  void setOutlineSize(float size);

  ///@cond DOXYGEN_HIDDEN

  /**
   * Get the lod outline value, below this lod value outline will not be displayed
   */
  float getHideOutlineLod();

  /**
   * Set the lod outline value, below this lod value outline will not be displayed
   */
  void setHideOutlineLod(float lod);

  /**
   * Sets if the y texture coordinates have to be inversed
   */
  void setInvertYTexture(bool invertYTexture);

  /**
   * Draw the polygon
   */
  void draw(float lod, Camera *camera) override;

  /**
   * Translate entity
   */
  void translate(const Coord &mouvement) override;

  /**
   * Scale entity
   */
  virtual void scale(const tlp::Size &factor);

  /**
   * Function to export data and type in outString (in XML format)
   */
  void getXML(std::string &outString) override;

  /**
   * Function to export data in outString (in XML format)
   */
  virtual void getXMLOnlyData(std::string &outString);

  /**
   * Function to set data with inString (in XML format)
   */
  void setWithXML(const std::string &outString, unsigned int &currentPosition) override;

  ///@endcond

protected:
  ///@cond DOXYGEN_HIDDEN

  /**
   * set Coords of the polygon
   */
  virtual void setPoints(const std::vector<Coord> &points);

  /**
   * set ith Coord of the polygon
   */
  virtual void setPoint(unsigned int index, const Coord &point);

  /**
   * set fill colors of the polygon
   */
  virtual void setFillColors(const std::vector<Color> &colors);

  /**
   * set outline colors of the polygon
   */
  virtual void setOutlineColors(const std::vector<Color> &colors);

  /**
   * Clear previous bounding box and expand bounding box with polygons' points
   */
  virtual void recomputeBoundingBox();

  /**
   * Clear previously generated VBO
   */
  virtual void clearGenerated();

  ///@endcond

  PolygonMode polygonMode;
  std::vector<Coord> points;
  std::vector<Color> fillColors;
  std::vector<Color> outlineColors;
  bool filled;
  bool outlined;
  bool lighting;
  bool invertYTexture;
  std::string textureName;
  float outlineSize;
  float hideOutlineLod;

  std::vector<Coord> normalArray;
  GLubyte *indices;
  GLubyte *auxIndices;
  GLfloat *texArray;

  bool generated;
  GLuint buffers[7];
};
}
#endif // TALIPOT_GL_ABSTRACT_POLYGON_H
