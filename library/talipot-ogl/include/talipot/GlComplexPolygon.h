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

#ifndef TALIPOT_GL_COMPLEX_POLYGON_H
#define TALIPOT_GL_COMPLEX_POLYGON_H

#include <vector>
#include <map>
#include <set>

#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/config.h>
#include <talipot/GlEntity.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief Class to create a complex polygon (concave polygon or polygon with hole)
 * If you want to create a complex polygon you have 4 constructors :
 * Constructors with vector of coords : to create a complex polygon without hole
 *   - In this case you have two constructor : with and without outline color
 *   - You can create a polygon like this :
 * \code
 *     vector <Coord> coords;
 *     coords.push_back(Coord(0,0,0));
 *     coords.push_back(Coord(10,0,0));
 *     coords.push_back(Coord(10,10,0));
 *     coords.push_back(Coord(0,10,0));
 *     GlComplexPolygon *complexPolygon=new GlComplexPolygon(coords,Color(255,0,0,255));
 *     layer->addGlEntity(complexPolygon,"complexPolygon");
 * \endcode
 *
 * Constructors with vector of vector of Coords : to create a complex polygon with hole
 *   - In this case you have two constructor : with and without outline color
 *   - The first vector of coords is the polygon and others vector are holes
 *   - You can create a polygon with hole like this :
 * \code
 *     vector <vector <Coord> > coords;
 *     vector <Coord> polygon;
 *     vector <Coord> hole;
 *     polygon.push_back(Coord(0,0,0));
 *     polygon.push_back(Coord(10,0,0));
 *     polygon.push_back(Coord(10,10,0));
 *     polygon.push_back(Coord(0,10,0));
 *     hole.push_back(Coord(4,4,0));
 *     hole.push_back(Coord(6,4,0));
 *     hole.push_back(Coord(6,6,0));
 *     hole.push_back(Coord(4,6,0));
 *     coords.push_back(polygon);
 *     coords.push_back(hole);
 *     GlComplexPolygon *complexPolygon=new GlComplexPolygon(coords,Color(255,0,0,255));
 *     layer->addGlEntity(complexPolygon,"complexPolygon");
 * \endcode
 *
 * In constructors you can specify the polygon border style : polygonEdgesType parameter (0 ->
 * straight lines, 1 -> catmull rom curves, 2 -> bezier curves)
 * You can also specify the texture name if you want to create a textured complex polygon
 *
 * In complex polygon you can add a smooth border : see activateQuadBorder(..) function
 * And you can specify the texture zoom : see setTextureZoom(...) function
 */
class TLP_GL_SCOPE GlComplexPolygon : public GlEntity {

public:
  /**
   * @brief Default constructor
   * @warning don't use this constructor if you want to create a complex polygon, see others
   * constructors
   */
  GlComplexPolygon() = default;
  /**
   * @brief Constructor with a vector of coords, a fill color, a polygon edges type(0 -> straight
   * lines, 1 -> catmull rom curves, 2 -> bezier curves) and a textureName if you want
   */
  GlComplexPolygon(const std::vector<Coord> &coords, Color fcolor, int polygonEdgesType = 0,
                   const std::string &textureName = "");
  /**
   * @brief Constructor with a vector of coords, a fill color, an outline color, a polygon edges
   * type(0 -> straight lines, 1 -> catmull rom curves, 2 -> bezier curves) and a textureName if you
   * want
   */
  GlComplexPolygon(const std::vector<Coord> &coords, Color fcolor, Color ocolor,
                   int polygonEdgesType = 0, const std::string &textureName = "");
  /**
   * @brief Constructor with a vector of vector of coords (the first vector of coord is the polygon
   * and others vectors are holes in polygon), a fill color, a polygon edges type(0 -> straight
   * lines, 1 -> catmull rom curves, 2 -> bezier curves) and a textureName if you want
   */
  GlComplexPolygon(const std::vector<std::vector<Coord>> &coords, Color fcolor,
                   int polygonEdgesType = 0, const std::string &textureName = "");
  /**
   * @brief Constructor with a vector of vector of coords (the first vector of coord is the polygon
   * and others vectors are holes in polygon), a fill color, an outline color a polygon edges type(0
   * -> straight lines, 1 -> catmull rom curves, 2 -> bezier curves) and a textureName if you want
   */
  GlComplexPolygon(const std::vector<std::vector<Coord>> &coords, Color fcolor, Color ocolor,
                   int polygonEdgesType = 0, const std::string &textureName = "");

