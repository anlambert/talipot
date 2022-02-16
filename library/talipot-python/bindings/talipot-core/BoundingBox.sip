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

struct BoundingBox {
%Docstring
This class represents the 3D bounding box of an object.
It is defined by two 3d points, the first one being the lowest point (bottom-left corner),
the second being the highest (top-right corner).

A bounding box can be instantiated and manipulated the following ways::

  # Initialize a non-valid bounding box.
  # The minimum is (1, 1, 1) and the maximum is (-1, -1, -1).
  bb = tlp.BoundingBox()

  # Initialize a bounding box by specifying the lowest and highest point.
  # The validity of the bounding box is checked in debug mode (an assertion is raised if it is not valid).
  bb2 = tlp.BoundingBox(tlp.Coord(0,0,0), tlp.Coord(10,10,10))

  # Reading / writing the bounding box components can be done through the [] operator
  min = bb2[0]
  max = bb2[1]
%End

%TypeHeaderCode
#include <talipot/BoundingBox.h>
%End

  BoundingBox();

  BoundingBox(const tlp::Coord& min, const tlp::Coord& max);

  tlp::Coord center() const;
%Docstring
tlp.BoundingBox.center()

Returns the geometrical center of the bounding box.

:rtype: :class:`tlp.Coord`
%End

  float width() const;
%Docstring
tlp.BoundingBox.width()

Returns the width of the bounding box.

:rtype: float
%End

  float height() const;
%Docstring
tlp.BoundingBox.height()

Returns the height of the bounding box.

:rtype: float
%End

  float depth() const;
%Docstring
tlp.BoundingBox.depth()

Returns the depth of the bounding box.

:rtype: float
%End

  void expand(const tlp::Coord& point);
%Docstring
tlp.BoundingBox.expand(point)

Expands the bounding box to one containing the vector passed as parameter.
If the vector is inside the bounding box, it remains unchanged.

:param point: A point in the 3D space we want the bounding box to encompass.
:type point: :class:`tlp.Coord`
%End

  void translate(const tlp::Coord& displacement);
%Docstring
tlp.BoundingBox.translate(displacement)

Translates the bounding box by the displacement given by the vector passed as parameter.

:param displacement: The displacement vector in 3D space to translate this bounding box by.
:type displacement: :class:`tlp.Coord`
%End

  void scale(const tlp::Coord& factor);
%Docstring
tlp.BoundingBox.scale(factor)

Scales the bounding box, i.e. multiplying its components by a factor vector.

:param factor: The factor vector to scale this bounding box by.
:type factor: :class:`tlp.Coord`
%End

  bool isValid() const;
%Docstring
tlp.BoundingBox.isValid()

Checks whether the bounding box's lowest point is less than it's highest point.
"Less Than" means axis-by-axis comparison, i.e. x1 < x2 && y1 < y2 && z1 < z2.

:rtype: boolean
%End

  bool contains(const tlp::Coord& coord) const;
%Docstring
tlp.BoundingBox.contains(point)

Checks if the given point is inside the current bounding box.
If the bounding box is invalid the result is always :const:`False`.

:param point: a point in the 3D space.
:type point: :class:`tlp.Coord`
:rtype: boolean
%End

  bool intersect(const tlp::BoundingBox &boundingBox) const;
%Docstring
tlp.BoundingBox.intersect(boundingBox)

Checks if the given bounding box intersects the current one.
If one of the bounding boxes is invalid returns :const:`False`.

:param boundingBox: the bounding box on which to check intersection
:type boundingBox: :class:`tlp.BoundingBox`
:rtype: boolean
%End

  bool intersect(const tlp::Coord &segStart, const tlp::Coord &segEnd) const;
%Docstring
tlp.BoundingBox.intersect(segStart, segEnd)

Checks if the bounding box intersects the given line segment.
If the bounding box is invalid the result is always :const:`False`.

:param segStart: the start point of the line segment on which to check intersection
:type segStart: :class:`tlp.Coord`
:param segEnd: the end point of the line segment on which to check intersection
:type segEnd: :class:`tlp.Coord`
:rtype: boolean
%End

  tlp::Coord& operator[](const uint i) const /NoCopy/;
%MethodCode
  if (a0 < 2) {
    sipRes = &(*sipCpp)[a0];
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  void __setitem__(int i, const tlp::Coord &value);
%MethodCode
  if (a0 < 2) {
    (*sipCpp)[a0] = *a1;
  } else {
    sipIsErr = -1;
    PyErr_SetNone(PyExc_IndexError);
  }
%End

  std::string __repr__() const;
%MethodCode
  std::ostringstream oss;
  oss << *sipCpp;
  std::string s = oss.str();
  for (size_t i = 0 ; i < s.length() ; ++i) {
    if (s[i] == '(') {
      s[i] = '[';
    } else if (s[i] == ')') {
      s[i] = ']';
    }
  }
  sipRes = new std::string(s);
%End

};

};
