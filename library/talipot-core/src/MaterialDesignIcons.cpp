/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/MaterialDesignIcons.h>
#include <talipot/TlpTools.h>

#include <talipot/hash.h>

using namespace std;
using namespace tlp;

namespace tlp {

static vector<string> iconsNames;

#include "MaterialDesignIconsData.cpp"

string MaterialDesignIcons::getTTFLocation() {
  return TalipotShareDir + "fonts/MaterialDesignIcons/" + "materialdesignicons-webfont.ttf";
}

string MaterialDesignIcons::getWOFF2Location() {
  return TalipotShareDir + "fonts/MaterialDesignIcons/" + "materialdesignicons-webfont.woff2";
}

bool MaterialDesignIcons::isIconSupported(const string &iconName) {
  return iconCodePoint.find(iconName.c_str()) != iconCodePoint.end();
}

const vector<string> &MaterialDesignIcons::getSupportedIcons() {
  if (iconsNames.empty()) {
    iconsNames.reserve(iconCodePoint.size());
    for (const auto &[iconName, codePoint] : iconCodePoint) {
      iconsNames.push_back(iconName);
    }
  }
  return iconsNames;
}

uint MaterialDesignIcons::getIconCodePoint(const string &iconName) {
  if (const auto it = iconCodePoint.find(iconName.c_str()); it != iconCodePoint.end()) {
    return it->second.first;
  }
  return 0;
}

string MaterialDesignIcons::getIconFamily(const string &) {
  return "materialdesignicons";
}

string MaterialDesignIcons::getIconUtf8String(const string &iconName) {
  try {
    return iconCodePoint.at(iconName.c_str()).second;
  } catch (std::out_of_range &) {
    tlp::warning() << iconName << " icon does not exist, falling back to "
                   << MaterialDesignIcons::HelpCircle << std::endl;
    return getIconUtf8String(MaterialDesignIcons::HelpCircle);
  }
}
}
