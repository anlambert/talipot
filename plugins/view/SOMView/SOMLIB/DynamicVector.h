/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef DYNAMIC_VECTOR_H
#define DYNAMIC_VECTOR_H

template <class T>
class DynamicVector {
  public:
    DynamicVector() : array(nullptr), size(0) {}

    DynamicVector(const DynamicVector<T> &vect) {
        array = new T[vect.size];
        size = vect.size;

        for (unsigned int i = 0; i < vect.size; ++i) {
            array[i] = vect.array[i];
        }
    }

    DynamicVector(unsigned int size) : size(size) {
        array = new T[size];
    }
    virtual ~DynamicVector() {
        if (size != 0) {
            delete[] array;
        }
    }

    DynamicVector<T> &operator=(const DynamicVector<T> &);

    T operator[](const unsigned int i) const;
    T &operator[](const unsigned int i);

    DynamicVector<T> &operator*=(const T &);
    DynamicVector<T> &operator*=(const DynamicVector<T> &);
    DynamicVector<T> &operator/=(const T &);
    DynamicVector<T> &operator/=(const DynamicVector<T> &);
    DynamicVector<T> &operator+=(const T &);
    DynamicVector<T> &operator+=(const DynamicVector<T> &);
    DynamicVector<T> &operator-=(const T &);
    DynamicVector<T> &operator-=(const DynamicVector<T> &);
    DynamicVector<T> &operator^=(const DynamicVector<T> &);

    DynamicVector<T> operator+(const DynamicVector<T> &) const;
    DynamicVector<T> operator+(const T &) const;
    DynamicVector<T> operator-(const DynamicVector<T> &) const;
    DynamicVector<T> operator-(const T &) const;

    DynamicVector<T> operator/(const DynamicVector<T> &) const;
    DynamicVector<T> operator/(const T &) const;
    DynamicVector<T> operator^(const DynamicVector<T> &) const;

    bool operator!=(const DynamicVector<T> &) const;
    bool operator==(const DynamicVector<T> &) const;
    DynamicVector<T> &fill(const T &val);
    T norm() const;
    T dist(const DynamicVector<T> &) const;
    T dotProduct(const DynamicVector<T> &) const;

    unsigned int getSize() const {
        return size;
    }

  protected:
    T *array;
    unsigned int size;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const DynamicVector<T> &);
template <class T>
std::istream &operator>>(std::istream &is, DynamicVector<T> &);

#include "DynamicVector.cxx"
#endif // DYNAMIC_VECTOR_H
