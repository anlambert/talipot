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

#ifndef TALIPOT_VECTOR_H
#define TALIPOT_VECTOR_H

#include <cassert>
#include <talipot/Array.h>
#include <talipot/config.h>
#include <talipot/TlpTools.h>
#include <talipot/hash.h>

#include <cmath>
#include <limits>
#include <cstring>
#include <iostream>

#define VECTOR Vector<TYPE, SIZE, OTYPE, DTYPE>
#define TEMPLATEVECTOR template <typename TYPE, size_t SIZE, typename OTYPE, typename DTYPE>

namespace tlp {

template <typename TYPE, typename OTYPE>
inline OTYPE tlpsqr(const TYPE a) {
  return static_cast<OTYPE>(a) * static_cast<OTYPE>(a);
}

template <typename TYPE, typename OTYPE>
inline TYPE tlpsqrt(const OTYPE a) {
  return static_cast<TYPE>(std::sqrt(a));
}

template <>
inline double tlpsqrt<double, long double>(long double a) {
  return double(sqrtl(a));
}

/**
 * @ingroup Structures
 * \brief class for mathematical vector
 *
 * Enables to create a Vector of TYPE (must be a numeric basic type) with a
 * fixed size and provides Mathematical operation. Mathematical
 * operators must be defined for TYPE. Out of bound accesses are only checked
 * in debug mode. The OTYPE is used for temporary computation to prevent overflow,
 * by default OTYPE is a double.
 *
 * \author : David Auber auber@labri.fr
 * \version 0.0.1 24/01/2003
 */
template <typename TYPE, size_t SIZE, typename OTYPE = double, typename DTYPE = TYPE>
class Vector : public Array<TYPE, SIZE> {
public:
  Vector();

  Vector(const VECTOR &v) = default;

  Vector(VECTOR &&v) = default;

  Vector(const Vector<TYPE, SIZE + 1, OTYPE> &v);

  Vector(const TYPE x);

  Vector(const TYPE x, const TYPE y);

  Vector(const TYPE x, const TYPE y, const TYPE z);

  Vector(const Vector<TYPE, 2, OTYPE> &v, const TYPE z);

  Vector(const TYPE x, const TYPE y, const TYPE z, const TYPE w);

  Vector(const Vector<TYPE, 2, OTYPE> &v, const TYPE z, const TYPE w);

  Vector(const Vector<TYPE, 3, OTYPE> &v, const TYPE w);

  void set(const TYPE x);

  void set(const TYPE x, const TYPE y);

  void set(const TYPE x, const TYPE y, const TYPE z);

  void set(const TYPE x, const TYPE y, const TYPE z, const TYPE w);

  void set(const Vector<TYPE, 2, OTYPE> &v, const TYPE z);

  void set(const Vector<TYPE, 2, OTYPE> &v, const TYPE z, const TYPE w);

  void set(const Vector<TYPE, 3, OTYPE> &v, const TYPE w);

  void set(const VECTOR &v);

  void set(const Vector<TYPE, SIZE + 1, OTYPE> &v);

  VECTOR &operator*=(const TYPE);

  VECTOR &operator*=(const VECTOR &);

  VECTOR &operator/=(const TYPE);

  VECTOR &operator/=(const VECTOR &);

  VECTOR &operator+=(const TYPE);

  VECTOR &operator+=(const VECTOR &);

  VECTOR &operator-=(const TYPE);

  VECTOR &operator-=(const VECTOR &);

  VECTOR &operator^=(const VECTOR &);

  bool operator>(const VECTOR &) const;

  bool operator<(const VECTOR &) const;

  bool operator!=(const VECTOR &) const;

  bool operator==(const VECTOR &) const;

  TYPE norm() const;

  TYPE length() const;

  VECTOR &normalize();

  DTYPE dist(const VECTOR &) const;

  TYPE dotProduct(const VECTOR &) const;

