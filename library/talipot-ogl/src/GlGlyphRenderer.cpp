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

#include <talipot/GlGlyphRenderer.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlShaderProgram.h>
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlBox.h>

using namespace std;

static string glyphShaderSrc = R"(#version 120

uniform vec3 pos;
uniform vec3 size;
uniform vec3 rotVector;
uniform float rotAngle;

mat4 scaleMatrix() {
  mat4 ret = mat4(1.0);
  ret[0][0] = size[0];
  ret[1][1] = size[1];
  ret[2][2] = size[2];
  return ret;
}

mat4 translationMatrix() {
  mat4 ret = mat4(1.0);
  ret[3][0] = pos[0];
  ret[3][1] = pos[1];
  ret[3][2] = pos[2];
  return ret;
}

mat4 rotationMatrix() {
  mat4 ret = mat4(1.0);
  float c = cos(rotAngle);
  float s = sin(rotAngle);
  ret[0][0] = rotVector[0] * rotVector[0] * (1.0 - c) + c;
  ret[1][0] = rotVector[0] * rotVector[1] * (1.0 - c) - rotVector[2] * s;
  ret[2][0] = rotVector[0] * rotVector[2] * (1.0 - c) + rotVector[1] * s;
  ret[0][1] = rotVector[1] * rotVector[0] * (1.0 - c) + rotVector[2] * s;
  ret[1][1] = rotVector[1] * rotVector[1] * (1.0 - c) + c;
  ret[2][1] = rotVector[1] * rotVector[2] * (1.0 - c) - rotVector[0] * s;
  ret[0][2] = rotVector[0] * rotVector[2] * (1.0 - c) - rotVector[1] * s;
  ret[1][2] = rotVector[1] * rotVector[2] * (1.0 - c) + rotVector[0] * s;
  ret[2][2] = rotVector[2] * rotVector[2] * (1.0 - c) + c;
  return ret;
}

void main() {
  gl_Position = gl_ModelViewProjectionMatrix * translationMatrix() * rotationMatrix()
                * scaleMatrix() * gl_Vertex;
  gl_FrontColor = gl_Color;
  gl_TexCoord[0] = gl_MultiTexCoord0;
}

)";

namespace tlp {

unique_ptr<GlShaderProgram> GlGlyphRenderer::_glyphShader;
unique_ptr<GlBox> GlGlyphRenderer::_selectionBox;

void GlGlyphRenderer::startRendering() {
  _nodeGlyphsToRender.clear();
  _edgeExtremityGlyphsToRender.clear();
  _nodeGlyphsToRender.reserve(_inputData->graph()->numberOfNodes());
  _edgeExtremityGlyphsToRender.reserve(_inputData->graph()->numberOfEdges());

  if (GlShaderProgram::shaderProgramsSupported() && _glyphShader.get() == nullptr) {
    _glyphShader.reset(new GlShaderProgram("glyph"));
    _glyphShader->addShaderFromSourceCode(Vertex, glyphShaderSrc);
    _glyphShader->link();
    _glyphShader->printInfoLog();
  }

  if (_glyphShader && _glyphShader->isLinked() && !GlShaderProgram::getCurrentActiveShader()) {
    _renderingStarted = true;
  }
}

bool GlGlyphRenderer::renderingHasStarted() const {
  return _renderingStarted;
}

void GlGlyphRenderer::addNodeGlyphRendering(Glyph *glyph, node n, float lod, const Coord &nodePos,
                                            const Size &nodeSize, float nodeRot, bool selected) {
  _nodeGlyphsToRender.push_back(NodeGlyphData(glyph, n, lod, nodePos, nodeSize, nodeRot, selected));
}

void GlGlyphRenderer::addEdgeExtremityGlyphRendering(EdgeExtremityGlyph *glyph, edge e, node source,
                                                     Color glyphColor, Color glyphBorderColor,
                                                     float lod, Coord beginAnchor, Coord srcAnchor,
                                                     Size size, bool selected) {
  _edgeExtremityGlyphsToRender.push_back(EdgeExtremityGlyphData(
      glyph, e, source, glyphColor, glyphBorderColor, lod, beginAnchor, srcAnchor, size, selected));
}

void GlGlyphRenderer::endRendering() {

  if (!_renderingStarted) {
    return;
  }

  if (!_selectionBox.get()) {
    _selectionBox.reset(new GlBox(Coord(0, 0, 0), Size(1, 1, 1), Color(0, 0, 255, 255),
                                  Color(0, 255, 0, 255), false, true));
    _selectionBox->setOutlineSize(3);
  }

  const Color &colorSelect = _inputData->renderingParameters()->getSelectionColor();

  _glyphShader->activate();

  for (const auto &glyphData : _nodeGlyphsToRender) {
    if (glyphData.selected) {
      glStencilFunc(GL_LEQUAL, _inputData->renderingParameters()->getSelectedNodesStencil(),
                    0xFFFF);
    } else {
      glStencilFunc(GL_LEQUAL, _inputData->renderingParameters()->getNodesStencil(), 0xFFFF);
    }

    _glyphShader->setUniformVec3Float("pos", glyphData.nodePos);
    _glyphShader->setUniformVec3Float("size", glyphData.nodeSize);
    _glyphShader->setUniformVec3Float("rotVector", Coord(0, 0, 1));
    _glyphShader->setUniformFloat("rotAngle", glyphData.nodeRot * M_PI / 180);

    if (glyphData.selected) {
      _selectionBox->setStencil(_inputData->renderingParameters()->getSelectedNodesStencil() - 1);
      _selectionBox->setOutlineColor(colorSelect);
      _selectionBox->draw(10, nullptr);
    }

    glyphData.glyph->draw(glyphData.n, glyphData.lod);
  }

  for (const auto &glyphData : _edgeExtremityGlyphsToRender) {
    if (glyphData.selected) {
      glStencilFunc(GL_LEQUAL, _inputData->renderingParameters()->getSelectedEdgesStencil(),
                    0xFFFF);
    } else {
      glStencilFunc(GL_LEQUAL, _inputData->renderingParameters()->getEdgesStencil(), 0xFFFF);
    }

    Coord dir = glyphData.srcAnchor - glyphData.beginAnchor;

    if (dir.norm() > 0) {
      dir /= dir.norm();
    }

    Coord cross = dir ^ Coord(1, 0, 0);

    if (cross.norm() > 0) {
      cross /= cross.norm();
    }

    _glyphShader->setUniformVec3Float("pos", glyphData.srcAnchor - glyphData.size / 2.f * dir);
    _glyphShader->setUniformVec3Float("size", glyphData.size);
    _glyphShader->setUniformVec3Float("rotVector", cross);
    _glyphShader->setUniformFloat("rotAngle", -acos(dir.dotProduct(Coord(1, 0, 0))));
    glyphData.glyph->draw(glyphData.e, glyphData.source, glyphData.glyphColor,
                          glyphData.glyphBorderColor, glyphData.lod);
  }

  _glyphShader->deactivate();

  _renderingStarted = false;
}
}
