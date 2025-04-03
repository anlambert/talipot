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

#ifndef TALIPOT_GL_GLYPH_RENDERER_H
#define TALIPOT_GL_GLYPH_RENDERER_H

#include <talipot/Coord.h>
#include <talipot/Size.h>
#include <talipot/Color.h>
#include <talipot/Node.h>
#include <talipot/Edge.h>

#include <memory>
#include <vector>

namespace tlp {

class GlGraphInputData;
class GlShaderProgram;
class Glyph;
class EdgeExtremityGlyph;
class GlBox;

struct TLP_GL_SCOPE NodeGlyphData {

    NodeGlyphData() = default;

    NodeGlyphData(Glyph *glyph, node n, float lod, const Coord &nodePos, const Size &nodeSize,
                  float nodeRot, bool selected)
        : glyph(glyph), n(n), lod(lod), nodePos(nodePos), nodeSize(nodeSize), nodeRot(nodeRot),
          selected(selected) {}

    Glyph *glyph;
    node n;
    float lod;
    Coord nodePos;
    Size nodeSize;
    float nodeRot;
    bool selected;
};

struct TLP_GL_SCOPE EdgeExtremityGlyphData {

    EdgeExtremityGlyphData() = default;

    EdgeExtremityGlyphData(EdgeExtremityGlyph *glyph, edge e, node source, Color glyphColor,
                           Color glyphBorderColor, float lod, Coord beginAnchor, Coord srcAnchor,
                           Size size, bool selected)
        : glyph(glyph), e(e), source(source), glyphColor(glyphColor),
          glyphBorderColor(glyphBorderColor), lod(lod), beginAnchor(beginAnchor),
          srcAnchor(srcAnchor), size(size), selected(selected) {}

    EdgeExtremityGlyph *glyph;
    edge e;
    node source;
    Color glyphColor;
    Color glyphBorderColor;
    float lod;
    Coord beginAnchor;
    Coord srcAnchor;
    Size size;
    bool selected;
};

class TLP_GL_SCOPE GlGlyphRenderer {

  public:
    GlGlyphRenderer(GlGraphInputData *inputData)
        : _inputData(inputData), _renderingStarted(false) {}

    void startRendering();

    bool renderingHasStarted() const;

    void addNodeGlyphRendering(Glyph *glyph, node n, float lod, const Coord &nodePos,
                               const Size &nodeSize, float nodeRot, bool selected);

    void addEdgeExtremityGlyphRendering(EdgeExtremityGlyph *glyph, edge e, node source,
                                        Color glyphColor, Color glyphBorderColor, float lod,
                                        Coord beginAnchor, Coord srcAnchor, Size size,
                                        bool selected);

    void endRendering();

  private:
    GlGraphInputData *_inputData;
    bool _renderingStarted;
    std::vector<NodeGlyphData> _nodeGlyphsToRender;
    std::vector<EdgeExtremityGlyphData> _edgeExtremityGlyphsToRender;
    static std::unique_ptr<GlShaderProgram> _glyphShader;
    static std::unique_ptr<GlBox> _selectionBox;
};
}

#endif // TALIPOT_GL_GLYPH_RENDERER_H
