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

#define VECTORTLP tlp::Vector<TYPE, SIZE, OTYPE, DTYPE>

//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector() {
  tlp::Array<TYPE, SIZE>::fill(0);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const tlp::Vector<TYPE, SIZE + 1, OTYPE> &v) {
  set(v);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const TYPE x) {
  Array<TYPE, SIZE>::fill(x);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const TYPE x, const TYPE y) : VECTORTLP() {
  set(x, y);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const TYPE x, const TYPE y, const TYPE z) : VECTORTLP() {
  set(x, y, z);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const tlp::Vector<TYPE, 2, OTYPE> &v, const TYPE z) : VECTORTLP() {
  set(v, z);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const TYPE x, const TYPE y, const TYPE z, const TYPE w) : VECTORTLP() {
  set(x, y, z, w);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const tlp::Vector<TYPE, 2, OTYPE> &v, const TYPE z, const TYPE w) : VECTORTLP() {
  set(v, z, w);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP::Vector(const tlp::Vector<TYPE, 3, OTYPE> &v, const TYPE w) : VECTORTLP() {
  set(v, w);
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const TYPE x) {
  (*this)[0] = x;
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const TYPE x, const TYPE y) {
  assert(SIZE > 1);
  (*this)[0] = x;
  (*this)[1] = y;
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const TYPE x, const TYPE y, const TYPE z) {
  assert(SIZE > 2);
  (*this)[0] = x;
  (*this)[1] = y;
  (*this)[2] = z;
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const TYPE x, const TYPE y, const TYPE z, const TYPE w) {
  assert(SIZE > 3);
  (*this)[0] = x;
  (*this)[1] = y;
  (*this)[2] = z;
  (*this)[3] = w;
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const tlp::Vector<TYPE, 2, OTYPE> &v, const TYPE z) {
  assert(SIZE > 2);
  set(v[0], v[1], z);
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const tlp::Vector<TYPE, 2, OTYPE> &v, const TYPE z, const TYPE w) {
  assert(SIZE > 3);
  set(v[0], v[1], z, w);
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const tlp::Vector<TYPE, 3, OTYPE> &v, const TYPE w) {
  assert(SIZE > 3);
  set(v[0], v[1], v[2], w);
  (*this)[3] = w;
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const VECTORTLP &v) {
  *this = v;
}
//======================================================
TEMPLATEVECTOR
void VECTORTLP::set(const tlp::Vector<TYPE, SIZE + 1, OTYPE> &v) {
  std::copy(v.begin(), v.begin() + SIZE, Array<TYPE, SIZE>::begin());
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::normalize() {
  OTYPE tmp = 0;

  for (size_t i = 0; i < SIZE; ++i) {
    tmp += tlpsqr<TYPE, OTYPE>((*this)[i]);
  }

  if (tmp < sqrt(std::numeric_limits<TYPE>::epsilon())) {
    return *this;
  }

  for (size_t i = 0; i < SIZE; ++i) {
    if ((*this)[i] < 0.) {
      (*this)[i] = -tlpsqrt<TYPE, OTYPE>(tlpsqr<TYPE, OTYPE>((*this)[i]) / tmp);
    } else {
      (*this)[i] = tlpsqrt<TYPE, OTYPE>(tlpsqr<TYPE, OTYPE>((*this)[i]) / tmp);
    }
  }

  return *this;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator*=(const TYPE scalaire) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] *= scalaire;
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator*=(const VECTORTLP &vecto) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] *= vecto[i];
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator/=(const TYPE scalaire) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] /= scalaire;
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator/=(const VECTORTLP &vecto) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] /= vecto[i];
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator+=(const TYPE scalaire) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] += scalaire;
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator+=(const VECTORTLP &vecto) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] += vecto[i];
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator-=(const TYPE scalaire) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] -= scalaire;
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator-=(const VECTORTLP &vecto) {
  for (size_t i = 0; i < SIZE; ++i) {
    (*this)[i] -= vecto[i];
  }

  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP &VECTORTLP::operator^=(const VECTORTLP &v) {
  (*this) = (*this) ^ v;
  return (*this);
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator+(const VECTORTLP &u, const VECTORTLP &v) {
  return VECTORTLP(u) += v;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator+(const VECTORTLP &u, const TYPE scalaire) {
  return VECTORTLP(u) += scalaire;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator-(const VECTORTLP &u, const VECTORTLP &v) {
  return VECTORTLP(u) -= v;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator-(const VECTORTLP &u, const TYPE scalaire) {
  return VECTORTLP(u) -= scalaire;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator*(const VECTORTLP &u, const VECTORTLP &v) {
  return VECTORTLP(u) *= v;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator*(const VECTORTLP &u, const TYPE scalaire) {
  return VECTORTLP(u) *= scalaire;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator*(const TYPE scalaire, const VECTORTLP &u) {
  return VECTORTLP(u) *= scalaire;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator/(const VECTORTLP &u, const VECTORTLP &v) {
  return VECTORTLP(u) /= v;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator/(const VECTORTLP &u, const TYPE scalaire) {
  return VECTORTLP(u) /= scalaire;
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator^(const VECTORTLP &u, const VECTORTLP &v) {

  if constexpr (SIZE == 3) {
    return VECTORTLP(static_cast<TYPE>(static_cast<OTYPE>(u[1]) * static_cast<OTYPE>(v[2]) -
                                       static_cast<OTYPE>(u[2]) * static_cast<OTYPE>(v[1])),
                     static_cast<TYPE>(static_cast<OTYPE>(u[2]) * static_cast<OTYPE>(v[0]) -
                                       static_cast<OTYPE>(u[0]) * static_cast<OTYPE>(v[2])),
                     static_cast<TYPE>(static_cast<OTYPE>(u[0]) * static_cast<OTYPE>(v[1]) -
                                       static_cast<OTYPE>(u[1]) * static_cast<OTYPE>(v[0])));
  } else {
    tlp::warning() << "cross product not implemented for dimension :" << SIZE << std::endl;
    VECTORTLP result;
    return result;
  }
}
//======================================================
TEMPLATEVECTOR
VECTORTLP tlp::operator-(const VECTORTLP &u) {
  return VECTORTLP(u) *= static_cast<TYPE>(-1);
}
//======================================================
TEMPLATEVECTOR
bool VECTORTLP::operator>(const VECTORTLP &vecto) const {
  return vecto < (*this);
}
//======================================================
TEMPLATEVECTOR
bool VECTORTLP::operator<(const VECTORTLP &v) const {
  if (std::numeric_limits<TYPE>::is_integer) {
    return std::memcmp(this->data(), v.data(), SIZE * sizeof(TYPE)) < 0;
  }

  for (size_t i = 0; i < SIZE; ++i) {
    OTYPE tmp = static_cast<OTYPE>((*this)[i]) - static_cast<OTYPE>(v[i]);

    if (tmp > std::sqrt(std::numeric_limits<TYPE>::epsilon()) ||
        tmp < -std::sqrt(std::numeric_limits<TYPE>::epsilon())) {
      if (tmp > 0) {
        return false;
      }

      if (tmp < 0) {
        return true;
      }
    }
  }

  return false;
}
//======================================================
TEMPLATEVECTOR
bool VECTORTLP::operator!=(const VECTORTLP &vecto) const {
  return (!((*this) == vecto));
}
//======================================================
TEMPLATEVECTOR
bool VECTORTLP::operator==(const VECTORTLP &v) const {
  if (std::numeric_limits<TYPE>::is_integer) {
    return std::memcmp(this->data(), v.data(), SIZE * sizeof(TYPE)) == 0;
  }

  for (size_t i = 0; i < SIZE; ++i) {
    OTYPE tmp = static_cast<OTYPE>((*this)[i]) - static_cast<OTYPE>(v[i]);

    if (tmp > std::sqrt(std::numeric_limits<TYPE>::epsilon()) ||
        tmp < -std::sqrt(std::numeric_limits<TYPE>::epsilon())) {
      return false;
    }
  }

  return true;
}
//======================================================
TEMPLATEVECTOR
TYPE VECTORTLP::dotProduct(const VECTORTLP &v) const {
  assert(SIZE > 0);
  OTYPE tmpO = static_cast<OTYPE>((*this)[0]) * static_cast<OTYPE>(v[0]);

  for (size_t i = 1; i < SIZE; ++i) {
    tmpO += static_cast<OTYPE>((*this)[i]) * static_cast<OTYPE>(v[i]);
  }

  return static_cast<TYPE>(tmpO);
}
//======================================================
TEMPLATEVECTOR
TYPE VECTORTLP::norm() const {
  if constexpr (SIZE == 1) {
    return (*this)[0];
  } else if constexpr (SIZE == 2) {
    return tlpsqrt<TYPE, OTYPE>(tlpsqr<TYPE, OTYPE>((*this)[0]) + tlpsqr<TYPE, OTYPE>((*this)[1]));
  } else if constexpr (SIZE == 3) {
    return tlpsqrt<TYPE, OTYPE>(tlpsqr<TYPE, OTYPE>((*this)[0]) + tlpsqr<TYPE, OTYPE>((*this)[1]) +
                                tlpsqr<TYPE, OTYPE>((*this)[2]));
  } else {
    auto tmp = tlpsqr<TYPE, OTYPE>((*this)[0]);
    for (size_t i = 1; i < SIZE; ++i) {
      tmp += tlpsqr<TYPE, OTYPE>((*this)[i]);
    }
    return tlpsqrt<TYPE, OTYPE>(tmp);
  }
}
//======================================================
TEMPLATEVECTOR
DTYPE VECTORTLP::dist(const VECTOR &c) const {
  if constexpr (SIZE == 1) {
    return static_cast<DTYPE>((*this)[0] > c[0] ? (*this)[0] - c[0] : c[0] - (*this)[0]);
  } else if constexpr (SIZE == 2) {
    return tlpsqrt<DTYPE, OTYPE>(tlpsqr<DTYPE, OTYPE>((*this)[0] - c[0]) +
                                 tlpsqr<DTYPE, OTYPE>((*this)[1] - c[1]));

  } else if constexpr (SIZE == 3) {
    return tlpsqrt<DTYPE, OTYPE>(tlpsqr<DTYPE, OTYPE>((*this)[0] - c[0]) +
                                 tlpsqr<DTYPE, OTYPE>((*this)[1] - c[1]) +
                                 tlpsqr<DTYPE, OTYPE>((*this)[2] - c[2]));

  } else {
    auto tmp = tlpsqr<DTYPE, OTYPE>((*this)[0] - c[0]);
    for (size_t i = 1; i < SIZE; ++i) {
      tmp += tlpsqr<DTYPE, OTYPE>((*this)[i] - c[i]);
    }
    return tlpsqrt<DTYPE, OTYPE>(tmp);
  }
}

//======================================================
TEMPLATEVECTOR
TYPE VECTORTLP::length() const {
  return norm();
}

#undef VECTORTLP
