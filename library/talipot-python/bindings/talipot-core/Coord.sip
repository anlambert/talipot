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

class Coord : tlp::Vec3f /TypeHint="tlp.Coord"/ {
%TypeHeaderCode
#include <talipot/Coord.h>
%End

%ConvertToTypeCode

if (sipIsErr == NULL) {
  if (sipCanConvertToType(sipPy, sipFindType("tlp::Coord"), SIP_NOT_NONE | SIP_NO_CONVERTORS)) {
    return 1;
  }
  if (isTupleAndCanConvertToVec3fType(sipPy)) {
    return 1;
  }
  return 0;
}

int state = 0, err = 0;

if (sipCanConvertToType(sipPy, sipFindType("tlp::Coord"), SIP_NOT_NONE | SIP_NO_CONVERTORS)) {
  const sipTypeDef *sipType = sipFindType("tlp::Coord");
  tlp::Coord *ps = static_cast<tlp::Coord *>(
      sipConvertToType(sipPy, sipType, NULL, SIP_NOT_NONE | SIP_NO_CONVERTORS, &state, &err));
  *sipCppPtr = new tlp::Coord(*ps);
  sipReleaseType(ps, sipType, state);
} else {
  *sipCppPtr = convertToVec3fType<tlp::Coord>(sipPy, 1);
}
return sipGetState(sipTransferObj);
%End

public:

  Coord(float x = 0, float y = 0, float z = 0);

  Coord(const Vec3f &v);

  void setX(float x);
%Docstring
tlp.Coord.setX(x)

Convenient method. Sets the first component of the vector.

:param x:
   a value

:type x:
   float
%End

  void setY(float y);
%Docstring
tlp.Coord.setY(y)

Convenient method. Sets the second component of the vector.

:param y:
   a value

:type y:
   float
%End

  void setZ(float z);
%Docstring
tlp.Coord.setZ(z)

Convenient method. Sets the third component of the vector.

:param z:
   a value

:type z:
   float
%End

  float getX() const;
%Docstring
tlp.Coord.getX()

Convenient method. Returns the first component of the vector.

:rtype:
   float
%End

  float getY() const;
%Docstring
tlp.Coord.getY()

Convenient method. Returns the second component of the vector.

:rtype:
   float
%End

  float getZ() const;
%Docstring
tlp.Coord.getZ()

Convenient method. Returns the third component of the vector.

:rtype:
   float
%End

  float x() const;
%Docstring
tlp.Coord.x()

Convenient method. Returns the first component of the vector.

:rtype:
   float
%End

  float y() const;
%Docstring
tlp.Coord.y()

Convenient method. Returns the second component of the vector.

:rtype:
   float
%End

  float z() const;
%Docstring
tlp.Coord.z()

Convenient method. Returns the third component of the vector.

:rtype:
   float
%End

};

};
