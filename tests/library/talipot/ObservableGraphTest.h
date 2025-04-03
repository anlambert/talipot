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

#ifndef OBSERVABLE_GRAPH_TEST_H
#define OBSERVABLE_GRAPH_TEST_H

#include <talipot/Graph.h>
#include <talipot/TlpTools.h>

#include "CppUnitIncludes.h"

class ObservableGraphTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ObservableGraphTest);
    CPPUNIT_TEST(testAddDel);
    CPPUNIT_TEST(testAddDelProperties);
    CPPUNIT_TEST(testClear);
    CPPUNIT_TEST(testSubgraph);
    CPPUNIT_TEST(testDeleteSubgraph);
    CPPUNIT_TEST(testReverse);
    CPPUNIT_TEST(testObserverWhenRemoveObservable);
    CPPUNIT_TEST(testDelInheritedPropertyExistWhenDelInheritedPropertyIsSend);
    CPPUNIT_TEST(testNotifyDelInheritedPropertyIsSendWhenLocalPropertyIsDeleted);
    CPPUNIT_TEST(testDeleteBug747);
    CPPUNIT_TEST(testAddEdgesEventForTLPBImport);
    CPPUNIT_TEST_SUITE_END();

  private:
    static tlp::Graph *graph;

  public:
    static void setGraph(tlp::Graph *g) {
        graph = g;
    }
    void setUp() override;
    void tearDown() override;

    void testAddDel();
    void testAddDelProperties();
    void testClear();
    void testSubgraph();
    void testDeleteSubgraph();
    void testReverse();
    void testObserverWhenRemoveObservable();
    void testDelInheritedPropertyExistWhenDelInheritedPropertyIsSend();
    void testNotifyDelInheritedPropertyIsSendWhenLocalPropertyIsDeleted();
    void testDeleteBug747();
    void testAddEdgesEventForTLPBImport();
};

#endif // OBSERVABLE_GRAPH_TEST_H
