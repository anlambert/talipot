/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ObservablePropertyTest.h"
#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>

using namespace std;
using namespace tlp;

const uint NB_NODES = 4;
const uint EDGE_RATIO = 3;
const uint NB_EDGES = EDGE_RATIO * NB_NODES;

CPPUNIT_TEST_SUITE_REGISTRATION(ObservablePropertyTest);

// these classes will capture
// everything that will happen to our properties
// synchronously or asynchronously
class ObserverPTest : public Observable {
public:
  std::set<Observable *> observables;

  ObserverPTest() = default;

  void reset() {
    observables.clear();
  }

  uint nbObservables() const {
    return observables.size();
  }

  bool found(Observable *obs) {
    return observables.contains(obs);
  }

  void treatEvents(const vector<Event> &events) override {
    if (events[0].type() == EventType::TLP_DELETE) {
      observables.insert(events[0].sender());
    } else {
      reset();

      for (const auto &event : events) {
        observables.insert(event.sender());
      }
    }
  }
};

static ObserverPTest *observer;

// this class will capture
// everything that will happen to our properties
class PropertyObserverTest : public Observable {
public:
  std::set<PropertyInterface *> properties;
  node lastNode;
  edge lastEdge;

  PropertyObserverTest() = default;

  void reset() {
    properties.clear();
  }

  uint nbProperties() const {
    return properties.size();
  }

  bool found(PropertyInterface *prop) {
    return properties.contains(prop);
  }

  node getNode() const {
    return lastNode;
  }

  edge getEdge() const {
    return lastEdge;
  }

  void beforeSetNodeValue(PropertyInterface *prop, const node n) {
    properties.insert(prop);
    lastNode = n;
  }

  virtual void beforeSetEdgeValue(PropertyInterface *prop, const edge e) {
    properties.insert(prop);
    lastEdge = e;
  }

  virtual void beforeSetAllNodeValue(PropertyInterface *prop) {
    properties.insert(prop);
  }
  virtual void beforeSetAllEdgeValue(PropertyInterface *prop) {
    properties.insert(prop);
  }
  virtual void destroy(PropertyInterface *prop) {
    properties.insert(prop);
  }
  void treatEvent(const Event &evt) override {
    const auto *propEvt = dynamic_cast<const PropertyEvent *>(&evt);

    if (propEvt) {
      PropertyInterface *prop = propEvt->getProperty();

      switch (propEvt->getType()) {
      case PropertyEventType::TLP_BEFORE_SET_NODE_VALUE:
        beforeSetNodeValue(prop, propEvt->getNode());
        return;

      case PropertyEventType::TLP_BEFORE_SET_EDGE_VALUE:
        beforeSetEdgeValue(prop, propEvt->getEdge());
        return;

      case PropertyEventType::TLP_BEFORE_SET_ALL_NODE_VALUE:
        beforeSetAllNodeValue(prop);
        return;

      case PropertyEventType::TLP_BEFORE_SET_ALL_EDGE_VALUE:
        beforeSetAllEdgeValue(prop);
        return;
      default:
        return;
      }
    } else {
      auto *prop = dynamic_cast<PropertyInterface *>(evt.sender());

      if (prop && evt.type() == EventType::TLP_DELETE) {
        destroy(prop);
      }
    }
  }
};

static PropertyObserverTest *pObserver;

#define DOUBLE_PROP 2
#define INTEGER_PROP 3
#define LAYOUT_PROP 4
#define SIZE_PROP 5
//==========================================================
void ObservablePropertyTest::setUp() {
#ifndef NDEBUG
  // choose the seed to ease the debugging
  tlp::setSeedOfRandomSequence(1);
  tlp::initRandomSequence();
#endif

  graph = tlp::newGraph();

  props[0] = graph->getBooleanProperty("boolProp");
  props[1] = graph->getColorProperty("colorProp");
  props[2] = graph->getDoubleProperty("doubleProp");
  props[3] = graph->getIntegerProperty("intProp");
  props[4] = graph->getLayoutProperty("layoutProp");
  props[5] = graph->getSizeProperty("sizeProp");
  props[6] = graph->getStringProperty("stringProp");

  vector<node> nodes = graph->addNodes(NB_NODES);

  for (uint i = 0; i < NB_EDGES; ++i) {
    graph->addEdge(graph->getRandomNode(), graph->getRandomNode());
  }

  observer = new ObserverPTest();
  pObserver = new PropertyObserverTest();

  addObservers();
}

//==========================================================
void ObservablePropertyTest::tearDown() {
  delete graph;
  delete observer;
  delete pObserver;
}

void ObservablePropertyTest::setNodeValue(PropertyInterface *prop, const char *val, bool all,
                                          bool found1, bool found2) {
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  pObserver->reset();
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);

  if (all) {
    prop->setAllNodeStringValue(string(val));
    CPPUNIT_ASSERT(observer->nbObservables() == (found1 ? 1 : 0));
    CPPUNIT_ASSERT(observer->found(prop) == found1);
    CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
    CPPUNIT_ASSERT(pObserver->found(prop) == found2);
  } else {
    for (auto n : graph->nodes()) {
      prop->setNodeStringValue(n, string(val));
      CPPUNIT_ASSERT(observer->nbObservables() == (found1 ? 1 : 0));
      CPPUNIT_ASSERT(observer->found(prop) == found1);
      CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
      CPPUNIT_ASSERT(pObserver->found(prop) == found2);
      CPPUNIT_ASSERT(!found2 || pObserver->getNode() == n);
    }
  }
}

