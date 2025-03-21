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

#include <talipot/Curves.h>
#include <talipot/GlTextureManager.h>
#include <talipot/GlTools.h>

using namespace std;
using namespace tlp;

namespace tlp {
//================================================
inline float sqrnorm(const Coord &v) {
  return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}
//================================================
static float sqrnorm(const Coord &p0, const Coord &p1) {
  float result = 0;
  for (uint i = 0; i < 3; ++i) {
    auto li = p0[i] - p1[i];
    result += li * li;
  }
  return result;
}
//================================================
static float lineLength(const Coord *line, uint lineSize, vector<float> &norms) {
  norms.resize(lineSize - 1);
  float ll = 0;
  for (uint i = 1; i < lineSize; ++i) {
    ll += norms[i - 1] = sqrnorm(line[i - 1], line[i]);
  }
  return ll;
}
//================================================
void getColors(const Coord *line, const uint lineSize, const Color &c1, const Color &c2,
               vector<Color> &result) {
  tlp::Vec4f _c1, _c2;

  for (uint i = 0; i < 4; ++i) {
    _c1[i] = c1[i];
    _c2[i] = c2[i];
  }

  result.resize(lineSize);
  result[0] = c1;
  result[lineSize - 1] = c2;

  if (lineSize == 2) {
    return;
  }

  _c2 -= _c1;
  vector<float> norms;
  _c2 /= lineLength(line, lineSize, norms);
  for (uint i = 1; i < lineSize - 1; ++i) {
    _c1 += _c2 * norms[i - 1];
    result[i].set(uchar(_c1[0]), uchar(_c1[1]), uchar(_c1[2]), uchar(_c1[3]));
  }
}
//================================================
void getColors(const vector<Coord> &line, const Color &c1, const Color &c2, vector<Color> &result) {
  getColors(line.data(), line.size(), c1, c2, result);
}
//================================================
void getSizes(const vector<Coord> &lines, float s1, float s2, vector<float> &result) {
  const auto sz = lines.size();
  result.resize(sz);
  result[0] = s1;
  result[sz - 1] = s2;
  s2 -= s1;

  vector<float> norms;
  s2 /= lineLength(lines.data(), sz, norms);
  for (uint i = 1; i < sz - 1; ++i) {
    s1 += s2 * norms[i - 1];
    result[i] = s1;
  }
}
//===============================================
struct CurvePoints {
  GLfloat *data;
  uint size;
  CurvePoints(uint size) : size(size) {
    data = new GLfloat[2 * size * 3];
  }
  CurvePoints(const CurvePoints &cp) {
    delete data;
    size = cp.size;
    data = new GLfloat[2 * size * 3];
    memcpy(data, cp.data, sizeof(GLfloat) * size * 6);
  }
  void addPoint() {
    auto *newData = new GLfloat[2 * (size + 1) * 3];
    memcpy(newData, data, sizeof(GLfloat) * size * 3);
    memcpy(newData + ((size + 1) * 3), data + (size * 3), sizeof(GLfloat) * size * 3);
    delete[] data;
    data = newData;
    ++size;
  }
  Coord &operator()(uint i, uint j) const {
    return reinterpret_cast<Coord &>(data[j * size * 3 + i * 3]);
  }
};
//================================================
GLfloat *buildCurvePoints(const vector<Coord> &vertices, const vector<float> &sizes,
                          const Coord &startN, const Coord &endN, uint &resultSize,
                          vector<uint> *dec) {
  uint resultDec = 0;
  bool inversion = false;
  CurvePoints result(vertices.size());
  // start point
  Coord xu = startN - vertices[0];
  xu /= xu.norm();
  Coord xv = {0, 0, 1.};
  Coord dir = xu ^ xv;

  if (fabs(dir.norm()) > 1e-3) {
    dir /= dir.norm();
  }

  result(0, 0) = vertices[0] - dir * sizes[0];
  result(0, 1) = vertices[0] + dir * sizes[0];

  //============
  for (uint i = 1; i < vertices.size() - 1; ++i) {

    Coord xu = vertices[i - 1] - vertices[i];
    Coord xv = vertices[i + 1] - vertices[i];
    float n_xu = xu.norm();
    float n_xv = xv.norm();
    xu /= n_xu;
    xv /= n_xv;

    Coord bi_xu_xv = xu + xv;

    if (bi_xu_xv == Coord(0, 0, 0)) {
      // two point at the same coord
      result(i + resultDec, 0) = result(i + resultDec - 1, 0);
      result(i + resultDec, 1) = result(i + resultDec - 1, 1);
      continue;
    }

    bi_xu_xv /= bi_xu_xv.norm();
    float newSize = sizes[i];
    Coord u = vertices[i - 1] - vertices[i];
    Coord v = vertices[i + 1] - vertices[i];
    auto angle =
        float(M_PI - acos((u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) / (u.norm() * v.norm())));

    if (isnan(angle)) {
      angle = 0;
    }

    newSize = newSize / float(cos(angle / 2.));

    if (angle < M_PI / 2 + M_PI / 4) {
      // normal form
      if ((xu ^ xv)[2] > 0) {
        result(i + resultDec, 0 + inversion) = vertices[i] + bi_xu_xv * newSize;
        result(i + resultDec, 1 - inversion) = vertices[i] - bi_xu_xv * newSize;
      } else {
        result(i + resultDec, 0 + inversion) = vertices[i] - bi_xu_xv * newSize;
        result(i + resultDec, 1 - inversion) = vertices[i] + bi_xu_xv * newSize;
      }

    } else {
      // broken form
      Coord vectUnit = {-bi_xu_xv[1], bi_xu_xv[0], bi_xu_xv[2]};

      if (!(newSize > u.norm() || newSize > v.norm() || fabs(angle - M_PI) < 1E-5)) {
        result.addPoint();

        if (dec) {
          dec->push_back(i);
        }

        if ((xu ^ xv)[2] > 0) {
          result(i + resultDec, 0 + inversion) = vertices[i] + bi_xu_xv * newSize;
          result(i + resultDec, 1 - inversion) = vertices[i] - vectUnit * sizes[i];
          result(i + resultDec + 1, 0 + inversion) = vertices[i] + bi_xu_xv * newSize;
          result(i + resultDec + 1, 1 - inversion) = vertices[i] + vectUnit * sizes[i];
        } else {
          result(i + resultDec, 0 + inversion) = vertices[i] + vectUnit * sizes[i];
          result(i + resultDec, 1 - inversion) = vertices[i] + bi_xu_xv * newSize;
          result(i + resultDec + 1, 0 + inversion) = vertices[i] - vectUnit * sizes[i];
          result(i + resultDec + 1, 1 - inversion) = vertices[i] + bi_xu_xv * newSize;
        }

        ++resultDec;
      } else {
        if ((xu ^ xv)[2] > 0) {
          result(i + resultDec, 0 + inversion) = vertices[i] + vectUnit * sizes[i];
          result(i + resultDec, 1 - inversion) = vertices[i] - vectUnit * sizes[i];
          inversion = !inversion;
        } else {
          result(i + resultDec, 0 + inversion) = vertices[i] - vectUnit * sizes[i];
          result(i + resultDec, 1 - inversion) = vertices[i] + vectUnit * sizes[i];
          inversion = !inversion;
        }
      }
    }
  }

  // end point
  xu = endN - vertices[vertices.size() - 1];
  xu /= xu.norm();
  xv.set(0, 0, -1);
  dir = xu ^ xv;

  if (fabs(dir.norm()) > 1e-3) {
    dir /= dir.norm();
  }

  result(vertices.size() - 1 + resultDec, 0 + inversion) =
      vertices[vertices.size() - 1] - dir * sizes[vertices.size() - 1];
  result(vertices.size() - 1 + resultDec, 1 - inversion) =
      vertices[vertices.size() - 1] + dir * sizes[vertices.size() - 1];
  resultSize = vertices.size() + resultDec;
  return result.data;
}

static float computeExtrusion(const Coord &pBefore, const Coord &pCurrent, const Coord &pAfter,
                              float sz, float inversion, vector<Coord> &result,
                              bool lastPoint = false, bool twoPointsCurve = false) {

  Coord u = pBefore - pCurrent;
  Coord v = pAfter - pCurrent;

  if (fabs(u[2]) < 1e-3) {
    u[2] = 0;
  }

  if (fabs(v[2]) < 1e-3) {
    v[2] = 0;
  }

  Coord xu = u;

  if (u.norm() != 0) {
    xu /= u.norm();
  }

  Coord xv = v;

  if (v.norm() != 0) {
    xv /= v.norm();
  }

  Coord bi_xu_xv = xu + xv;
  auto norm = bi_xu_xv.norm();

  if (!result.empty() && !lastPoint && norm < 1e-3) {
    return inversion;
  }

  if (norm != 0) {
    bi_xu_xv /= norm;
  }

  float nsz = sz;
  float angle = float(M_PI - atan2((u ^ v).norm(), u.dotProduct(v)));

  bool angleOk = true;

  if (isnan(angle) || fabs(angle) < 1e-3) {
    angleOk = false;

    if (!lastPoint) {
      bi_xu_xv = xv;
    } else {
      bi_xu_xv = xu;
    }

    if (bi_xu_xv[0] == 0 && bi_xu_xv[1] == 0 && fabs(bi_xu_xv[2]) == 1) {
      bi_xu_xv[0] = bi_xu_xv[2];
      bi_xu_xv[2] = 0;
    }
  } else {
    nsz /= float(cos(angle / 2.0));
  }

  Coord bi_xu_xv_nsz = {bi_xu_xv[0] * nsz, bi_xu_xv[1] * nsz, bi_xu_xv[2] * nsz};
  bi_xu_xv_nsz *= inversion;

  if (angleOk && angle < M_PI / 2 + M_PI / 4) {
    // normal form
    if ((xu ^ xv)[2] > 0) {
      result.push_back(pCurrent + bi_xu_xv_nsz);
      result.push_back(pCurrent - bi_xu_xv_nsz);
    } else {
      result.push_back(pCurrent - bi_xu_xv_nsz);
      result.push_back(pCurrent + bi_xu_xv_nsz);
    }
  } else {
    // broken form

    Coord vunit_sz = {-bi_xu_xv[1] * sz, bi_xu_xv[0] * sz, bi_xu_xv[2] * sz};
    vunit_sz *= inversion;

    if (angleOk && !twoPointsCurve &&
        !(nsz > u.norm() || nsz > v.norm() || fabs(angle - M_PI) < 1E-3)) {
      if ((xu ^ xv)[2] > 0) {
        result.push_back(pCurrent + bi_xu_xv_nsz);
        result.push_back(pCurrent - vunit_sz);
        result.push_back(pCurrent + bi_xu_xv_nsz);
        result.push_back(pCurrent + vunit_sz);
      } else {
        result.push_back(pCurrent + vunit_sz);
        result.push_back(pCurrent + bi_xu_xv_nsz);
        result.push_back(pCurrent - vunit_sz);
        result.push_back(pCurrent + bi_xu_xv_nsz);
      }
    } else {
      result.push_back(pCurrent + vunit_sz);
      result.push_back(pCurrent - vunit_sz);
      inversion *= -1;
    }
  }

  return inversion;
}

void buildCurvePoints(const vector<Coord> &vertices, const vector<float> &sizes,
                      const Coord &startN, const Coord &endN, vector<Coord> &result) {

  const auto sz = vertices.size();
  bool twoPointsCurve = (sz == 2);
  result.reserve(sz * 2);

  float inversion = 1;

  if (startN != vertices[0]) {
    inversion = computeExtrusion(startN, vertices[0], vertices[1], sizes[0], inversion, result,
                                 false, twoPointsCurve);
  } else {
    inversion = computeExtrusion(vertices[0] - (vertices[1] - vertices[0]), vertices[0],
                                 vertices[1], sizes[0], inversion, result, false, twoPointsCurve);
  }

  for (uint i = 1; i < sz - 1; ++i) {
    inversion = computeExtrusion(vertices[i - 1], vertices[i], vertices[i + 1], sizes[i], inversion,
                                 result, false, twoPointsCurve);
  }

  if (endN != vertices[sz - 1]) {
    inversion = computeExtrusion(vertices[sz - 2], vertices[sz - 1], endN, sizes[sizes.size() - 1],
                                 inversion, result, true, twoPointsCurve);
  } else {
    inversion = computeExtrusion(vertices[sz - 2], vertices[sz - 1],
                                 vertices[sz - 1] + (vertices[sz - 1] - vertices[sz - 2]),
                                 sizes[sizes.size() - 1], inversion, result, true, twoPointsCurve);
  }
}
//==============================================
vector<Coord> splineCurve(const vector<Coord> &vertices) {
  vector<Coord> curve;
  curve.reserve(2 + 3 * (vertices.size() - 2));
  curve.push_back(vertices[0]);

  for (uint i = 1; i < vertices.size() - 1; ++i) {
    Coord xu = vertices[i - 1] - vertices[i];
    Coord xv = vertices[i + 1] - vertices[i];

    if ((xu ^ xv).norm() < 1E-3) {
      continue;
    }

    float n_xu = xu.norm();
    float n_xv = xv.norm();
    xu /= n_xu;
    xv /= n_xv;
    Coord bi_xu_xv = xu + xv;
    bi_xu_xv /= bi_xu_xv.norm();
    Coord tgt_xu_xv = xu ^ xv;
    tgt_xu_xv /= tgt_xu_xv.norm();
    Coord dir = tgt_xu_xv ^ bi_xu_xv;
    dir /= dir.norm();
    curve.push_back(vertices[i] - (dir * (n_xu / 5.0f)));
    curve.push_back(vertices[i]);
    curve.push_back(vertices[i] + (dir * (n_xv / 5.0f)));
  }

  curve.push_back(vertices.back());
  return curve;
}

void computeCleanVertices(const vector<Coord> &bends, const Coord &startPoint,
                          const Coord &endPoint, Coord &startN, Coord &endN, vector<Coord> &result,
                          bool adjustTangent) {

  if (!bends.empty()) {
    result.reserve(bends.size() + 2);
    result.push_back(startPoint);
    Coord lastPoint = bends[0];

    if ((startPoint - lastPoint).norm() > 1E-4) {
      result.push_back(lastPoint);
    }

    for (uint i = 1; i < bends.size(); ++i) {
      const Coord &currentPoint = bends[i];

      if ((currentPoint - lastPoint).norm() > 1E-4) {
        result.push_back(currentPoint);
      }

      lastPoint = currentPoint;
    }

    if ((endPoint - lastPoint).norm() > 1E-4) {
      lastPoint = endPoint;
      result.push_back(endPoint);
    }

    if (result.size() < 2) { // only one valid point for a line
      result.clear();
      return;
    }

    // Adjust tangent direction
    if (adjustTangent) {
      auto norm = (startN - startPoint).norm();
      if (norm && (norm < 1E-4)) {
        startN = startPoint - (result[1] - startPoint);
      }

      norm = (endN - lastPoint).norm();
      if (norm && (norm < 1E-4)) {
        endN = lastPoint + lastPoint - result[result.size() - 2];
      }
    }
  } else {
    if ((startPoint - endPoint).norm() > 1E-4) {
      result.reserve(2);
      result.push_back(startPoint);
      result.push_back(endPoint);

      // Adjust tangent direction
      if (adjustTangent) {
        auto norm = (startN - startPoint).norm();
        if (norm && (norm < 1E-4)) {
          startN = startPoint - (endPoint - startPoint);
        }

        norm = (endN - endPoint).norm();
        if (norm && (norm < 1E-4)) {
          endN = endPoint + endPoint - startPoint;
        }
      }
    }
  }
}
//=============================================
void polyLine(const vector<Coord> &vertices, const Color &c1, const Color &c2) {

  vector<Color> colors;
  getColors(vertices, c1, c2, colors);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, &colors[0]);
  glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}
