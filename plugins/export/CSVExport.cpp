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

#include "CSVExport.h"

#include <talipot/StringCollection.h>
#include <talipot/StringProperty.h>
#include <talipot/BooleanProperty.h>

PLUGIN(CsvExport)

using namespace tlp;
using namespace std;

static constexpr std::string_view paramHelp[] = {
    // the type of element to export
    "This parameter enables to choose the type of graph elements to export",
    // export selection
    "This parameter indicates if only selected elements have to be exported",
    // export selection property
    "This parameters enables to choose the property used for the selection",
    // export id of graph elements
    "This parameter indicates if the id of graph elements has to be exported",
    // export visual properties selection
    "This parameter indicates if the visual properties of Talipot will be exported",
    // the field separator
    "This parameter indicates the field separator (sequence of one or more characters used to "
    "specify the boundary between two consecutive fields).",
    // the field separator custom value
    "This parameter allows to indicate a custom field separator. The 'Field separator' parameter "
    "must be set to 'Custom'",
    // the text delimiter
    "This parameter indicates the text delimiter (sequence of one or more characters used to "
    "specify the boundary of value of type text).",
    // the decimal mark
    "This parameter indicates the character used to separate the integer part from the fractional "
    "part of a number written in decimal form.",
};

#define ELT_TYPE "Type of elements"
#define ELT_TYPES "nodes;edges;both"
#define NODE_TYPE 0
#define EdgeType 1
#define BOTH_TYPES 2

#define EXPORT_SELECTION "Export selection"

#define EXPORT_ID "Export id"

#define EXPORT_VISUAL_PROPERTIES "Export visual properties"

#define FIELD_SEPARATOR "Field separator"
#define FIELD_SEPARATORS " \\; ; , ;Tab;Space;Custom"
#define CUSTOM_SEPARATOR 4
#define COMMA_SEPARATOR 1
#define TAB_SEPARATOR 2
#define SPACE_SEPARATOR 3
#define SEMICOLON_SEPARATOR 0
#define FIELD_SEPARATOR_CUSTOM "Custom separator"
#define CUSTOM_MARK ";"

#define STRING_DELIMITER "String delimiter"
#define STRING_DELIMITERS " \" ; ' "
#define DBL_QUOTE_DELIMITER 0
#define QUOTE_DELIMITER 1
#define DECIMAL_MARK "Decimal mark"
#define DECIMAL_MARKS " . ; , "

//================================================================================
CsvExport::CsvExport(const PluginContext *context) : ExportModule(context) {
  addInParameter<StringCollection>(ELT_TYPE, paramHelp[0].data(), ELT_TYPES);
  addInParameter<bool>(EXPORT_SELECTION, paramHelp[1].data(), "false");
  addInParameter<BooleanProperty>("Export selection property", paramHelp[2].data(),
                                  "viewSelection");
  addInParameter<bool>(EXPORT_ID, paramHelp[3].data(), "false");
  addInParameter<bool>(EXPORT_VISUAL_PROPERTIES, paramHelp[4].data(), "false");
  addInParameter<StringCollection>(FIELD_SEPARATOR, paramHelp[5].data(), FIELD_SEPARATORS);
  addInParameter<string>(FIELD_SEPARATOR_CUSTOM, paramHelp[6].data(), CUSTOM_MARK);
  addInParameter<StringCollection>(STRING_DELIMITER, paramHelp[7].data(), STRING_DELIMITERS);
  addInParameter<StringCollection>(DECIMAL_MARK, paramHelp[8].data(), DECIMAL_MARKS);
}

//================================================================================
// define a special facet to force the output
// of a comma as decimal mark
struct decimal_comma : std::numpunct<char> {
  char do_decimal_point() const override {
    return ',';
  }
};

