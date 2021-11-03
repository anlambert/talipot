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

#include <talipot/GlNode.h>

#include <talipot/GlMetaNodeRenderer.h>
#include <talipot/GlyphManager.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlVertexArrayManager.h>
#include <talipot/GlGlyphRenderer.h>

//====================================================

#define LOD_MIN_TRESHOLD 10.0

using namespace std;

namespace tlp {

Singleton<GlLabel> GlNode::label;

void GlNode::init(const GlGraphInputData *data) {
  coord = data->layout()->getNodeValue(n);
  glyph = data->shapes()->getNodeValue(n);
  size = data->sizes()->getNodeValue(n);
  rot = data->rotations()->getNodeValue(n);
  selected = data->selection()->getNodeValue(n);
  labelRot = data->labelRotations()->getNodeValue(n);
}

BoundingBox GlNode::getBoundingBox(const GlGraphInputData *data) {
  init(data);

  Coord tmp1 = size / 2.f;
  if (rot == 0) {
    BoundingBox box = {coord - tmp1, coord + tmp1};
    assert(box.isValid());
    return box;
  } else {
    float cosAngle = cos(rot / 180. * M_PI);
    float sinAngle = sin(rot / 180. * M_PI);
    Coord tmp2 = {tmp1[0], -tmp1[1], tmp1[2]};
    Coord tmp3 = {-tmp1[0], -tmp1[1], -tmp1[2]};
    Coord tmp4 = {-tmp1[0], tmp1[1], -tmp1[2]};
    tmp1.set(tmp1[0] * cosAngle - tmp1[1] * sinAngle, tmp1[0] * sinAngle + tmp1[1] * cosAngle,
             tmp1[2]);
    tmp2.set(tmp2[0] * cosAngle - tmp2[1] * sinAngle, tmp2[0] * sinAngle + tmp2[1] * cosAngle,
             tmp2[2]);
    tmp3.set(tmp3[0] * cosAngle - tmp3[1] * sinAngle, tmp3[0] * sinAngle + tmp3[1] * cosAngle,
             tmp3[2]);
    tmp4.set(tmp4[0] * cosAngle - tmp4[1] * sinAngle, tmp4[0] * sinAngle + tmp4[1] * cosAngle,
             tmp4[2]);

    BoundingBox bb = {coord + tmp1, coord + tmp2};
    bb.expand(coord + tmp3);
    bb.expand(coord + tmp4);
    return bb;
  }
}

void GlNode::draw(float lod, const GlGraphInputData *data, Camera *camera) {
  init(data);
  const Color &colorSelect2 = data->renderingParameters()->getSelectionColor();

  glEnable(GL_CULL_FACE);

  // do not render metanode is lod is too low
  if (data->graph()->isMetaNode(n) && lod >= LOD_MIN_TRESHOLD) {
    data->metaNodeRenderer()->render(n, lod, camera);
  }

  // less than four pixel on screen, we use points instead of glyphs
  if (lod < LOD_MIN_TRESHOLD) {
    if (lod < 1) {
      lod = 1;
    }

    if (data->glVertexArrayManager()->renderingIsBegin()) {
      data->glVertexArrayManager()->activatePointNodeDisplay(this, selected);
    } else {
      glDisable(GL_LIGHTING);
      setColor(selected ? colorSelect2
                        : ((data->borderWidths()->getNodeValue(n) > 0)
                               ? data->borderColors()->getNodeValue(n)
                               : data->colors()->getNodeValue(n)));
      glPointSize(4);
      glBegin(GL_POINTS);
      glVertex3f(coord[0], coord[1], coord[2] + size[2] / 2.);
      glEnd();
      glEnable(GL_LIGHTING);
    }

    return;
  }

  if (!data->renderingParameters()->isDisplayNodes()) {
    return;
  }

  // If node size in z is equal to 0 we have to scale with FLT_EPSILON to preserve normal
  // (because if we do a scale of (x,y,0) and if we have a normal like (0,0,1) the new normal after
  // scale will be (0,0,0) and we will have light problem)
  Size nodeSize = size;
  if (nodeSize[2] == 0) {
    nodeSize[2] = FLT_EPSILON;
  }

  auto *glyphObj = data->glyphManager()->getGlyph(glyph);
  // Some glyphs can not benefit from the shader rendering optimization
  // due to the use of quadrics or modelview matrix modification or lighting effect
  if (data->glGlyphRenderer()->renderingHasStarted() && glyphObj->shaderSupported()) {
    data->glGlyphRenderer()->addNodeGlyphRendering(glyphObj, n, lod, coord, nodeSize, rot,
                                                   selected);
  } else {

    if (selected) {
      glStencilFunc(GL_LEQUAL, data->renderingParameters()->getSelectedNodesStencil(), 0xFFFF);
    } else {
      glStencilFunc(GL_LEQUAL, data->renderingParameters()->getNodesStencil(), 0xFFFF);
    }

    // draw a glyph or make recursive call for meta nodes
    glPushMatrix();
    glTranslatef(coord[0], coord[1], coord[2]);
    glRotatef(rot, 0., 0., 1.);

    glScalef(nodeSize[0], nodeSize[1], nodeSize[2]);

    if (selected) {
      selectionBox.setStencil(data->renderingParameters()->getSelectedNodesStencil() - 1);
      selectionBox.setOutlineColor(colorSelect2);
      selectionBox.draw(10, nullptr);
    }

    glyphObj->draw(n, lod);

    glPopMatrix();
  }
}

void GlNode::drawLabel(bool drawSelect, OcclusionTest *test, const GlGraphInputData *data,
                       float lod) {
  init(data);

  if (drawSelect != selected) {
    return;
  }

  drawLabel(test, data, lod);
}

void GlNode::drawLabel(OcclusionTest *test, const GlGraphInputData *data) {
  GlNode::drawLabel(test, data, 1000.);
}

void GlNode::drawLabel(OcclusionTest *test, const GlGraphInputData *data, float lod,
                       Camera *camera) {
  init(data);
  // If glyph cannot render label: return
  if (data->glyphManager()->getGlyph(glyph)->renderLabel()) {
    return;
  }

  // Color of the label : selected or not
  const Color &fontColor = selected ? data->renderingParameters()->getSelectionColor()
                                    : data->labelColors()->getNodeValue(n);
  const Color &fontBorderColor = selected ? data->renderingParameters()->getSelectionColor()
                                          : data->labelBorderColors()->getNodeValue(n);
  float fontBorderWidth = data->labelBorderWidths()->getNodeValue(n);

  // If we have transparent label : return
  if (fontColor.getA() == 0 && (fontBorderColor.getA() == 0 || fontBorderWidth == 0)) {
    return;
  }

  // Node text
  const string &tmp = data->labels()->getNodeValue(n);

  if (tmp.length() < 1) {
    return;
  }

  if (selected) {
    label.instance().setStencil(data->renderingParameters()->getSelectedNodesStencil());
  } else {
    label.instance().setStencil(data->renderingParameters()->getNodesLabelStencil());
  }

  int fontSize = data->fontSizes()->getNodeValue(n);

  if (fontSize <= 0) {
    return;
  }

  if (selected) {
    fontSize += 2;
  }

  int labelPos = data->labelPositions()->getNodeValue(n);

  BoundingBox includeBB = data->glyphManager()->getGlyph(glyph)->getTextBoundingBox(n);
  Coord centerBB = includeBB.center();
  Vec3f sizeBB = includeBB[1] - includeBB[0];

  label.instance().setFontNameSizeAndColor(data->fonts()->getNodeValue(n), fontSize, fontColor);
  label.instance().setOutlineColor(fontBorderColor);
  label.instance().setOutlineSize(fontBorderWidth);
  label.instance().setText(tmp);
  label.instance().setTranslationAfterRotation(centerBB * size);
  label.instance().setSize(Size(size[0] * sizeBB[0], size[1] * sizeBB[1], 0));
  label.instance().setSizeForOutAlign(Size(size[0], size[1], 0));
  label.instance().rotate(0, 0, labelRot);
  label.instance().setAlignment(labelPos);
  label.instance().setScaleToSize(data->renderingParameters()->isLabelScaled());
  label.instance().setUseLODOptimisation(true, this->getBoundingBox(data));
  label.instance().setLabelsDensity(data->renderingParameters()->getLabelsDensity());
  label.instance().setUseMinMaxSize(!data->renderingParameters()->isLabelFixedFontSize());
  label.instance().setMinSize(data->renderingParameters()->getMinSizeOfLabel());
  label.instance().setMaxSize(data->renderingParameters()->getMaxSizeOfLabel());
  label.instance().setOcclusionTester(test);
  label.instance().setBillboarded(data->renderingParameters()->getLabelsAreBillboarded());

  if (includeBB[1][2] != 0 && !data->renderingParameters()->getLabelsAreBillboarded()) {
    label.instance().setPosition(Coord(coord[0], coord[1], coord[2] + size[2] / 2.));
  } else {
    label.instance().setPosition(Coord(coord[0], coord[1], coord[2]));
  }

  label.instance().drawWithStencil(lod, camera);
}
}
