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

#ifndef TALIPOT_SERIALIZABLE_TYPE_H
#define TALIPOT_SERIALIZABLE_TYPE_H

#include <set>
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <cfloat>

#include <talipot/TypeInterface.h>

#define FORWARD_TOSTRING(T)                             \
    static std::string toString(const T::RealType &v) { \
        std::ostringstream oss;                         \
        write(oss, v);                                  \
        return oss.str();                               \
    }
#define FORWARD_FROMSTRING(T)                                      \
    static bool fromString(T::RealType &v, const std::string &s) { \
        std::istringstream iss(s);                                 \
        return read(iss, v);                                       \
    }
#define FORWARD_STRING_METHODS(T) FORWARD_FROMSTRING(T) FORWARD_TOSTRING(T)

namespace tlp {
template <typename T>
class TLP_SCOPE SerializableType : public TypeInterface<T> {
  public:
    static void write(std::ostream &oss, const REAL_TYPE(TypeInterface<T>) & v) {
        oss << v;
    }
    static bool read(std::istream &iss, REAL_TYPE(TypeInterface<T>) & v) {
        return bool(iss >> v);
    }
    FORWARD_STRING_METHODS(typename TypeInterface<T>)
};

template <typename ELT_TYPE, typename ELT_READER, int openParen>
class TLP_SCOPE SerializableVectorType : public TypeInterface<std::vector<ELT_TYPE>> {
    static bool readVector(std::istream &is, std::vector<ELT_TYPE> &v, char openChar, char sepChar,
                           char closeChar) {
        v.clear();

        char c = ' ';
        ELT_TYPE val;
        bool firstVal = true;
        bool sepFound = false;

        // go to first non space char
        while ((is >> c) && isspace(c)) {
        }

        if (openChar) {
            if (c != openChar) {
                return false;
            }
        } else {
            is.unget();
        }

        for (;;) {
            if (!(is >> c)) {
                return (!sepFound && !closeChar);
            }

            if (isspace(c)) {
                continue;
            }

            if (c == closeChar) {
                return !(!openChar || sepFound);
            }

            if (c == sepChar) {
                if (firstVal || sepFound) {
                    return false;
                }

                sepFound = true;
            } else {
                if (firstVal || sepFound) {
                    if (openParen && c != '(') {
                        return false;
                    }

                    is.unget();

                    if (!ELT_READER::read(is, val)) {
                        return false;
                    }

                    v.push_back(val);
                    firstVal = false;
                    sepFound = false;
                } else {
                    return false;
                }
            }
        }
    }
    static void writeVector(std::ostream &os, const std::vector<ELT_TYPE> &v) {
        os << '(';

        for (uint i = 0; i < v.size(); i++) {
            if (i) {
                os << ", ";
            }

            os << v[i];
        }

        os << ')';
    }

  public:
    static void write(std::ostream &oss,
                      const REAL_TYPE(TypeInterface<std::vector<ELT_TYPE>>) & v) {
        writeVector(oss, v);
    }
    static void writeb(std::ostream &oss,
                       const REAL_TYPE(TypeInterface<std::vector<ELT_TYPE>>) & v) {
        uint vSize = v.size();
        oss.write(reinterpret_cast<const char *>(&vSize), sizeof(vSize));
        oss.write(reinterpret_cast<const char *>(v.data()), vSize * sizeof(ELT_TYPE));
    }
    static bool read(std::istream &iss, REAL_TYPE(TypeInterface<std::vector<ELT_TYPE>>) & v,
                     char openChar = '(', char sepChar = ',', char closeChar = ')') {
        return readVector(iss, v, openChar, sepChar, closeChar);
    }
    static bool readb(std::istream &iss, REAL_TYPE(TypeInterface<std::vector<ELT_TYPE>>) & v) {
        uint vSize;

        if (bool(iss.read(reinterpret_cast<char *>(&vSize), sizeof(vSize)))) {
            v.resize(vSize);
            return bool(iss.read(reinterpret_cast<char *>(v.data()), vSize * sizeof(ELT_TYPE)));
        }
        return false;
    }
    static bool read(const std::vector<std::string> &vs,
                     REAL_TYPE(TypeInterface<std::vector<ELT_TYPE>>) & v) {
        v.clear();
        v.reserve(vs.size());

        for (const std::string &s : vs) {
            ELT_TYPE val;
            std::istringstream is(s);
            if (!ELT_READER::read(is, val)) {
                return false;
            }

            v.push_back(val);
        }
        return true;
    }
    static bool tokenize(const std::string &s, std::vector<std::string> &v, char openChar,
                         char sepChar, char closeChar) {
        v.clear();

        std::istringstream is(s);
        char c = ' ';
        ELT_TYPE val;
        bool firstVal = true;
        bool sepFound = false;

        // go to first non space char
        while ((is >> c) && isspace(c)) {
        }

        if (openChar) {
            if (c != openChar) {
                return false;
            }
        } else {
            is.unget();
        }

        for (;;) {
            if (!(is >> c)) {
                return (!sepFound && !closeChar);
            }

            if (isspace(c)) {
                continue;
            }

            if (c == closeChar) {
                return !(!openChar || sepFound);
            }

            if (c == sepChar) {
                if (firstVal || sepFound) {
                    return false;
                }

                sepFound = true;
            } else {
                if (firstVal || sepFound) {
                    if (openParen && c != '(') {
                        return false;
                    }

                    is.unget();

                    auto pos = is.tellg();
                    if (!ELT_READER::read(is, val)) {
                        return false;
                    }

                    v.push_back(s.substr(pos, is.tellg() - pos)),

                        firstVal = false;
                    sepFound = false;
                } else {
                    return false;
                }
            }
        }
    }
    static uint valueSize() {
        return 0; // means is not fixed
    }
    FORWARD_STRING_METHODS(typename TypeInterface<std::vector<ELT_TYPE>>)
};
}

#endif // TALIPOT_SERIALIZABLE_TYPE_H
