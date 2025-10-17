/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_FONT_AWESOME_H
#define TALIPOT_FONT_AWESOME_H

#include <talipot/config.h>

#include <vector>

namespace tlp {

/**
 * @brief Helper class for the configuration of a Font Awesome glyph.
 *
 * Font Awesome is a free iconic font by Dave Gandy (see https://fontawesome.com)
 * offering more than 500 customizable scalable vector icons.
 *
 * A glyph has been added to Talipot enabling to use these great icons
 * as nodes and edges extremities shapes.
 *
 * That class offers utility functions and supported icons names constants.
 *
 * To set a node glyph as a Font Awesome icon, the Icon glyph must be associated
 * to the node through the modification of the "viewShape" integer property attached to the graph.
 * The name of the icon to use must then be set in the "viewIcon" string property.
 * As an example, the following code snippet activates the Font Awesome glyph for all nodes
 * and sets the "user" icon.
 *
 * @code
 *    // graph is a pointer to a tlp::Graph object
 *    tlp::IntegerProperty *viewShape = graph->getIntegerProperty("viewShape");
 *    tlp::StringProperty *viewIcon = graph->getStringProperty("viewIcon");
 *
 *    // sets the Icon glyph on all nodes
 *    viewShape->setAllNodeValue(tlp::NodeShape::Icon);
 *    // sets the "user" glyph for all nodes
 *    viewIcon->setAllNodeValue(tlp::FontAwesome::Solid::User);
 * @endcode
 **/

class TLP_SCOPE FontAwesome {

public:
  /**
   * Returns the version of the Font Awesome icons bundled with Talipot
   */
  static std::string getVersion();

  /**
   * Returns the location of the Font Awesome .ttf file bundled with Talipot
   */
  static std::string getTTFLocation(const std::string &iconName);

  /**
   * Returns the location of the Font Awesome .woff2 file bundled with Talipot
   */
  static std::string getWOFF2Location(const std::string &iconName);

  /**
   * Returns the list of supported Font Awesome icons names
   */
  static const std::vector<std::string> &getSupportedIcons();

  /**
   * Checks if the provided Font Awesome icon name is supported
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
   * @param iconName the name of the icon
   */
  static std::string getIconFamily(const std::string &iconName);

  /**
   * Returns an UTF-8 encoded string of a Font Awesome icon
   * @param iconName a Font Awesome icon name
   * @return
   */
  static std::string getIconUtf8String(const std::string &iconName);

#include "FontAwesomeConstants.h"
};
}

#endif // TALIPOT_FONT_AWESOME_H
