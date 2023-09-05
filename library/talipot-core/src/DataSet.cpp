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

#include <talipot/BooleanProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/GraphProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>

using namespace std;
using namespace tlp;

bool DataType::isTalipotProperty(const std::string &typeName) {
#define ISPROP(T) typeName == typeid(T).name()
  return (ISPROP(tlp::BooleanProperty *) || ISPROP(tlp::BooleanVectorProperty *) ||
          ISPROP(tlp::DoubleProperty *) || ISPROP(tlp::DoubleVectorProperty *) ||
          ISPROP(tlp::LayoutProperty *) || ISPROP(tlp::CoordVectorProperty *) ||
          ISPROP(tlp::StringProperty *) || ISPROP(tlp::StringVectorProperty *) ||
          ISPROP(tlp::IntegerProperty *) || ISPROP(tlp::IntegerVectorProperty *) ||
          ISPROP(tlp::SizeProperty *) || ISPROP(tlp::SizeVectorProperty *) ||
          ISPROP(tlp::ColorProperty *) || ISPROP(tlp::ColorVectorProperty *) ||
          ISPROP(tlp::NumericProperty *) || ISPROP(tlp::PropertyInterface *) ||
          ISPROP(tlp::GraphProperty *) || ISPROP(tlp::BooleanProperty) ||
          ISPROP(tlp::BooleanVectorProperty) || ISPROP(tlp::DoubleProperty) ||
          ISPROP(tlp::DoubleVectorProperty) || ISPROP(tlp::LayoutProperty) ||
          ISPROP(tlp::CoordVectorProperty) || ISPROP(tlp::StringProperty) ||
          ISPROP(tlp::StringVectorProperty) || ISPROP(tlp::IntegerProperty) ||
          ISPROP(tlp::IntegerVectorProperty) || ISPROP(tlp::SizeProperty) ||
          ISPROP(tlp::SizeVectorProperty) || ISPROP(tlp::ColorProperty) ||
          ISPROP(tlp::ColorVectorProperty) || ISPROP(tlp::NumericProperty) ||
          ISPROP(tlp::PropertyInterface) || ISPROP(tlp::GraphProperty));
}

DataSet::DataSet(const DataSet &set) {
  *this = set;
}

DataSet &DataSet::operator=(const DataSet &set) {
  if (this != &set) {
    clear();
    for (const auto &[key, value] : set.data) {
      data[key] = value->clone();
    }
  }
  return *this;
}

DataSet::~DataSet() {
  clear();
}

bool DataSet::exists(const string &str) const {
  return data.find(str) != data.end();
}

std::string DataSet::getTypeName(const string &str) const {
  if (const auto it = data.find(str); it != data.end()) {
    return it->second->getTypeName();
  }
  return "";
}

void DataSet::remove(const string &str) {
  if (const auto it = data.find(str); it != data.end()) {
    if (it->second) {
      delete it->second;
    }

    data.erase(it);
  }
}

DataType *DataSet::getData(const string &str) const {
  if (const auto it = data.find(str); it != data.end()) {
    return it->second ? it->second->clone() : nullptr;
  }
  return nullptr;
}

void DataSet::setData(const std::string &str, const DataType *value) {
  DataType *val = value ? value->clone() : nullptr;
  if (auto it = data.find(str); it != data.end()) {
    if (it->second) {
      delete it->second;
    }
    it->second = val;
    return;
  }
  data[str] = val;
}

uint DataSet::size() const {
  return uint(data.size());
}

bool DataSet::empty() const {
  return data.empty();
}

Iterator<pair<string, DataType *>> *DataSet::getValues() const {
  return stlMapIterator(data);
}

// management of the serialization
// the 2 hash maps

DataTypeSerializerContainer DataSet::serializerContainer;

// registering of a data type serializer
void DataSet::registerDataTypeSerializer(const std::string &typeName, DataTypeSerializer *dts) {

#ifndef NDEBUG

  if (const auto it = serializerContainer.tnTodts.find(typeName);
      it != serializerContainer.tnTodts.end()) {
    tlp::warning() << "Warning: a data type serializer is already registered for type "
                   << demangleClassName(typeName) << std::endl;
  }

  if (const auto it = serializerContainer.otnTodts.find(dts->outputTypeName);
      it != serializerContainer.otnTodts.end()) {
    tlp::warning() << "Warning: a data type serializer is already registered for read type "
                   << dts->outputTypeName << std::endl;
  }

#endif

  serializerContainer.tnTodts[typeName] = serializerContainer.otnTodts[dts->outputTypeName] = dts;
}

