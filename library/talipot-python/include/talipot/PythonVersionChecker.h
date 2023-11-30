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

#ifndef TALIPOT_PYTHON_VERSION_CHECKER_H
#define TALIPOT_PYTHON_VERSION_CHECKER_H

#include <QString>
#include <QStringList>

#include <talipot/config.h>

namespace tlp {
class TLP_PYTHON_SCOPE PythonVersionChecker {
public:
  static QStringList installedVersions();
  static QString compiledVersion();
  static bool isPythonVersionMatching();
#ifdef WIN32
  static QString getPythonHome();
#endif
private:
  static QStringList _installedVersions;
  static bool _installedVersionsChecked;
};
}

#endif // TALIPOT_PYTHON_VERSION_CHECKER_H
