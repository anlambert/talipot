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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <FTGL/ftgl.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <talipot/GlLabel.h>
#include <talipot/OcclusionTest.h>
#include <talipot/OcclusionTest.h>
#include <talipot/GlTextureManager.h>
#include <talipot/GlXMLTools.h>
#include <talipot/ViewSettings.h>

#include <fribidi/fribidi.h>

using namespace std;

namespace tlp {

// FTGL fonts must be cached to avoid to much memory consumption
static flat_hash_map<std::string, unique_ptr<FTPolygonFont>> polygonFonts;
static flat_hash_map<std::string, unique_ptr<FTGLOutlineFont>> outlineFonts;

static FTPolygonFont *getPolygonFont(const std::string &name) {
  auto itf = polygonFonts.find(name);

  if (itf == polygonFonts.end()) {
    auto *font = new FTPolygonFont(name.c_str());
    polygonFonts[name].reset(font);
    return font;
  }

  return itf->second.get();
}

static FTGLOutlineFont *getOutlineFont(const std::string &name) {
  auto itf = outlineFonts.find(name);

  if (itf == outlineFonts.end()) {
    auto *font = new FTGLOutlineFont(name.c_str());
    outlineFonts[name].reset(font);
    return font;
  }
  return itf->second.get();
}

// Utility function to reshape text for visual display using the
// Unicode Bidirectional Algorithm implemented by FriBidi library.
// It enables to improve the rendering of Complex text layout
// (arabic scripts for instance).
static std::string getVisualText(const std::string &text) {
  std::string visualText;
  const FriBidiCharSet enc = FRIBIDI_CHAR_SET_UTF8;
  FriBidiParType direction = FRIBIDI_PAR_ON;
  size_t len = text.size() * 2;

  auto *str_in = new FriBidiChar[len];
  auto *str_out = new FriBidiChar[len];

  // convert UTF8 to UTF32
  FriBidiStrIndex ulen = fribidi_charset_to_unicode(enc, text.c_str(), text.size(), str_in);
  // reshape the UTF32 string
  FriBidiLevel lvl = fribidi_log2vis(str_in, ulen, &direction, str_out, 0, 0, 0);
  if (lvl) {
    // convert shaped text back to UTF8
    char *output = new char[len];
    fribidi_unicode_to_charset(enc, str_out, ulen, output);
    visualText = output;
    delete[] output;
  } else {
    visualText = text;
  }

  delete[] str_in;
  delete[] str_out;
  return visualText;
}

static const int SpaceBetweenLine = 5;

GlLabel::GlLabel() : leftAlign(false), oldCamera(nullptr) {
  init();
}
GlLabel::GlLabel(Coord centerPosition, Size size, Color fontColor, bool leftAlign)
    : centerPosition(centerPosition), size(size), color(fontColor), leftAlign(leftAlign),
      oldCamera(nullptr) {
  init();
}

GlLabel::~GlLabel() = default;
//============================================================
void GlLabel::init() {
  font = Font::defaultFont();
  initFont();
  fontSize = 20;
  outlineColor.set(0, 0, 0, 255);
  outlineSize = 1.;
  renderingMode = 0;
  translationAfterRotation.set(0, 0, 0);
  alignment = LabelPosition::Center;
  scaleToSize = true;
  useMinMaxSize = false;
  minSize = 10;
  maxSize = 30;
  depthTestEnabled = true;
  occlusionTester = nullptr;
  xRot = 0;
  yRot = 0;
  zRot = 0;
  useLOD = false;
  labelsDensity = 100;
  billboarded = false;
  sizeForOutAlign = size;
  oldLod = -1;
}
//============================================================
void GlLabel::initFont() {
  TLP_LOCK_SECTION(init_talipot_font) {
    ftglPolygonFont = getPolygonFont(font.fontFile());

    if (ftglPolygonFont->Error() == 0) { // no error
      ftglOutlineFont = getOutlineFont(font.fontFile());
    } else {
      if (ftglPolygonFont->Error() || ftglOutlineFont->Error()) {
        tlp::warning() << "Error in font loading: \"" << font.fontFile() << "\" cannot be loaded "
                       << endl;
      }
    }
  }
  TLP_UNLOCK_SECTION(init_talipot_font);
}
//============================================================
void GlLabel::setText(const string &text) {

  this->text = text;

  if (ftglPolygonFont->Error()) {
    return;
  }

  if (ftglPolygonFont->FaceSize() != uint(fontSize)) {
    ftglPolygonFont->FaceSize(fontSize);
    ftglOutlineFont->FaceSize(fontSize);
  }

  // split each line
  textVector.clear();
  textWidthVector.clear();
  size_t lastPos = 0;
  size_t pos = this->text.find_first_of("\n");

  while (pos != string::npos) {
    string s = this->text.substr(lastPos, pos - lastPos);
    textVector.push_back(s);
    lastPos = pos + 1;
    pos = this->text.find_first_of("\n", pos + 1);
  }

  string s = this->text.substr(lastPos) + " ";
  textVector.push_back(s);

  // Text bounding box computation
  textBoundingBox = BoundingBox();

  float x1, y1, z1, x2, y2, z2;

  // After we compute width of text
  for (auto it = textVector.begin(); it != textVector.end(); ++it) {
    ftglPolygonFont->BBox(it->c_str(), x1, y1, z1, x2, y2, z2);
    textWidthVector.push_back(x2 - x1);

    if (it == textVector.begin()) {
      textBoundingBox.expand(Coord(0, y1, z1));
      textBoundingBox.expand(Coord(x2 - x1, y2, z2));
    } else {
      ftglPolygonFont->BBox(it->c_str(), x1, y1, z1, x2, y2, z2);

      if (x2 - x1 > textBoundingBox[1][0]) {
        textBoundingBox[1][0] = (x2 - x1);
      }

      textBoundingBox[0][1] -= fontSize + SpaceBetweenLine;
    }
  }
}
//============================================================
BoundingBox GlLabel::getBoundingBox() {
  if (!leftAlign) {
    return BoundingBox(centerPosition - size / 2.f, centerPosition + size / 2.f);
  } else {
    return BoundingBox(centerPosition - Coord(0, size[1] / 2.f, 0),
                       centerPosition + Coord(size[0], size[1] / 2.f, 0));
  }
}
//============================================================
void GlLabel::setFontName(const std::string &name) {
  if (font.fontName() == name || name.empty()) {
    return;
  }
  font = Font::fromName(name);
  initFont();
}
//============================================================
void GlLabel::setFontNameSizeAndColor(const std::string &name, const int &size,
                                      const Color &color) {
  setFontName(name);
  fontSize = size;
  this->color = color;
}
//============================================================
void GlLabel::drawWithStencil(float lod, Camera *camera) {
  glStencilFunc(GL_LEQUAL, stencil, 0xFFFF);
  draw(lod, camera);
}
//============================================================
float GlLabel::getHeightAfterScale() {
  float w = textBoundingBox[1][0] - textBoundingBox[0][0];
  float h = textBoundingBox[1][1] - textBoundingBox[0][1];
  float div_w, div_h;

  div_w = size[0] / w;
  div_h = size[1] / h;

  if (div_h * w > size[0]) {
    if (div_w < 4) {
      return size[1] * (div_w / 4.);
    }
  }

  return size[1];
}
//============================================================
void GlLabel::draw(float, Camera *camera) {

  if (fontSize <= 0 || ftglPolygonFont->Error()) {
    return;
  }

  bool computeLOD = false;

  if (oldLod == -1) {
    computeLOD = true;
  } else if (camera) {
    computeLOD = camera->getEyes() != oldCamera.getEyes() ||
                 camera->getCenter() != oldCamera.getCenter() ||
                 camera->getZoomFactor() != oldCamera.getZoomFactor() ||
                 camera->getViewport() != oldViewport;
  }

  float lod = oldLod;

  if (computeLOD && camera) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    Coord test =
        camera->viewportTo3DWorld(Coord(1, 1, 1)) - camera->viewportTo3DWorld(Coord(0, 0, 0));
    test /= test.norm();
    lod = (camera->worldTo2DViewport(test) - camera->worldTo2DViewport(Coord(0, 0, 0))).norm();
    oldLod = lod;
    oldCamera = *camera;
    oldViewport = camera->getViewport();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  if (depthTestEnabled) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  glPolygonMode(GL_FRONT, GL_FILL);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);

