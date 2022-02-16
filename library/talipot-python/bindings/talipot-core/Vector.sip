/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
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
#include <talipot/Vector.h>
#include <sstream>
%End

namespace tlp {
class Vec3f  {
%Docstring
This class represents a vector with 3 floating point components.
It can be instantiated as illustrated in the code below: ::

  # Initializes a Vec3f with all components set to 0.0
  v = tlp.Vec3f()

  # Initializes a Vec3f with all components set to 1.0
  v2 = tlp.Vec3f(1.0)

  # Initializes a Vec3f by specifying the value of each component
  v3 = tlp.Vec3f(1.0, 2.0, 3.0)

  # Initializes a Vec3f by copy
  v4 = tlp.Vec3f(v3)

Numerous mathematical operations are available to work with vectors. The sample code below illustrates them: ::

  # Instantiate two vectors
  v = tlp.Vec3f(4.5, 1.0, 3.0)
  v2 = tlp.Vec3f(1.5, 2.0, 6.0)

  # Add two vectors, operator += is also available
  v3 = v + v2

  # Add a value to all components of a vector, operator += is also available
  v4 = v + 2.0

  # Subtract two vectors, operator -= is also available
  v5 = v - v2

  # Subtract a value to all components of a vector, operator -= is also available
  v6 = v - 3.0

  # Multiply two vectors (not vector product), operator *= is also available
  v7 = v * v2

  # Multiply each component of a vector by a value, operator *= is also available
  v8 = v * 2.0

  # Divide two vectors, operator /= is also available
  v9 = v / v2

  # Divide each component of a vector by a value, operator /= is also available
  v10 = v / 2.0

  # Perform a vector product, operator ^= is also available
  v11 = v ^ v2

Each component of the vector can be read / written through the [] operator: ::

  v = tlp.Vec3f(1.0, 2.0, 3.0)

  # read first component
  a = v[0]

  # write third component
  v[2] = 4.0

Operators for vectors equality (==), vectors difference (!=) and vectors comparison (<, >) are also available.

%End

// Convert Python list or tuple of numbers to a tlp::Vec3f
%ConvertToTypeCode
  const sipTypeDef *sipType = sipFindType("tlp::Vec3f");
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (sipCanConvertToType(sipPy, sipType, SIP_NOT_NONE|SIP_NO_CONVERTORS)) {
      return 1;
    }
    return isTupleAndCanConvertToVec3fType(sipPy);
  }
  if (sipCanConvertToType(sipPy, sipType, SIP_NOT_NONE|SIP_NO_CONVERTORS)) {
    int state=0, err=0;
    tlp::Vec3f *v = static_cast<tlp::Vec3f*>(sipConvertToType(sipPy, sipType, sipTransferObj, SIP_NOT_NONE|SIP_NO_CONVERTORS, &state, &err));
    *sipCppPtr = new tlp::Vec3f(*v);
    sipReleaseType(v, sipType, state);
  } else {
    *sipCppPtr = convertToVec3fType<tlp::Vec3f>(sipPy);
  }
  return sipGetState(sipTransferObj);
%End

public:

  Vec3f();

  Vec3f(const float a0);

  Vec3f(const float a0, const float a1, const float a2=0);

  Vec3f(const tlp::Vec3f &);

