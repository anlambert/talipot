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

#include <talipot/ParametricCurves.h>
#include <talipot/ParallelTools.h>
#include <map>

using namespace std;

namespace tlp {

static void computeLinearBezierPoints(const Coord &p0, const Coord &p1, vector<Coord> &curvePoints,
                                      const uint nbCurvePoints) {
  float h = 1.0 / float((nbCurvePoints - 1));
  Coord firstFD = (p1 - p0) * h;
  Coord c = p0;

  curvePoints.resize(nbCurvePoints);

  // Compute points at each step
  curvePoints[0] = c;

  for (uint i = 0; i < nbCurvePoints - 2; ++i) {

    c += firstFD;

    curvePoints[i + 1] = c;
  }

  curvePoints[nbCurvePoints - 1] = p1;
}

static void computeQuadraticBezierPoints(const Coord &p0, const Coord &p1, const Coord &p2,
                                         vector<Coord> &curvePoints, const uint nbCurvePoints) {

  // Compute our step size
  float h = 1.0 / float(nbCurvePoints - 1);
  float h2 = h * h;

  // Compute Initial forward difference
  Coord firstFD = p0 * (h2 - 2 * h) + p1 * (-2 * h2 + 2 * h) + p2 * h2;
  Coord secondFD = p0 * 2.f * h2 - p1 * 4.f * h2 + p2 * 2.f * h2;

  Coord c = p0;

  curvePoints.resize(nbCurvePoints);

  // Compute points at each step
  curvePoints[0] = c;

  for (uint i = 0; i < nbCurvePoints - 2; ++i) {

    c += firstFD;

    firstFD += secondFD;

    curvePoints[i + 1] = c;
  }

  curvePoints[nbCurvePoints - 1] = p2;
}

static void computeCubicBezierPoints(const Coord &p0, const Coord &p1, const Coord &p2,
                                     const Coord &p3, vector<Coord> &curvePoints,
                                     const uint nbCurvePoints) {

  // Compute polynomial coefficients from Bezier points
  Coord A = p0 * -1.f + (p1 - p2) * 3.f + p3;
  Coord B = p0 * 3.f - p1 * 6.f + p2 * 3.f;
  Coord C = p0 * -3.f + p1 * 3.f;

  // Compute our step size
  float h = 1.0 / float(nbCurvePoints - 1);
  float h2 = h * h;
  float h3 = h2 * h;
  float h36 = h3 * 6;
  float h22 = h2 * 2;

  // Compute forward differences from Bezier points and "h"
  Coord firstFD = A * h3 + B * h2 + C * h;
  Coord thirdFD = A * h36;
  Coord secondFD = thirdFD + B * h22;

  Coord c = p0;

  curvePoints.resize(nbCurvePoints);

  // Compute points at each step
  curvePoints[0] = c;

  for (uint i = 0; i < nbCurvePoints - 2; ++i) {

    c += firstFD;

    firstFD += secondFD;

    secondFD += thirdFD;

    curvePoints[i + 1] = c;
  }

  curvePoints[nbCurvePoints - 1] = p3;
}

Coord computeBezierPoint(const vector<Coord> &controlPoints, const float t) {

  size_t nbControlPoints = controlPoints.size();

  Vec3d bezierPoint;
  bezierPoint[0] = bezierPoint[1] = bezierPoint[2] = 0;
  double curCoeff = 1.0;
  auto r = double(nbControlPoints);

  for (size_t i = 0; i < controlPoints.size(); ++i) {
    Vec3d controlPoint;
    controlPoint[0] = controlPoints[i][0];
    controlPoint[1] = controlPoints[i][1];
    controlPoint[2] = controlPoints[i][2];
    bezierPoint += controlPoint * curCoeff * pow(double(t), double(i)) *
                   pow(1.0 - double(t), double(nbControlPoints - 1 - i));
    auto c = double(i + 1);
    curCoeff *= (r - c) / c;
  }

  return Coord(bezierPoint[0], bezierPoint[1], bezierPoint[2]);
}

void computeBezierPoints(const vector<Coord> &controlPoints, vector<Coord> &curvePoints,
                         uint nbCurvePoints) {
  assert(controlPoints.size() > 1);

  switch (controlPoints.size()) {
  case 2:
    computeLinearBezierPoints(controlPoints[0], controlPoints[1], curvePoints, nbCurvePoints);
    break;

  case 3:
    computeQuadraticBezierPoints(controlPoints[0], controlPoints[1], controlPoints[2], curvePoints,
                                 nbCurvePoints);
    break;

  case 4:
    computeCubicBezierPoints(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3],
                             curvePoints, nbCurvePoints);
    break;

  default:
    curvePoints.resize(nbCurvePoints);
    float h = 1.0 / float(nbCurvePoints - 1);
    TLP_PARALLEL_MAP_INDICES(
        nbCurvePoints, [&](uint i) { curvePoints[i] = computeBezierPoint(controlPoints, i * h); });
  }
}

