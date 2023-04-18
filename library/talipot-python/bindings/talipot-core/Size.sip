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

class Size : tlp::Vec3f /TypeHint="tlp.Size"/ {
%TypeHeaderCode
#include <talipot/Size.h>
%End

%ConvertToTypeCode

if (sipIsErr == NULL) {
  if (sipCanConvertToType(sipPy, sipFindType("tlp::Size"), SIP_NOT_NONE | SIP_NO_CONVERTORS)) {
    return 1;
  }
  if (isTupleAndCanConvertToVec3fType(sipPy)) {
    return 1;
  }
  return 0;
}

int state = 0, err = 0;

if (sipCanConvertToType(sipPy, sipFindType("tlp::Size"), SIP_NOT_NONE | SIP_NO_CONVERTORS)) {
  const sipTypeDef *sipType = sipFindType("tlp::Size");
  tlp::Size *ps = static_cast<tlp::Size *>(
      sipConvertToType(sipPy, sipType, NULL, SIP_NOT_NONE | SIP_NO_CONVERTORS, &state, &err));
  *sipCppPtr = new tlp::Size(*ps);
  sipReleaseType(ps, sipType, state);
} else {
  *sipCppPtr = convertToVec3fType<tlp::Size>(sipPy, 1);
}

return sipGetState(sipTransferObj);
%End


 public:

  Size(float w = 1, float h = 1, float d = 1);

  Size(const tlp::Vec3f &s);

  void setW(const float width);
%Docstring
tlp.Vec3f.setW(width)

Convenient method. Sets the first component of the vector.

:param width:
   a value

:type width:
   float
%End

  void setH(const float height);
%Docstring
tlp.Vec3f.setH(height)

Convenient method. Sets the second component of the vector.

:param height:
   a value

:type height:
   float
%End

  void setD(const float depth);
%Docstring
tlp.Vec3f.setD(depth)

Convenient method. Sets the third component of the vector.

:param depth:
   a value

:type depth:
   float
%End

  float getW() const;
%Docstring
tlp.Vec3f.getW()

Convenient method. Returns the first component of the vector.

:rtype:
   float
%End

  float getH() const;
%Docstring
tlp.Vec3f.getH()

Convenient method. Returns the second component of the vector.

:rtype:
   float
%End

  float getD() const;
%Docstring
tlp.Vec3f.getD()

Convenient method. Returns the third component of the vector.

:rtype:
   float
%End

  float width() const;
%Docstring
tlp.Vec3f.width()

Convenient method. Returns the first component of the vector.

:rtype:
   float
%End

  float height() const;
%Docstring
tlp.Vec3f.height()

Convenient method. Returns the second component of the vector.

:rtype:
   float
%End

  float depth() const;
%Docstring
tlp.Vec3f.depth()

Convenient method. Returns the third component of the vector.

:rtype:
   float
%End

};

};

