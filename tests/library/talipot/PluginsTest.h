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

#ifndef PLUGINS_TEST_H
#define PLUGINS_TEST_H

#include <string>

#include "CppUnitIncludes.h"

#include <talipot/Graph.h>

class PluginsTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PluginsTest);
    CPPUNIT_TEST(testloadPlugin);
    CPPUNIT_TEST(testCircularPlugin);
    CPPUNIT_TEST(testAncestorGraph);
    CPPUNIT_TEST(availablePlugins);
    CPPUNIT_TEST(pluginInformation);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;

  public:
    void setUp() override;
    void tearDown() override;
    void testloadPlugin();
    void testCircularPlugin();
    void testAncestorGraph();
    void availablePlugins();
    void pluginInformation();
};

#endif // PLUGINS_TEST_H
