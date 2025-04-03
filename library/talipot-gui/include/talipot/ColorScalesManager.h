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

#ifndef TALIPOT_COLOR_SCALES_MANAGER_H
#define TALIPOT_COLOR_SCALES_MANAGER_H

#include <talipot/ColorScale.h>

#include <list>

namespace tlp {

/**
 * @brief Helper class for handling Tulip color scales
 *
 * That static class aims to facilitate working with Tulip color scales.
 * It allows to easily retrieve Tulip predefined ones but also to register new ones
 * in a persistent database for further reuse.
 *
 */
class TLP_QT_SCOPE ColorScalesManager {

  public:
    /**
     * @brief Returns a list of the registered color scales names.
     *
     */
    static std::list<std::string> getColorScalesList();

    /**
     * @brief Returns the color scale registered with the provided name.
     *
     * @warning If there is no such colorscale, the returned one will be empty.
     *
     */
    static tlp::ColorScale getColorScale(const std::string &colorScaleName);

    /**
     * @brief Registers a color scale in the persistent database for further reuse.
     *
     * @warning If there is already a color scale registered with such a name, it will not be
     * overwritten.
     *
     */
    static void registerColorScale(const std::string &colorScaleName,
                                   const tlp::ColorScale &colorScale);

    /**
     * @brief Removes the color scale registered with the provided name from the persistent
     * database.
     *
     */
    static void removeColorScale(const std::string &colorScaleName);

    static ColorScale getLatestColorScale();
    static void setLatestColorScale(ColorScale &cs);

  private:
    static void getColorScalesFromDir(const std::string &colorScalesDir,
                                      std::list<std::string> &colorScalesList);
    static std::string findColorScaleFile(const std::string &rootDir,
                                          const std::string &colorScaleName);
};
}

#endif // TALIPOT_COLOR_SCALES_MANAGER_H
