/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef ORIENTABLE_COORD_H
#define ORIENTABLE_COORD_H

#include <talipot/Coord.h>
class OrientableLayout;

class OrientableCoord : public tlp::Coord {
  public:
    OrientableCoord(OrientableLayout *fatherParam, const float x = 0, const float y = 0,
                    const float z = 0);
    OrientableCoord(OrientableLayout *fatherParam, const tlp::Coord &v);

    void set(const float x = 0, const float y = 0, const float z = 0);
    void set(const tlp::Coord &v);

    void setX(float x);
    void setY(float y);
    void setZ(float z);

    float getX() const;
    float getY() const;
    float getZ() const;

    void setInvertedX(float x);
    void setInvertedY(float y);
    void setInvertedZ(float z);

    float getInvertedX() const;
    float getInvertedY() const;
    float getInvertedZ() const;

  protected:
    OrientableLayout *father;
};

#endif // ORIENTABLE_COORD_H