  // Store width and height of the text
  float w = textBoundingBox[1][0] - textBoundingBox[0][0];
  float h = textBoundingBox[1][1] - textBoundingBox[0][1];

  // avoid a division by zero for strings with only space chars
  if (h == 0) {
    h = 1;
  }

  // Compute scale of the text if text is scaled to a size
  float div_w, div_h;
  div_w = size[0] / w;
  div_h = size[1] / h;

  float viewportH;
  float multiLineH = h;

  // Here the 4.5 magic number is the size of space between two lines
  if (textVector.size() > 1) {
    multiLineH = (h - (textVector.size() - 1) * 4.5) / textVector.size();
  }

  // We compute the size of the text on the viewport
  viewportH = (multiLineH * lod) / 2.f;

  // Scale of the text
  float scaleToApply = 1.;

  if (scaleToSize) {
    if (div_h * w > size[0]) {
      scaleToApply = div_w;
    } else {
      scaleToApply = div_h;
    }
  } else {
    scaleToApply = 0.05f;

    if (useMinMaxSize) {
      float tmpViewportH = viewportH * 0.05f;

      if (tmpViewportH < minSize) {
        scaleToApply *= minSize / tmpViewportH;
      }

      if (tmpViewportH > maxSize) {
        scaleToApply *= maxSize / tmpViewportH;
      }
    }
  }

