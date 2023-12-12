/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
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

struct FontIconData {

  string fontFile;
  uint iconCodePoint;
  GLuint renderingDataBuffer;
  GLuint indicesBuffer;
  uint nbVertices;
  uint nbIndices;
  uint nbOutlineIndices;
  BoundingBox boundingBox;

public:
  FontIconData()
      : iconCodePoint(0), renderingDataBuffer(0), indicesBuffer(0), nbVertices(0), nbIndices(0),
        nbOutlineIndices(0) {}

  FontIconData(const std::string &iconName)
      : fontFile(IconicFont::getTTFLocation(iconName)),
        iconCodePoint(IconicFont::getIconCodePoint(iconName)), renderingDataBuffer(0),
        indicesBuffer(0), nbVertices(0), nbIndices(0), nbOutlineIndices(0) {}

  ~FontIconData() {
    if (renderingDataBuffer != 0) {
      glDeleteBuffers(1, &renderingDataBuffer);
    }

    if (indicesBuffer != 0) {
      glDeleteBuffers(1, &indicesBuffer);
    }
  }

  void render(const tlp::Color &fillColor, const tlp::Color &outlineColor,
              const float outlineSize) {

    if (renderingDataBuffer <= 10) {
      // delay icon geometry cache in GPU memory after a dozen of total icons
      // rendering to prevent glitches on linux using wayland (likely due to
      // uninitialized OpenGL state)
      glDeleteBuffers(1, &renderingDataBuffer);
      glDeleteBuffers(1, &indicesBuffer);
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

        if (!vertexIdx.contains(p)) {
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

static FontIconData defaultFontIconData;
static unordered_map<string, FontIconData> fontIconsData;

static FontIconData &getFontIconData(const string &iconName, ElementType eltType, uint eltId) {
  auto it = fontIconsData.find(iconName);
  if (it == fontIconsData.end()) {
    it = fontIconsData.insert({iconName, FontIconData(iconName)}).first;
  }

  if (it->second.iconCodePoint == 0) {
    // initialization of defaultFontIconData is delayed
    if (defaultFontIconData.iconCodePoint == 0) {
      static const std::string defaultIconName = FontAwesome::Solid::QuestionCircle;
      defaultFontIconData.iconCodePoint = IconicFont::getIconCodePoint(defaultIconName);
      defaultFontIconData.fontFile = IconicFont::getTTFLocation(defaultIconName);
    }

    if (iconName.empty()) {
      tlp::warning() << "Icon name for " << (eltType == ElementType::NODE ? "node " : "edge ")
                     << eltId << " is empty." << std::endl;
    } else {
      tlp::warning() << "Icon name '" << iconName << "' for "
                     << (eltType == ElementType::NODE ? "node " : "edge ") << eltId
                     << " does not exist." << std::endl;
    }

    return defaultFontIconData;
  }

  return it->second;
}

static void drawIcon(FontIconData &FontIconData, const Color &color, const Color &outlineColor,
                     const float outlineSize, const string &texture) {
  if (!texture.empty()) {
    GlTextureManager::activateTexture(texture);
  }

  FontIconData.render(color, outlineColor, outlineSize);

  GlTextureManager::deactivateTexture();
}

class FontIconDataGlyph : public Glyph {

public:
  GLYPHINFORMATION("2D - Icon", "Antoine Lambert", "26/02/2015", "Icon", "2.0", NodeShape::Icon)

  FontIconDataGlyph(const tlp::PluginContext *context = nullptr) : Glyph(context) {}

  ~FontIconDataGlyph() override = default;

  void draw(node n, float) override {
    const tlp::Color &nodeColor = glGraphInputData->colors()->getNodeValue(n);
    const tlp::Color &nodeBorderColor = glGraphInputData->borderColors()->getNodeValue(n);
    float nodeBorderWidth = glGraphInputData->borderWidths()->getNodeValue(n);
    const string &nodeTexture = glGraphInputData->renderingParameters()->getTexturePath() +
                                glGraphInputData->textures()->getNodeValue(n);

    drawIcon(getNodeFontIconData(n), nodeColor, nodeBorderColor, nodeBorderWidth, nodeTexture);
  }

  BoundingBox getIncludeBoundingBox(node n) override {
    return getNodeFontIconData(n).getBoundingBox();
  }

private:
  FontIconData &getNodeFontIconData(node n) {
    StringProperty *viewIcon = glGraphInputData->icons();
    const string &iconName = viewIcon->getNodeValue(n);
    return getFontIconData(iconName, ElementType::NODE, n.id);
  }
};

PLUGIN(FontIconDataGlyph)

class EEFontIconDataGlyph : public EdgeExtremityGlyph {

public:
  GLYPHINFORMATION("2D - Icon extremity", "Antoine Lambert", "02/03/2015",
                   "Icon for edge extremities", "2.0", EdgeExtremityShape::Icon)

  EEFontIconDataGlyph(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node n, const Color &glyphColor, const Color &borderColor, float) override {
    StringProperty *viewIcon = edgeExtGlGraphInputData->icons();
    const string &iconName = viewIcon->getEdgeValue(e);

    string edgeTexture = edgeExtGlGraphInputData->renderingParameters()->getTexturePath() +
                         edgeExtGlGraphInputData->textures()->getEdgeValue(e);
    float borderWidth = edgeExtGlGraphInputData->borderWidths()->getEdgeValue(e);

    // apply some rotation before rendering the icon in order
    // to visually encode the edge direction
    if (edgeExtGlGraphInputData->graph()->source(e) == n) {
      // anchor the bottom of the icon to the source node
      glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    } else {
      // anchor the top of the icon to the target node
      glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    }

    // icon must be mirrored along its Y axis to get a correct rendering
    glScalef(-1.0f, 1.0f, 1.0f);

    drawIcon(getFontIconData(iconName, ElementType::EDGE, e.id), glyphColor, borderColor,
             borderWidth, edgeTexture);
  }
};

PLUGIN(EEFontIconDataGlyph)