void ObservablePropertyTest::setEdgeValue(PropertyInterface *prop, const char *val, bool all,
                                          bool found1, bool found2) {
  observer->reset();
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  pObserver->reset();
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);

  if (all) {
    prop->setAllEdgeStringValue(string(val));
    CPPUNIT_ASSERT(observer->nbObservables() == (found1 ? 1 : 0));
    CPPUNIT_ASSERT(observer->found(prop) == found1);
    CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
    CPPUNIT_ASSERT(pObserver->found(prop) == found2);
  } else {
    for (auto e : graph->edges()) {
      prop->setEdgeStringValue(e, string(val));
      CPPUNIT_ASSERT(observer->nbObservables() == (found1 ? 1 : 0));
      CPPUNIT_ASSERT(observer->found(prop) == found1);
      CPPUNIT_ASSERT(pObserver->nbProperties() == (found2 ? 1 : 0));
      CPPUNIT_ASSERT(pObserver->found(prop) == found2);
      CPPUNIT_ASSERT(!found2 || pObserver->getEdge() == e);
    }
  }
}

//==========================================================
void ObservablePropertyTest::addObservers() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);

  for (auto *prop : props) {
    prop->addObserver(observer);
    CPPUNIT_ASSERT(observer->nbObservables() == 0);
    prop->addListener(pObserver);
    CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  }
}
//==========================================================
void ObservablePropertyTest::testAddObserver() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);

  for (auto *prop : props) {
    CPPUNIT_ASSERT(prop->countObservers() == 1);
    // CPPUNIT_ASSERT(props[i]->countPropertyObservers() == 1); //same as above
  }
}
//==========================================================
void ObservablePropertyTest::testSynchronousSetNodeValue() {
  setNodeValue(props[0], "true", false, true);
  setNodeValue(props[1], "(255, 255, 255, 0)", false, true);
  setNodeValue(props[2], "1.0", false, true);
  setNodeValue(props[3], "1", false, true);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", false, true);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", false, true);
  setNodeValue(props[6], "talipot", false, true);
}

//==========================================================
void ObservablePropertyTest::testSynchronousSetEdgeValue() {
  setEdgeValue(props[0], "true", false, true);
  setEdgeValue(props[1], "(255, 255, 255, 0)", false, true);
  setEdgeValue(props[2], "1.0", false, true);
  setEdgeValue(props[3], "1", false, true);
  setEdgeValue(props[4], "()", false, true);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", false, true);
  setEdgeValue(props[6], "talipot", false, true);
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetNodeValue() {
  Observable::holdObservers();
  setNodeValue(props[0], "true", false, false);
  setNodeValue(props[1], "(255, 255, 255, 0)", false, false);
  setNodeValue(props[2], "1.0", false, false);
  setNodeValue(props[3], "1", false, false);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", false, false);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", false, false);
  setNodeValue(props[6], "talipot", false, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);

  for (auto *prop : props) {
    CPPUNIT_ASSERT(observer->found(prop));
  }
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetEdgeValue() {
  Observable::holdObservers();
  setEdgeValue(props[0], "true", false, false);
  setEdgeValue(props[1], "(255, 255, 255, 0)", false, false);
  setEdgeValue(props[2], "1.0", false, false);
  setEdgeValue(props[3], "1", false, false);
  setEdgeValue(props[4], "()", false, false);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", false, false);
  setEdgeValue(props[6], "talipot", false, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);

  for (auto *prop : props) {
    CPPUNIT_ASSERT(observer->found(prop));
  }
}

//==========================================================
void ObservablePropertyTest::testSynchronousSetAllNodeValue() {
  setNodeValue(props[0], "true", true, true);
  setNodeValue(props[1], "(255, 255, 255, 0)", true, true);
  setNodeValue(props[2], "1.0", true, true);
  setNodeValue(props[3], "1", true, true);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", true, true);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", true, true);
  setNodeValue(props[6], "talipot", true, true);
}

//==========================================================
void ObservablePropertyTest::testSynchronousSetAllEdgeValue() {
  setEdgeValue(props[0], "true", true, true);
  setEdgeValue(props[1], "(255, 255, 255, 0)", true, true);
  setEdgeValue(props[2], "1.0", true, true);
  setEdgeValue(props[3], "1", true, true);
  setEdgeValue(props[4], "()", true, true);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", true, true);
  setEdgeValue(props[6], "talipot", true, true);
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetAllNodeValue() {
  Observable::holdObservers();
  setNodeValue(props[0], "true", true, false);
  setNodeValue(props[1], "(255, 255, 255, 0)", true, false);
  setNodeValue(props[2], "1.0", true, false);
  setNodeValue(props[3], "1", true, false);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", true, false);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", true, false);
  setNodeValue(props[6], "talipot", true, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);

  for (auto *prop : props) {
    CPPUNIT_ASSERT(observer->found(prop));
  }
}

