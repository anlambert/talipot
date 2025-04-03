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

#include "StringPropertyTest.h"

#include <talipot/StringProperty.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(StringPropertyTest);

void StringPropertyTest::setUp() {
    graph = tlp::newGraph();
}

void StringPropertyTest::tearDown() {
    delete graph;
}

void StringPropertyTest::simpleVectorTest() {
    node n = graph->addNode();
    StringVectorProperty *vectorProperty = graph->getStringVectorProperty("data_1");
    vector<string> tmp;

    for (uint i = 0; i < 5; ++i) {
        stringstream tmpstr;
        tmpstr << "\"test string #\"  : " << i;
        tmp.push_back(tmpstr.str());
    }

    vectorProperty->setNodeValue(n, tmp);

    const vector<string> &value = vectorProperty->getNodeValue(n);
    CPPUNIT_ASSERT_EQUAL(size_t(5), value.size());

    for (uint i = 0; i < value.size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(tmp[i], value[i]);
    }
}

void StringPropertyTest::complexVectorTest() {
    node n = graph->addNode();
    StringVectorProperty *vectorProperty = graph->getStringVectorProperty("stp");
    vector<string> tmp;

    for (uint i = 0; i < 5000; ++i) {
        stringstream tmpstr;
        tmpstr << "\"test string #\"  : " << i;
        tmp = vectorProperty->getNodeValue(n);
        CPPUNIT_ASSERT(tmp.size() == i);
        tmp.push_back(tmpstr.str());
        vectorProperty->setNodeValue(n, tmp);
    }

    const vector<string> &value = vectorProperty->getNodeValue(n);

    CPPUNIT_ASSERT_EQUAL(size_t(5000), value.size());

    for (uint i = 0; i < value.size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(tmp[i], value[i]);
    }
}
