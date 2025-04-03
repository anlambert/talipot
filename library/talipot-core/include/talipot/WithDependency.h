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

#ifndef TALIPOT_WITH_DEPENDENCY_H
#define TALIPOT_WITH_DEPENDENCY_H

#include <list>
#include <string>
#include <typeinfo>
#include <talipot/config.h>

namespace tlp {

/**
 * @ingroup Plugins
 *
 * @brief Represents a plugin's dependency to another plugin.
 * In addition to maganing plugin registration, Talipot also handles a dependency mechanism between
 * plugins.
 * Every Talipot plugin inherits from the tlp::WithDependency interface which allows to declare that
 * another plugin should be loaded in order for this plugin to run.
 * When declaring a dependency, a plugin state the name and the version of the dependency. This is
 * done by calling tlp::WithDependency::addDependency()
 *
 * @see tlp::WithDependency
 */
struct Dependency {
    /**
     * @brief The name of the plug-in, as registered in the Talipot plug-in system.
     */
    std::string pluginName;
    /**
     * @brief The required version of the plug-in.
     */
    std::string pluginRelease;

    /**
     * @brief Constructs a new dependency.
     *
     * @param pName The name of the plug-in, as registered in the Talipot plug-in system.
     * @param pRelease The required version of the plug-in.
     */
    Dependency(std::string pName, std::string pRelease)
        : pluginName(pName), pluginRelease(pRelease) {}
};

/**
 * @ingroup Plugins
 * @brief Describes the dependencies of a plug-in on other plug-ins, identified by their name and
 * their version number.
 *
 * This allows to have a plug-in inner workings depend on other plug-ins without linking them
 * statically, or hoping depended plug-in will be there.
 */
class WithDependency {
  protected:
    /**
     * @brief The inner list of dependencies.
     */
    std::list<Dependency> _dependencies;

  public:
    /**
     * @brief Adds a dependency upon another plug-in.
     *
     * @param factory The type name of the plug-in (e.g. 'DoubleAlgorithm')
     * @param name The name of the plug-in, as registered in the Talipot plug-in system.
     * @param release The required version of the depended plug-in.
     */
    void addDependency(const char *name, const char *release) {
        _dependencies.push_back(Dependency(name, release));
    }

    /**
     * @brief Gets the list of Dependencies of this plug-in.
     *
     * @return list<Dependency> The list of dependencies of this plug-in.
     */
    const std::list<Dependency> &dependencies() const {
        return _dependencies;
    }
};
}
#endif // TALIPOT_WITH_DEPENDENCY_H