  float operator[](const uint i) const;
%MethodCode
  if (a0 < 3) {
    sipRes = (*sipCpp)[a0];
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  void __setitem__(int i, const float &value);
%MethodCode
  if (a0 < 3) {
    (*sipCpp)[a0] = a1;
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  tlp::Vec3f & operator*=(const float &);

  tlp::Vec3f & operator*=(const tlp::Vec3f &);

  tlp::Vec3f & operator/=(const float &);
%MethodCode
  if (a0 != 0) {
    sipCpp->tlp::Vec3f::operator/=(a0);
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[3] by zero");
  }
%End

  tlp::Vec3f & operator/=(const tlp::Vec3f &);
%MethodCode
  if ((*a0)[0] && (*a0)[1] && (*a0)[2]) {
    sipCpp->tlp::Vec3f::operator/=(*a0);
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[3] by a float vector[3] with a zero element");
  }
%End

  tlp::Vec3f & operator+=(const float &);

  tlp::Vec3f & operator+=(const tlp::Vec3f &);

  tlp::Vec3f & operator-=(const float &);

  tlp::Vec3f & operator-=(const tlp::Vec3f &);

  tlp::Vec3f & operator^=(const tlp::Vec3f &);

  tlp::Vec3f operator*(const  tlp::Vec3f &) const;

  tlp::Vec3f operator*(const float &) const;

  tlp::Vec3f operator+(const  tlp::Vec3f &) const;

  tlp::Vec3f operator+(const float &) const;

  tlp::Vec3f operator-(const  tlp::Vec3f &) const;

  tlp::Vec3f operator-(const float &) const;

  tlp::Vec3f operator/(const  tlp::Vec3f &) const;
%MethodCode
  if ((*a1)[0] && (*a1)[1] && (*a1)[2]) {
    sipRes = new tlp::Vec3f((*a0 / *a1));
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[3] by a float vector[3] with a zero element");
  }
%End

  tlp::Vec3f operator/(const float &) const;
%MethodCode
  if (a1 != 0) {
    sipRes = new tlp::Vec3f((*a0 / a1));
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[3] by zero");
  }
%End

  tlp::Vec3f operator^(const  tlp::Vec3f &) const;

  bool operator>(const tlp::Vec3f &) const;

  bool operator<(const tlp::Vec3f &) const;

  bool operator!=(const tlp::Vec3f &) const;

  bool operator==(const tlp::Vec3f &) const;

  void set(const float x=0,const float y=0, const float z=0);
%Docstring
tlp.Vec3f.set(x, y, z)

Sets each component of the vector.

:param x: the first component value
:type x: float
:param y: the second component value
:type y: float
:param z: the third component value
:type z: float
%End

  void set(const tlp::Vec3f& v);
%Docstring
tlp.Vec3f.set(v)

Sets each component of the vector from another one.

:param v: a vector
:type c: :class:`tlp.Vec3f`
%End

  void fill(const float &obj);
%Docstring
tlp.Vec3f.fill(val)

Set the same value to each component of the vector.

:param val: a value
:type val: float
%End

  float norm () const;
%Docstring
tlp.Vec3f.norm()

Computes and returns the euclidean norm of the vector.

:rtype: float
%End

  float dist (const tlp::Vec3f &) const;
%Docstring
tlp.Vec3f.dist(v)

Computes and returns the distance between the vector (3d point) and another one.

:param v: a vector
:type v: :class:`tlp.Vec3f`
:rtype: float
%End

  float dotProduct(const tlp::Vec3f &) const;
%Docstring
tlp.Vec3f.dotProduct(v)

Computes and returns the dot product (scalar product) of the vector with another one

:param v: a vector
:type v: :class:`tlp.Vec3f`
:rtype: float
%End

  std::string __repr__() const;
%MethodCode
  std::ostringstream oss;
  oss << *sipCpp;
  sipRes = new std::string(oss.str());
%End

};

tlp::Vec3f operator*(const float &, const tlp::Vec3f &) const;

class Vec4f  {

%Docstring
This class represents a vector with 4 floating point components.
Constructors, operators and methods are similar to those from the :class:`tlp.Vec3f` class.
%End

public:

  Vec4f();

  Vec4f(const float a0);

  Vec4f(const float a0, const float a1);

  Vec4f(const float a0, const float a1, const float a2);

  Vec4f(const float a0, const float a1, const float a2, const float a3);

  Vec4f(const tlp::Vec4f &);

  float operator[](const uint i) const;
%MethodCode
  if (a0 < 4) {
    sipRes = (*sipCpp)[a0];
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  void __setitem__(int i, const float &value);
%MethodCode
  if (a0 < 4) {
    (*sipCpp)[a0] = a1;
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  tlp::Vec4f & operator*=(const float &);

  tlp::Vec4f & operator*=(const tlp::Vec4f &);

  tlp::Vec4f & operator/=(const float &);
%MethodCode
  if (a0 != 0) {
    sipCpp->tlp::Vec4f::operator/=(a0);
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[4] by zero");
  }
%End

  tlp::Vec4f & operator/=(const tlp::Vec4f &);
%MethodCode
  if ((*a0)[0] && (*a0)[1] && (*a0)[2] && (*a0)[3]) {
    sipCpp->tlp::Vec4f::operator/=(*a0);
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[4] by a float vector[4] with a zero element");
  }
%End

  tlp::Vec4f & operator+=(const float &);

  tlp::Vec4f & operator+=(const tlp::Vec4f &);

  tlp::Vec4f & operator-=(const float &);

  tlp::Vec4f & operator-=(const tlp::Vec4f &);

  tlp::Vec4f & operator^=(const tlp::Vec4f &);

  tlp::Vec4f operator*(const  tlp::Vec4f &) const;

  tlp::Vec4f operator*(const tlp::Mat4f &mat) const;

  tlp::Vec4f operator*(const float &) const;

  tlp::Vec4f operator+(const  tlp::Vec4f &) const;

  tlp::Vec4f operator+(const float &) const;

  tlp::Vec4f operator-(const  tlp::Vec4f &) const;

  tlp::Vec4f operator-(const float &) const;

  tlp::Vec4f operator/(const  tlp::Vec4f &) const;
%MethodCode
  if ((*a1)[0] && (*a1)[1] && (*a1)[2] && (*a1)[3]) {
    sipRes = new tlp::Vec4f((*a0 / *a1));
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[4] by a float vector[4] with a zero element");
  }
%End

  tlp::Vec4f operator/(const float &) const;
%MethodCode
  if (a1 != 0) {
    sipRes = new tlp::Vec4f((*a0 / a1));
  } else {
    sipIsErr = -1;
    PyErr_SetString(PyExc_ZeroDivisionError, "division of a float vector[4] by zero");
  }
%End

  tlp::Vec4f operator^(const  tlp::Vec4f &) const;

  bool operator>(const tlp::Vec4f &) const;

  bool operator<(const tlp::Vec4f &) const;

  bool operator!=(const tlp::Vec4f &) const;

  bool operator==(const tlp::Vec4f &) const;

  void set(const float x=0,const float y=0, const float z=0, const float w=0);

  void set(const tlp::Vec4f& v);

  void fill(const float &obj);

  float norm () const;

  float dist (const tlp::Vec4f &) const;

  float dotProduct(const tlp::Vec4f &) const;

  std::string __repr__() const;
%MethodCode
  std::ostringstream oss;
  oss << *sipCpp;
  sipRes = new std::string(oss.str());
%End

};

tlp::Vec4f operator*(const float &, const tlp::Vec4f &) const;

class Vec4i  {

%Docstring
This class represents a vector with 4 integer components.
Constructors, operators and methods are similar to those from the :class:`tlp.Vec3f` class.
%End

public:

  Vec4i();

  Vec4i(const int a0);

  Vec4i(const int a0, const int a1);

  Vec4i(const int a0, const int a1, const int a2);

  Vec4i(const int a0, const int a1, const int a2, const int a3);

  Vec4i(const tlp::Vec4i &);

  int operator[](const uint i) const;
%MethodCode
  if (a0 < 4) {
    sipRes = (*sipCpp)[a0];
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  void __setitem__(int i, const int &value);
%MethodCode
  if (a0 < 4) {
    (*sipCpp)[a0] = a1;
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  tlp::Vec4i & operator*=(const int &);

  tlp::Vec4i & operator*=(const tlp::Vec4i &);

  tlp::Vec4i & operator/=(const int &);

  tlp::Vec4i & operator/=(const tlp::Vec4i &);

  tlp::Vec4i & operator+=(const int &);

  tlp::Vec4i & operator+=(const tlp::Vec4i &);

  tlp::Vec4i & operator-=(const int &);

  tlp::Vec4i & operator-=(const tlp::Vec4i &);

  tlp::Vec4i & operator^=(const tlp::Vec4i &);

  tlp::Vec4i operator*(const  tlp::Vec4i &) const;

  tlp::Vec4i operator*(const int &) const;

  tlp::Vec4i operator+(const  tlp::Vec4i &) const;

  tlp::Vec4i operator+(const int &) const;

  tlp::Vec4i operator-(const  tlp::Vec4i &) const;

  tlp::Vec4i operator-(const int &) const;

  tlp::Vec4i operator/(const  tlp::Vec4i &) const;

  tlp::Vec4i operator/(const int &) const;

  tlp::Vec4i operator^(const  tlp::Vec4i &) const;

  bool operator>(const tlp::Vec4i &) const;

  bool operator<(const tlp::Vec4i &) const;

  bool operator!=(const tlp::Vec4i &) const;

  bool operator==(const tlp::Vec4i &) const;

  void fill(const int &obj);

  std::string __repr__() const;
%MethodCode
  std::ostringstream oss;
  oss << *sipCpp;
  sipRes = new std::string(oss.str());
%End

};

tlp::Vec4i operator*(const int &, const tlp::Vec4i &) const;

};

