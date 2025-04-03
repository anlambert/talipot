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

#ifndef ORIENTABLE_SIZE_H
#define ORIENTABLE_SIZE_H

#include <talipot/Size.h>
class OrientableSizeProxy;

class OrientableSize : public tlp::Size {
  public:
    OrientableSize(OrientableSizeProxy *fatherParam, const float width = 0, const float height = 0,
                   const float depth = 0);
    OrientableSize(OrientableSizeProxy *fatherParam, const tlp::Size &size);

    void set(const float width = 0, const float height = 0, const float depth = 0);
    void set(const tlp::Size &size);

    void setW(const float width);
    void setH(const float height);
    void setD(const float depth);

    float getW() const;
    float getH() const;
    float getD() const;

    void get(float *width, float *height, float *depth) const;

  protected:
    OrientableSizeProxy *father;
};

#endif // ORIENTABLE_SIZE_H
