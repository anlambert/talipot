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

%ModuleHeaderCode
#include <string>

extern std::string convertPythonUnicodeObjectToStdString(PyObject *pyUnicodeObj);
%End

%ModuleCode
#include <string>

std::string convertPythonUnicodeObjectToStdString(PyObject *pyUnicodeObj) {
  PyObject *utf8Str = PyUnicode_AsUTF8String(pyUnicodeObj);
  std::string ret(PyBytes_AsString(utf8Str));
  Py_DECREF(utf8Str);
  return ret;
}
%End

%ExportedHeaderCode
#define getSipType(sipTypename) (sipResolveTypedef(sipTypename) ? sipFindType(sipResolveTypedef(sipTypename)) : sipFindType(sipTypename))
%End

%Include std_list.sip
%Include std_pair.sip
%Include std_set.sip
%Include std_string.sip
%Include std_vector.sip
%Include std_map.sip
%Include std_ostream.sip

typedef std::vector<std::string> vectorStringType;
typedef std::list<std::string> listStringType;
typedef std::set<std::string> setStringType;
