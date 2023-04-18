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

class edge {
%Docstring
An instance of this class represent an edge in a Talipot graph. It encapsulates an integer
identifier. Use specific methods in the :class:`tlp.Graph` class to get references on such objects (
for instance :meth:`tlp.Graph.addEdge`, :meth:`tlp.Graph.getEdges`, :meth:`tlp.Graph.getInEdges`,
...). Edge objects can be used as keys in a Python dictionary.

The edge identifier can be accessed through the **id** class member.
%End

%TypeHeaderCode
#include <talipot/config.h>
#include <talipot/Edge.h>
%End

public:

  uint id;

  edge();
  edge(uint);

  bool __eq__(const tlp::edge) const;

  bool __ne__(const tlp::edge) const;

  bool isValid() const;
%Docstring
tlp.edge.isValid()

Returns :const:`True` if the edge is a valid one, meaning it is attached to a graph.

:rtype:
   boolean
%End

  std::string __repr__() const;
%MethodCode
std::ostringstream oss;
oss << "<edge " << sipCpp->id << ">";
sipRes = new std::string(oss.str());
%End

  Py_hash_t __hash__() const;
%MethodCode
return static_cast<long>(sipCpp->id);
%End

  SIP_PYOBJECT __copy__() const;
%MethodCode
sipRes = sipConvertFromNewType(new tlp::edge(sipCpp->id), sipFindType("tlp::edge"), NULL);
%End

  SIP_PYOBJECT __deepcopy__(SIP_PYOBJECT) const;
%MethodCode
sipRes = sipConvertFromNewType(new tlp::edge(sipCpp->id), sipFindType("tlp::edge"), NULL);
%End
};
};
