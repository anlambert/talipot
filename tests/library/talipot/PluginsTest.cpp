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

#include "PluginsTest.h"

#include <talipot/BooleanProperty.h>
#include <talipot/PluginLibraryLoader.h>
#include <talipot/PluginLoaderTxt.h>
#include <talipot/Plugin.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(PluginsTest);

#if defined(_WIN32)
const std::string suffix = "dll";
#elif defined(__APPLE__)
const std::string suffix = "dylib";
#else
const std::string suffix = "so";
#endif

//==========================================================
void PluginsTest::setUp() {
  graph = tlp::newGraph();
}
//==========================================================
void PluginsTest::tearDown() {
  delete graph;
}
//==========================================================
void PluginsTest::testloadPlugin() {
  string pluginName = "Test";
  // plugin does not exist yet
  CPPUNIT_ASSERT(!tlp::PluginsManager::pluginExists(pluginName));
  PluginLoaderTxt loader;
  PluginLibraryLoader::loadPluginLibrary("./testPlugin." + suffix, &loader);
  // plugin should exist now
  CPPUNIT_ASSERT(tlp::PluginsManager::pluginExists(pluginName));
  const list<Dependency> &deps = tlp::PluginsManager::getPluginDependencies(pluginName);
  // only one dependency (see testPlugin.cpp)
  CPPUNIT_ASSERT_EQUAL(size_t(1), deps.size());
  CPPUNIT_ASSERT_EQUAL(pluginName, deps.front().pluginName);
}
//==========================================================
void PluginsTest::testCircularPlugin() {
  string name = "Test";
  string err = "Error";
  // ensure graph is not empty
  graph->addNode();
  tlp::BooleanProperty sel(graph);
  CPPUNIT_ASSERT(graph->applyPropertyAlgorithm(name, &sel, err) == false);
}
//==========================================================
void PluginsTest::testAncestorGraph() {
  PluginLibraryLoader::loadPluginLibrary("./testPlugin2." + suffix);
  string simpleAlgorithm = "Test2";
  string invalidAlgorithm = "Test3";
  string err;

  /**
   * The graph Hierarchy is as follows
   * graph --------- child1* ------- grandchild
   *          \_____ child2
   *
   * The property belongs to child1, so only him and grandchild can use it
   **/
  Graph *child1 = graph->addSubGraph();
  Graph *grandchild = child1->addSubGraph();
  BooleanProperty sel(child1);

  Graph *child2 = graph->addSubGraph();

  // since the property belongs to a descendant graph, this fails
  bool result = graph->applyPropertyAlgorithm(simpleAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, !result);

  // since the property belongs to a descendant of a sibling graph, this fails
  result = child2->applyPropertyAlgorithm(simpleAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, !result);

  // These will fail because the graph is empty
  result = child1->applyPropertyAlgorithm(simpleAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, !result);

  result = grandchild->applyPropertyAlgorithm(simpleAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, !result);

  grandchild->addNode();

  // now the graph is not empty they will pass
  result = child1->applyPropertyAlgorithm(simpleAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, result);

  result = grandchild->applyPropertyAlgorithm(simpleAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, result);

  // now testing with an algorithm that does not exists
  result = child1->applyPropertyAlgorithm(invalidAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, !result);

  result = grandchild->applyPropertyAlgorithm(invalidAlgorithm, &sel, err);
  CPPUNIT_ASSERT_MESSAGE(err, !result);
}

void PluginsTest::availablePlugins() {
  CPPUNIT_ASSERT_MESSAGE("The 'Test' plugin is not listed by the PluginsManager",
                         PluginsManager::pluginExists("Test"));
  CPPUNIT_ASSERT_MESSAGE("The 'Test2' plugin is not listed by the PluginsManager",
                         PluginsManager::pluginExists("Test2"));
}

void PluginsTest::pluginInformation() {
  string pluginName = "Test";
  CPPUNIT_ASSERT_MESSAGE("'Test' plugin must be loaded", PluginsManager::pluginExists(pluginName));

  std::list<Dependency> dependencies = PluginsManager::getPluginDependencies(pluginName);
  CPPUNIT_ASSERT_EQUAL(size_t(1), dependencies.size());
  CPPUNIT_ASSERT_EQUAL(pluginName, dependencies.begin()->pluginName);
  CPPUNIT_ASSERT_EQUAL(string("1.0"), dependencies.begin()->pluginRelease);

  tlp::ParameterDescriptionList parameters = PluginsManager::getPluginParameters(pluginName);

  Iterator<ParameterDescription> *it = parameters.getParameters();
  CPPUNIT_ASSERT_MESSAGE("Test plugin has no parameters", it->hasNext());
  ParameterDescription param = it->next();
  CPPUNIT_ASSERT_EQUAL(string("result"), param.getName());
  param = it->next();
  CPPUNIT_ASSERT_EQUAL(string("testParameter"), param.getName());
  CPPUNIT_ASSERT_MESSAGE("test parameter should not be mandatory", !param.isMandatory());
  CPPUNIT_ASSERT_EQUAL(string("0"), param.getDefaultValue());
#ifndef _MSC_VER
  CPPUNIT_ASSERT_EQUAL(string("i"), param.getTypeName());
#else
  CPPUNIT_ASSERT_EQUAL(string("int"), param.getTypeName());
#endif
  delete it;

  const Plugin &factory(PluginsManager::pluginInformation(pluginName));
  CPPUNIT_ASSERT_EQUAL(string("Jezequel"), factory.author());
  CPPUNIT_ASSERT_EQUAL(string("03/11/2004"), factory.date());
  CPPUNIT_ASSERT_EQUAL(string(""), factory.group());
  CPPUNIT_ASSERT_EQUAL(0, factory.id());
  CPPUNIT_ASSERT_EQUAL(string("1"), factory.major());
  CPPUNIT_ASSERT_EQUAL(string("0"), factory.minor());
  CPPUNIT_ASSERT_EQUAL(pluginName, factory.name());
  CPPUNIT_ASSERT_EQUAL(string("1.0"), factory.release());
  CPPUNIT_ASSERT_EQUAL(tlp::getMajor(TALIPOT_VERSION), factory.talipotMajor());
  CPPUNIT_ASSERT_EQUAL(tlp::getMinor(TALIPOT_VERSION), factory.talipotMinor());
  CPPUNIT_ASSERT_EQUAL(string(TALIPOT_VERSION), factory.talipotRelease());
  CPPUNIT_ASSERT_EQUAL(getTalipotVersion(), factory.talipotRelease());
  CPPUNIT_ASSERT_EQUAL(tlp::getMajor(getTalipotVersion()), factory.talipotMajor());
  CPPUNIT_ASSERT_EQUAL(tlp::getMinor(getTalipotVersion()), factory.talipotMinor());
}
