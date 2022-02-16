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


%ModuleHeaderCode
#include <talipot/ImportModule.h>
#include <talipot/PluginContext.h>
%End

namespace tlp {

class ImportModule : tlp::Plugin {
%Docstring
Bases: :class:`tlp.Plugin`

This abstract class describes an import plugin.

.. rubric:: class attributes

.. py:attribute:: graph

	An empty :class:`tlp.Graph` to populate when running the import process

.. py:attribute:: dataSet

	A dictionary containing parameters for this import plugin, if any

.. py:attribute:: pluginProgress

	A :class:`tlp.PluginProgress` to give feedback to the user. It can be :const:`None`, so use with caution

.. rubric:: class methods

%End

%TypeHeaderCode
#include <talipot/ImportModule.h>
%End

public:

  ImportModule (const tlp::PluginContext* context);
  virtual ~ImportModule();

  virtual std::string category() const;

  virtual std::list<std::string> fileExtensions() const;
%Docstring
tlp.ImportModule.fileExtensions()

This method returns the extensions of file formats this plugin can import.
Derived classes can implement it in order for the plugin to be automatically called
by the :func:`tlp.loadGraph` function.

:rtype: list of string
%End

  virtual std::list<std::string> gzipFileExtensions() const;
%Docstring
tlp.ImportModule.gzipFileExtensions()

This method returns the extensions of gzipped file formats this plugin can import.
Derived classes can implement it in order for the plugin to be automatically called
by the :func:`tlp.loadGraph` function.

:rtype: list of string
%End

  std::list<std::string> allFileExtensions() const;
%Docstring
tlp.ImportModule.allFileExtensions()

This method returns all the extensions (normal and gzipped) of file formats this plugin can import.

:rtype: list of string
%End

  virtual bool importGraph() = 0;
%Docstring
tlp.ImportModule.importGraph()

This method is called to import a graph.
Derived classes must implement it. A boolean must also be returned
to indicate if the import was successful.

:rtype: boolean
%End

  tlp::Graph *graph;
  tlp::PluginProgress *pluginProgress;
  tlp::DataSet *dataSet
  {
%GetCode
  // add file parameters to the dataset without the special prefix used internally by Tulip
  std::vector<std::string> filesPrefixes;
  filesPrefixes.push_back("file::");
  filesPrefixes.push_back("anyfile::");
  filesPrefixes.push_back("dir::");
  for(const std::pair<std::string, tlp::DataType*> &entry :
      tlp::stableIterator(sipCpp->dataSet->getValues())) {
    for (size_t i = 0 ; i < filesPrefixes.size() ; ++i) {
      if (entry.first.length() > filesPrefixes[i].length() && entry.first.substr(0, filesPrefixes[i].length()) == filesPrefixes[i]) {
        std::string paramName = entry.first.substr(filesPrefixes[i].length());
        if (!sipCpp->dataSet->exists(paramName)) {
          sipCpp->dataSet->set(paramName, *(static_cast<std::string*>(entry.second->value)));
        }
      }
    }
  }
  sipPy = sipConvertFromType(sipCpp->dataSet, sipFindType("tlp::DataSet"), NULL);
%End
  };

};

};

