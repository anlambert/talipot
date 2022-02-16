/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PLUGIN_LISTER_H
#define TALIPOT_PLUGIN_LISTER_H

#include <list>
#include <string>
#include <map>

#include <talipot/Plugin.h>
#include <talipot/PluginLoader.h>
#include <talipot/Observable.h>
#include <talipot/Singleton.h>
#include <talipot/TlpTools.h>

namespace tlp {
class PluginContext;

/**
 * @ingroup Plugins
 * @brief The base class for plugin factories.
 *
 * A plugin factory handles the creation process of a tlp::Plugin subclass. This class should never
 *be used directly. See the PLUGIN macro for additional information.
 * @see PLUGIN
 **/
class FactoryInterface {
public:
  virtual tlp::Plugin *createPluginObject(tlp::PluginContext *context) = 0;
};

class PluginsManager;
DECLARE_DLL_TEMPLATE_INSTANCE(Singleton<PluginsManager>, TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Plugins
 *
 * @brief The PluginsManager class is a singleton used to list plugins currently loaded into Talipot
 * and retrieve information about them.
 *
 * This class holds various methods to check information about plugins currently loaded into
 * Talipot. You can use it to list plugins, get dependencies information or create an instance of a
 * plugin.
 *
 * @note Since a plugin name is unique, Plugins are mainly identified by their name
 * (tlp::Plugin::name()) when interfaced with the plugin lister.
 *
 * @see tlp::Plugin
 * @see tlp::PluginLoader
 * @see tlp::PluginLibraryLoader
 */
class TLP_SCOPE PluginsManager : public Observable, public Singleton<PluginsManager> {

  friend class Singleton<PluginsManager>;

  struct PluginDescription {
    FactoryInterface *factory;
    std::string library;
    Plugin *info;
    bool deprecated;

    PluginDescription() : factory(nullptr), info(nullptr), deprecated(false) {}
    ~PluginDescription() {
      delete info;
    }
  };

  // Stores the factories and info of all the plugins
  // that register into this map
  std::map<std::string, PluginDescription> _plugins;

public:
  static PluginLoader *currentLoader;

  ~PluginsManager();

  /**
   * @brief Checks if all registered plug-ins have their dependencies met.
   *
   * @param loader If there are errors, the loader is informed about them so they can be displayed.
   * @return void
   **/
  static void checkLoadedPluginsDependencies(tlp::PluginLoader *loader);

  /**
   * @brief Constructs a plug-in.
   *
   * @param name The name of the plug-in to instantiate.
   * @param p The context to give to the plug-in.
   * @return ObjectType* The newly constructed plug-in.
   **/
  static tlp::Plugin *getPluginObject(const std::string &name,
                                      tlp::PluginContext *context = nullptr);

  /**
   * @brief Checks if a plugin of a given type is loaded
   * This method checks the plugin "pluginName" is currently loaded into Talipot and if it's of type
   * PluginType.
   * @param PluginType the class type of the plugin
   * @param pluginName the name of the plugin
   * @return true if a matching plugin is currently loaded into Talipot.
   */
  template <typename PluginType>
  static bool pluginExists(const std::string &pluginName) {
    return instance().pluginExistsImpl<PluginType>(pluginName);
  }

  /**
   * @brief Similar to tlp::PluginsManager::getPluginObject() but returns a typed instance
   *
   * This method instantiate a plugin from its name and returns it casted into the given type.
   *
   * @param name The plugin's name
   * @param context The context to give to the plugin
   *
   * @return The plugin instance. If there is no such plugin or if the plugin does not match the
   * required type, this method returns nullptr
   */
  template <typename PluginType>
  static PluginType *getPluginObject(const std::string &name,
                                     tlp::PluginContext *context = nullptr) {
    return instance().getPluginObjectImpl<PluginType>(name, context);
  }

  /**
   * @brief Gets the list of plugins of a given type (template parameter).
   * @return A std::list<std::string> containing plugin names.
   **/
  static std::list<std::string> availablePlugins();

  template <typename PluginType>
  static std::list<std::string> availablePlugins() {
    return instance().availablePluginsImpl<PluginType>();
  }

  /**
   * @brief Gets more detailed information about one specific plug-in.
   *
   * @param name The name of the plugin to retrieve information for.
   * @return :const Plugin& The information on the plugin.
   **/
  static const Plugin &pluginInformation(const std::string &name);

  /**
   * @brief Checks if a given name is registered in this factory.
   *
   * @param pluginName The name of the plug-in to look for.
   * @return bool Whether there is a plug-in with the given name registered in this factory.
   **/
  static bool pluginExists(const std::string &pluginName);

  /**
   * @brief Gets the list of parameters for the given plug-in.
   *
   * @param name The name of the plug-in to retrieve the parameters of.
   * @return :ParameterDescriptionList The parameters of the plug-in.
   **/
  static const ParameterDescriptionList &getPluginParameters(const std::string &name);

  /**
   * @brief Gets the dependencies of a plug-in.
   *
   * @param name The name of the plug-in to retrieve the dependencies of.
   * @return :list< tlp::Dependency, std::allocator< tlp::Dependency > > The list of dependencies of
   *the plug-in.
   **/
  static const std::list<tlp::Dependency> &getPluginDependencies(const std::string &name);

  /**
   * @brief Gets the library from which a plug-in has been loaded.
   *
   * @param name The name of the plug-in to retrieve the library of.
   * @return std::string The library from which the plug-in was loaded.
   **/
  static std::string getPluginLibrary(const std::string &name);

  /**
   * @brief Removes a plug-in from this factory.
   * This is useful when a plug-in has unmet dependencies, or appears more than once.
   *
   * @param name The name of the plug-in to remove.
   * @return void
   **/
  static void removePlugin(const std::string &name);

  /**
   * @brief Registers a plugin into Talipot
   *
   * @warning This method should only be called by tlp::FactoryInterface subclasses
   * @see PLUGIN
   */
  static void registerPlugin(FactoryInterface *objectFactory);

  void sendPluginAddedEvent(const std::string &pluginName);

  void sendPluginRemovedEvent(const std::string &pluginName);

private:
  PluginsManager() = default;

  template <typename PluginType>
  bool pluginExistsImpl(const std::string &pluginName) {
    const auto it = _plugins.find(pluginName);
    return (it != _plugins.end() && (dynamic_cast<const PluginType *>(it->second.info) != nullptr));
  }

  template <typename PluginType>
  PluginType *getPluginObjectImpl(const std::string &name, tlp::PluginContext *context = nullptr) {
    if (const auto it = _plugins.find(name);
        it != _plugins.end() && (dynamic_cast<const PluginType *>(it->second.info) != nullptr)) {
      std::string pluginName = it->second.info->name();
      if (name != pluginName) {
        tlp::warning() << "Warning: '" << name << "' is a deprecated plugin name. Use '"
                       << pluginName << "' instead." << std::endl;
      }

      return static_cast<PluginType *>(it->second.factory->createPluginObject(context));
    }
    return nullptr;
  }

  template <typename PluginType>
  std::list<std::string> availablePluginsImpl() {
    std::list<std::string> keys;
    for (const auto &[pluginName, pluginDescription] : _plugins) {
      auto *plugin = dynamic_cast<PluginType *>(pluginDescription.info);

      if (plugin != nullptr &&
          // deprecated names are not listed
          pluginName == pluginDescription.info->name()) {
        keys.push_back(pluginName);
      }
    }

    return keys;
  }

  /**
   * @brief Gets the release number of the given plug-in.
   *
   * @param name The name of the plug-in to retrieve the version number of.
   * @return :string The version number, usually formatted as X[.Y], where X is the major, and Y
   *the minor.
   **/
  static std::string getPluginRelease(const std::string &name);
};

class TLP_SCOPE PluginEvent : public Event {
public:
  enum PluginEventType { TLP_ADD_PLUGIN = 0, TLP_REMOVE_PLUGIN = 1 };

  // constructor for node/edge events
  PluginEvent(PluginEventType pluginEvtType, const std::string &pluginName)
      : Event(tlp::PluginsManager::instance(), EventType::TLP_MODIFICATION), evtType(pluginEvtType),
        pluginName(pluginName) {}

  PluginEventType getType() const {
    return evtType;
  }

  std::string getPluginName() const {
    return pluginName;
  }

protected:
  PluginEventType evtType;
  std::string pluginName;
};
}

#endif // TALIPOT_PLUGIN_LISTER_H
