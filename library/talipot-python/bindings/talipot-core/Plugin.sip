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

namespace tlp {
class Plugin : tlp::WithParameter, tlp::WithDependency {
%Docstring
Bases: :class:`tlp.WithParameter`

This is the base class for all Talipot plugins.
%End

%TypeHeaderCode
#include <talipot/Plugin.h>
%End

public:

  virtual ~Plugin();

  virtual std::string category() const = 0;
%Docstring
tlp.Plugin.category()

Returns the category of the plugin.

:rtype: string
%End

  virtual std::string name() const = 0;
%Docstring
tlp.Plugin.name()

Returns the name of the plugin.

:rtype: string
%End

  virtual std::string group() const = 0;
%Docstring
tlp.Plugin.group()

Returns the group of the plugin.

:rtype: string
%End

  virtual std::string author() const = 0;
%Docstring
tlp.Plugin.author()

Returns the author of the plugin.

:rtype: string
%End

  virtual std::string date() const = 0;
%Docstring
tlp.Plugin.category()

Returns the creation date of the plugin.

:rtype: string
%End

  virtual std::string info() const = 0;
%Docstring
tlp.Plugin.category()

Returns some information about the plugin.

:rtype: string
%End

  virtual std::string release() const = 0;
%Docstring
tlp.Plugin.category()

Returns the release number of the plugin.

:rtype: string
%End

  virtual std::string talipotRelease() const = 0;
%Docstring
tlp.Plugin.category()

Returns the Talipot release number.

:rtype: string
%End

  virtual std::string icon() const;
%Docstring
tlp.Plugin.icon()

Returns the icon file path associated to the plugin.

:rtype: string
%End

  virtual std::string major() const;

  virtual std::string minor() const;

  virtual std::string talipotMajor() const;

  virtual std::string talipotMinor() const;

  virtual int id() const;

  virtual std::string programmingLanguage() const;

};

class FactoryInterface {
%TypeHeaderCode
#include <talipot/Plugin.h>
%End

public:

  virtual tlp::Plugin* createPluginObject(tlp::PluginContext* context) = 0 /Factory/;

  void registerPlugin();
%MethodCode
  tlp::PluginsManager::registerPlugin(sipCpp);
%End

};

};