  // Occlusion part of the label
  if (occlusionTester && camera && labelsDensity != 100) {
    BoundingBox labelBoundingBox;
    Coord baseCoord = centerPosition;

    float wModified = w;
    float hModified = h;

    if (labelsDensity <= 0) {
      wModified -= labelsDensity;
      hModified -= labelsDensity;
    } else {
      wModified = w - w * (float(labelsDensity) / 100);
      hModified = h - h * (float(labelsDensity) / 100);
    }

    switch (alignment) {
    case LabelPosition::Center:
      break;

    case LabelPosition::Left:
      baseCoord[0] -= sizeForOutAlign[0] / 2 + wModified * scaleToApply / 2.;
      break;

    case LabelPosition::Right:
      baseCoord[0] += sizeForOutAlign[0] / 2 + wModified * scaleToApply / 2.;
      break;

    case LabelPosition::Top:
      baseCoord[1] += sizeForOutAlign[1] / 2 + hModified * scaleToApply / 2.;
      break;

    case LabelPosition::Bottom:
      baseCoord[1] -= sizeForOutAlign[1] / 2 + hModified * scaleToApply / 2.;
      break;

    default:
      break;
    }

    Size occlusionSize = {wModified * scaleToApply / 2.f, hModified * scaleToApply / 2.f};

    float angle = M_PI * zRot / 180;

    if (zRot != 0) {
      BoundingBox tmpBB;
      float size0Cos = occlusionSize[0] * cos(angle);
      float size0Sin = occlusionSize[0] * sin(angle);
      float size1Cos = occlusionSize[1] * cos(angle);
      float size1Sin = occlusionSize[1] * sin(angle);
      tmpBB.expand(Coord(size0Cos - size1Sin, size0Sin + size1Cos, 0));
      tmpBB.expand(Coord(size0Cos + size1Sin, size0Sin - size1Cos, 0));
      tmpBB.expand(Coord(-size0Cos + size1Sin, -size0Sin - size1Cos, 0));
      tmpBB.expand(Coord(-size0Cos - size1Sin, -size0Sin + size1Cos, 0));

      occlusionSize[0] = tmpBB[1][0];
      occlusionSize[1] = tmpBB[1][1];
    }

    baseCoord[0] +=
        translationAfterRotation[0] * cos(angle) - translationAfterRotation[1] * sin(angle);
    baseCoord[1] +=
        translationAfterRotation[0] * sin(angle) + translationAfterRotation[1] * cos(angle);

    MatrixGL modelviewMatrix, projectionMatrix, transformMatrix;

    glGetFloatv(GL_MODELVIEW_MATRIX, reinterpret_cast<GLfloat *>(&modelviewMatrix));
    glGetFloatv(GL_PROJECTION_MATRIX, reinterpret_cast<GLfloat *>(&projectionMatrix));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixf(reinterpret_cast<GLfloat *>(&projectionMatrix));
    glMultMatrixf(reinterpret_cast<GLfloat *>(&modelviewMatrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, reinterpret_cast<GLfloat *>(&transformMatrix));
    glPopMatrix();

    BoundingBox bbTmp;
    bbTmp.expand(
        Coord(baseCoord[0] + occlusionSize[0], baseCoord[1] + occlusionSize[1], baseCoord[2]));
    bbTmp.expand(
        Coord(baseCoord[0] + occlusionSize[0], baseCoord[1] - occlusionSize[1], baseCoord[2]));
    bbTmp.expand(
        Coord(baseCoord[0] - occlusionSize[0], baseCoord[1] - occlusionSize[1], baseCoord[2]));
    bbTmp.expand(
        Coord(baseCoord[0] - occlusionSize[0], baseCoord[1] + occlusionSize[1], baseCoord[2]));

    labelBoundingBox.expand(projectPoint(
        Coord(baseCoord[0] + occlusionSize[0], baseCoord[1] + occlusionSize[1], baseCoord[2]),
        transformMatrix, camera->getViewport()));
    labelBoundingBox.expand(projectPoint(
        Coord(baseCoord[0] + occlusionSize[0], baseCoord[1] - occlusionSize[1], baseCoord[2]),
        transformMatrix, camera->getViewport()));
    labelBoundingBox.expand(projectPoint(
        Coord(baseCoord[0] - occlusionSize[0], baseCoord[1] - occlusionSize[1], baseCoord[2]),
        transformMatrix, camera->getViewport()));
    labelBoundingBox.expand(projectPoint(
        Coord(baseCoord[0] - occlusionSize[0], baseCoord[1] + occlusionSize[1], baseCoord[2]),
        transformMatrix, camera->getViewport()));

    if (!occlusionTester->addRectangle(
            RectangleInt2D(labelBoundingBox[0][0], labelBoundingBox[0][1], labelBoundingBox[1][0],
                           labelBoundingBox[1][1]))) {
      glPopAttrib();
      return;
    }
  }

  glPushMatrix();

  // Translation and rotation
  glTranslatef(centerPosition[0], centerPosition[1], centerPosition[2]);

  if (xRot != 0.) {
    glRotatef(xRot, 1., 0., 0.);
  }

  if (yRot != 0.) {
    glRotatef(yRot, 0., 1., 0.);
  }

  if (zRot != 0.) {
    glRotatef(zRot, 0., 0., 1.);
  }

  glTranslatef(translationAfterRotation[0], translationAfterRotation[1],
               translationAfterRotation[2]);

  if (!billboarded) {
    // Alignment translation
    switch (alignment) {
    case LabelPosition::Center:

      break;

    case LabelPosition::Left:
      glTranslatef(-sizeForOutAlign[0] / 2, 0, 0);
      break;

    case LabelPosition::Right:
      glTranslatef(sizeForOutAlign[0] / 2, 0, 0);
      break;

    case LabelPosition::Top:
      glTranslatef(0, sizeForOutAlign[1] / 2, 0);
      break;

    case LabelPosition::Bottom:
      glTranslatef(0, -sizeForOutAlign[1] / 2, 0);
      break;

    default:
      break;
    }
  } else {

    MatrixGL modelviewMatrix, projectionMatrix, transformMatrix;

    glGetFloatv(GL_MODELVIEW_MATRIX, reinterpret_cast<GLfloat *>(&modelviewMatrix));
    glGetFloatv(GL_PROJECTION_MATRIX, reinterpret_cast<GLfloat *>(&projectionMatrix));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixf(reinterpret_cast<GLfloat *>(&projectionMatrix));
    glMultMatrixf(reinterpret_cast<GLfloat *>(&modelviewMatrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, reinterpret_cast<GLfloat *>(&transformMatrix));
    glPopMatrix();

    MatrixGL invTransformMatrix(transformMatrix);
    invTransformMatrix.inverse();

    Coord baseCenter = projectPoint(centerPosition, transformMatrix, camera->getViewport());
    baseCenter = unprojectPoint(baseCenter, invTransformMatrix, camera->getViewport());

    BoundingBox billboardedBB;
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(sizeForOutAlign[0] / 2., sizeForOutAlign[1] / 2.,
                                            sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(-sizeForOutAlign[0] / 2., sizeForOutAlign[1] / 2.,
                                            sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(sizeForOutAlign[0] / 2., -sizeForOutAlign[1] / 2.,
                                            sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(-sizeForOutAlign[0] / 2., -sizeForOutAlign[1] / 2.,
                                            sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(sizeForOutAlign[0] / 2., sizeForOutAlign[1] / 2.,
                                            -sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(-sizeForOutAlign[0] / 2., sizeForOutAlign[1] / 2.,
                                            -sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(sizeForOutAlign[0] / 2., -sizeForOutAlign[1] / 2.,
                                            -sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));
    billboardedBB.expand(
        projectPoint(centerPosition + Coord(-sizeForOutAlign[0] / 2., -sizeForOutAlign[1] / 2.,
                                            -sizeForOutAlign[2] / 2.),
                     transformMatrix, camera->getViewport()));

    Coord billboardedTranlation;

    switch (alignment) {

    case LabelPosition::Left:
      billboardedTranlation =
          unprojectPoint(billboardedBB.center() + Coord(-billboardedBB.width() / 2., 0, 0),
                         invTransformMatrix, camera->getViewport()) -
          baseCenter;
      break;

    case LabelPosition::Right:
      billboardedTranlation =
          unprojectPoint(billboardedBB.center() + Coord(billboardedBB.width() / 2., 0, 0),
                         invTransformMatrix, camera->getViewport()) -
          baseCenter;
      break;

    case LabelPosition::Top:
      billboardedTranlation =
          unprojectPoint(billboardedBB.center() + Coord(0, billboardedBB.height() / 2., 0),
                         invTransformMatrix, camera->getViewport()) -
          baseCenter;
      break;

    case LabelPosition::Bottom:
      billboardedTranlation =
          unprojectPoint(billboardedBB.center() + Coord(0, -billboardedBB.height() / 2., 0),
                         invTransformMatrix, camera->getViewport()) -
          baseCenter;
      break;

    default:
      break;
    }

    glTranslatef(billboardedTranlation[0], billboardedTranlation[1], billboardedTranlation[2]);

    // Billboard computation
    float mdlM[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mdlM);
    glMatrixMode(GL_MODELVIEW);

    mdlM[0] = 1.;
    mdlM[5] = 1.;
    mdlM[10] = 1.;
    mdlM[1] = mdlM[2] = 0.0f;
    mdlM[4] = mdlM[6] = 0.0f;
    mdlM[8] = mdlM[9] = 0.0f;
    glLoadMatrixf(mdlM);
  }

  glScalef(scaleToApply, scaleToApply, 1);
  viewportH *= scaleToApply;

  if (viewportH < 0) {
    viewportH = -viewportH;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // The label is too small to be read, draw a line
  if (viewportH < 2 && useLOD) {
    float wAlign = 0;
    float hAlign = 0;

    switch (alignment) {
    case LabelPosition::Top:
      hAlign = h / 2.;
      break;

    case LabelPosition::Bottom:
      hAlign = -(h / 2.);
      break;

    case LabelPosition::Left:
      wAlign = -(w / 2.);
      break;

    case LabelPosition::Right:
      wAlign = w / 2.;
      break;

    default:
      break;
    }

    glLineWidth(viewportH);

    if (outlineColor.getA() == 0 || outlineSize == 0) {
      setMaterial(Color(color[0], color[1], color[2], color[3]));
    } else {
      setMaterial(Color(outlineColor[0], outlineColor[1], outlineColor[2], outlineColor[3]));
    }

    glBegin(GL_LINES);
    glVertex3f(-w / 2. + wAlign, hAlign, 0);
    glVertex3f(w / 2. + wAlign, hAlign, 0);
    glEnd();
    glLineWidth(1);
  } else {
    // Draw labels

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // For left and right alignment
    float xAlignFactor = .5;

    switch (alignment) {
    case LabelPosition::Left:
      xAlignFactor = 1.;
      break;

    case LabelPosition::Right:
      xAlignFactor = .0;
      break;

    default:
      break;
    }

    // Label shift when we have an alignment
    float xShiftFactor = 0.;
    float yShiftFactor = 0.;

    switch (alignment) {
    case LabelPosition::Left:
      xShiftFactor = -0.5;
      break;

    case LabelPosition::Right:
      xShiftFactor = 0.5;
      break;

    case LabelPosition::Top:
      yShiftFactor = 0.5;
      break;

    case LabelPosition::Bottom:
      yShiftFactor = -0.5;
      break;

    default:
      break;
    }

    // space between lines
    float yShift = 0.;

    float x1, y1, z1, x2, y2, z2;
    auto itW = textWidthVector.begin();

    for (const auto &text : textVector) {
      auto visualText = getVisualText(text);
      ftglPolygonFont->BBox(text.c_str(), x1, y1, z1, x2, y2, z2);

      FTPoint shift(-(textBoundingBox[1][0] - textBoundingBox[0][0]) / 2. - x1 +
                        ((textBoundingBox[1][0] - textBoundingBox[0][0]) - (*itW)) * xAlignFactor +
                        (textBoundingBox[1][0] - textBoundingBox[0][0]) * xShiftFactor,
                    -textBoundingBox[1][1] + (textBoundingBox[1][1] - textBoundingBox[0][1]) / 2. +
                        yShift + (textBoundingBox[1][1] - textBoundingBox[0][1]) * yShiftFactor);

      if (!textureName.empty()) {
        GlTextureManager::activateTexture(textureName);
      }

      setMaterial(color);

      ftglPolygonFont->Render(visualText.c_str(), -1, shift);

      if (!textureName.empty()) {
        GlTextureManager::deactivateTexture();
      }

      if (outlineSize > 0) {
        if (!useLOD || viewportH > 25) {
          glLineWidth(outlineSize);
        } else {
          glLineWidth(1);
        }

        setMaterial(outlineColor);

        ftglOutlineFont->Render(visualText.c_str(), -1, shift);
      }

      yShift -= fontSize + 5;
      ++itW;
    }
  }

  glPopMatrix();
  glPopAttrib();
}
//===========================================================
void GlLabel::translate(const Coord &move) {
  centerPosition += move;
}
//===========================================================
void GlLabel::rotate(float xRot, float yRot, float zRot) {
  this->xRot = xRot;
  this->yRot = yRot;
  this->zRot = zRot;
}
//===========================================================
void GlLabel::getXML(string &outString) {
  GlXMLTools::createProperty(outString, "type", "GlLabel", "GlEntity");

  GlXMLTools::getXML(outString, "text", text);
  GlXMLTools::getXML(outString, "renderingMode", renderingMode);
  GlXMLTools::getXML(outString, "fontName", font.fontName());
  GlXMLTools::getXML(outString, "centerPosition", centerPosition);
  GlXMLTools::getXML(outString, "translationAfterRotation", translationAfterRotation);
  GlXMLTools::getXML(outString, "size", size);
  GlXMLTools::getXML(outString, "color", color);
  GlXMLTools::getXML(outString, "alignment", alignment);
  GlXMLTools::getXML(outString, "scaleToSize", scaleToSize);
  GlXMLTools::getXML(outString, "useMinMaxSize", useMinMaxSize);
  GlXMLTools::getXML(outString, "minSize", minSize);
  GlXMLTools::getXML(outString, "maxSize", maxSize);
  GlXMLTools::getXML(outString, "depthTestEnabled", depthTestEnabled);
  GlXMLTools::getXML(outString, "leftAlign", leftAlign);
  GlXMLTools::getXML(outString, "xRot", xRot);
  GlXMLTools::getXML(outString, "yRot", yRot);
  GlXMLTools::getXML(outString, "zRot", zRot);
  GlXMLTools::getXML(outString, "outlineColor", outlineColor);
  GlXMLTools::getXML(outString, "outlineSize", outlineSize);
  GlXMLTools::getXML(outString, "textureName", textureName);
}
//============================================================
void GlLabel::setWithXML(const string &inString, uint &currentPosition) {
  string fontName;
  GlXMLTools::setWithXML(inString, currentPosition, "text", text);
  GlXMLTools::setWithXML(inString, currentPosition, "renderingMode", renderingMode);
  GlXMLTools::setWithXML(inString, currentPosition, "fontName", fontName);
  GlXMLTools::setWithXML(inString, currentPosition, "centerPosition", centerPosition);
  GlXMLTools::setWithXML(inString, currentPosition, "translationAfterRotation",
                         translationAfterRotation);
  GlXMLTools::setWithXML(inString, currentPosition, "size", size);
  GlXMLTools::setWithXML(inString, currentPosition, "color", color);
  GlXMLTools::setWithXML(inString, currentPosition, "alignment", alignment);
  GlXMLTools::setWithXML(inString, currentPosition, "scaleToSize", scaleToSize);
  GlXMLTools::setWithXML(inString, currentPosition, "useMinMaxSize", useMinMaxSize);
  GlXMLTools::setWithXML(inString, currentPosition, "minSize", minSize);
  GlXMLTools::setWithXML(inString, currentPosition, "maxSize", maxSize);
  GlXMLTools::setWithXML(inString, currentPosition, "depthTestEnabled", depthTestEnabled);
  GlXMLTools::setWithXML(inString, currentPosition, "leftAlign", leftAlign);
  GlXMLTools::setWithXML(inString, currentPosition, "xRot", xRot);
  GlXMLTools::setWithXML(inString, currentPosition, "yRot", yRot);
  GlXMLTools::setWithXML(inString, currentPosition, "zRot", zRot);
  GlXMLTools::setWithXML(inString, currentPosition, "outlineColor", outlineColor);
  GlXMLTools::setWithXML(inString, currentPosition, "outlineSize", outlineSize);
  GlXMLTools::setWithXML(inString, currentPosition, "textureName", textureName);
  setFontName(fontName);
}
}