//==========================================================
void ObservablePropertyTest::testAsynchronousSetAllEdgeValue() {
  Observable::holdObservers();
  setEdgeValue(props[0], "true", true, false);
  setEdgeValue(props[1], "(255, 255, 255, 0)", true, false);
  setEdgeValue(props[2], "1.0", true, false);
  setEdgeValue(props[3], "1", true, false);
  setEdgeValue(props[4], "()", true, false);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", true, false);
  setEdgeValue(props[6], "talipot", true, false);
  Observable::unholdObservers();
  CPPUNIT_ASSERT(observer->nbObservables() == 7);

  for (auto *prop : props) {
    CPPUNIT_ASSERT(observer->found(prop));
  }
}

//==========================================================
void ObservablePropertyTest::testSynchronousDelete() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);

  for (uint i = 0; i < 7; ++i) {
    PropertyInterface *prop = props[i];
    graph->delLocalProperty(prop->getName());
    CPPUNIT_ASSERT(observer->nbObservables() == i + 1);
    CPPUNIT_ASSERT(observer->found(prop));
    CPPUNIT_ASSERT(pObserver->nbProperties() == i + 1);
    CPPUNIT_ASSERT(pObserver->found(prop));
  }
}

//==========================================================
void ObservablePropertyTest::testAsynchronousDelete() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  Observable::holdObservers();

  for (uint i = 0; i < 7; ++i) {
    PropertyInterface *prop = props[i];
    graph->delLocalProperty(prop->getName());
    // deletion is not asynchronous
    CPPUNIT_ASSERT(observer->nbObservables() == i + 1);
    CPPUNIT_ASSERT(observer->found(prop));
    CPPUNIT_ASSERT(pObserver->nbProperties() == i + 1);
    CPPUNIT_ASSERT(pObserver->found(prop));
  }

  Observable::unholdObservers();
}

//==========================================================
void ObservablePropertyTest::testRemoveObserver() {
  CPPUNIT_ASSERT(observer->nbObservables() == 0);
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);

  for (auto *prop : props) {
    prop->removeObserver(observer);
    prop->removeListener(pObserver);
    CPPUNIT_ASSERT(prop->countObservers() == 0);
    // CPPUNIT_ASSERT(props[i]->countPropertyObservers() == 0); same as above
  }

  // no more notification
  setNodeValue(props[0], "true", true, false, false);
  setNodeValue(props[1], "(255, 255, 255, 0)", true, false, false);
  setNodeValue(props[2], "1.0", true, false, false);
  setNodeValue(props[3], "1", true, false, false);
  setNodeValue(props[4], "(0.0, 0.0, 0.0)", true, false, false);
  setNodeValue(props[5], "(0.0, 0.0, 0.0)", true, false, false);
  setNodeValue(props[6], "talipot", true, false, false);
  setEdgeValue(props[0], "true", true, false, false);
  setEdgeValue(props[1], "(255, 255, 255, 0)", true, false, false);
  setEdgeValue(props[2], "1.0", true, false, false);
  setEdgeValue(props[3], "1", true, false, false);
  setEdgeValue(props[4], "()", true, false, false);
  setEdgeValue(props[5], "(0.0, 0.0, 0.0)", true, false, false);
  setEdgeValue(props[6], "talipot", true, false, false);

  for (auto *prop : props) {
    graph->delLocalProperty(prop->getName());
    CPPUNIT_ASSERT(observer->nbObservables() == 0);
    CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
  }
}

//==========================================================
void ObservablePropertyTest::testObserverWhenRemoveObservable() {
  CPPUNIT_ASSERT(props[0]->countListeners() == 1);
  CPPUNIT_ASSERT(props[0]->countObservers() == 1);
  auto *pObserverTmp = new PropertyObserverTest();
  props[0]->addListener(pObserverTmp);
  CPPUNIT_ASSERT(props[0]->countListeners() == 2);
  CPPUNIT_ASSERT(props[0]->countObservers() == 1);
  delete pObserverTmp;
  CPPUNIT_ASSERT(props[0]->countListeners() == 1);
  CPPUNIT_ASSERT(props[0]->countObservers() == 1);
}

void ObservablePropertyTest::testNoPropertiesEventsAfterGraphClear() {
  // create a clone subgraph with a local property
  Graph *clone = graph->addCloneSubGraph("clone");
  PropertyInterface *cloneProp = clone->getDoubleProperty("cloneProp");
  // listen to local property of clone subgraph
  cloneProp->addListener(pObserver);

  // clear the clone subgraph (remove nodes/edges)
  clone->clear();
  // check that no properties events have been received
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);

  // delete the local property
  clone->delLocalProperty(cloneProp->getName());
  // reset the property observer as it just received the notification that a property has been
  // deleted
  pObserver->reset();

  // clear the graph (remove nodes/edges/subgraphs)
  graph->clear();
  // check that no properties events have been received
  CPPUNIT_ASSERT(pObserver->nbProperties() == 0);
}