// data write
void DataSet::writeData(std::ostream &os, const std::string &prop, const DataType *dt) const {
  if (const auto it = serializerContainer.tnTodts.find(dt->getTypeName());
      it == serializerContainer.tnTodts.end()) {

    tlp::warning() << "Write error: No data serializer found for type "
                   << demangleClassName(dt->getTypeName()) << std::endl;

    return;
  } else {
    DataTypeSerializer *dts = (*it).second;
    os << '(' << dts->outputTypeName << " \"" << prop << "\" ";
    dts->writeData(os, dt);
    os << ')' << endl;
  }
}

void DataSet::write(std::ostream &os, const DataSet &ds) {
  os << endl;
  // iterate over pair attribute/value
  for (const auto &[key, value] : ds.getValues()) {
    ds.writeData(os, key, value);
  }
}

// data read
bool DataSet::readData(std::istream &is, const std::string &prop,
                       const std::string &outputTypeName) {
  if (auto it = serializerContainer.otnTodts.find(outputTypeName);
      it == serializerContainer.otnTodts.end()) {
    tlp::warning() << "Read error: No data type serializer found for read type " << outputTypeName
                   << std::endl;
    return false;
  } else {

    DataTypeSerializer *dts = (*it).second;
    DataType *dt = dts->readData(is);

    if (dt) {
      // replace any preexisting value associated to prop
      if (auto it = data.find(prop); it != data.end()) {
        if (it->second) {
          delete it->second;
        }
        it->second = dt;
        return true;
      }

      // no preexisting value
      data[prop] = dt;
      return true;
    }

    return false;
  }
}

bool DataSet::read(std::istream &is, DataSet &ds) {
  is.unsetf(ios_base::skipws);

  for (;;) {
    char c;

    if (!(is >> c)) {
      return is.eof();
    }

    if (isspace(c)) {
      continue;
    }

    if (c == ')') {
      // no open paren at the beginning
      // so the close paren must be read by the caller
      is.unget();
      return true;
    }

    if (c == '(') {
      bool ok;

      // skip spaces before output type name
      while ((ok = bool(is >> c)) && isspace(c)) {
      }

      if (!ok) {
        return false;
      }

      string otn;

      // read output type name until next space char
      do {
        otn.push_back(c);
      } while ((ok = bool(is >> c)) && !isspace(c));

      // skip spaces before prop
      while ((ok = bool(is >> c)) && isspace(c)) {
      }

      if (!ok) {
        return false;
      }

      if (c != '"') {
        return false;
      }

      string prop;

      // read prop until next "
      while ((ok = bool(is >> c)) && (c != '"')) {
        prop.push_back(c);
      }

      if (!ok) {
        return false;
      }

      // skip spaces before data type
      while ((ok = bool(is >> c)) && isspace(c)) {
      }

      if (!ok) {
        return false;
      }

      is.unget();

      // read data type
      if (!ds.readData(is, prop, otn)) {
        return false;
      }

      // skip spaces before )
      while ((ok = bool(is >> c)) && isspace(c)) {
      }

      if (!ok) {
        return false;
      }

      if (c != ')') {
        return false;
      }

    } else {
      return false;
    }
  }
}

DataTypeSerializer *DataSet::typenameToSerializer(const std::string &name) {
  if (serializerContainer.tnTodts.count(name) == 0) {
    return nullptr;
  }

  return serializerContainer.tnTodts[name];
}

string DataSet::toString() const {
  stringstream ss;
  for (const auto &[key, value] : getValues()) {
    DataTypeSerializer *serializer = DataSet::typenameToSerializer(value->getTypeName());

    if (serializer) {
      ss << "'" << key << "'=";
      ss << serializer->toString(value);
      ss << " ";
    } else {
      if (value->isTalipotProperty()) {
        PropertyInterface *prop = *(static_cast<PropertyInterface **>(value->value));
        ss << "'" << key << "'=";

        if (prop) {
          ss << '"' << prop->getName() << '"';
        } else {
          ss << "None";
        }

        ss << " ";
      }
    }
  }
  return ss.str();
}

void DataSet::clear() {
  for (const auto &[key, value] : data) {
    delete value;
  }
  data.clear();
}