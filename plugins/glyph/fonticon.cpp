/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <FTVectoriser.h>
#include <FTLibrary.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <talipot/BoundingBox.h>
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/IconicFont.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/GlTextureManager.h>
#include <talipot/ViewSettings.h>
#include <talipot/FontAwesome.h>

#define ushort_cast(x) static_cast<unsigned short>((x))

#define HRES 64
#define HRESf 64.f
#define DPI 72

using namespace std;
using namespace tlp;

struct FontIcon {

  string fontFile;
  uint iconCodePoint;
  GLuint renderingDataBuffer;
  GLuint indicesBuffer;
  uint nbVertices;
  uint nbIndices;
  uint nbOutlineIndices;
  BoundingBox boundingBox;

public:
  FontIcon()
      : iconCodePoint(0), renderingDataBuffer(0), indicesBuffer(0), nbVertices(0), nbIndices(0),
        nbOutlineIndices(0) {}

  FontIcon(const std::string &iconName)
      : fontFile(IconicFont::getTTFLocation(iconName)),
        iconCodePoint(IconicFont::getIconCodePoint(iconName)), renderingDataBuffer(0),
        indicesBuffer(0), nbVertices(0), nbIndices(0), nbOutlineIndices(0) {}

  ~FontIcon() {
    if (renderingDataBuffer != 0) {
      glDeleteBuffers(1, &renderingDataBuffer);
    }

    if (indicesBuffer != 0) {
      glDeleteBuffers(1, &indicesBuffer);
    }
  }

  void render(const tlp::Color &fillColor, const tlp::Color &outlineColor,
              const float outlineSize) {

    if (renderingDataBuffer == 0) {
      tesselateIcon();
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, renderingDataBuffer);
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(nbVertices * 3 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    setMaterial(fillColor);
    glDrawElements(GL_TRIANGLES, nbIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if (outlineSize > 0) {
      setMaterial(outlineColor);
      glLineWidth(outlineSize);
      glDrawElements(GL_LINES, nbOutlineIndices, GL_UNSIGNED_SHORT,
                     BUFFER_OFFSET(nbIndices * sizeof(unsigned short)));
    }

    glDisableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  const BoundingBox &getBoundingBox() const {
    return boundingBox;
  }

  void tesselateIcon() {

    const FT_Library *library = FTLibrary::Instance().GetLibrary();

    FT_Face face;

    FT_Error err = FT_New_Face(*library, fontFile.c_str(), 0, &face);

    if (err) {
      return;
    }

    err = FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    if (err) {
      return;
    }

    float size = 20;

    err = FT_Set_Char_Size(face, int(size * HRES), 0, DPI * HRES, DPI * HRES);

    if (err) {
      return;
    }

    FT_UInt glyph_index = FT_Get_Char_Index(face, iconCodePoint);

    err = FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_HINTING);

    if (err) {
      return;
    }

    FTVectoriser vectoriser(face->glyph);

    vectoriser.MakeMesh(1.0, 1, 0.0);

    const FTMesh *mesh = vectoriser.GetMesh();

    tlp::BoundingBox meshBB;

    vector<Coord> vertices;
    vector<Vec2f> texCoords;
    vector<unsigned short> indices;
    vector<unsigned short> outlineIndices;

    unordered_map<Coord, uint> vertexIdx;

    uint idx = 0;

    for (uint t = 0; t < mesh->TesselationCount(); ++t) {
      const FTTesselation *subMesh = mesh->Tesselation(t);

      for (uint i = 0; i < subMesh->PointCount(); ++i) {
        FTPoint point = subMesh->Point(i);
        Coord p = {point.Xf() / HRESf, point.Yf() / HRESf};

        if (vertexIdx.find(p) == vertexIdx.end()) {
          meshBB.expand(p);
          vertices.push_back(p);
          indices.push_back(idx++);
          vertexIdx[vertices.back()] = indices.back();
        } else {
          indices.push_back(vertexIdx[p]);
        }
      }
    }

    for (uint t = 0; t < vectoriser.ContourCount(); ++t) {
      const FTContour *contour = vectoriser.Contour(t);

      for (uint i = 0; i < contour->PointCount() - 1; ++i) {
        FTPoint point = contour->Point(i);
        Coord p = {point.Xf() / HRESf, point.Yf() / HRESf};
        outlineIndices.push_back(ushort_cast(vertexIdx[p]));
        point = contour->Point(i + 1);
        p = {point.Xf() / HRESf, point.Yf() / HRESf};
        outlineIndices.push_back(ushort_cast(vertexIdx[p]));
      }

      FTPoint point = contour->Point(contour->PointCount() - 1);
      Coord p = {point.Xf() / HRESf, point.Yf() / HRESf};
      outlineIndices.push_back(ushort_cast(vertexIdx[p]));
      point = contour->Point(0);
      p = {point.Xf() / HRESf, point.Yf() / HRESf};
      outlineIndices.push_back(ushort_cast(vertexIdx[p]));
    }

    tlp::Coord minC = meshBB[0];
    tlp::Coord maxC = meshBB[1];

    for (auto &vertice : vertices) {
      if (meshBB.height() > meshBB.width()) {
        vertice[0] = ((vertice[0] - minC[0]) / (maxC[0] - minC[0]) - 0.5) *
                     (meshBB.width() / float(meshBB.height()));
        vertice[1] = ((vertice[1] - minC[1]) / (maxC[1] - minC[1])) - 0.5;
      } else {
        vertice[0] = ((vertice[0] - minC[0]) / (maxC[0] - minC[0])) - 0.5;
        vertice[1] = (((vertice[1] - minC[1]) / (maxC[1] - minC[1])) - 0.5) *
                     (meshBB.height() / float(meshBB.width()));
      }

      const tlp::Coord &v = vertice;
      texCoords.push_back(Vec2f(v[0] + 0.5, v[1] + 0.5));
    }

    glGenBuffers(1, &renderingDataBuffer);
    glGenBuffers(1, &indicesBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, renderingDataBuffer);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() * 3 + texCoords.size() * 2) * sizeof(float),
                 nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * 3 * sizeof(float), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float),
                    texCoords.size() * 2 * sizeof(float), &texCoords[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (indices.size() + outlineIndices.size()) * sizeof(unsigned short), nullptr,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned short),
                    &indices[0]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short),
                    outlineIndices.size() * sizeof(unsigned short), &outlineIndices[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    nbVertices = vertices.size();
    nbIndices = indices.size();
    nbOutlineIndices = outlineIndices.size();

    for (const auto &vertice : vertices) {
      boundingBox.expand(vertice);
    }
  }
};

