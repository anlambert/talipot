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

#ifndef TALIPOT_MATERIAL_DESIGN_ICONS_H
#define TALIPOT_MATERIAL_DESIGN_ICONS_H

#include <talipot/config.h>

#include <vector>

#undef linux

namespace tlp {

/**
 * @brief Helper class for the configuration of a Material Design Icon glyph.
 *
 * Material Design Icons (https://materialdesignicons.com/) :
 * a growing icon collection allows designers and developers
 * targeting various platforms to download icons in the format,
 * color and size they need for any project.
 *
 * A glyph has been added to Talipot enabling to use these great icons
 * as nodes and edges extremities shapes.
 *
 * That class offers utility functions and supported icons names constants.
 *
 * To set a node glyph as a Material Design icon, the Icon glyph must be associated
 * to the node through the modification of the "viewShape" integer property attached to the graph.
 * The name of the icon to use must then be set in the "viewIcon" string property.
 * As an example, the following code snippet activates the Material Design glyph for all nodes
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
 *    viewIcon->setAllNodeValue(tlp::MaterialDesignIcons::User);
 * @endcode
 **/

class TLP_SCOPE MaterialDesignIcons {

public:
  /**
   * Returns the current Material Design Icons version
   */
  static std::string getVersion();

  /**
   * Returns the location of the Material Design Icons .woff2 file bundled with Talipot
   */
  static std::string getWOFF2Location();

  /**
   * Returns the list of supported Material Design icons names
   */
  static const std::vector<std::string> &getSupportedIcons();

  /**
   * Checks if the provided Material Design icon name is supported
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
   * Returns an UTF-8 encoded string of a Material Design icon
   * @param iconName a Material Design icon name
   * @return
   */
  static std::string getIconUtf8String(const std::string &iconName);

#include "MaterialDesignIconsConstants.h"
};

}
#endif // TALIPOT_MATERIAL_DESIGN_ICONS_H
