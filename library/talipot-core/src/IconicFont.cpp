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

#include <cassert>

#include "talipot/IconicFont.h"
#include "talipot/FontAwesome.h"
#include "talipot/MaterialDesignIcons.h"

using namespace tlp;

std::string IconicFont::getWOFF2Location(const std::string &iconName) {
  assert(isIconSupported(iconName));
  return (iconName.find("fa") == 0) ? FontAwesome::getWOFF2Location(iconName)
                                    : MaterialDesignIcons::getWOFF2Location();
}

bool IconicFont::isIconSupported(const std::string &iconName) {
  return (iconName.find("fa") == 0) ? FontAwesome::isIconSupported(iconName)
                                    : MaterialDesignIcons::isIconSupported(iconName);
}

uint IconicFont::getIconCodePoint(const std::string &iconName) {
  assert(isIconSupported(iconName));
  return (iconName.find("fa") == 0) ? FontAwesome::getIconCodePoint(iconName)
                                    : MaterialDesignIcons::getIconCodePoint(iconName);
}

std::string IconicFont::getIconFamily(const std::string &iconName) {
  assert(isIconSupported(iconName));
  return (iconName.find("fa") == 0) ? FontAwesome::getIconFamily(iconName)
                                    : MaterialDesignIcons::getIconFamily(iconName);
}

std::string IconicFont::getIconUtf8String(const std::string &iconName) {
  assert(isIconSupported(iconName));
  return (iconName.find("fa") == 0) ? FontAwesome::getIconUtf8String(iconName)
                                    : MaterialDesignIcons::getIconUtf8String(iconName);
}
