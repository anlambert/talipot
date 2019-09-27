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
#include <tulip/Plugin.h>
#include <tulip/TlpTools.h>

using namespace tlp;
using namespace std;

std::string tlp::getMinor(const std::string &release) {
#ifdef _MSC_VER
  char sep = '_';

  // handle the case where the version number separator is not an underscore (e.g. for Tulip
  // plugins)
  if (release.find(sep) == std::string::npos) {
    sep = '.';
  }

#else
  char sep = '.';
#endif
  size_t pos = release.find(sep);

  // if there is no minor version number, return a default '0'
  if (pos == std::string::npos)
    return std::string("0");

  size_t rpos = release.rfind(sep);

  // if there is only one dot, return everything after it
  if (pos == rpos)
    return release.substr(pos + 1);

  // if there is more than one dot, return everything between the first and last dots
  return release.substr(pos + 1, rpos - pos - 1);
}

std::string tlp::getMajor(const std::string &release) {
#ifdef _MSC_VER
  char sep = '_';

  // handle the case where the version number separator is not an underscore (e.g. for Tulip
  // plugins)
  if (release.find(sep) == std::string::npos) {
    sep = '.';
  }

#else
  char sep = '.';
#endif
  size_t pos = release.find(sep);
  return release.substr(0, pos);
}

string Plugin::major() const {
  return tlp::getMajor(release());
}

string Plugin::minor() const {
  return tlp::getMinor(release());
}

string Plugin::tulipMajor() const {
  return tlp::getMajor(tulipRelease());
}

string Plugin::tulipMinor() const {
  return tlp::getMinor(tulipRelease());
}

int Plugin::id() const {
  return 0;
}

std::string Plugin::icon() const {
  return ":/tulip/gui/icons/logo32x32.png";
}

std::string Plugin::programmingLanguage() const {
  return "C++";
}

void Plugin::declareDeprecatedName(const std::string &previousName) {
  if (oldName.empty())
    oldName = previousName;
  else
    tlp::warning() << "Warning: '" << previousName
                   << "' cannot be declared as deprecated name of Plugin '" << name()
                   << "' because '" << oldName << "' already is." << std::endl;
}
