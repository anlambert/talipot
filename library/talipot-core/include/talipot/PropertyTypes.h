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

#ifndef TALIPOT_PROPERTY_TYPES_H
#define TALIPOT_PROPERTY_TYPES_H

#include <set>
#include <vector>
#include <sstream>
#include <talipot/StoredType.h>
#include <talipot/Coord.h>
#include <talipot/Size.h>
#include <talipot/Color.h>
#include <talipot/Edge.h>
#include <talipot/Node.h>
#include <talipot/TypeInterface.h>
#include <talipot/SerializableType.h>
#include <talipot/DataSet.h>

namespace tlp {

class Graph;

class TLP_SCOPE GraphType : public TypeInterface<tlp::Graph *> {
public:
  static RealType undefinedValue();
  static RealType defaultValue();
  static void write(std::ostream &oss, const RealType &v);
  static void writeb(std::ostream &oss, const RealType &v);
  static bool read(std::istream &iss, RealType &v);
  static bool readb(std::istream &iss, RealType &v);
  FORWARD_STRING_METHODS(GraphType)
};

class TLP_SCOPE EdgeSetType : public TypeInterface<std::set<tlp::edge>> {
public:
  static void write(std::ostream &oss, const RealType &v);
  static void writeb(std::ostream &oss, const RealType &v);
  static bool read(std::istream &iss, RealType &v);
  static bool readb(std::istream &iss, RealType &v);
  FORWARD_STRING_METHODS(EdgeSetType)
};

class TLP_SCOPE DoubleType : public TypeInterface<double> {
public:
  static double undefinedValue();
  static double defaultValue();
  static void write(std::ostream &oss, const RealType &v) {
    oss << v;
  }
  static bool read(std::istream &iss, RealType &v);
  FORWARD_STRING_METHODS(DoubleType)
};

class TLP_SCOPE FloatType : public SerializableType<float> {
public:
  static float undefinedValue();
  static float defaultValue();
  static bool read(std::istream &iss, RealType &v);
  FORWARD_STRING_METHODS(FloatType)
};

typedef SerializableVectorType<double, DoubleType, false> DoubleVectorType;

class TLP_SCOPE IntegerType : public SerializableType<int> {
public:
  static int undefinedValue();
  static int defaultValue();
};

class TLP_SCOPE LongType : public SerializableType<long> {
public:
  static long undefinedValue();
  static long defaultValue();
};

class TLP_SCOPE UnsignedIntegerType : public SerializableType<uint> {
public:
  static uint undefinedValue();
  static uint defaultValue();
};

typedef SerializableVectorType<int, IntegerType, false> IntegerVectorType;
typedef SerializableVectorType<uint, UnsignedIntegerType, false> UnsignedIntegerVectorType;

class TLP_SCOPE BooleanType : public TypeInterface<bool> {
public:
  static RealType undefinedValue();
  static RealType defaultValue();
  static void write(std::ostream &, const RealType &);
  static bool read(std::istream &, RealType &, bool untilEnd = false);
  static bool fromString(RealType &, const std::string &, bool untilEnd = false);
  FORWARD_TOSTRING(BooleanType)
};

class TLP_SCOPE BooleanVectorType : public TypeInterface<std::vector<bool>> {
public:
  static void write(std::ostream &, const RealType &);
  static void writeb(std::ostream &, const RealType &);
  static bool read(std::istream &, RealType &, char openChar = '(', char sepChar = ',',
                   char closeChar = ')');
  static bool read(const std::vector<std::string> &vs, RealType &v);
  static bool tokenize(const std::string &, std::vector<std::string> &, char openChar = '(',
                       char sepChar = ',', char closeChar = ')');
  static bool readb(std::istream &, RealType &);
  FORWARD_STRING_METHODS(BooleanVectorType)
};

class TLP_SCOPE PointType : public SerializableType<tlp::Coord> {
public:
  static RealType undefinedValue();
  static RealType defaultValue();
  static bool read(std::istream &, RealType &);
  static bool fromString(RealType &, const std::string &);
  FORWARD_TOSTRING(PointType)
};

class TLP_SCOPE LineType : public SerializableVectorType<tlp::Coord, PointType, false> {
public:
  static bool read(std::istream &, RealType &, char openChar = '(', char sepChar = ',',
                   char closeChar = ')');
  FORWARD_STRING_METHODS(LineType)
};

class TLP_SCOPE SizeType : public SerializableType<tlp::Size> {
public:
  static RealType undefinedValue();
  static RealType defaultValue();
  static bool read(std::istream &, RealType &);
  static bool fromString(RealType &, const std::string &);
  FORWARD_TOSTRING(SizeType)
};

typedef SerializableVectorType<tlp::Size, SizeType, true> SizeVectorType;

class TLP_SCOPE StringType : public TypeInterface<std::string> {
public:
  static RealType undefinedValue();
  static RealType defaultValue();