  ~GlComplexPolygon() override = default;

  /**
   * @brief Draw the complex polygon
   */
  void draw(float lod, Camera *camera) override;

  /**
   * @brief Set if the polygon is outlined or not
   */
  void setOutlineMode(const bool);

  /**
   * @brief Set size of outline
   */
  void setOutlineSize(double size);

  /**
   * @brief Set if the outline is stippled or not
   */
  void setOutlineStippled(bool stippled);

  /**
   * @brief Get fill color of GlComplexPolygon
   */
  Color getFillColor() const {
    return fillColor;
  }

  /**
   * @brief Set fill color of GlComplexPolygon
   */
  void setFillColor(const Color &color) {
    fillColor = color;
  }

  /**
   * @brief Get outline color of GlComplexPolygon
   */
  Color getOutlineColor() const {
    return outlineColor;
  }

  /**
   * @brief Set outline color of GlComplexPolygon
   */
  void setOutlineColor(const Color &color) {
    outlineColor = color;
  }

  /**
   * @brief Get the texture zoom factor
   */
  float getTextureZoom() const {
    return textureZoom;
  }

  /**
   * @brief Set the texture zoom factor
   *
   * By default if you have a polygon with a size bigger than (1,1,0) the texture will be repeated
   * If you want to don't have this texture repeat you have to modify texture zoom
   * For example if you have a polygon with coords ((0,0,0),(5,0,0),(5,5,0),(0,5,0)) you can set
   * texture zoom to 5. to don't have texture repeat
   */
  void setTextureZoom(float zoom) {
    textureZoom = zoom;
    runTessellation();
  }

  /**
   * @brief Get the textureName
   */
  std::string getTextureName();

  /**
   * @brief Set the textureName
   */
  void setTextureName(const std::string &name);

  /**
   * @brief Draw a thick (textured) border around the polygon.
   *
   * The graphic card must support geometry shader to make this feature to work.
   * The position parameter determines the way the border is drawn (depending on the polygon points
   * ordering):
   *     - 0 : the border is drawn outside (or inside) the polygon
   *     - 1 : the border is centered on the polygon outline
   *     - 2 : the border is drawn inside (or outside) the polygon
   *
   * The texCoordFactor parameter determines the way the texture is applied : if < 1, the texture
   * will be expanded and > 1, the texture will be compressed
   * The polygonId parameter determines on which contour of the polygon, the border will be applied
   */
  void activateQuadBorder(const float borderWidth, const Color &color,
                          const std::string &texture = "", const int position = 1,
                          const float texCoordFactor = 1.f, const int polygonId = 0);

  /**
   * @brief Deactivate the textured quad border
   */
  void deactivateQuadBorder(const int polygonId = 0);

  /**
   * @brief Translate entity
   */
  void translate(const Coord &move) override;

  /**
   * @brief Function to export data and type outString (in XML format)
   */
  void getXML(std::string &outString) override;

  /**
   * @brief Function to export data in outString (in XML format)
   */
  virtual void getXMLOnlyData(std::string &outString);

  /**
   * @brief Function to set data with inString (in XML format)
   */
  void setWithXML(const std::string &inString, uint &currentPosition) override;

  const std::vector<std::vector<Coord>> &getPolygonSides() const {
    return points;
  }

protected:
  /**
   * @brief Add a new point in polygon
   */
  virtual void addPoint(const Coord &point);
  /**
   * @brief Begin a new hole in the polygon
   */
  virtual void beginNewHole();

  void runTessellation();
  void createPolygon(const std::vector<Coord> &coords, int polygonEdgesType);

  std::vector<std::vector<Coord>> points;
  std::vector<std::vector<float>> pointsIdx;
  std::vector<float> verticesData;
  std::vector<uint> verticesIndices;
  int currentVector;
  bool outlined;
  Color fillColor;
  Color outlineColor;
  double outlineSize;
  bool outlineStippled;
  std::string textureName;
  float textureZoom;
  std::vector<bool> quadBorderActivated;
  std::vector<float> quadBorderWidth;
  std::vector<Color> quadBorderColor;
  std::vector<std::string> quadBorderTexture;
  std::vector<int> quadBorderPosition;
  std::vector<float> quadBorderTexFactor;
};
}
#endif // TALIPOT_GL_COMPLEX_POLYGON_H
