/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/WithParameter.h>
#include <talipot/DataSet.h>
#include <talipot/PropertyTypes.h>
#include <talipot/BooleanProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/StlIterator.h>
#include <talipot/ColorScale.h>
#include <talipot/StringCollection.h>
#include <talipot/TlpTools.h>

using namespace tlp;
using namespace std;

#define TYPE_SECTION "type"
#define VALUES_SECTION "values"
#define DEFAULT_SECTION "default"
#define DIRECTION_SECTION "direction"

#define BOOLEAN_TYPE "Boolean"
#define INT_TYPE "integer"
#define UINT_TYPE "unsigned integer"
#define FLOAT_TYPE "floating point number"
#define DOUBLE_TYPE "floating point number (double precision)"
#define STRING_TYPE "string"
#define FILE_PATH_TYPE "file pathname"
#define DIR_PATH_TYPE "directory pathname"

#define IN_DIRECTION "input"
#define OUT_DIRECTION "output"
#define INOUT_DIRECTION "input/output"

static string html_help_def(const string &A, const string &B) {
  return "<tr><td><b>" + A + "</b><td class=\"b\">" + B + "</td></tr>";
}

static string getParameterTypename(const string &name, const string &typeId) {
  if (name.substr(0, 6) == "file::" || name.substr(0, 9) == "anyfile::") {
    return FILE_PATH_TYPE;
  } else if (name.substr(0, 5) == "dir::") {
    return DIR_PATH_TYPE;
  } else if (typeId == typeid(bool).name()) {
    return BOOLEAN_TYPE;
  } else if (typeId == typeid(int).name()) {
    return INT_TYPE;
  } else if (typeId == typeid(unsigned int).name()) {
    return UINT_TYPE;
  } else if (typeId == typeid(float).name()) {
    return FLOAT_TYPE;
  } else if (typeId == typeid(double).name()) {
    return DOUBLE_TYPE;
  } else if (typeId == typeid(string).name()) {
    return STRING_TYPE;
  } else {
    string typeName = demangleTlpClassName(typeId.c_str());

    // remove pointer mark if any
    if (typeName[typeName.size() - 1] == '*') {
      return typeName.substr(0, typeName.size() - 1);
    } else {
      return typeName;
    }
  }
}

string ParameterDescriptionList::generateParameterHTMLDocumentation(
    const string &name, const string &help, const string &type, const string &defaultValue,
    const string &valuesDescription, const ParameterDirection &direction) {

  static string htmlDocheader = HTML_HELP_OPEN();

  // for backward compatibility for external plugins using the old plugin parameters doc system
  if (help.substr(0, htmlDocheader.size()) == htmlDocheader) {
    return help;
  }

  string doc = htmlDocheader;
  doc += html_help_def(TYPE_SECTION, getParameterTypename(name, type));

  if (!valuesDescription.empty()) {
    doc += html_help_def(VALUES_SECTION, valuesDescription);
  }

  if (!defaultValue.empty()) {
    if (type != typeid(tlp::StringCollection).name()) {
      doc += html_help_def(DEFAULT_SECTION, defaultValue);
    } else {
      doc += html_help_def(DEFAULT_SECTION, defaultValue.substr(0, defaultValue.find(";")));
    }
  }

  if (direction == IN_PARAM) {
    doc += html_help_def(DIRECTION_SECTION, IN_DIRECTION);
  } else if (direction == OUT_PARAM) {
    doc += html_help_def(DIRECTION_SECTION, OUT_DIRECTION);
  } else {
    doc += html_help_def(DIRECTION_SECTION, INOUT_DIRECTION);
  }

  if (!help.empty()) {
    doc += HTML_HELP_BODY();
    doc += help;
  }

  doc += HTML_HELP_CLOSE();

  return doc;
}

const ParameterDescriptionList &tlp::WithParameter::getParameters() const {
  return parameters;
}

Iterator<ParameterDescription> *ParameterDescriptionList::getParameters() const {
  return new StlIterator<ParameterDescription, vector<ParameterDescription>::const_iterator>(
      parameters.begin(), parameters.end());
}

ParameterDescription *ParameterDescriptionList::getParameter(const string &name) {
  for (unsigned int i = 0; i < parameters.size(); ++i) {
    if (name == parameters[i].getName())
      return &parameters[i];
  }

#ifndef NDEBUG
  tlp::warning() << __PRETTY_FUNCTION__ << name << " does not exists";
#endif

  return nullptr;
}

const string &ParameterDescriptionList::getDefaultValue(const string &name) const {
  return const_cast<ParameterDescriptionList *>(this)->getParameter(name)->getDefaultValue();
}

