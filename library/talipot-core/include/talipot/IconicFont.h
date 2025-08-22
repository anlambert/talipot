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

#ifndef TALIPOT_ICONIC_FONT_H
#define TALIPOT_ICONIC_FONT_H

#include <talipot/config.h>

namespace tlp {

/**
 * @brief Helper class to ease the use of iconic fonts bundled with Talipot
 */
class TLP_SCOPE IconicFont {

public:
  /**
   * Returns the location of the .woff2 font file bundled with Talipot
   * defining the icon with the given name
   * @param iconName the name of the icon
   */
  static std::string getWOFF2Location(const std::string &iconName);

  /**
   * Checks if the provided icon name is supported
   * @param iconName the name of the icon to check support
   */
  static bool isIconSupported(const std::string &iconName);

  /**
   * Returns the Unicode code point associated to an icon name
   * @param iconName the name of the icon to get the codepoint
   */
  static uint getIconCodePoint(const std::string &iconName);

  /**
   * Returns the font family name associated to an icon name
   * @param iconName the icon name
   */
  static std::string getIconFamily(const std::string &iconName);

  /**
   * Returns an UTF-8 encoded string of a Font Awesome icon
   * @param iconName the icon name
   * @return
   */
  static std::string getIconUtf8String(const std::string &iconName);
};
}

#endif // TALIPOT_ICONIC_FONT_H
