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


%ModuleHeaderCode
#include <talipot/Matrix.h>
namespace tlp {
typedef tlp::Matrix<float, 4> Mat4f;
}
typedef std::vector<float> vectorFloat;
%End

typedef std::vector<float> vectorFloat;

namespace tlp {

class Mat4f {
%TypeHeaderCode
#include <talipot/Matrix.h>
%End

public:

  Mat4f();

  Mat4f(const tlp::Mat4f&a);

  Mat4f(const std::vector<vectorFloat> &covarianceMatrix /TypeHint="List[List[float]]"/);

  tlp::Mat4f& fill(float obj);

  float determinant() const;

  tlp::Mat4f& transpose();

  tlp::Mat4f& inverse();

  tlp::Mat4f operator+(const tlp::Mat4f &mat) const;

  tlp::Mat4f & operator+=(const tlp::Mat4f &mat);

  tlp::Mat4f operator-(const tlp::Mat4f &mat) const;

  tlp::Mat4f & operator-=(const tlp::Mat4f &mat);

  bool operator==(const tlp::Mat4f &) const;

  bool operator!=(const tlp::Mat4f &) const;

  tlp::Mat4f & operator*=(const tlp::Mat4f &mat);

  tlp::Mat4f & operator*=(const float obj);

  tlp::Mat4f operator*(const tlp::Mat4f &mat) const;

  tlp::Vec4f operator*(const tlp::Vec4f &v) const;

  tlp::Mat4f operator*(const float obj) const;

  tlp::Mat4f & operator/=(const tlp::Mat4f &mat);

  tlp::Mat4f & operator/=(const float obj);

  tlp::Mat4f operator/(const tlp::Mat4f &mat) const;

  tlp::Mat4f operator/(const float obj) const;

  tlp::Mat4f cofactor() const;

  tlp::Vec4f powerIteration(const uint nIterations) const;

  tlp::Vec4f& operator[](const uint i)  /NoCopy/;
%MethodCode
if (a0 < 4) {
  sipRes = &(*sipCpp)[a0];
} else {
  sipIsErr = -1;
  PyErr_SetNone(PyExc_IndexError);
}
%End

  void __setitem__(int i, const tlp::Vec4f &value);
%MethodCode
if (a0 < 4) {
  (*sipCpp)[a0] = *a1;
} else {
  sipIsErr = -1;
  PyErr_SetNone(PyExc_IndexError);
}
%End

  std::string __repr__() const;
%MethodCode
std::string repr = "";
for (int i = 0; i < 4; ++i) {
  std::ostringstream oss;
  oss << (*sipCpp)[i];
  std::string s = oss.str();
  s[0] = '[';
  s[s.size() - 1] = ']';
  repr += (s + "\n");
}
sipRes = new std::string(repr);
%End

};

};
