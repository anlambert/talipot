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

#include "GradientManager.h"

using namespace std;
using namespace tlp;

GradientManager::GradientManager() : beginColorRange(0), endColorRange(359), minSVal(0) {}

GradientManager::~GradientManager() {
  cleanAllGradients();
}

void GradientManager::cleanAllGradients() {
  for (const auto &it : colorScaleMap) {
    delete it.second;
  }

  colorScaleMap.clear();
}

void GradientManager::init(const std::vector<std::string> &properties) {
  // Save old map in order to preserve user definition
  cleanAllGradients();

  if (properties.empty()) {
    return;
  }

  int shift = int(floor(double((endColorRange - beginColorRange) / properties.size())));
  pair<Color, Color> newColors;
  newColors.first.setV(255);
  newColors.first.setS(255);
  newColors.second.setV(255);
  newColors.second.setS(255);

  for (uint i = 0; i < properties.size(); ++i) {

    // newColors.second.setS(minSVal);
    newColors.first.setS(255);
    newColors.first.setV(minSVal);
    newColors.first.setH(beginColorRange + (i * shift));

    newColors.second.setS(255);
    newColors.second.setV(255);
    newColors.second.setH(beginColorRange + (i * shift));
    vector<Color> vc;
    vc.push_back(newColors.first);
    vc.push_back(newColors.second);
    colorScaleMap[properties[i]] = new ColorScale();
    colorScaleMap[properties[i]]->setColorScale(vc);
  }
}

ColorScale *GradientManager::getColorScale(const std::string &propertyName) {
  if (colorScaleMap.contains(propertyName)) {
    return colorScaleMap[propertyName];
  } else {
    return nullptr;
  }
}