//=============================================
void polyQuad(const vector<Coord> &vertices, const Color &c1, const Color &c2, float s1, float s2,
              const Coord &startN, const Coord &endN, bool colorInterpolate,
              const Color &borderColor, const string &textureName, const float outlineWidth) {

  vector<float> sizes;
  getSizes(vertices, s1, s2, sizes);

  vector<Coord> quadVertices;

  buildCurvePoints(vertices, sizes, startN, endN, quadVertices);

  float length = 0;
  const uint nbQuads_div2 = quadVertices.size() / 2;

  vector<Coord> centerLine;
  centerLine.reserve(nbQuads_div2);
  vector<uint> bottomOutlineIndices(nbQuads_div2);
  vector<uint> topOutlineIndices(nbQuads_div2);
  vector<float> texCoords(quadVertices.size() * 2);

  for (size_t i = 0; i < nbQuads_div2; ++i) {
    centerLine.push_back((quadVertices[2 * i] + quadVertices[2 * i + 1]) / 2.f);
    bottomOutlineIndices[i] = 2 * i;
    topOutlineIndices[i] = 2 * i + 1;

    if (i == 0) {
      texCoords[0] = 0;
      texCoords[1] = 1;
      texCoords[2] = 0;
      texCoords[3] = 0;
    } else {
      const Coord &p1_0 = quadVertices[2 * (i - 1)];
      const Coord &p1_1 = quadVertices[2 * i];
      const Coord &p2_0 = quadVertices[2 * (i - 1) + 1];
      const Coord &p2_1 = quadVertices[2 * i + 1];
      length += ((p1_1 + p2_1) / 2.f - (p1_0 + p2_0) / 2.f).norm() / (p1_0 - p2_0).norm();
      texCoords[i * 4] = length;
      texCoords[i * 4 + 1] = 1;
      texCoords[i * 4 + 2] = length;
      texCoords[i * 4 + 3] = 0;
    }
  }

  vector<Color> colors;
  getColors(centerLine, c1, c2, colors);

  vector<Color> quadColors;
  quadColors.resize(quadVertices.size());

  for (size_t i = 0; i < colors.size(); ++i) {
    quadColors[i * 2] = colors[i];
    quadColors[i * 2 + 1] = colors[i];
  }

  if (!textureName.empty()) {
    GlTextureManager::activateTexture(textureName);
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, &quadVertices[0]);
  glColorPointer(4, GL_UNSIGNED_BYTE, 0, &quadColors[0]);
  glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);

  glDrawArrays(GL_QUAD_STRIP, 0, quadVertices.size());

  if (!textureName.empty()) {
    GlTextureManager::deactivateTexture();
  }

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  if (outlineWidth > 0) {
    glLineWidth(outlineWidth);

    if (!colorInterpolate) {
      glDisableClientState(GL_COLOR_ARRAY);
      glColor4ubv(reinterpret_cast<const GLubyte *>(&borderColor));
    }

    glDrawElements(GL_LINE_STRIP, bottomOutlineIndices.size(), GL_UNSIGNED_INT,
                   &bottomOutlineIndices[0]);
    glDrawElements(GL_LINE_STRIP, topOutlineIndices.size(), GL_UNSIGNED_INT, &topOutlineIndices[0]);

    glLineWidth(1.0);
  }

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}
//=============================================
void simpleQuad(const vector<Coord> &vertices, const Color &c1, const Color &c2, float s1, float s2,
                const Coord &startN, const Coord &endN, const Coord &lookDir, bool colorInterpolate,
                const Color &borderColor, const string &textureName, const float outlineWidth) {

  auto sz = vertices.size();

  vector<Color> colors;
  getColors(vertices, c1, c2, colors);
  vector<float> sizes;
  getSizes(vertices, s1, s2, sizes);

  CurvePoints result(sz);
  // start point
  Coord xu = startN - vertices[0];
  xu /= xu.norm();
  Coord dir = xu ^ lookDir;

  if (fabs(dir.norm()) > 1e-3) {
    dir /= dir.norm();
  }

  result(0, 0) = vertices[0] - dir * s1;
  result(0, 1) = vertices[0] + dir * s1;

  //============
  Coord v = vertices[1] - vertices[0];
  for (uint i = 1; i < sz - 1; ++i) {
    float newSize = sizes[i];
    Coord u = -v;
    v = vertices[i + 1] - vertices[i];
    auto angle =
        float(M_PI - acos((u[0] * v[0] + u[1] * v[1] + u[2] * v[2]) / (u.norm() * v.norm())));

    if (isnan(angle)) {
      angle = 0;
    }

    newSize /= float(cos(angle / 2.));

    Coord xu = u;
    Coord xv = v;
    xu = xu ^ lookDir;
    xv = xv ^ (-lookDir);

    float nrm = xu.norm();

    if (nrm > 0.00000001f) {
      xu /= nrm;
    }

    nrm = xv.norm();

    if (nrm > 0.00000001f) {
      xv /= nrm;
    }

    Coord xu_xv = xu + xv;

    nrm = xu_xv.norm();

    if (nrm > 0.00000001f) {
      xu_xv /= xu_xv.norm();
    }
    result(i, 0) = vertices[i] - xu_xv * newSize;
    result(i, 1) = vertices[i] + xu_xv * newSize;
  }

  // end point
  xu = endN - vertices[sz - 1];
  xu = xu ^ lookDir;
  xu /= xu.norm();
  result(sz - 1, 1) = vertices[sz - 1] - xu * sizes[sz - 1];
  result(sz - 1, 0) = vertices[sz - 1] + xu * sizes[sz - 1];

  GLfloat *points = result.data;

  if (!textureName.empty()) {
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    GlTextureManager::activateTexture(textureName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
  }

  string newTextureName1 = TalipotBitmapDir + "cylinderTexture.png";
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  GlTextureManager::activateTexture(newTextureName1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

  float length = 0;
  glBegin(GL_QUAD_STRIP);

  for (uint i = 0; i < sz; ++i) {
    glColor4ubv(reinterpret_cast<const GLubyte *>(&colors[i]));

    if (i == 0) {
      glMultiTexCoord2f(GL_TEXTURE0, 0, 1.0f);
      glMultiTexCoord2f(GL_TEXTURE1, 0, 1.0f);
      glVertex3fv(&points[i * 3]);
      glMultiTexCoord2f(GL_TEXTURE0, 0, 0.0f);
      glMultiTexCoord2f(GL_TEXTURE1, 0, 0.0f);
      glVertex3fv(&points[i * 3 + sz * 3]);
    } else {
      Coord p1_0 = {points[i * 3 - 3], points[i * 3 - 2], points[i * 3 - 1]};
      Coord p1_1 = {points[i * 3], points[i * 3 + 1], points[i * 3 + 2]};
      Coord p2_0 = {points[i * 3 + sz * 3 - 3], points[i * 3 + sz * 3 - 2],
                    points[i * 3 + sz * 3 - 1]};
      Coord p2_1 = {points[i * 3 + sz * 3], points[i * 3 + sz * 3 + 1], points[i * 3 + sz * 3 + 2]};

      length += ((p1_1 + p2_1) / 2.f - (p1_0 + p2_0) / 2.f).norm() / (p1_0 - p2_0).norm();
      glMultiTexCoord2f(GL_TEXTURE0, length, 1.0f);
      glMultiTexCoord2f(GL_TEXTURE1, length, 1.0f);
      glVertex3fv(&points[i * 3]);
      glMultiTexCoord2f(GL_TEXTURE0, length, 0.0f);
      glMultiTexCoord2f(GL_TEXTURE1, length, 0.0f);
      glVertex3fv(&points[i * 3 + sz * 3]);
    }
  }

  glEnd();

  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);

  if (!textureName.empty()) {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    GlTextureManager::deactivateTexture();
  }

  if (outlineWidth > 0) {

    glLineWidth(outlineWidth);

    glBegin(GL_LINE_STRIP);

    if (!colorInterpolate) {
      glColor4ubv(reinterpret_cast<const GLubyte *>(&borderColor));
    }

    for (uint i = 0; i < sz; ++i) {
      if (colorInterpolate) {
        glColor4ubv(reinterpret_cast<const GLubyte *>(&colors[i]));
      }

      glVertex3fv(&points[i * 3]);
    }

    glEnd();

    glBegin(GL_LINE_STRIP);

    if (!colorInterpolate) {
      glColor4ubv(reinterpret_cast<const GLubyte *>(&borderColor));
    }

    for (uint i = 0; i < sz; ++i) {
      if (colorInterpolate) {
        glColor4ubv(reinterpret_cast<const GLubyte *>(&colors[i]));
      }

      glVertex3fv(&points[i * 3 + sz * 3]);
    }

    glEnd();
  }

  delete[] points;
}

void bezierQuad(const vector<Coord> &vertices, const Color &c1, const Color &c2, float s1, float s2,
                const Coord &startN, const Coord &endN) {
  uint MAX_BENDS = 8;

  if (vertices.size() > MAX_BENDS) {
    vector<float> sizes;
    getSizes(vertices, s1, s2, sizes);
    vector<Color> colors;
    getColors(vertices, c1, c2, colors);
    vector<Coord> points(MAX_BENDS);

    for (uint i = 0; i < MAX_BENDS; ++i) {
      points[i] = vertices[i];
    }

    Coord dir = vertices[MAX_BENDS - 1] - vertices[(MAX_BENDS - 2)];
    dir /= dir.norm();
    dir *= ((vertices[MAX_BENDS - 1] - vertices[MAX_BENDS]).norm() / 5.f);
    bezierQuad(points, c1, colors[MAX_BENDS - 1], s1, sizes[MAX_BENDS - 1], startN,
               vertices[MAX_BENDS - 1] + dir);
    vector<Coord> newCurve(vertices.size() - (MAX_BENDS - 2));
    newCurve[0] = vertices[MAX_BENDS - 1];
    newCurve[1] = vertices[MAX_BENDS - 1] + dir;

    for (uint i = MAX_BENDS; i < vertices.size(); ++i) {
      newCurve[i - (MAX_BENDS) + 2] = vertices[i];
    }

    bezierQuad(newCurve, colors[MAX_BENDS - 1], c2, sizes[MAX_BENDS - 1], s2,
               vertices[MAX_BENDS - 2], endN);
    return;
  }

  uint steps = 40;
  Vec4f baseColor, delta;

  for (uint i = 0; i < 4; ++i) {
    baseColor[i] = c1[i];
    delta[i] = float(c2[i]) - float(c1[i]);
  }

  delta /= float(steps);
  uint size;
  vector<float> sizes;
  getSizes(vertices, s1, s2, sizes);
  GLfloat *points = buildCurvePoints(vertices, sizes, startN, endN, size);
  glMap2f(GL_MAP2_VERTEX_3, 0., 1.0, 3, size, 0.0, 1.0, size * 3, 2, points);
  glEnable(GL_MAP2_VERTEX_3);

  glBegin(GL_QUAD_STRIP);
  glNormal3f(0.0f, 0.0f, 1.0f);

  Vec4f color = baseColor;

  for (uint i = 0; i <= steps; ++i) {
    glColor4ub(uchar(color[0]), uchar(color[1]), uchar(color[2]), uchar(color[3]));
    glTexCoord2f(0.0f, 0.0f);
    glEvalCoord2f(i / GLfloat(steps), 0);
    glColor4ub(uchar(color[0]), uchar(color[1]), // Need to be done, bug of opengl ???
               uchar(color[2]), uchar(color[3]));
    glTexCoord2f(1.0f, 1.0f);
    glEvalCoord2f(i / GLfloat(steps), 1);
    color += delta;
  }

  glEnd();

  color = baseColor;
  glBegin(GL_LINE_STRIP);

  for (uint i = 0; i <= steps; ++i) {
    glColor4ub(uchar(color[0]), uchar(color[1]), uchar(color[2]), uchar(color[3]));
    glEvalCoord2f(i / GLfloat(steps), 0);
    color += delta;
  }

  glEnd();
  color = baseColor;
  glBegin(GL_LINE_STRIP);

  for (uint i = 0; i <= steps; ++i) {
    glColor4ub(uchar(color[0]), uchar(color[1]), uchar(color[2]), uchar(color[3]));
    glEvalCoord2f(i / GLfloat(steps), 1);
    color += delta;
  }

  glEnd();

  glDisable(GL_MAP2_VERTEX_3);
  delete[] points;
}

void bezierLine(const vector<Coord> &vertices, const Color &c1, const Color &c2) {

  uint MAX_BENDS = 8;

  if (vertices.size() > MAX_BENDS) {
    vector<Coord> points(MAX_BENDS);
    vector<Color> colors;
    getColors(vertices, c1, c2, colors);

    for (uint i = 0; i < MAX_BENDS; ++i) {
      points[i] = vertices[i];
    }

    bezierLine(points, c1, colors[MAX_BENDS - 1]);

    Coord dir = vertices[MAX_BENDS - 1] - vertices[(MAX_BENDS - 2)];
    dir /= dir.norm();
    dir *= ((vertices[MAX_BENDS - 1] - vertices[MAX_BENDS]).norm() / 5.f);

    vector<Coord> newCurve(vertices.size() - (MAX_BENDS - 2));
    newCurve[0] = vertices[MAX_BENDS - 1];
    newCurve[1] = vertices[MAX_BENDS - 1] + dir;

    for (uint i = MAX_BENDS; i < vertices.size(); ++i) {
      newCurve[i - (MAX_BENDS) + 2] = vertices[i];
    }

    bezierLine(newCurve, colors[MAX_BENDS - 1], c2);
    return;
  }

  vector<GLfloat> buff;
  buff.reserve(3 * vertices.size());
  memcpy(buff.data(), vertices.data(), 3 * vertices.size() * sizeof(GLfloat));

  glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, vertices.size(), buff.data());
  glEnable(GL_MAP1_VERTEX_3);

  glBegin(GL_LINE_STRIP);

  uint steps = 40;
  Vec4f color, delta;

  for (uint i = 0; i < 4; ++i) {
    color[i] = c1[i];
    delta[i] = float(c2[i]) - float(c1[i]);
  }

  delta /= float(steps);
  for (uint i = 0; i <= steps; ++i) {
    setColor(Color(uchar(color[0]), uchar(color[1]), uchar(color[2]), uchar(color[3])));
    glEvalCoord1f(i / GLfloat(steps));
    color += delta;
  }

  glEnd();

  glDisable(GL_MAP1_VERTEX_3);
}

void splineQuad(const vector<Coord> &vertices, const Color &c1, const Color &c2, float s1, float s2,
                const Coord &startN, const Coord &endN) {
  tlp::bezierQuad(splineCurve(vertices), c1, c2, s1, s2, startN, endN);
}
void splineLine(const vector<Coord> &vertices, const Color &c1, const Color &c2) {
  tlp::bezierLine(splineCurve(vertices), c1, c2);
}
}
