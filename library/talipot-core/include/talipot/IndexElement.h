/**
 *
 * Copyright (C) 2021-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_INDEX_ELEMENT_H
#define TALIPOT_INDEX_ELEMENT_H

#include <climits>
#include <functional>
#include <iostream>
#include <vector>

#include <talipot/config.h>

#define TLP_INDEX_ELEMENT(element)                                                      \
    struct element {                                                                    \
                                                                                        \
        uint id;                                                                        \
                                                                                        \
        element() : id(UINT_MAX) {}                                                     \
                                                                                        \
        element(uint j) : id(j) {}                                                      \
                                                                                        \
        operator uint() const {                                                         \
            return id;                                                                  \
        }                                                                               \
                                                                                        \
        element &operator=(uint id) {                                                   \
            this->id = id;                                                              \
            return *this;                                                               \
        }                                                                               \
                                                                                        \
        element &operator++() {                                                         \
            ++id;                                                                       \
            return *this;                                                               \
        }                                                                               \
                                                                                        \
        element &operator+=(uint i) {                                                   \
            id += i;                                                                    \
            return *this;                                                               \
        }                                                                               \
                                                                                        \
        element &operator--() {                                                         \
            --id;                                                                       \
            return *this;                                                               \
        }                                                                               \
                                                                                        \
        bool operator!=(const element n) const {                                        \
            return id != n.id;                                                          \
        }                                                                               \
                                                                                        \
        bool operator==(const element n) const {                                        \
            return id == n.id;                                                          \
        }                                                                               \
                                                                                        \
        bool operator==(const uint id) const {                                          \
            return this->id == id;                                                      \
        }                                                                               \
                                                                                        \
        bool isValid() const {                                                          \
            return id != UINT_MAX;                                                      \
        }                                                                               \
    };                                                                                  \
                                                                                        \
    inline std::ostream &operator<<(std::ostream &os, const std::vector<element> &vn) { \
        os << "(";                                                                      \
        for (uint i = 0; i < vn.size(); ++i) {                                          \
            os << #element << "(" << vn[i].id << ")";                                   \
            if (i != vn.size() - 1) {                                                   \
                os << ", ";                                                             \
            }                                                                           \
        }                                                                               \
        os << ")";                                                                      \
        return os;                                                                      \
    }

#define TLP_INDEX_ELEMENT_HASH(element)                                        \
    namespace std {                                                            \
    template <>                                                                \
    struct hash<tlp::element> {                                                \
        size_t operator()(const tlp::element n) const {                        \
            return n.id;                                                       \
        }                                                                      \
    };                                                                         \
    template <>                                                                \
    struct equal_to<tlp::element> {                                            \
        size_t operator()(const tlp::element n, const tlp::element n2) const { \
            return n.id == n2.id;                                              \
        }                                                                      \
    };                                                                         \
    template <>                                                                \
    struct less<tlp::element> {                                                \
        size_t operator()(const tlp::element n, const tlp::element n2) const { \
            return n.id < n2.id;                                               \
        }                                                                      \
    };                                                                         \
    }

#endif // TALIPOT_INDEX_ELEMENT_H
