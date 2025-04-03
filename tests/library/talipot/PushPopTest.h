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

#ifndef PUSH_POP_TEST_H
#define PUSH_POP_TEST_H

#include <talipot/Graph.h>
#include <talipot/TlpTools.h>

#include "CppUnitIncludes.h"

class PushPopTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PushPopTest);
    CPPUNIT_TEST(testAddDel);
    CPPUNIT_TEST(testSetValue);
    CPPUNIT_TEST(testSetEltValue);
    CPPUNIT_TEST(testVectorValue);
    CPPUNIT_TEST(testSetEnds);
    CPPUNIT_TEST(testCopyProperty);
    CPPUNIT_TEST(testSubgraph);
    CPPUNIT_TEST(testPushAddSubgraphsPop);
    CPPUNIT_TEST(testDelSubgraph);
    CPPUNIT_TEST(testDelAllSgPopUnpop);
    CPPUNIT_TEST(testDeletePushPopFalse);
    CPPUNIT_TEST(testTests);
    CPPUNIT_TEST(testAddDelProps);
    CPPUNIT_TEST(testRenameProps);
    CPPUNIT_TEST(testObserveDelProps);
    CPPUNIT_TEST(testAddSubgraphProp);
    CPPUNIT_TEST(testMetaNode);
    CPPUNIT_TEST(testAddDelLoopsOneByOne);
    CPPUNIT_TEST(testAddDelLoopsBatch);

    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;

  public:
    void setUp() override;
    void tearDown() override;

    void testAddDel();
    void testSetValue();
    void testSetEltValue();
    void testVectorValue();
    void testSetEnds();
    void testCopyProperty();
    void testSubgraph();
    void testPushAddSubgraphsPop();
    void testDelSubgraph();
    void testDelAllSgPopUnpop();
    void testDeletePushPopFalse();
    void testTests();
    void testAddDelProps();
    void testRenameProps();
    void testObserveDelProps();
    void testAddSubgraphProp();
    void testMetaNode();
    void testAddDelLoopsOneByOne();
    void testAddDelLoopsBatch();
};

#endif // PUSH_POP_TEST_H