static FontIcon defaultFontIcon;
static unordered_map<string, FontIcon> fontIcons;

static FontIcon &getFontIcon(const string &iconName) {
  if (iconName.empty() || !IconicFont::isIconSupported(iconName)) {
    // initialization of defaultFontIcon is delayed
    if (defaultFontIcon.iconCodePoint == 0) {
      static const std::string defaultIconName = FontAwesome::Solid::QuestionCircle;
      defaultFontIcon.iconCodePoint = IconicFont::getIconCodePoint(defaultIconName);
      defaultFontIcon.fontFile = IconicFont::getTTFLocation(defaultIconName);
    }
    return defaultFontIcon;
  }
  auto it = fontIcons.find(iconName);
  if (fontIcons.find(iconName) == fontIcons.end()) {
    it = fontIcons.insert({iconName, FontIcon(iconName)}).first;
  }
  return it->second;
}

static void drawIcon(FontIcon &fontIcon, const Color &color, const Color &outlineColor,
                     const float outlineSize, const string &texture) {
  if (!texture.empty()) {
    GlTextureManager::activateTexture(texture);
  }

  fontIcon.render(color, outlineColor, outlineSize);

  GlTextureManager::deactivateTexture();
}

class FontIconGlyph : public Glyph {

public:
  GLYPHINFORMATION("2D - Icon", "Antoine Lambert", "26/02/2015", "Icon", "2.0", NodeShape::Icon)

  FontIconGlyph(const tlp::PluginContext *context = nullptr) : Glyph(context) {}

  ~FontIconGlyph() override = default;

  void draw(node n, float) override {
    const tlp::Color &nodeColor = glGraphInputData->getElementColor()->getNodeValue(n);
    const tlp::Color &nodeBorderColor = glGraphInputData->getElementBorderColor()->getNodeValue(n);
    float nodeBorderWidth = glGraphInputData->getElementBorderWidth()->getNodeValue(n);
    const string &nodeTexture = glGraphInputData->parameters->getTexturePath() +
                                glGraphInputData->getElementTexture()->getNodeValue(n);

    drawIcon(getNodeFontIcon(n), nodeColor, nodeBorderColor, nodeBorderWidth, nodeTexture);
  }

  BoundingBox getIncludeBoundingBox(node n) override {
    return getNodeFontIcon(n).getBoundingBox();
  }

private:
  FontIcon &getNodeFontIcon(node n) {
    StringProperty *viewIcon = glGraphInputData->getElementIcon();
    const string &iconName = viewIcon->getNodeValue(n);
    return getFontIcon(iconName);
  }
};

PLUGIN(FontIconGlyph)

class EEFontIconGlyph : public EdgeExtremityGlyph {

public:
  GLYPHINFORMATION("2D - Icon extremity", "Antoine Lambert", "02/03/2015",
                   "Icon for edge extremities", "2.0", EdgeExtremityShape::Icon)

  EEFontIconGlyph(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node n, const Color &glyphColor, const Color &borderColor, float) override {
    StringProperty *viewIcon = edgeExtGlGraphInputData->getElementIcon();
    const string &iconName = viewIcon->getEdgeValue(e);

    string edgeTexture = edgeExtGlGraphInputData->parameters->getTexturePath() +
                         edgeExtGlGraphInputData->getElementTexture()->getEdgeValue(e);
    float borderWidth = edgeExtGlGraphInputData->getElementBorderWidth()->getEdgeValue(e);

    // apply some rotation before rendering the icon in order
    // to visually encode the edge direction
    if (edgeExtGlGraphInputData->getGraph()->source(e) == n) {
      // anchor the bottom of the icon to the source node
      glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    } else {
      // anchor the top of the icon to the target node
      glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    }

    // icon must be mirrored along its Y axis to get a correct rendering
    glScalef(-1.0f, 1.0f, 1.0f);

    drawIcon(getFontIcon(iconName), glyphColor, borderColor, borderWidth, edgeTexture);
  }
};

PLUGIN(EEFontIconGlyph)