void ParameterDescriptionList::setDefaultValue(const string &name, const string &val) {
  getParameter(name)->setDefaultValue(val);
}

void ParameterDescriptionList::setDirection(const string &name, ParameterDirection direction) {
  getParameter(name)->setDirection(direction);
}

bool ParameterDescriptionList::isMandatory(const string &name) const {
  return const_cast<ParameterDescriptionList *>(this)->getParameter(name)->isMandatory();
}

#define CHECK_PROPERTY(T)                                                                          \
  if (type.compare(typeid(T).name()) == 0) {                                                       \
    if (!g || defaultValue.empty() || !g->existProperty(defaultValue))                             \
      dataSet.set(name, static_cast<T *>(nullptr));                                                \
    else                                                                                           \
      dataSet.set(name, static_cast<T *>(g->getProperty<T>(defaultValue)));                        \
    continue;                                                                                      \
  }

void ParameterDescriptionList::buildDefaultDataSet(DataSet &dataSet, Graph *g) const {
  for (const ParameterDescription &param : getParameters()) {
    const string &name = param.getName();
    const string &type = param.getTypeName();
    const string &defaultValue = param.getDefaultValue();

    DataTypeSerializer *dts = DataSet::typenameToSerializer(type);

    if (dts) {
      bool result = dts->setData(dataSet, name, defaultValue);

      if (!result)
        tlp::error() << "Unable to parse \"" << defaultValue.c_str()
                     << "\" as a default value for parameter \"" << name.c_str() << "\"" << endl;

      assert(result);
      continue;
    } else {
      if (type.compare(typeid(tlp::ColorScale).name()) == 0) {
        vector<Color> colors;
        ColorVectorType::fromString(colors, defaultValue);
        dataSet.set(name, ColorScale(colors));
        continue;
      }
    }

    CHECK_PROPERTY(tlp::BooleanProperty);
    CHECK_PROPERTY(tlp::DoubleProperty);
    CHECK_PROPERTY(tlp::LayoutProperty);
    CHECK_PROPERTY(tlp::StringProperty);
    CHECK_PROPERTY(tlp::IntegerProperty);
    CHECK_PROPERTY(tlp::SizeProperty);
    CHECK_PROPERTY(tlp::ColorProperty);
    CHECK_PROPERTY(tlp::BooleanVectorProperty);
    CHECK_PROPERTY(tlp::DoubleVectorProperty);
    CHECK_PROPERTY(tlp::CoordVectorProperty);
    CHECK_PROPERTY(tlp::StringVectorProperty);
    CHECK_PROPERTY(tlp::IntegerVectorProperty);
    CHECK_PROPERTY(tlp::SizeVectorProperty);
    CHECK_PROPERTY(tlp::ColorVectorProperty);

    if (type.compare(typeid(NumericProperty *).name()) == 0) {
      if (!g || defaultValue.empty())
        dataSet.set(name, static_cast<NumericProperty *>(nullptr));
      else {
        PropertyInterface *prop = g->getProperty(defaultValue);

        if (!dynamic_cast<NumericProperty *>(prop)) {
          tlp::error() << "NumericProperty '" << defaultValue.c_str()
                       << "' not found for parameter '" << name.c_str() << endl;
          prop = nullptr;
        }

        dataSet.set(name, static_cast<NumericProperty *>(prop));
      }

      continue;
    }

    if (type.compare(typeid(PropertyInterface *).name()) == 0) {
      if (!g || defaultValue.empty())
        dataSet.set(name, static_cast<PropertyInterface *>(nullptr));
      else {
        if (!g->existProperty(defaultValue)) {
          tlp::error() << "Property '" << defaultValue.c_str() << "' not found for parameter '"
                       << name.c_str() << endl;
          dataSet.set(name, static_cast<PropertyInterface *>(nullptr));
        } else
          dataSet.set(name, g->getProperty(defaultValue));
      }
    }
  }
}

bool WithParameter::inputRequired() const {
  for (const ParameterDescription &param : parameters.getParameters()) {
    if (param.getDirection() != OUT_PARAM)
      return true;

    const string &type = param.getTypeName();

    if (type.compare(typeid(tlp::BooleanProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::ColorProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::DoubleProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::IntegerProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::LayoutProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::SizeProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::StringProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::BooleanVectorProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::ColorVectorProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::DoubleVectorProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::IntegerVectorProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::CoordVectorProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::SizeProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::StringProperty).name()) == 0)
      return true;

    if (type.compare(typeid(tlp::PropertyInterface *).name()) == 0)
      return true;
  }
  return false;
}
