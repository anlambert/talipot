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

namespace tlp {

class StringCollection {
%Docstring

.. deprecated:: deprecated
   You should never have to use that class directly from now on.
   Algorithm parameters of that type are now internally handled for commodity of use.

This class represents a list of selectable string entries that can be used as plugin parameter.
The list will appear as a combo box in the Plugin Parameter Dialog from the Talipot GUI.
%End

%TypeHeaderCode
#include <talipot/StringCollection.h>
%End

%ConvertFromTypeCode
return sipConvertFromNewType(new std::string(sipCpp->getCurrentString()),
                             sipFindType("std::string"), NULL);
%End

public:

// ===========================================================================

  StringCollection();

  StringCollection(const std::vector<std::string> &vectorParam /TypeHint="List[str]"/);

  StringCollection(const std::string param);

  StringCollection(const std::vector<std::string>& vectorParam /TypeHint="List[str]"/, int currentParam);

  StringCollection(const std::vector<std::string>& vectorParam /TypeHint="List[str]"/, std::string currentString);

// ===========================================================================

  std::string getCurrentString();
%Docstring
tlp.StringCollection.getCurrentString()

Returns the current selected string value.

:rtype:
   string
%End

// ===========================================================================

  bool setCurrent(uint param);
%Docstring
tlp.StringCollection.setCurrent(index)

Sets the current selected string index. Returns :const:`True` if the provided index is valid.

:param index:
   a valid index in the string collection

:type index:
   integer

:rtype:
   boolean
%End

// ===========================================================================

  bool setCurrent(std::string param);
%Docstring
tlp.StringCollection.setCurrent(value)

Sets the current selected string value. Returns :const:`True` if the string value exists in the
collection.

:param value:
   a string value from the collection

:type value:
   string

:rtype:
   boolean
%End

// ===========================================================================

  int getCurrent();
%Docstring
tlp.StringCollection.getCurrent()

Returns the index of the current selected string.

:rtype:
   integer
%End

// ===========================================================================

  void push_back(const std::string& element);
%Docstring
tlp.StringCollection.push_back(element)

Adds a string value to this string collection.

:param element:
   the string to add to the collection

:type element:
   string
%End

// ===========================================================================

  bool empty();
%Docstring
tlp.StringCollection.empty()

Returns :const:`True` if the collection is empty.

:rtype:
   boolean
%End

// ===========================================================================

  std::string at(const uint index);
%Docstring
tlp.StringCollection.at(index)

Returns the string element at a certain index in the collection.

:param index:
   a valid index in the collection

:type index:
   integer

:rtype:
   string
%End

// ===========================================================================

  uint size();
%Docstring
tlp.StringCollection.size()

Returns the number of strings in the collection.

:rtype:
   integer
%End

// ===========================================================================

  std::string operator[](const uint i) const;
%MethodCode
if (a0 < sipCpp->size()) {
  sipRes = new std::string((*sipCpp)[a0]);
} else {
  sipIsErr = -1;
  PyErr_SetNone(PyExc_IndexError);
}
%End

  void __setitem__(int i, const std::string &value);
%MethodCode
(*sipCpp)[a0] = *a1;
%End

};
};

