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

#if defined(_WIN32) || defined(__MINGW32__)
#include <windows.h>
#ifndef GL_TABLE_TOO_LARGE
#ifdef GL_TABLE_TOO_LARGE_EXT
#define GL_TABLE_TOO_LARGE GL_TABLE_TOO_LARGE_EXT
#else
#define GL_TABLE_TOO_LARGE 0x8031
#endif
#endif
#endif

#include <talipot/Rectangle.h>
#include <talipot/GlTools.h>

using namespace std;
namespace tlp {

static char hullVertexTable[][7] = {
    {0, 0, 0, 0, 0, 0, 0}, // 0
    {4, 0, 4, 7, 3, 0, 0}, // 1
    {4, 1, 2, 6, 5, 0, 0}, // 2
    {0, 0, 0, 0, 0, 0, 0}, // 3
    {4, 0, 1, 5, 4, 0, 0}, // 4
    {6, 0, 1, 5, 4, 7, 3}, // 5
    {6, 0, 1, 2, 6, 5, 4}, // 6
    {0, 0, 0, 0, 0, 0, 0}, // 7
    {4, 2, 3, 7, 6, 0, 0}, // 8
    {6, 4, 7, 6, 2, 3, 0}, // 9
    {6, 2, 3, 7, 6, 5, 1}, // 10
    {0, 0, 0, 0, 0, 0, 0}, // 11
    {0, 0, 0, 0, 0, 0, 0}, // 12
    {0, 0, 0, 0, 0, 0, 0}, // 13
    {0, 0, 0, 0, 0, 0, 0}, // 14
    {0, 0, 0, 0, 0, 0, 0}, // 15
    {4, 0, 3, 2, 1, 0, 0}, // 16
    {6, 0, 4, 7, 3, 2, 1}, // 17
    {6, 0, 3, 2, 6, 5, 1}, // 18
    {0, 0, 0, 0, 0, 0, 0}, // 19
    {6, 0, 3, 2, 1, 5, 4}, // 20
    {6, 2, 1, 5, 4, 7, 3}, // 21
    {6, 0, 3, 2, 6, 5, 4}, // 22
    {0, 0, 0, 0, 0, 0, 0}, // 23
    {6, 0, 3, 7, 6, 2, 1}, // 24
    {6, 0, 4, 7, 6, 2, 1}, // 25
    {6, 0, 3, 7, 6, 5, 1}, // 26
    {0, 0, 0, 0, 0, 0, 0}, // 27
    {0, 0, 0, 0, 0, 0, 0}, // 28
    {0, 0, 0, 0, 0, 0, 0}, // 29
    {0, 0, 0, 0, 0, 0, 0}, // 30
    {0, 0, 0, 0, 0, 0, 0}, // 31
    {4, 4, 5, 6, 7, 0, 0}, // 32
    {6, 4, 5, 6, 7, 3, 0}, // 33
    {6, 1, 2, 6, 7, 4, 5}, // 34
    {0, 0, 0, 0, 0, 0, 0}, // 35
    {6, 0, 1, 5, 6, 7, 4}, // 36
    {6, 0, 1, 5, 6, 7, 3}, // 37
    {6, 0, 1, 2, 6, 7, 4}, // 38
    {0, 0, 0, 0, 0, 0, 0}, // 39
    {6, 2, 3, 7, 4, 5, 6}, // 40
    {6, 0, 4, 5, 6, 2, 3}, // 41
    {6, 1, 2, 3, 7, 4, 5}  // 42
};
//====================================================
void setColor(const Color &c) {
  glColor4ubv(reinterpret_cast<const unsigned char *>(&c));
}
//====================================================
void setColor(GLfloat *c) {
  glColor4fv(c);
}
//====================================================
void setMaterial(const Color &c) {
  float colorMat[4];
  colorMat[0] = c[0] / 255.f;
  colorMat[1] = c[1] / 255.f;
  colorMat[2] = c[2] / 255.f;
  colorMat[3] = c[3] / 255.f;
  setColor(c);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colorMat);
}
//====================================================
bool cameraIs3D() {
  return glIsEnabled(GL_LIGHT0);
}
//====================================================
Coord projectPoint(const Coord &obj, const MatrixGL &transform, const Vec4i &viewport) {
  Vec4f point;
  point[0] = obj[0];
  point[1] = obj[1];
  point[2] = obj[2];
  point[3] = 1.0f;
  point = point * transform;
#ifndef NDEBUG
  if (fabs(point[3]) < 1E-6) {
    tlp::debug() << "Error in projectPoint with coord : " << obj
                 << " and transform matrix : " << transform << std::endl;
  }
#endif
  Coord result = {point[0], point[1], point[2]};
  result /= point[3];

  result[0] = viewport[0] + (1.0f + result[0]) * viewport[2] * 0.5f;
  result[1] = viewport[1] + (1.0f + result[1]) * viewport[3] * 0.5f;
  result[2] = (1.0f + result[2]) * 0.5f;
  return result;
}
//====================================================
Coord unprojectPoint(const Coord &obj, const MatrixGL &invtransform, const Vec4i &viewport) {
  Vec4f point;

  point[0] = (obj[0] - viewport[0]) / viewport[2] * 2.0f - 1.0f;
  point[1] = (obj[1] - viewport[1]) / viewport[3] * 2.0f - 1.0f;
  point[2] = 2.0f * obj[2] - 1.0f;
  point[3] = 1.0f;

  point = point * invtransform;
#ifndef NDEBUG

  if (fabs(point[3]) < 1E-6) {
    tlp::debug() << "Error in unprojectPoint with coord : " << obj
                 << " and transform matrix : " << invtransform << std::endl;
  }

#endif

  Coord result = {point[0], point[1], point[2]};
  result /= point[3];

  return result;
}
//====================================================
GLfloat projectSize(const Coord &position, const Size &size, const MatrixGL &projectionMatrix,
                    const MatrixGL &modelviewMatrix, const Vec4i &viewport) {
  BoundingBox box;
  box.expand(position - size / 2.f);
  box.expand(position + size / 2.f);
  return projectSize(box, projectionMatrix, modelviewMatrix, viewport);
}
//====================================================
GLfloat projectSize(const BoundingBox &bb, const MatrixGL &projectionMatrix,
                    const MatrixGL &modelviewMatrix, const Vec4i &viewport) {
  Coord bbSize = bb[1] - bb[0];
  float nSize = bbSize.norm(); // Enclosing bounding box

  MatrixGL translate;
  translate.fill(0);

  for (uint i = 0; i < 4; ++i) {
    translate[i][i] = 1;
  }

  for (uint i = 0; i < 3; ++i) {
    translate[3][i] = bb[0][i] + bbSize[i] / 2;
  }

  MatrixGL tmp(translate * modelviewMatrix);

  tmp[0][0] = nSize;
  tmp[0][1] = 0;
  tmp[0][2] = 0;
  tmp[1][0] = 0;
  tmp[1][1] = 0;
  tmp[1][2] = 0;
  tmp[2][0] = 0;
  tmp[2][1] = 0;
  tmp[2][2] = 0;

  tmp *= projectionMatrix;

  Vec4f vect1;
  vect1[0] = 0.5;
  vect1[1] = 0;
  vect1[2] = 0;
  vect1[3] = 1.0;
  Vec4f proj1 = vect1 * tmp;

  Vec4f vect2;
  vect2.fill(0);
  vect2[3] = 1.0;
  Vec4f proj2 = vect2 * tmp;

  float x1 = (proj1[0] / proj1[3] * 0.5 + 0.5) * viewport[2];
  float x2 = (proj2[0] / proj2[3] * 0.5 + 0.5) * viewport[2];

  float width = fabs(x1 - x2);
  float size = pow(2. * width, 2);

  // Test of visibility
  x2 += viewport[0];
  float y2 = (proj2[1] / proj2[3] * 0.5 + 0.5) * viewport[3] + viewport[1];
  Vector<float, 2> upleft;
  upleft[0] = x2 - width;
  upleft[1] = y2 - width;
  Vector<float, 2> downright;
  downright[0] = x2 + width;
  downright[1] = y2 + width;
  Rectangle<float> r1;
  r1[0] = upleft;
  r1[1] = downright;

  Vector<float, 2> upleftV;
  upleftV[0] = viewport[0];
  upleftV[1] = viewport[1];

  Vector<float, 2> downrightV;
  downrightV[0] = viewport[0] + viewport[2];
  downrightV[1] = viewport[1] + viewport[3];

  Rectangle<float> r2;
  r2[0] = upleftV;
  r2[1] = downrightV;

  if (!r1.intersect(r2)) {
    size *= -1.0;
  }

  return size;
}
//====================================================
float calculateAABBSize(const BoundingBox &bb, const Coord &eye, const MatrixGL &transformMatrix,
                        const Vec4i &globalViewport, const Vec4i &currentViewport) {
  BoundingBox bbTmp = bb;
  Coord src[8];
  Coord dst[8];
  int pos;
  int num;

  for (int i = 0; i < 3; i++) {
    if (bbTmp[0][i] > bbTmp[1][i]) {
      float tmp = bbTmp[0][i];
      bbTmp[0][i] = bbTmp[1][i];
      bbTmp[1][i] = tmp;
    }
  }

  bbTmp.getCompleteBB(src);
  pos = ((eye[0] < src[0][0])) + ((eye[0] > src[6][0]) << 1) + ((eye[1] < src[0][1]) << 2) +
        ((eye[1] > src[6][1]) << 3) + ((eye[2] < src[0][2]) << 4) + ((eye[2] > src[6][2]) << 5);
  assert(pos <= 42);

  // If pos==0 : camera are inside the entity so we return a arbitrary lod
  if (pos == 0) {
    return 10.;
  }

  num = hullVertexTable[pos][0];

  if (num == 0) {
    return -1;
  }

  for (int i = 0; i < num; i++) {
    dst[i] = projectPoint(src[int(hullVertexTable[pos][i + 1])], transformMatrix, globalViewport);
    dst[i][1] = globalViewport[1] + globalViewport[3] - (dst[i][1] - globalViewport[1]);
  }

  bool inScreen = false;
  float bbBox[4] = {0.f, 0.f, 0.f, 0.f};

  for (int i = 0; i < num; i++) {
    if ((dst[i][0] >= currentViewport[0]) &&
        (dst[i][0] <= currentViewport[0] + currentViewport[2]) &&
        (dst[i][1] >= currentViewport[1]) &&
        (dst[i][1] <= currentViewport[1] + currentViewport[3])) {
      inScreen = true;
    }

    if (i == 0) {
      bbBox[0] = dst[i][0];
      bbBox[2] = dst[i][0];
      bbBox[1] = dst[i][1];
      bbBox[3] = dst[i][1];
    } else {
      if (dst[i][0] < bbBox[0]) {
        bbBox[0] = dst[i][0];
      }

      if (dst[i][0] > bbBox[2]) {
        bbBox[2] = dst[i][0];
      }

      if (dst[i][1] < bbBox[1]) {
        bbBox[1] = dst[i][1];
      }

      if (dst[i][1] > bbBox[3]) {
        bbBox[3] = dst[i][1];
      }
    }

    if (bbBox[0] < currentViewport[0] + currentViewport[2] && bbBox[2] > currentViewport[0] &&
        bbBox[1] < currentViewport[1] + currentViewport[3] && bbBox[3] > currentViewport[1]) {
      inScreen = true;
    }
  }

  if (!inScreen) {
    return -1;
  } else {
    return sqrt((bbBox[2] - bbBox[0]) * (bbBox[2] - bbBox[0]) +
                (bbBox[3] - bbBox[1]) * (bbBox[3] - bbBox[1])) *
           2;
  }
}
//====================================================
float calculate2DLod(const BoundingBox &bb, const Vec4i &, const Vec4i &) {
  return (bb[1][0] - bb[0][0]) * (bb[1][1] - bb[0][1]);
}
//====================================================

