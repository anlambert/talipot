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

std::unique_ptr<PluginsManager> PluginsManager::_instance;
std::once_flag PluginsManager::_onceFlag;

PluginLoader *PluginsManager::currentLoader = nullptr;

PluginsManager *PluginsManager::instance() {
  std::call_once(PluginsManager::_onceFlag, []() { _instance.reset(new PluginsManager); });
  return _instance.get();
}

PluginsManager::~PluginsManager() {
  for (auto &it : _plugins) {
    // avoid double free
    if (it.second.deprecated) {
      it.second.info = nullptr;
    }
  }
  _plugins.clear();
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

  auto &plugins = instance()->_plugins;

  for (const auto &it : plugins) {
    // deprecated names are not listed
    if (it.first == it.second.info->name()) {
      keys.push_back(it.first);
    }
  }

  return keys;
}

const Plugin &PluginsManager::pluginInformation(const std::string &name) {
  auto &plugins = instance()->_plugins;
  return *(plugins.find(name)->second.info);
}

void PluginsManager::registerPlugin(FactoryInterface *objectFactory) {

  tlp::Plugin *information = objectFactory->createPluginObject(nullptr);
  std::string pluginName = information->name();

  auto &plugins = instance()->_plugins;

  if (plugins.find(pluginName) == plugins.end()) {
    PluginDescription &description = plugins[pluginName];
    description.factory = objectFactory;
    description.library = PluginLibraryLoader::getCurrentPluginFileName();
    description.info = information;

    if (currentLoader != nullptr) {
      currentLoader->loaded(information, information->dependencies());
    }

    instance()->sendPluginAddedEvent(pluginName);

    // register under a deprecated name if needed
    std::string oldName = information->deprecatedName();
    if (!oldName.empty()) {
      if (!pluginExists(oldName)) {
        plugins[oldName] = plugins[pluginName];
        plugins[oldName].deprecated = true;
      } else if (currentLoader != nullptr) {
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
  auto &plugins = instance()->_plugins;
  plugins.erase(name);
  instance()->sendPluginRemovedEvent(name);
}

tlp::Plugin *PluginsManager::getPluginObject(const std::string &name, PluginContext *context) {
  auto &plugins = instance()->_plugins;
  auto it = plugins.find(name);

  if (it != plugins.end()) {
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
  auto &plugins = instance()->_plugins;
  return plugins.find(name)->second.library;
}

bool PluginsManager::pluginExists(const std::string &pluginName) {
  auto &plugins = instance()->_plugins;
  return plugins.find(pluginName) != plugins.end();
}

void PluginsManager::sendPluginAddedEvent(const std::string &pluginName) {
  sendEvent(PluginEvent(PluginEvent::TLP_ADD_PLUGIN, pluginName));
}

void PluginsManager::sendPluginRemovedEvent(const std::string &pluginName) {
  sendEvent(PluginEvent(PluginEvent::TLP_REMOVE_PLUGIN, pluginName));
}