bool CsvExport::exportGraph(std::ostream &os) {
  // initialize parameters with default values
  // only nodes are exported
  StringCollection eltTypes(ELT_TYPES);
  int eltType = 0;
  bool first = true;
  eltTypes.setCurrent(0);
  // export all
  bool exportSelection = false;
  // ids are not exported
  bool exportId = false;
  // export visual properties
  bool exportVisualProperties = false;
  // field separator is Custom
  StringCollection fieldSeparators(FIELD_SEPARATORS);
  fieldSeparators.setCurrent(0);
  // custom field separator is ;
  string fieldSeparatorCustom(CUSTOM_MARK);
  // string delimiter is "
  StringCollection stringDelimiters(STRING_DELIMITERS);
  stringDelimiters.setCurrent(0);
  // decimal mark is .
  StringCollection decimalMarks(DECIMAL_MARKS);
  decimalMarks.setCurrent(0);

  // get chosen values of plugin parameters
  if (dataSet != nullptr) {
    if (dataSet->get(ELT_TYPE, eltTypes)) {
      eltType = eltTypes.getCurrent();
    }

    dataSet->get(EXPORT_SELECTION, exportSelection);
    dataSet->get(EXPORT_ID, exportId);
    dataSet->get(EXPORT_VISUAL_PROPERTIES, exportVisualProperties);
    dataSet->get(FIELD_SEPARATOR_CUSTOM, fieldSeparatorCustom);

    if (dataSet->get(FIELD_SEPARATOR, fieldSeparators)) {
      switch (fieldSeparators.getCurrent()) {
      case COMMA_SEPARATOR:
        fieldSeparator = ',';
        break;

      case TAB_SEPARATOR:
        fieldSeparator = '\t';
        break;

      case SPACE_SEPARATOR:
        fieldSeparator = ' ';
        break;

      case SEMICOLON_SEPARATOR:
        fieldSeparator = ';';
        break;

      default:
        fieldSeparator = fieldSeparatorCustom;
      }
    }

    if (dataSet->get(STRING_DELIMITER, stringDelimiters)) {
      stringDelimiter = stringDelimiters.getCurrent() == DBL_QUOTE_DELIMITER ? '"' : '\'';
    }

    if (dataSet->get(DECIMAL_MARK, decimalMarks)) {
      decimalMark = decimalMarks.getCurrent() ? ',' : '.';
    }
  }

  // export names of fields
  // export ids if needed
  if (exportId) {
    if (eltType != EdgeType) {
      exportString(os, string("node id"));
    }

    if (eltType == BOTH_TYPES) {
      os << fieldSeparator;
    }

    if (eltType != NODE_TYPE) {
      exportString(os, string("src id"));
      os << fieldSeparator;
      exportString(os, string("tgt id"));
    }

    first = false;
  }

  // export non talipot defined properties
  // use vectors for further access to exported properties
  vector<PropertyInterface *> props;
  vector<bool> propIsString;
  uint nbProps = 0;

  for (PropertyInterface *prop : graph->getObjectProperties()) {
    const string &propName = prop->getName();

    if (propName.substr(0, 4) != "view" || exportVisualProperties) {
      ++nbProps;
      props.push_back(prop);
      propIsString.push_back(dynamic_cast<tlp::StringProperty *>(prop));

      if (!first) {
        os << fieldSeparator;
      } else {
        first = false;
      }

      exportString(os, propName);
    }
  }

  os << endl;

  // export nodes
  BooleanProperty *prop = graph->getBooleanProperty("viewSelection");

  if (exportSelection && dataSet != nullptr) {
    dataSet->get("Export selection property", prop);
  }

  // get global locale
  std::locale prevLocale;

  // change decimal point of global locale if needed
  if (decimalMark == ',') {
    std::locale::global(std::locale(prevLocale, new decimal_comma));
  }

  if (eltType != EdgeType) {
    Iterator<node> *it = exportSelection ? prop->getNodesEqualTo(true, graph) : graph->getNodes();

    for (auto n : it) {

      if (exportId) {
        os << n;

        if (eltType == BOTH_TYPES) {
          os << fieldSeparator << fieldSeparator;
        }

        if (nbProps > 0) {
          os << fieldSeparator;
        }
      }

      for (uint i = 0; i < nbProps; ++i) {
        PropertyInterface *prop = props[i];
        string value = prop->getNodeStringValue(n);

        if (!value.empty()) {
          if (propIsString[i]) {
            exportString(os, value);
          } else {
            os << value;
          }
        }

        if (i != nbProps - 1) {
          os << fieldSeparator;
        }
      }

      os << endl;
    }
  }

  // export edges
  if (eltType != NODE_TYPE) {
    Iterator<edge> *it = exportSelection ? prop->getEdgesEqualTo(true, graph) : graph->getEdges();

    for (auto e : it) {

      if (exportId) {
        if (eltType == BOTH_TYPES) {
          os << fieldSeparator;
        }

        const auto &[src, tgt] = graph->ends(e);
        os << src.id << fieldSeparator << tgt.id;

        if (nbProps > 0) {
          os << fieldSeparator;
        }
      }

      for (uint i = 0; i < nbProps; ++i) {
        PropertyInterface *prop = props[i];
        string value = prop->getEdgeStringValue(e);

        if (!value.empty()) {
          if (propIsString[i]) {
            exportString(os, value);
          } else {
            os << value;
          }
        }

        if (i != nbProps - 1) {
          os << fieldSeparator;
        }
      }

      os << endl;
    }
  }

  // restore global locale
  std::locale::global(prevLocale);

  return true;
}