static void computeCatmullRomGlobalParameter(const vector<Coord> &controlPoints,
                                             vector<float> &globalParameter, const float alpha) {
  globalParameter.resize(controlPoints.size());
  globalParameter[0] = 0.0f;
  globalParameter[controlPoints.size() - 1] = 1.0f;
  vector<float> cumDist;
  cumDist.resize(controlPoints.size());
  cumDist[0] = 0.0f;
  float totalDist = 0.f;

  for (size_t i = 1; i < controlPoints.size(); ++i) {
    float dist = pow(controlPoints[i - 1].dist(controlPoints[i]), alpha);
    cumDist[i] = cumDist[i - 1] + dist;
    totalDist += dist;
  }

  for (size_t i = 1; i < controlPoints.size() - 1; ++i) {
    globalParameter[i] = cumDist[i] / totalDist;
  }
}

static size_t computeSegmentIndex(float t, const vector<Coord> &controlPoints,
                                  const vector<float> &globalParameter) {
  if (t == 0.0) {
    return 0;
  } else if (t == 1.0) {
    return controlPoints.size() - 1;
  } else {
    size_t i = 0;

    while (t >= globalParameter[i + 1]) {
      ++i;
    }

    return i;
  }
}

static void computeBezierSegmentControlPoints(const Coord &pBefore, const Coord &pStart,
                                              const Coord &pEnd, const Coord &pAfter,
                                              vector<Coord> &bezierSegmentControlPoints,
                                              const float alpha) {
  bezierSegmentControlPoints.push_back(pStart);
  float d1 = pBefore.dist(pStart);
  float d2 = pStart.dist(pEnd);
  float d3 = pEnd.dist(pAfter);
  float d1alpha = pow(d1, alpha);
  float d12alpha = pow(d1, 2 * alpha);
  float d2alpha = pow(d2, alpha);
  float d22alpha = pow(d2, 2 * alpha);
  float d3alpha = pow(d3, alpha);
  float d32alpha = pow(d3, 2 * alpha);
  bezierSegmentControlPoints.push_back(
      Coord((d12alpha * pEnd - d22alpha * pBefore +
             (2 * d12alpha + 3 * d1alpha * d2alpha + d22alpha) * pStart) /
            (3 * d1alpha * (d1alpha + d2alpha))));
  bezierSegmentControlPoints.push_back(
      Coord((d32alpha * pStart - d22alpha * pAfter +
             (2 * d32alpha + 3 * d3alpha * d2alpha + d22alpha) * pEnd) /
            (3 * d3alpha * (d3alpha + d2alpha))));
  bezierSegmentControlPoints.push_back(pEnd);
}

static Coord computeCatmullRomPointImpl(const vector<Coord> &controlPoints, const float t,
                                        const vector<float> &globalParameter,
                                        const bool closedCurve, const float alpha) {
  size_t i = computeSegmentIndex(t, controlPoints, globalParameter);
  float localT = 0.0;

  if (t >= 1.0) {
    localT = 1.0;
  } else if (t != 0.0) {
    localT = (t - globalParameter[i]) / (globalParameter[i + 1] - globalParameter[i]);
  }

  vector<Coord> bezierControlPoints;

  if (i == 0) {
    Coord firstPoint = controlPoints[controlPoints.size() - 2];
    if (!closedCurve) {
      firstPoint = controlPoints[i] - (controlPoints[i + 1] - controlPoints[i]);
    }
    computeBezierSegmentControlPoints(firstPoint, controlPoints[i], controlPoints[i + 1],
                                      controlPoints[i + 2], bezierControlPoints, alpha);
  } else if (i == controlPoints.size() - 2) {
    Coord lastPoint = controlPoints[1];
    if (!closedCurve) {
      lastPoint = controlPoints[i + 1] + (controlPoints[i + 1] - controlPoints[i]);
    }
    computeBezierSegmentControlPoints(controlPoints[i - 1], controlPoints[i], controlPoints[i + 1],
                                      lastPoint, bezierControlPoints, alpha);
  } else if (i == controlPoints.size() - 1) {
    Coord lastPoint = controlPoints[1];
    if (!closedCurve) {
      lastPoint = controlPoints[i] + (controlPoints[i] - controlPoints[i - 1]);
    }
    computeBezierSegmentControlPoints(controlPoints[i - 2], controlPoints[i - 1], controlPoints[i],
                                      lastPoint, bezierControlPoints, alpha);
  } else {
    computeBezierSegmentControlPoints(controlPoints[i - 1], controlPoints[i], controlPoints[i + 1],
                                      controlPoints[i + 2], bezierControlPoints, alpha);
  }

  float t2 = localT * localT;
  float t3 = t2 * localT;
  float s = 1.0 - localT;
  float s2 = s * s;
  float s3 = s2 * s;
  return bezierControlPoints[0] * s3 + bezierControlPoints[1] * 3.0f * localT * s2 +
         bezierControlPoints[2] * 3.0f * t2 * s + bezierControlPoints[3] * t3;
}

