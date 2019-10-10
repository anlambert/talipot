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

#include <cstdlib>
#include <talipot/PluginsManager.h>
#include <talipot/PluginLoader.h>
#include <talipot/PluginLibraryLoader.h>

using namespace tlp;
using namespace std;

PluginLoader *PluginsManager::currentLoader = nullptr;

struct PluginsManagerInstance : public PluginsManager {
  // there is only one instance of this class
  // It is first used by PluginsManager::registerPlugin
  // at the talipot-core library loading time while
  // it is only 'zero' initialized
  bool created;
  PluginsManagerInstance() : created(true) {}
  inline void sendPluginAddedEvent(const std::string &pluginName) {
    if (created)
      sendEvent(PluginEvent(PluginEvent::TLP_ADD_PLUGIN, pluginName));
  }
  inline void sendPluginRemovedEvent(const std::string &pluginName) {
    if (created)
      sendEvent(PluginEvent(PluginEvent::TLP_REMOVE_PLUGIN, pluginName));
  }
};

PluginsManagerInstance _instance;

PluginsManager *PluginsManager::instance() {
  return &_instance;
}

void PluginsManager::checkLoadedPluginsDependencies(tlp::PluginLoader *loader) {
  // plugins dependencies loop
  bool depsNeedCheck;

  do {
    depsNeedCheck = false;

    // loop over plugins
    std::list<std::string> plugins = PluginsManager::availablePlugins();

    for (const string &pluginName : plugins) {
      std::list<Dependency> dependencies = PluginsManager::getPluginDependencies(pluginName);

      // loop over dependencies
      for (const Dependency &dep : dependencies) {
        std::string pluginDepName = dep.pluginName;

        if (!PluginsManager::pluginExists(pluginDepName)) {
          if (loader)
            loader->aborted(pluginName, " '" + pluginName +
                                            "' will be removed, it depends on missing " + "'" +
                                            pluginDepName + "'.");

          PluginsManager::removePlugin(pluginName);
          depsNeedCheck = true;
          break;
        }

        std::string release = PluginsManager::getPluginRelease(pluginDepName);
        const std::string &releaseDep = dep.pluginRelease;

        if (tlp::getMajor(release) != tlp::getMajor(releaseDep) ||
            tlp::getMinor(release) < tlp::getMinor(releaseDep)) {
          if (loader) {
            loader->aborted(pluginName, " '" + pluginName +
                                            "' will be removed, it depends on release " +
                                            releaseDep + " of" + " '" + pluginDepName + "' but " +
                                            release + " is loaded.");
          }

          PluginsManager::removePlugin(pluginName);
          depsNeedCheck = true;
          break;
        }
      }
    }
  } while (depsNeedCheck);
}

std::list<std::string> PluginsManager::availablePlugins() {
  std::list<std::string> keys;

  for (auto it = _plugins.begin(); it != _plugins.end(); ++it) {
    // deprecated names are not listed
    if (it->first == it->second.info->name())
      keys.push_back(it->first);
  }

  return keys;
}

const Plugin &PluginsManager::pluginInformation(const std::string &name) {
  return *(_plugins.find(name)->second.info);
}

// used to initialize the _plugins map
// it is first called by registerPlugin at the library loading time
// while _plugins is only 'zero' initialized
std::map<std::string, PluginsManager::PluginDescription> &PluginsManager::getPluginsMap() {
  static std::map<std::string, PluginDescription> plugins;
  return plugins;
}

// the _plugins map
std::map<std::string, PluginsManager::PluginDescription> &PluginsManager::_plugins =
    PluginsManager::getPluginsMap();

void PluginsManager::registerPlugin(FactoryInterface *objectFactory) {
  // because the talipot-core library contains some import/export plugins
  // we must ensure plugins map initialization at the library loading time
  // while _plugins is only 'zero' initialized
  std::map<std::string, PluginsManager::PluginDescription> &plugins = getPluginsMap();

  tlp::Plugin *information = objectFactory->createPluginObject(nullptr);
  std::string pluginName = information->name();

  if (plugins.find(pluginName) == plugins.end()) {
    PluginDescription &description = plugins[pluginName];
    description.factory = objectFactory;
    description.library = PluginLibraryLoader::getCurrentPluginFileName();
    description.info = information;

    if (currentLoader != nullptr) {
      currentLoader->loaded(information, information->dependencies());
    }

    _instance.sendPluginAddedEvent(pluginName);

    // register under a deprecated name if needed
    std::string oldName = information->deprecatedName();
    if (!oldName.empty()) {
      if (!pluginExists(oldName))
        plugins[oldName] = plugins[pluginName];
      else if (currentLoader != nullptr) {
        std::string tmpStr;
        tmpStr += "'" + oldName + "' cannot be a deprecated name of plugin '" + pluginName + "'";
        currentLoader->aborted(tmpStr, "multiple definitions found; check your plugin libraries.");
      }
    }
  } else {
    if (currentLoader != nullptr) {
      std::string tmpStr;
      tmpStr += "'" + pluginName + "' plugin";
      currentLoader->aborted(tmpStr, "multiple definitions found; check your plugin libraries.");
    }

    delete information;
  }
}

void tlp::PluginsManager::removePlugin(const std::string &name) {
  _plugins.erase(name);
  _instance.sendPluginRemovedEvent(name);
}

tlp::Plugin *PluginsManager::getPluginObject(const std::string &name, PluginContext *context) {
  auto it = _plugins.find(name);

  if (it != _plugins.end()) {
    std::string pluginName = it->second.info->name();
    if (name != pluginName)
      tlp::warning() << "Warning: '" << name << "' is a deprecated plugin name. Use '" << pluginName
                     << "' instead." << std::endl;

    return it->second.factory->createPluginObject(context);
  }

  return nullptr;
}

const tlp::ParameterDescriptionList &PluginsManager::getPluginParameters(const std::string &name) {
  return pluginInformation(name).getParameters();
}

std::string PluginsManager::getPluginRelease(const std::string &name) {
  return pluginInformation(name).release();
}

const std::list<tlp::Dependency> &PluginsManager::getPluginDependencies(const std::string &name) {
  return pluginInformation(name).dependencies();
}

std::string PluginsManager::getPluginLibrary(const std::string &name) {
  return _plugins.find(name)->second.library;
}

bool PluginsManager::pluginExists(const std::string &pluginName) {
  return _plugins.find(pluginName) != _plugins.end();
}