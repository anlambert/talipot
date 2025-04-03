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

#ifndef GRADIENT_MANAGER_H
#define GRADIENT_MANAGER_H

#include <map>
#include <string>
#include <vector>
#include <talipot/Color.h>
#include <talipot/ColorScale.h>

/**
 * @brief Object used to store ColorScale for different properties.
 */

namespace tlp {

class GradientManager {
  public:
    GradientManager();
    virtual ~GradientManager();

    void init(const std::vector<std::string> &properties);

    tlp::ColorScale *getColorScale(const std::string &propertyName);

  protected:
    void cleanAllGradients();

    std::map<std::string, tlp::ColorScale *> colorScaleMap;
    int beginColorRange;
    int endColorRange;
    int minSVal;
};
}
#endif // GRADIENT_MANAGER_H
