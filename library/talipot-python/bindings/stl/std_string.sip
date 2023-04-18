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

%MappedType std::string /TypeHint="str"/
{
%TypeHeaderCode
// Include the library interface to the type being mapped.
#include <string>
%End

%ConvertToTypeCode
if (sipIsErr == NULL) {
  return PyUnicode_Check(sipPy);
}

if (sipPy == Py_None) {
  *sipCppPtr = new std::string;
  return 1;
}
*sipCppPtr = new std::string(convertPythonUnicodeObjectToStdString(sipPy));
return 1;
%End

%ConvertFromTypeCode
return PyUnicode_FromString(sipCpp->c_str());
%End
};
