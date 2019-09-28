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

#include <GL/glew.h>

#include <string>

#include <talipot/StringProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/Size.h>
#include <talipot/Coord.h>
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlTextureManager.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/Graph.h>
#include <talipot/GlTools.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>
#include <talipot/DrawingTools.h>
#include <talipot/OpenGlConfigManager.h>

using namespace std;
using namespace tlp;

namespace tlp {

static vector<Coord> ringVertices;
static vector<Vec2f> ringTexCoords;
static vector<unsigned short> ringIndices;
static vector<unsigned short> ringOutlineIndices;
static vector<unsigned int> buffers;

static void drawRing() {
  if (ringVertices.empty()) {
    const unsigned int numberOfSides = 30;
    ringVertices = computeRegularPolygon(30, Coord(0, 0, 0), Size(0.5, 0.5));
    vector<Coord> tmp = computeRegularPolygon(numberOfSides, Coord(0, 0, 0), Size(0.25, 0.25));
    ringVertices.insert(ringVertices.end(), tmp.begin(), tmp.end());

    for (unsigned int i = 0; i < numberOfSides - 1; ++i) {
      ringIndices.push_back(i);
      ringIndices.push_back(i + 1);
      ringIndices.push_back(numberOfSides + i);

      ringIndices.push_back(numberOfSides + i);
      ringIndices.push_back(i + 1);
      ringIndices.push_back(numberOfSides + i + 1);

      ringOutlineIndices.push_back(i);
      ringOutlineIndices.push_back(i + 1);

      ringOutlineIndices.push_back(numberOfSides + i);
      ringOutlineIndices.push_back(numberOfSides + i + 1);
    }

    ringIndices.push_back(numberOfSides - 1);
    ringIndices.push_back(0);
    ringIndices.push_back(numberOfSides + numberOfSides - 1);

    ringIndices.push_back(numberOfSides);
    ringIndices.push_back(0);
    ringIndices.push_back(numberOfSides + numberOfSides - 1);

    ringOutlineIndices.push_back(numberOfSides - 1);
    ringOutlineIndices.push_back(0);

    ringOutlineIndices.push_back(numberOfSides + numberOfSides - 1);
    ringOutlineIndices.push_back(numberOfSides);

    for (size_t i = 0; i < ringVertices.size(); ++i) {
      ringTexCoords.push_back(Vec2f(ringVertices[i][0] + 0.5, ringVertices[i][1] + 0.5));
    }

    buffers.resize(4);
    glGenBuffers(4, &buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, ringVertices.size() * 3 * sizeof(float), &ringVertices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, ringTexCoords.size() * 2 * sizeof(float), &ringTexCoords[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ringIndices.size() * sizeof(unsigned short),
                 &ringIndices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ringOutlineIndices.size() * sizeof(unsigned short),
                 &ringOutlineIndices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
  glDrawElements(GL_TRIANGLES, ringIndices.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void drawRingBorder() {
  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
  glDrawElements(GL_LINES, ringOutlineIndices.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  glDisableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void drawGlyph(const Color &glyphColor, const string &texture, const string &texturePath,
                      double borderWidth, const Color &borderColor, float lod) {

  setMaterial(glyphColor);

  if (!texture.empty()) {
    GlTextureManager::activateTexture(texturePath + texture);
  }

  drawRing();

  GlTextureManager::desactivateTexture();

  if (lod > 20 && borderWidth > 0) {
    glLineWidth(borderWidth);
    glDisable(GL_LIGHTING);
    setColor(borderColor);
    drawRingBorder();
    glEnable(GL_LIGHTING);
  }
}

/** \addtogroup glyph */

/// A 2D glyph
/**
 * This glyph draws a textured disc with a circular hole using the
 * "viewTexture" node property value.
 * If this property has no value, the ring
 * is then colored using the "viewColor" node property value.
 */
class Ring : public Glyph {
public:
  GLYPHINFORMATION("2D - Ring", "David Auber", "09/07/2002", "Textured Ring", "1.0",
                   NodeShape::Ring)
  Ring(const tlp::PluginContext *context = nullptr);
  ~Ring() override;
  void getIncludeBoundingBox(BoundingBox &boundingBox, node) override;
  virtual string getName() {
    return string("Ring");
  }
  void draw(node n, float lod) override;
};
PLUGIN(Ring)
Ring::Ring(const tlp::PluginContext *context) : Glyph(context) {}
Ring::~Ring() {}
void Ring::getIncludeBoundingBox(BoundingBox &boundingBox, node) {
  boundingBox[0] = Coord(-0.35f, -0.35f, 0);
  boundingBox[1] = Coord(0.35f, 0.35f, 0);
}
void Ring::draw(node n, float lod) {
  drawGlyph(glGraphInputData->getElementColor()->getNodeValue(n),
            glGraphInputData->getElementTexture()->getNodeValue(n),
            glGraphInputData->parameters->getTexturePath(),
            glGraphInputData->getElementBorderWidth()->getNodeValue(n),
            glGraphInputData->getElementBorderColor()->getNodeValue(n), lod);
}

class EERing : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Ring extremity", "David Auber", "09/07/2002",
                   "Textured Ring for edge extremities", "1.0", EdgeExtremityShape::Ring)

  EERing(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    glDisable(GL_LIGHTING);
    drawGlyph(glyphColor, edgeExtGlGraphInputData->getElementTexture()->getEdgeValue(e),
              edgeExtGlGraphInputData->parameters->getTexturePath(),
              edgeExtGlGraphInputData->getElementBorderWidth()->getEdgeValue(e), borderColor, lod);
  }
};
PLUGIN(EERing)

} // end of namespace tlp
