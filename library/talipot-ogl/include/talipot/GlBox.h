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

#ifndef TALIPOT_GL_BOX_H
#define TALIPOT_GL_BOX_H

#include <talipot/OpenGlIncludes.h>

#include <talipot/Color.h>
#include <talipot/Size.h>
#include <talipot/GlEntity.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief General class used to render boxes as GlEntity.
 */
class TLP_GL_SCOPE GlBox : public GlEntity {

public:
  /**
   * @brief Don't use this constructor
   */
  GlBox();

  /**
   * @brief Constructor
   *
   * @param position The center of the box.
   * @param size The length of each dimension of the box.
   * @param fillColor The fill color of the box.
   * @param outlineColor The outline color of the box
   * @param filled Fill the box ?
   * @param outlined outline the box ?
   * @param outlineSize The size of the outline
   */
  GlBox(const Coord &position, const Size &size, const Color &fillColor, const Color &outlineColor,
        bool filled = true, bool outlined = true, const std::string &textureName = "",
        float outlineSize = 1.);

  /**
   * @brief Destructor.
   */
  ~GlBox() override;

  void draw(float lod, Camera *camera) override;

  /**
   * @brief Draw a GlBox
   */
  static void draw(const Color &fillColor, const Color &outlineColor, float outlineWidth,
                   const std::string &textureName, float lod);

  static Coord getAnchor(const Coord &vector);

  /**
   * @brief Accessor in reading to the size.
   */
  Size getSize() const;

  /**
   * @brief Accessor in writing to the size of the box
   */
  void setSize(const Size &size);

  /**
   * @brief Accessor in reading to the position.
   */
  Coord *getPosition() const;

  /**
   * @brief Accessor in writing to the position.
   */
  void setPosition(const Coord &position);

  /**
   * @brief Accessor in reading to the fill color.
   */
  Color getFillColor() const;

  /**
   * @brief Accessor in writing to the fill color of the box
   */
  void setFillColor(const Color &color);

  /**
   * @brief Accessor in reading to the outline color.
   */
  Color getOutlineColor() const;

  /**
   * @brief Accessor in writing to the outline color of the box
   */
  void setOutlineColor(const Color &color);

  /**
   * @brief Accessor in reading to the outline size.
   */
  float getOutlineSize() const;

  /**
   * @brief Accessor in writing to the outline size of the box
   */
  void setOutlineSize(float size);

  /**
   * @brief Accessor in reading to the texture name.
   */
  std::string getTextureName() const;

  /**
   * @brief Accessor in writing to the texture name of the box
   */
  void setTextureName(const std::string &textureName);

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
  virtual void clearGenerated();

  Coord position;                   /**< The position of the center of the box*/
  Size size;                        /**< size is the "radius" of the box */
  std::vector<Color> fillColors;    /**< fillColor of the box */
  std::vector<Color> outlineColors; /**< outlineColor of the box */
  bool filled;                      /**< the box is filled ? */
  bool outlined;                    /**< the box is outlined ? */
  std::string textureName;
  float outlineSize; /**< size of the outline */

  float *newCubeCoordArrays;
  bool generated;
  GLuint buffers[5];
};
}
#endif // TALIPOT_GL_BOX_H
