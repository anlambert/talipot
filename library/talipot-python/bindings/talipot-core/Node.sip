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

namespace tlp {
class node {
%Docstring
An instance of this class represent a node in a Talipot graph. It encapsulates an integer identifier.
Use specific methods in the :class:`tlp.Graph` class to get references on such objects (
for instance :meth:`tlp.Graph.addNode`, :meth:`tlp.Graph.getNodes`, :meth:`tlp.Graph.getOutNodes`, ...).
Node objects can be used as keys in a Python dictionary.

The node identifier can be accessed through the **id** class member.
%End

%TypeHeaderCode
#include <talipot/Node.h>
%End

public:
  uint id;
  node();
  node(uint);
  bool __eq__(const tlp::node);
  bool __ne__(const tlp::node);
  bool isValid();
%Docstring
tlp.node.isValid()

Returns :const:`True` if the node is a valid one, meaning it is attached to a graph.

:rtype: boolean
%End

  std::string __repr__() const;
%MethodCode
  std::ostringstream oss;
  oss << "<node " << sipCpp->id << ">";
  sipRes = new std::string(oss.str());
%End

  Py_hash_t __hash__() const;
%MethodCode
  return static_cast<long>(sipCpp->id);
%End

  SIP_PYOBJECT __copy__() const;
%MethodCode
  sipRes = sipConvertFromNewType(new tlp::node(sipCpp->id), sipFindType("tlp::node"), NULL);
%End

  SIP_PYOBJECT __deepcopy__(SIP_PYOBJECT) const;
%MethodCode
  sipRes = sipConvertFromNewType(new tlp::node(sipCpp->id), sipFindType("tlp::node"), NULL);
%End

};
};
