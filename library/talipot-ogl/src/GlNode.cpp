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
  coord = data->getElementLayout()->getNodeValue(n);
  glyph = data->getElementShape()->getNodeValue(n);
  size = data->getElementSize()->getNodeValue(n);
  rot = data->getElementRotation()->getNodeValue(n);
  selected = data->getElementSelected()->getNodeValue(n);
  labelRot = data->getElementLabelRotation()->getNodeValue(n);
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
  const Color &colorSelect2 = data->parameters->getSelectionColor();

  glEnable(GL_CULL_FACE);

  // do not render metanode is lod is too low
  if (data->getGraph()->isMetaNode(n) && lod >= LOD_MIN_TRESHOLD) {
    data->getMetaNodeRenderer()->render(n, lod, camera);
  }

  // less than four pixel on screen, we use points instead of glyphs
  if (lod < LOD_MIN_TRESHOLD) {
    if (lod < 1) {
      lod = 1;
    }

    if (data->getGlVertexArrayManager()->renderingIsBegin()) {
      data->getGlVertexArrayManager()->activatePointNodeDisplay(this, selected);
    } else {
      glDisable(GL_LIGHTING);
      setColor(selected ? colorSelect2
                        : ((data->getElementBorderWidth()->getNodeValue(n) > 0)
                               ? data->getElementBorderColor()->getNodeValue(n)
                               : data->getElementColor()->getNodeValue(n)));
      glPointSize(4);
      glBegin(GL_POINTS);
      glVertex3f(coord[0], coord[1], coord[2] + size[2] / 2.);
      glEnd();
      glEnable(GL_LIGHTING);
    }

    return;
  }

  if (!data->parameters->isDisplayNodes()) {
    return;
  }

  // If node size in z is equal to 0 we have to scale with FLT_EPSILON to preserve normal
  // (because if we do a scale of (x,y,0) and if we have a normal like (0,0,1) the new normal after
  // scale will be (0,0,0) and we will have light problem)
  Size nodeSize = size;
  if (nodeSize[2] == 0) {
    nodeSize[2] = FLT_EPSILON;
  }

  auto *glyphObj = data->glyphManager->getGlyph(glyph);
  // Some glyphs can not benefit from the shader rendering optimization
  // due to the use of quadrics or modelview matrix modification or lighting effect
  if (data->getGlGlyphRenderer()->renderingHasStarted() && glyphObj->shaderSupported()) {
    data->getGlGlyphRenderer()->addNodeGlyphRendering(glyphObj, n, lod, coord, nodeSize, rot,
                                                      selected);
  } else {

    if (selected) {
      glStencilFunc(GL_LEQUAL, data->parameters->getSelectedNodesStencil(), 0xFFFF);
    } else {
      glStencilFunc(GL_LEQUAL, data->parameters->getNodesStencil(), 0xFFFF);
    }

    // draw a glyph or make recursive call for meta nodes
    glPushMatrix();
    glTranslatef(coord[0], coord[1], coord[2]);
    glRotatef(rot, 0., 0., 1.);

    glScalef(nodeSize[0], nodeSize[1], nodeSize[2]);

    if (selected) {
      selectionBox.setStencil(data->parameters->getSelectedNodesStencil() - 1);
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
  if (data->glyphManager->getGlyph(glyph)->renderLabel()) {
    return;
  }

  // Color of the label : selected or not
  const Color &fontColor = selected ? data->parameters->getSelectionColor()
                                    : data->getElementLabelColor()->getNodeValue(n);
  const Color &fontBorderColor = selected ? data->parameters->getSelectionColor()
                                          : data->getElementLabelBorderColor()->getNodeValue(n);
  float fontBorderWidth = data->getElementLabelBorderWidth()->getNodeValue(n);

  // If we have transparent label : return
  if (fontColor.getA() == 0 && (fontBorderColor.getA() == 0 || fontBorderWidth == 0)) {
    return;
  }

  // Node text
  const string &tmp = data->getElementLabel()->getNodeValue(n);

  if (tmp.length() < 1) {
    return;
  }

  if (selected) {
    label.instance().setStencil(data->parameters->getSelectedNodesStencil());
  } else {
    label.instance().setStencil(data->parameters->getNodesLabelStencil());
  }

  int fontSize = data->getElementFontSize()->getNodeValue(n);

  if (fontSize <= 0) {
    return;
  }

  if (selected) {
    fontSize += 2;
  }

  int labelPos = data->getElementLabelPosition()->getNodeValue(n);

  BoundingBox includeBB = data->glyphManager->getGlyph(glyph)->getTextBoundingBox(n);
  Coord centerBB = includeBB.center();
  Vec3f sizeBB = includeBB[1] - includeBB[0];

  label.instance().setFontNameSizeAndColor(data->getElementFont()->getNodeValue(n), fontSize,
                                           fontColor);
  label.instance().setOutlineColor(fontBorderColor);
  label.instance().setOutlineSize(fontBorderWidth);
  label.instance().setText(tmp);
  label.instance().setTranslationAfterRotation(centerBB * size);
  label.instance().setSize(Size(size[0] * sizeBB[0], size[1] * sizeBB[1], 0));
  label.instance().setSizeForOutAlign(Size(size[0], size[1], 0));
  label.instance().rotate(0, 0, labelRot);
  label.instance().setAlignment(labelPos);
  label.instance().setScaleToSize(data->parameters->isLabelScaled());
  label.instance().setUseLODOptimisation(true, this->getBoundingBox(data));
  label.instance().setLabelsDensity(data->parameters->getLabelsDensity());
  label.instance().setUseMinMaxSize(!data->parameters->isLabelFixedFontSize());
  label.instance().setMinSize(data->parameters->getMinSizeOfLabel());
  label.instance().setMaxSize(data->parameters->getMaxSizeOfLabel());
  label.instance().setOcclusionTester(test);
  label.instance().setBillboarded(data->parameters->getLabelsAreBillboarded());

  if (includeBB[1][2] != 0 && !data->parameters->getLabelsAreBillboarded()) {
    label.instance().setPosition(Coord(coord[0], coord[1], coord[2] + size[2] / 2.));
  } else {
    label.instance().setPosition(Coord(coord[0], coord[1], coord[2]));
  }

  label.instance().drawWithStencil(lod, camera);
}
}
