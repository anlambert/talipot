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

#if _MSC_VER
// disable Visual Studio warning about using this in base member initializer list
// this is not critical in our case as we only store a pointer
#pragma warning(disable : 4355)
#endif

#include <talipot/Observable.h>
#include <talipot/ConversionIterator.h>
#include <talipot/FilterIterator.h>
#include <talipot/Exception.h>
#include <talipot/GraphStorage.h>
#include <talipot/VectorProperty.h>

using namespace std;
using namespace tlp;

namespace tlp {

// we need a lock to protect the update
// of the observation graph
TLP_DEFINE_GLOBAL_LOCK(ObservableGraphUpdate);
//=================================

static bool oGraphDestroyed = false;
class ObservationGraph : public GraphStorage {
public:
  // a pointer to the object represented by a node
  tlp::NodeVectorProperty<Observable *> pointer;
  // whether an object has been deleted or not
  tlp::NodeVectorProperty<bool> alive;
  // the count of events scheduled to be treated by an object
  // the object's associated node is deleted only when this count is null
  // in order to prevent the node reuse and ensure the alive associated value
  tlp::NodeVectorProperty<uint> eventsToTreat;
  // the type of relation between two Observable Objects
  tlp::EdgeVectorProperty<unsigned char> type;

  ~ObservationGraph() {
    oGraphDestroyed = true;
  }
};

static ObservationGraph observationGraph;

//_oDelayedDelNode store deleted nodes, to remove them at the end of the notify
static std::vector<tlp::node> _oDelayedDelNode;
static std::set<std::pair<tlp::node, tlp::node>> _oDelayedEvents;
//_oNotifying counter of nested notify calls
static uint _oNotifying = 0;
//_oUnholding counter of nested unhold calls
static uint _oUnholding = 0;
//----------------------------------
uint Observable::_oHoldCounter = 0;
bool Observable::_oDisabled = false;

class ObservableException : public tlp::Exception {
public:
  ObservableException(const std::string &desc) : tlp::Exception(desc) {}
};

//----------------------------------
Iterator<node> *Observable::getInObjects() const {
  assert(_n.isValid());
  return filterIterator(observationGraph.getInNodes(_n),
                        [&](node n) { return observationGraph.alive[n.id]; });
}
//----------------------------------
Iterator<node> *Observable::getOutObjects() const {
  assert(_n.isValid());
  return filterIterator(observationGraph.getOutNodes(_n),
                        [&](node n) { return observationGraph.alive[n.id]; });
}
//----------------------------------
node Observable::getNode() const {
  return _n;
}
//----------------------------------
node Observable::getBoundNode() {
  if (!_n.isValid()) {
    _n = observationGraph.addNode();
    observationGraph.pointer[_n.id] = this;
    observationGraph.alive[_n.id] = true;
    observationGraph.eventsToTreat[_n.id] = 0;
  }
  return _n;
}
//----------------------------------
bool Observable::isAlive(tlp::node n) {
  return observationGraph.alive[n.id];
}
//----------------------------------
uint Observable::getScheduled(tlp::node n) {
  return observationGraph.eventsToTreat[n.id];
}
//----------------------------------
Observable *Observable::getObject(node n) {
  assert(observationGraph.alive[n.id]);

  if (!observationGraph.alive[n.id]) {
    throw ObservableException("That object has been deleted it is no more accessible");
  }

  return observationGraph.pointer[n.id];
}
//----------------------------------
tlp::node Observable::getNode(const Observable *obs) {
  return obs->_n;
}
//=================================
Event::Event(const Observable &sender, EventType type) : _sender(sender._n), _type(type) {
  assert(_type != EventType::TLP_DELETE);

  if (_type == EventType::TLP_DELETE) {
    throw ObservableException("It is forbidden to create a delete events, DeleteEvents are "
                              "autmotically generated at the observable destruction");
  }
}
//----------------------------------
Event::~Event() = default;
//----------------------------------
Observable *Event::sender() const {
  return Observable::getObject(_sender); /** only Observable can be use to create event */
}
//=================================
// define a class for an empty Iterator of Observable *
class NoObservableIterator : public Iterator<Observable *> {
public:
  Observable *next() override {
    return nullptr;
  }
  bool hasNext() override {
    return false;
  }
};
//=================================
void Observable::treatEvents(const std::vector<Event> &) {
  tlp::debug() << __PRETTY_FUNCTION__ << ": not implemented" << std::endl;
}
//=================================
void Observable::treatEvent(const Event &) {
  tlp::debug() << __PRETTY_FUNCTION__ << ": not implemented" << std::endl;
}
//=================================
Observable::Observable() : _deleteMsgSent(false), _queuedEvent(false) {
  _sent = _received = 0;
}
//----------------------------------
Observable::Observable(const Observable &) : _deleteMsgSent(false), _queuedEvent(false) {
  _sent = _received = 0;
}
//----------------------------------
Observable &Observable::operator=(const Observable &) {
  tlp::debug() << "[Observable Warning]: Observable object should reimplement their operator= else "
                  "nothing is copied"
               << endl;
  return *this;
}
//----------------------------------
Observable::~Observable() {
  if (oGraphDestroyed || !_n.isValid()) {
    return;
  }

  if (!_deleteMsgSent) {
    observableDeleted();
  }

  TLP_GLOBALLY_LOCK_SECTION(ObservableGraphUpdate) {
    assert(observationGraph.alive[_n.id]);

    if (!observationGraph.alive[_n.id]) {
      tlp::error() << "[ERROR]: in " << __PRETTY_FUNCTION__ << " at " << __FILE__ << ':' << __LINE__
                   << endl;
      tlp::error() << "Observable object has already been deleted, possible double free!!!" << endl;
      std::terminate();
    }

    observationGraph.alive[_n.id] = false;

    bool noDelay = (_oNotifying == 0) && (_oUnholding == 0) && (_oHoldCounter == 0);

    if ((!noDelay) && (observationGraph.eventsToTreat[_n.id] == 0)) {
      // _n cannot be deleted only if it is observed
      // then its deletion is delayed until the observers are unhold
      noDelay = true;
      for (auto e : observationGraph.incidence(_n)) {
        if (_n == observationGraph.target(e) && observationGraph.type[e.id] & OBSERVER) {
          noDelay = false;
          break;
        }
      }
    }

    if (noDelay) {
      assert(observationGraph.eventsToTreat[_n.id] == 0);
      observationGraph.delNode(_n);
    } else {
      _oDelayedDelNode.push_back(_n);
      vector<edge> incidence = observationGraph.incidence(_n);
      for (auto e : incidence) {
        observationGraph.delEdge(e);
      }
    }
  }
  TLP_GLOBALLY_UNLOCK_SECTION(ObservableGraphUpdate);
}

//----------------------------------------
void Observable::holdObservers() {
  TLP_GLOBALLY_LOCK_SECTION(ObservableGraphUpdate) {
    ++_oHoldCounter;
  }
  TLP_GLOBALLY_UNLOCK_SECTION(ObservableGraphUpdate);
}
//----------------------------------------
void Observable::unholdObservers() {
  {
    assert(_oHoldCounter > 0);

    if (_oHoldCounter == 0) {
#ifndef NDEBUG
      throw ObservableException("unhold call without a previous call to hold");
#endif
      return;
    }

    --_oHoldCounter;
    {
      if (_oHoldCounter > 0 || _oDelayedEvents.empty()) {
        return;
      }

      ++_oUnholding;
      ++_oHoldCounter; /** rehold the observer to buffer message sent during unholding */

      set<pair<node, node>> backupEvents;
      backupEvents.swap(_oDelayedEvents);
      map<node, vector<Event>> preparedEvents;

      for (const auto &[src, tgt] : backupEvents) {
        if (observationGraph.alive[src.id]) {
          auto *sender = static_cast<Observable *>(observationGraph.pointer[src.id]);
          sender->_queuedEvent = false;
          if (observationGraph.alive[tgt.id]) {
            observationGraph.eventsToTreat[tgt.id] += 1;
            preparedEvents[tgt].push_back(Event(*sender, EventType::TLP_MODIFICATION));
          }
        }
      }

      {

        for (const auto &[n, events] : preparedEvents) {
          // treat scheduled events
          observationGraph.eventsToTreat[n.id] -= events.size();

          if (observationGraph.alive[n.id]) {
            auto *obs = static_cast<Observable *>(observationGraph.pointer[n.id]);
            ++(obs->_received);
            obs->treatEvents(events);
          }
        }
      }

      --_oUnholding;
      unholdObservers(); /** treat queued events during the unhold */
      updateObserverGraph();

      if (_oHoldCounter > 0) {
        throw ObservableException("Observable after unholdd call, bad nested hold/unhold function "
                                  "call in an Observer:: update() function");
      }
    }
  }
}
//----------------------------------------
Iterator<Observable *> *Observable::getOnlookers() const {
  if (isBound()) {
    assert(observationGraph.alive[_n.id]);

    if (!observationGraph.alive[_n.id]) {
      throw ObservableException("getObservers called on a deleted Observable");
    }

    return conversionIterator<Observable *>(
        getInObjects(), [&](const node &n) { return observationGraph.pointer[n.id]; });
  }

  return new NoObservableIterator();
}
//----------------------------------------
void Observable::addOnlooker(const Observable &obs, OBSERVABLEEDGETYPE type) const {

  TLP_GLOBALLY_LOCK_SECTION(ObservableGraphUpdate) {

    if (isBound() && !observationGraph.alive[_n.id]) {
      throw ObservableException("addObserver called on a deleted Observable");
    }

    // check for an existing link
    edge link;

    if (isBound() && obs.isBound()) {
      auto edges = observationGraph.getEdges(obs._n, _n, false);
      if (!edges.empty()) {
        link = edges[0];
      }
    }

    if (!link.isValid()) {
      // add new link
      // at this time both Observables need to be bound
      link = observationGraph.addEdge(const_cast<Observable &>(obs).getBoundNode(),
                                      const_cast<Observable *>(this)->getBoundNode());
      observationGraph.type[link.id] = type;
    } else {
      // add the bit for the given type on the edge
      observationGraph.type[link.id] |= type;
    }
  }
  TLP_GLOBALLY_UNLOCK_SECTION(ObservableGraphUpdate);
}
//----------------------------------------
void Observable::addObserver(Observable *const observer) const {
  assert(observer != nullptr);
  _queuedEvent = false;
  addOnlooker(*observer, OBSERVER);
}
//----------------------------------------
void Observable::addListener(Observable *const listener) const {
  assert(listener != nullptr);
  addOnlooker(*listener, LISTENER);
}
//----------------------------------------
void Observable::observableDeleted() {
  assert(_deleteMsgSent == false);

  if (_deleteMsgSent) {
    throw ObservableException("Delete message has been sent several time.");
  }

  _deleteMsgSent = true;

  if (hasOnlookers()) {
    Event msg(*this, EventType::TLP_INVALID); // create a modify event to prevent raise exception,
                                              // (hack) to forbid creation of Delete exception
                                              // without calling that function
    msg._type = EventType::TLP_DELETE;
    sendEvent(msg);
  }
}
//----------------------------------------
void Observable::sendEvent(const Event &message) {
  if ((_oDisabled && message._type != EventType::TLP_DELETE) || !isBound()) {
    return;
  }

  if (!observationGraph.isElement(_n) || !observationGraph.alive[_n.id]) {
    throw ObservableException("Notify called on a deleted Observable");
  }

  const uint RECCALL = 200;

  if (_oNotifying > RECCALL) {
    std::stringstream str;
    str << "Maximum number of nested calls (" << RECCALL << ") reached";
    throw ObservableException(str.str());
  }

  node backn = _n; /** to keep trace of the node if the observable is deleted during the
                      notification, in that crazy case, (*this) is dead thus n is not accessible*/
  ++_oNotifying;

  // create two separate list of observer & listeners
  // could be extended if we need recorders
  vector<pair<Observable *, node>> observerTonotify;
  vector<pair<Observable *, node>> listenerTonotify;
  bool delayedEventAdded = false;
  for (auto e : observationGraph.incidence(_n)) {
    node src = observationGraph.source(e);

    if (_n != src && observationGraph.alive[src.id]) {
      Observable *obs = observationGraph.pointer[src.id];
      assert(obs != nullptr);

      if ((observationGraph.type[e.id] & OBSERVER) &&
          (message.type() != EventType::TLP_INFORMATION)) {
        if (_oHoldCounter == 0 || message.type() == EventType::TLP_DELETE) {
          // schedule event
          observationGraph.eventsToTreat[backn.id] += 1;
          observationGraph.eventsToTreat[src.id] += 1;
          observerTonotify.push_back({obs, src});
        } else if (!_queuedEvent) {
          delayedEventAdded = true;
          TLP_GLOBALLY_LOCK_SECTION(ObservableGraphUpdate) {
            _oDelayedEvents.insert({_n, src});
          }
          TLP_GLOBALLY_UNLOCK_SECTION(ObservableGraphUpdate);
        }
      }

      if (observationGraph.type[e.id] & LISTENER) {
        // schedule event
        observationGraph.eventsToTreat[backn.id] += 1;
        observationGraph.eventsToTreat[src.id] += 1;
        listenerTonotify.push_back({obs, src});
      }
    }
  }

  if (delayedEventAdded) {
    _queuedEvent = true;
  }

  // send message to listeners
  for (const auto &[obs, n] : listenerTonotify) {
    if (n == backn && message.type() == EventType::TLP_DELETE) {
      tlp::debug() << "[Observable info]: An observable onlook itself Event::DELETE msg can't be "
                      "sent to it."
                   << endl;
      // treat scheduled event
      observationGraph.eventsToTreat[backn.id] -= 2;
      continue;
    }

    // treat scheduled event
    observationGraph.eventsToTreat[n.id] -= 1;

    if (observationGraph.alive[n.id]) { // other listeners/observers could be
                                        // destroyed during the treat event
      ++(obs->_received);
      obs->treatEvent(message);
    }

    // we decrement after treating event
    // to prevent reuse of backn
    observationGraph.eventsToTreat[backn.id] -= 1;

    assert(observationGraph.alive[backn.id]);

    if (!observationGraph.alive[backn.id]) {
      throw ObservableException("An observable has been deleted during the notifification of its "
                                "observer (ie. an observer has deleted its caller during an "
                                "update)");
    }
  }

  // send simple event to observers
  if (!observerTonotify.empty()) {
    vector<Event> tmp(1, message);

    for (const auto &[obs, n] : observerTonotify) {
      if (n == backn && message.type() == EventType::TLP_DELETE) {
        tlp::debug() << "[Observable info]: An observable onlook itself Event::DELETE msg can't be "
                        "sent to it."
                     << endl;
        // treat scheduled event
        observationGraph.eventsToTreat[backn.id] -= 2;
        continue;
      }

      // treat scheduled event
      observationGraph.eventsToTreat[n.id] -= 1;

      if (observationGraph.alive[n.id]) { // other listeners/observers could be
                                          // destroyed during the treat event
        ++(obs->_received);
        obs->treatEvents(tmp);
      }

      // we decrement after treating event
      // to prevent reuse of backn
      observationGraph.eventsToTreat[backn.id] -= 1;

      assert(observationGraph.alive[backn.id]);

      if (!observationGraph.alive[backn.id]) {
        throw ObservableException("An observable has been deleted during the notifification of its "
                                  "observer (ie. an observer has deleted its caller during an "
                                  "update)");
      }
    }
  }

  ++_sent;
  --_oNotifying;

  if (!observerTonotify.empty() || !listenerTonotify.empty() ||
      message.type() == EventType::TLP_DELETE) {
    updateObserverGraph();
  }
}
//----------------------------------------
void Observable::updateObserverGraph() {
  if (_oNotifying == 0 && _oUnholding == 0 && _oHoldCounter == 0) {
    TLP_GLOBALLY_LOCK_SECTION(ObservableGraphUpdate) {
      for (auto toDel : _oDelayedDelNode) {
        if (observationGraph.eventsToTreat[toDel.id] == 0) {
          observationGraph.delNode(toDel);
        }
      }
    }
    TLP_GLOBALLY_UNLOCK_SECTION(ObservableGraphUpdate);
    _oDelayedDelNode.clear();
  }
}
//----------------------------------------
void Observable::removeOnlooker(const Observable &obs, OBSERVABLEEDGETYPE type) const {
  // nothing to do if one of the observables is unbound
  if (!isBound() || !obs.isBound()) {
    return;
  }

  TLP_GLOBALLY_LOCK_SECTION(ObservableGraphUpdate) {
    assert(observationGraph.alive[_n.id]);

    if (!observationGraph.alive[_n.id]) {
      throw ObservableException("removeOnlooker called on a deleted Observable");
    }

    auto edges = observationGraph.getEdges(obs._n, _n, false);
    edge link;
    if (!edges.empty()) {
      link = edges[0];
    }

    if (link.isValid()) {
      observationGraph.type[link.id] =
          observationGraph.type[link.id] &
          ~type; // bitwise operation to remove the bit  for the given type on the edge

      if (observationGraph.type[link.id] == 0) {
        observationGraph.delEdge(link);
      }
    }
  }
  TLP_GLOBALLY_UNLOCK_SECTION(ObservableGraphUpdate);
}
//----------------------------------------
void Observable::removeObserver(Observable *const observerver) const {
  assert(observerver != nullptr);
  removeOnlooker(*observerver, OBSERVER);
}
//----------------------------------------
void Observable::removeListener(Observable *const listener) const {
  assert(listener != nullptr);
  removeOnlooker(*listener, LISTENER);
}
//----------------------------------------
bool Observable::hasOnlookers() const {
  if (!isBound()) {
    return false;
  }

  if (!observationGraph.alive[_n.id]) {
    throw ObservableException("hasOnlookers called on a deleted Observable");
  }

  return observationGraph.indeg(_n) > 0;
}
//----------------------------------------
uint Observable::countListeners() const {
  if (!hasOnlookers()) {
    return 0;
  }

  uint count = 0;
  for (auto e : observationGraph.incidence(_n)) {
    if (_n == observationGraph.target(e) && (observationGraph.type[e.id] & LISTENER)) {
      ++count;
    }
  }
  return count;
}
//----------------------------------------
uint Observable::countObservers() const {
  if (!hasOnlookers()) {
    return 0;
  }

  uint count = 0;
  for (auto e : observationGraph.incidence(_n)) {
    if (_n == observationGraph.target(e) && (observationGraph.type[e.id] & OBSERVER)) {
      ++count;
    }
  }
  return count;
}
//----------------------------------------
const std::vector<tlp::node> &Observable::nodes() {
  return observationGraph.nodes();
}
}