  VECTOR &operator=(const VECTOR &) = default;
};

TEMPLATEVECTOR
inline TYPE dotProduct(const VECTOR &a, const VECTOR &b) {
  return a.dotProduct(b);
}

TEMPLATEVECTOR
inline TYPE dist(const VECTOR &a, const VECTOR &b) {
  return a.dist(b);
}

/**
 * Return the minimum of each dimension of the two vectors
 * for instance for a 2 vectors of dim 2 :
 * min(V1, V2) = (min(V1[0], v2[0]), min(V1[1], v2[1))
 */
TEMPLATEVECTOR
inline VECTOR minVector(const VECTOR &u, const VECTOR &v) {
  VECTOR tmp;

  for (size_t i = 0; i < SIZE; ++i) {
    tmp[i] = std::min(u[i], v[i]);
  }

  return tmp;
}
/**
 * Return the maximum of each dimension of the two vectors
 * for instance for a 2 vectors of dim 2 :
 * max(V1, V2) = (max(V1[0], v2[0]), max(V1[1], v2[1))
 */
TEMPLATEVECTOR
inline VECTOR maxVector(const VECTOR &u, const VECTOR &v) {
  VECTOR tmp;

  for (size_t i = 0; i < SIZE; ++i) {
    tmp[i] = std::max(u[i], v[i]);
  }

  return tmp;
}

/**
 * compute the minimum/maximum of each dimension of the two vectors
 */
TEMPLATEVECTOR
inline void minMaxVectors(const VECTOR &u, const VECTOR &v, VECTOR &min, VECTOR &max) {
  for (size_t i = 0; i < SIZE; ++i) {
    if (u[i] > v[i]) {
      max[i] = u[i];
      min[i] = v[i];
    } else {
      min[i] = u[i];
      max[i] = v[i];
    }
  }
}

TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator*(const VECTOR &, const VECTOR &);
TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator*(const TYPE, const VECTOR &);
TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator*(const VECTOR &, const TYPE);

TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator+(const VECTOR &, const VECTOR &);
TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator+(const VECTOR &, const TYPE);

TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator-(const VECTOR &, const VECTOR &);
TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator-(const VECTOR &, const TYPE);

TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator/(const VECTOR &, const VECTOR &);
TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator/(const VECTOR &, const TYPE);

TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator^(const VECTOR &, const VECTOR &);
TEMPLATEVECTOR
extern TLP_TEMPLATE_DECLARE_SCOPE VECTOR operator-(const VECTOR &);
/**
 * @brief typedef for 2D vector of uint
 */
typedef Vector<uint, 2> Vec2ui;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<uint, 2>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 3D vector of uint
 */
typedef Vector<uint, 3> Vec3ui;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<uint, 3>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 4D vector of uint
 */
typedef Vector<uint, 4> Vec4ui;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<uint, 4>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 2D vector of int
 */
typedef Vector<int, 2> Vec2i;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<int, 2>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 3D vector of int
 */
typedef Vector<int, 3> Vec3i;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<int, 3>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 4D vector of int
 */
typedef Vector<int, 4> Vec4i;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<int, 4>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 2D vector of double
 */
typedef Vector<double, 2, long double> Vec2d;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 2, long double>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 1, double>), TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 2, double>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 3D vector of double
 */
typedef Vector<double, 3, long double> Vec3d;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 3, long double>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 3, double>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 4D vector of double
 */
typedef Vector<double, 4, long double> Vec4d;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 4, long double>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 4, double>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 5D vector of double
 */
typedef Vector<double, 5, long double> Vec5d;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 5, long double>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<double, 5, double>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 2D vector of float
 */
typedef Vector<float, 2, double> Vec2f;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<float, 1, double>), TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<float, 2, double>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 3D vector of float
 */
typedef Vector<float, 3, double> Vec3f;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<float, 3, double>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 4D vector of float
 */
typedef Vector<float, 4, double> Vec4f;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<float, 4, double>), TLP_TEMPLATE_DECLARE_SCOPE)
/**
 * @brief typedef for 5D vector of float
 */
typedef Vector<float, 5, double> Vec5f;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<float, 5, double>), TLP_TEMPLATE_DECLARE_SCOPE)

DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(Vector<unsigned char, 4>), TLP_TEMPLATE_DECLARE_SCOPE)
}

#ifdef _MSC_VER
static double sqrt(tlp::Vector<float, 5> &v) {
  return sqrt((double)v[0]);
}
#endif

namespace std {
TEMPLATEVECTOR
size_t hash_vector(const tlp::VECTOR &v) {
  size_t seed = 0;

  for (size_t i = 0; i < SIZE; ++i) {
    tlp_hash_combine(seed, v[i]);
  }

  return seed;
}

template <>
struct hash<tlp::Vec2ui> {
  std::size_t operator()(const tlp::Vec2ui &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec3ui> {
  std::size_t operator()(const tlp::Vec3ui &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec4ui> {
  std::size_t operator()(const tlp::Vec4ui &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec2i> {
  std::size_t operator()(const tlp::Vec2i &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec3i> {
  std::size_t operator()(const tlp::Vec3i &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec4i> {
  std::size_t operator()(const tlp::Vec4i &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec2d> {
  std::size_t operator()(const tlp::Vec2d &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec3d> {
  std::size_t operator()(const tlp::Vec3f &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec4d> {
  std::size_t operator()(const tlp::Vec4d &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec2f> {
  std::size_t operator()(const tlp::Vec2f &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec3f> {
  std::size_t operator()(const tlp::Vec3f &v) const {
    return hash_vector(v);
  }
};

template <>
struct hash<tlp::Vec4f> {
  std::size_t operator()(const tlp::Vec4f &v) const {
    return hash_vector(v);
  }
};
} // namespace std

#ifdef DLL_TALIPOT
#include "cxx/Vector.cxx"
#endif

// fix for bug #3598871: allow use of VECTOR keyword in other software
#undef VECTOR
#undef TEMPLATEVECTOR

#endif // TALIPOT_VECTOR_H
