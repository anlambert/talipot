/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef PIXEL_ORIENTED_MEDIATOR_H
#define PIXEL_ORIENTED_MEDIATOR_H

#include <talipot/Vector.h>

#include "UniformDeformationScreen.h"
#include "FishEyesScreen.h"
#include "SpiralLayout.h"
#include "HSIColorMapping.h"
#include "ColorFunction.h"
#include "DimensionBase.h"

#include <map>

class PixelOrientedMediator {

public:
  PixelOrientedMediator(LayoutFunction *layout = new SpiralLayout(),
                        ColorFunction *color = new HSIColorMapping());

  ~PixelOrientedMediator();

  void setLayoutFunction(LayoutFunction *layout) {
    this->layout = layout;
  }
  LayoutFunction *getLayoutFunction() {
    return layout;
  }

  void setColorFunction(ColorFunction *color) {
    this->color = color;
  }
  ColorFunction *getColorFunction() {
    return color;
  }

  void setImageSize(unsigned int width, unsigned int height) {
    imageSize[0] = width;
    imageSize[1] = height;
  }
  unsigned int getImageWidth() const {
    return imageSize[0];
  }
  unsigned int getImageHeight() const {
    return imageSize[1];
  }

  void changeZoom(int i);

  void backupScreenFunctionsParameters();
  void setScreenFunctionsParameters(double zoom, double translationX, double translationY,
                                    double fishEyeRadius);
  void restoreScreenFunctionsParameters();

  unsigned int getCenterItem() const {
    return centerItem;
  }
  void setCenterItem(const unsigned int centerItem) {
    this->centerItem = centerItem;
  }

  void setLastMousePosition(const int x, const int y);
  void updateFishEyePosition(const int x, const int y, DimensionBase *data);
  void translateFishEye(const int x, const int y);
  tlp::Vec2i getFishEyeTranslationVector() const {
    return fishTranslation;
  }
  tlp::Vec2f getFishEyeCenter() const {
    return fishCenter;
  }

  tlp::Color getColorForPixelAtPos(tlp::Vec2i pos, DimensionBase *data, bool withFishEye = false);
  unsigned int getRankForPixelPos(tlp::Vec2i pos);
  tlp::Vec2i getPixelPosForRank(const unsigned int rank);

private:
  tlp::Vec2f screenToScene(const tlp::Vec2i &p);
  tlp::Vec2i sceneToScreen(const tlp::Vec2i &p);

  tlp::Vec2i imageSize;

  LayoutFunction *layout;
  ColorFunction *color;
  FishEyesScreen *trans1;
  UniformDeformationScreen *trans2;

  double zoomBak, translationXBak, translationYBak, fishEyeRadiusBak;

  unsigned int centerItem;

  double zoom;
  tlp::Vec2i lastMousePosition;
  tlp::Vec2i totalMove;
  tlp::Vec2i totalFishMove;
  tlp::Vec2i fishTranslation;
  tlp::Vec2f fishCenter;
};

#endif // PIXEL_ORIENTED_MEDIATOR_H
