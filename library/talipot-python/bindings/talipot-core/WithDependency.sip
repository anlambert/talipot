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

namespace tlp {
%TypeHeaderCode
#include <talipot/WithDependency.h>
%End

struct Dependency {
  std::string pluginName;
  std::string pluginRelease;

  Dependency(std::string pName, std::string pRelease);
};

class WithDependency {

public:

  const std::list<tlp::Dependency>& dependencies();
%Docstring
tlp.WithDependency.getDependencies() -> list of tlp.Dependency
%End

  void addDependency(const std::string &algorithm,const std::string &release );
%Docstring
tlp.WithParameter.addBooleanAlgorithmDependency(algorithm, release )

Add a dependency on a boolean algorithm.

:param name:
   the name of the algorithm

:type name:
   string

:param release:
   the version of the algorithm

:type release:
   string
%End

%MethodCode
sipCpp->addDependency((*a0).c_str(), (*a1).c_str());
%End
};
};