  static void write(std::ostream &, const RealType &, char openCloseChar = '"');
  static void writeb(std::ostream &, const RealType &);
  static bool read(std::istream &, RealType &, char openChar = '"', char closeChar = '"');
  static bool readb(std::istream &, RealType &);

  static std::string toString(const RealType &v);
  static bool fromString(RealType &v, const std::string &s);
};

class TLP_SCOPE StringVectorType : public TypeInterface<std::vector<std::string>> {
public:
  static void write(std::ostream &, const RealType &);
  static void writeb(std::ostream &oss, const RealType &vStr);
  static bool read(std::istream &, RealType &, char openChar = '(', char sepchar = ',',
                   char closeChar = ')');
  static bool read(const std::vector<std::string> &vs, RealType &v) {
    v.clear();
    v.reserve(vs.size());
    for (const std::string &s : vs) {
      v.push_back(s);
    }

    return true;
  }
  static bool tokenize(const std::string &s, std::vector<std::string> &v, char openChar = '(',
                       char sepChar = ',', char closeChar = ')') {
    std::istringstream is(s);
    return read(is, v, openChar, sepChar, closeChar);
  }
  static bool readb(std::istream &iss, RealType &vStr);
  FORWARD_STRING_METHODS(StringVectorType)
};

class TLP_SCOPE ColorType : public TypeInterface<tlp::Color> {
public:
  static RealType undefinedValue();

  static void write(std::ostream &, const RealType &);
  static bool read(std::istream &, RealType &);

  static std::string toString(const RealType &v);
  static bool fromString(RealType &v, const std::string &s);
};

typedef SerializableVectorType<tlp::Color, ColorType, true> ColorVectorType;
typedef SerializableVectorType<tlp::Coord, PointType, true> CoordVectorType;

DECL_STORED_STRUCT(tlp::ColorType::RealType)
DECL_STORED_STRUCT(tlp::EdgeSetType::RealType)
DECL_STORED_STRUCT(std::set<tlp::node>)
DECL_STORED_STRUCT(tlp::DoubleVectorType::RealType)
DECL_STORED_STRUCT(tlp::IntegerVectorType::RealType)
DECL_STORED_STRUCT(tlp::BooleanVectorType::RealType)
DECL_STORED_STRUCT(tlp::LineType::RealType)
DECL_STORED_STRUCT(tlp::PointType::RealType)
DECL_STORED_STRUCT(tlp::SizeType::RealType)
DECL_STORED_STRUCT(tlp::SizeVectorType::RealType)
DECL_STORED_STRUCT(tlp::StringType::RealType)
DECL_STORED_STRUCT(tlp::StringVectorType::RealType)
DECL_STORED_STRUCT(tlp::ColorVectorType::RealType)

// template class to automate definition of serializers
template <typename T>
struct KnownTypeSerializer : public TypedDataSerializer<REAL_TYPE(T)> {
  KnownTypeSerializer(const std::string &otn) : TypedDataSerializer<REAL_TYPE(T)>(otn) {}
  KnownTypeSerializer(const char *otn) : TypedDataSerializer<REAL_TYPE(T)>(std::string(otn)) {}

  DataTypeSerializer *clone() const override {
    return new KnownTypeSerializer<T>(this->outputTypeName);
  }

  void write(std::ostream &os, const REAL_TYPE(T) & v) override {
    T::write(os, v);
  }
  bool read(std::istream &iss, REAL_TYPE(T) & v) override {
    return T::read(iss, v);
  }
  bool setData(tlp::DataSet &ds, const std::string &prop, const std::string &value) override {
    bool result = true;
    REAL_TYPE(T) val;

    if (value.empty()) {
      val = T::defaultValue();
    } else {
      result = T::fromString(val, value);
    }

    ds.set(prop, val);
    return result;
  }
};

extern void initTypeSerializers();
#ifdef _MSC_VER
template class SerializableType<double>;
template class SerializableType<float>;
template class SerializableType<int>;
template class SerializableType<long>;
template class SerializableType<uint>;
template class SerializableType<tlp::Coord>;
template class SerializableType<tlp::Size>;
template class SerializableVectorType<double, DoubleType, false>;
template class SerializableVectorType<int, IntegerType, false>;
template class SerializableVectorType<uint, UnsignedIntegerType, false>;
template class SerializableVectorType<tlp::Coord, PointType, false>;
template class SerializableVectorType<tlp::Size, SizeType, true>;
template class SerializableVectorType<tlp::Color, ColorType, true>;
template class SerializableVectorType<tlp::Coord, PointType, true>;
template class TypeInterface<tlp::Graph *>;
template class TypeInterface<std::set<tlp::edge>>;
template class TypeInterface<bool>;
template class TypeInterface<std::vector<bool>>;
template class TypeInterface<std::vector<std::string>>;
template class TypeInterface<tlp::Color>;
template class TypeInterface<std::string>;
#endif
}

#endif // TALIPOT_PROPERTY_TYPES_H