std::vector<Coord> computeNormals(const std::vector<Coord> &vertices,
                                  const std::vector<unsigned short> &facesIndices) {
  return computeNormals(vertices, std::vector<uint>(facesIndices.begin(), facesIndices.end()));
}

std::vector<Coord> computeNormals(const std::vector<Coord> &vertices,
                                  const std::vector<uint> &facesIndices) {
  assert(vertices.size() >= 3);
  assert(facesIndices.size() >= 3 && facesIndices.size() % 3 == 0);
  std::vector<Coord> normals;
  normals.resize(vertices.size(), Coord(0, 0, 0));

  for (size_t i = 0; i < facesIndices.size(); i += 3) {
    Coord v1 = vertices[facesIndices[i]], v2 = vertices[facesIndices[i + 1]],
          v3 = vertices[facesIndices[i + 2]];
    Coord normal = (v2 - v1) ^ (v3 - v1);

    if (normal.norm() != 0) {
      normal /= normal.norm();
    }

    normals[facesIndices[i]] += normal;
    normals[facesIndices[i + 1]] += normal;
    normals[facesIndices[i + 2]] += normal;
  }

  for (auto &normal : normals) {
    if (normal.norm() != 0) {
      normal /= normal.norm();
    }
  }

  return normals;
}
}