Coord computeCatmullRomPoint(const vector<Coord> &controlPoints, const float t,
                             const bool closedCurve, const float alpha) {
  assert(controlPoints.size() > 2);
  vector<float> globalParameter;
  vector<Coord> controlPointsCp(controlPoints);

  if (closedCurve) {
    controlPointsCp.push_back(controlPoints[0]);
  }

  computeCatmullRomGlobalParameter(controlPointsCp, globalParameter, alpha);
  return computeCatmullRomPointImpl(controlPointsCp, t, globalParameter, closedCurve, alpha);
}

void computeCatmullRomPoints(const vector<Coord> &controlPoints, vector<Coord> &curvePoints,
                             const bool closedCurve, const uint nbCurvePoints, const float alpha) {
  // assert(controlPoints.size() > 2);
  if (controlPoints.size() <= 2) {
    return;
  }

  vector<float> globalParameter;
  vector<Coord> controlPointsCp(controlPoints);

  if (closedCurve) {
    controlPointsCp.push_back(controlPoints[0]);
  }

  computeCatmullRomGlobalParameter(controlPointsCp, globalParameter, alpha);
  curvePoints.resize(nbCurvePoints);
  TLP_PARALLEL_MAP_INDICES(nbCurvePoints, [&](uint i) {
    curvePoints[i] = computeCatmullRomPointImpl(controlPointsCp, i / float(nbCurvePoints - 1),
                                                globalParameter, closedCurve, alpha);
  });
}

static float clamp(float f, float minVal, float maxVal) {
  return min(max(f, minVal), maxVal);
}

Coord computeOpenUniformBsplinePoint(const vector<Coord> &controlPoints, const float t,
                                     const uint curveDegree) {
  assert(controlPoints.size() > 3);
  uint nbKnots = controlPoints.size() + curveDegree + 1;
  float stepKnots = 1.0f / ((float(nbKnots) - 2.0f * (float(curveDegree) + 1.0f)) + 2.0f - 1.0f);

  if (t == 0.0) {
    return controlPoints[0];
  } else if (t >= 1.0) {
    return controlPoints[controlPoints.size() - 1];
  } else {
    auto *coeffs = new float[curveDegree + 1];
    memset(coeffs, 0, (curveDegree + 1) * sizeof(float));
    int k = curveDegree;
    int cpt = 0;

    while (t > (cpt * stepKnots) && t >= ((cpt + 1) * stepKnots)) {
      ++k;
      ++cpt;
    }

    float knotVal = (cpt * stepKnots);
    coeffs[curveDegree] = 1.0;

    for (int i = 1; i <= int(curveDegree); ++i) {
      coeffs[curveDegree - i] =
          (clamp(knotVal + stepKnots, 0.0, 1.0) - t) /
          (clamp(knotVal + stepKnots, 0.0, 1.0) - clamp(knotVal + (-i + 1) * stepKnots, 0.0, 1.0)) *
          coeffs[curveDegree - i + 1];
      int tabIdx = curveDegree - i + 1;

      for (int j = -i + 1; j <= -1; ++j) {
        coeffs[tabIdx] = ((t - clamp(knotVal + j * stepKnots, 0.0, 1.0)) /
                          (clamp(knotVal + (j + i) * stepKnots, 0.0, 1.0) -
                           clamp(knotVal + j * stepKnots, 0.0, 1.0))) *
                             coeffs[tabIdx] +
                         ((clamp(knotVal + (j + i + 1) * stepKnots, 0.0, 1.0) - t) /
                          (clamp(knotVal + (j + i + 1) * stepKnots, 0.0, 1.0) -
                           clamp(knotVal + (j + 1) * stepKnots, 0.0, 1.0))) *
                             coeffs[tabIdx + 1];
        ++tabIdx;
      }

      coeffs[curveDegree] = ((t - knotVal) / (clamp(knotVal + i * stepKnots, 0.0, 1.0) - knotVal)) *
                            coeffs[curveDegree];
    }

    Coord curvePoint;
    int startIdx = k - curveDegree;

    for (uint i = 0; i <= curveDegree; ++i) {
      curvePoint += coeffs[i] * controlPoints[startIdx + i];
    }

    delete[] coeffs;
    return curvePoint;
  }
}

void computeOpenUniformBsplinePoints(const vector<Coord> &controlPoints, vector<Coord> &curvePoints,
                                     const uint curveDegree, const uint nbCurvePoints) {
  curvePoints.resize(nbCurvePoints);
  TLP_PARALLEL_MAP_INDICES(nbCurvePoints, [&](uint i) {
    curvePoints[i] =
        computeOpenUniformBsplinePoint(controlPoints, i / float(nbCurvePoints - 1), curveDegree);
  });
}
}
