/**
 *
 * Copyright (C) 2025-2026  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <string>
#include <iostream>
#include <vector>

#include <talipot/BooleanProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>

#include "PropertyArraySubscriptTest.h"

using namespace tlp;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(PropertyArraySubscriptTest);

void PropertyArraySubscriptTest::setUp() {
  graph = newGraph();
  n1 = graph->addNode();
  n2 = graph->addNode();
  n3 = graph->addNode();
  e1 = graph->addEdge(n1, n2);
  e2 = graph->addEdge(n2, n3);
  e3 = graph->addEdge(n3, n1);
}

void PropertyArraySubscriptTest::tearDown() {
  delete graph;
}

void PropertyArraySubscriptTest::testBooleanProperty() {
  BooleanProperty &bp = *graph->getBooleanProperty("boolean");

  bp[e1] = bp[n1] = true;
  bp[e2] = bp[n2] = !bp[n1];
  bp[e3] = bp[n3] = bp[n1];

  CPPUNIT_ASSERT(bp[n1]);
  CPPUNIT_ASSERT(bp[n2] != bp[n1]);
  CPPUNIT_ASSERT(!bp[n2]);
  CPPUNIT_ASSERT(bp[n3] == bp[n1]);
  CPPUNIT_ASSERT(bp[e1]);
  CPPUNIT_ASSERT(bp[e2] != bp[n1]);
  CPPUNIT_ASSERT(!bp[e2]);
  CPPUNIT_ASSERT(bp[e3] == bp[n1]);

  // test as PropertyInterface
  auto &bpi = *graph->getProperty("boolean");
  CPPUNIT_ASSERT(bpi[n1] == "true");
  CPPUNIT_ASSERT(bp[n1]);
  bpi[n2] = bpi[n1];
  CPPUNIT_ASSERT(bpi[n2] == bpi[n1]);
  CPPUNIT_ASSERT(bp[n2] == bp[n1]);
  CPPUNIT_ASSERT(bpi[e2] == "false");
  CPPUNIT_ASSERT(bp[e2] == false);
  bpi[e3] = "false";
  CPPUNIT_ASSERT(bpi[e3] == "false");
  CPPUNIT_ASSERT(bpi[e3] == bpi[e2]);
  CPPUNIT_ASSERT(bp[e3] == bp[e2]);
}

void PropertyArraySubscriptTest::testIntegerProperty() {
  IntegerProperty &ip = *graph->getIntegerProperty("integer");

  ip[e1] = ip[n1] = 1;
  ip[e2] = ip[n2] = ip[n1] + 1;
  ip[e3] = ip[n3] = ip[n1] - 1;

  CPPUNIT_ASSERT(ip[n1]);
  CPPUNIT_ASSERT(ip[n2] == 2);
  CPPUNIT_ASSERT(ip[n2] >= ip[n1]);
  CPPUNIT_ASSERT(ip[n3] < ip[n1]);
  CPPUNIT_ASSERT(!ip[n3]);
  CPPUNIT_ASSERT(ip[e1] == 1);
  CPPUNIT_ASSERT(ip[e2] == 2);
  CPPUNIT_ASSERT(ip[e2] >= ip[n1]);
  CPPUNIT_ASSERT(ip[e3] < ip[n1]);
  CPPUNIT_ASSERT(!ip[e3]);

  ++ip[n1];
  CPPUNIT_ASSERT(ip[n1] == 2);
  auto i1 = ip[n1]++;
  CPPUNIT_ASSERT(ip[n1] == 3);
  CPPUNIT_ASSERT(i1 == 2);
  --ip[n1];
  CPPUNIT_ASSERT(ip[n1] == 2);
  i1 = ip[n1]--;
  CPPUNIT_ASSERT(ip[n1] == 1);
  CPPUNIT_ASSERT(i1 == 2);
  ip[n1] += i1;
  CPPUNIT_ASSERT(ip[n1] == 3);
  ip[n1] -= i1;
  CPPUNIT_ASSERT(ip[n1] == 1);
  ++ip[e1];
  CPPUNIT_ASSERT(ip[e1] == 2);
  i1 = ip[e1]++;
  CPPUNIT_ASSERT(ip[e1] == 3);
  CPPUNIT_ASSERT(i1 == 2);
  --ip[e1];
  CPPUNIT_ASSERT(ip[e1] == 2);
  i1 = ip[e1]--;
  CPPUNIT_ASSERT(ip[e1] == 1);
  CPPUNIT_ASSERT(i1 == 2);
  ip[e1] += i1;
  CPPUNIT_ASSERT(ip[e1] == 3);
  ip[e1] -= i1;
  CPPUNIT_ASSERT(ip[e1] == 1);

  // test as PropertyInterface
  auto &ipi = *graph->getProperty("integer");
  CPPUNIT_ASSERT(ipi[n1] == "1");
  CPPUNIT_ASSERT(ip[n1] == 1);
  ipi[n2] = ipi[n1];
  CPPUNIT_ASSERT(ipi[n2] == ipi[n1]);
  CPPUNIT_ASSERT(ip[n2] == ip[n1]);
  CPPUNIT_ASSERT(ipi[e1] == "1");
  CPPUNIT_ASSERT(ip[e1] == 1);
  ipi[e2] = ipi[e1];
  CPPUNIT_ASSERT(ipi[e2] == "1");
  CPPUNIT_ASSERT(ipi[e2] == ipi[e1]);
  CPPUNIT_ASSERT(ip[e2] == ip[e1]);
}

void PropertyArraySubscriptTest::testDoubleProperty() {
  DoubleProperty &dp = *graph->getDoubleProperty("double");

  dp[e1] = dp[n1] = 2;
  dp[e2] = dp[n2] = dp[n1] + 1;
  dp[e3] = dp[n3] = dp[n1] - 2;

  CPPUNIT_ASSERT(dp[n1]);
  CPPUNIT_ASSERT(dp[n2] == 3);
  CPPUNIT_ASSERT(dp[n2] >= dp[n1]);
  CPPUNIT_ASSERT(dp[n3] < dp[n1]);
  CPPUNIT_ASSERT(!dp[n3]);
  CPPUNIT_ASSERT(dp[e1] == 2);
  CPPUNIT_ASSERT(dp[e2] == 3);
  CPPUNIT_ASSERT(dp[e2] >= dp[n1]);
  CPPUNIT_ASSERT(dp[e3] < dp[n1]);
  CPPUNIT_ASSERT(!dp[e3]);

  ++dp[n1];
  CPPUNIT_ASSERT(dp[n1] == 3);
  auto d1 = dp[n1]++;
  CPPUNIT_ASSERT(dp[n1] == 4);
  CPPUNIT_ASSERT(d1 == 3);
  --dp[n1];
  CPPUNIT_ASSERT(dp[n1] == 3);
  d1 = dp[n1]--;
  CPPUNIT_ASSERT(dp[n1] == 2);
  CPPUNIT_ASSERT(d1 == 3);
  ++dp[e1];
  CPPUNIT_ASSERT(dp[e1] == 3);
  d1 = dp[e1]++;
  CPPUNIT_ASSERT(dp[e1] == 4);
  CPPUNIT_ASSERT(d1 == 3);
  --dp[e1];
  CPPUNIT_ASSERT(dp[e1] == 3);
  d1 = dp[e1]--;
  CPPUNIT_ASSERT(dp[e1] == 2);
  CPPUNIT_ASSERT(d1 == 3);
  dp[e1] += d1;
  CPPUNIT_ASSERT(dp[e1] == 5);
  dp[e1] -= d1;
  CPPUNIT_ASSERT(dp[e1] == 2);

  // test as PropertyInterface
  auto &dpi = *graph->getProperty("double");
  CPPUNIT_ASSERT(dpi[n1] == "2");
  CPPUNIT_ASSERT(dp[n1] == 2);
  dpi[n2] = dpi[n1];
  CPPUNIT_ASSERT(dpi[n2] == dpi[n1]);
  CPPUNIT_ASSERT(dp[n2] == dp[n1]);
  CPPUNIT_ASSERT(dpi[e1] == "2");
  CPPUNIT_ASSERT(dp[e1] == 2);
  dpi[e2] = dpi[e1];
  CPPUNIT_ASSERT(dpi[e2] == dpi[e1]);
  CPPUNIT_ASSERT(dp[e2] == dp[e1]);
  CPPUNIT_ASSERT(dpi[e2] == "2");
}

void PropertyArraySubscriptTest::testLayoutProperty() {
  LayoutProperty &lp = *graph->getLayoutProperty("layout");

  lp[n1] = {1.f, 2.f, 3.f};
  lp[n2] = {4.f, 5.f, 6.f};

  lp[e1] = {lp[n1], lp[n2]};
  lp[e2] = {lp[n2], lp[n1]};

  const Coord &c = lp[n1];
  lp[n3] = c;

  const vector<Coord> &vc = lp[e1];
  lp[e3] = vc;

  CPPUNIT_ASSERT(lp[n2] != lp[n1]);
  CPPUNIT_ASSERT(lp[n3] == lp[n1]);

  CPPUNIT_ASSERT(lp[e2] != lp[e1]);
  CPPUNIT_ASSERT(lp[e3] == lp[e1]);
}
