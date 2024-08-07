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

namespace tlp {

enum class EventType {TLP_DELETE = 0, TLP_MODIFICATION = 1, TLP_INFORMATION = 2, TLP_INVALID = 2};

class Event {
%Docstring
This class is the base one for representing an event in Talipot. Events are objects sent by objects
whose classes derives from the :class:`tlp.Observable` one. An event is characterized by its type.
The base :class:`tlp.Event` class only carries information as to the type of event, nothing specific
:

   -  :const:`tlp.EventType.TLP_DELETE` : send directly to all observers/listeners, not affected by
      :meth:`tlp.Observable.holdObservers`.
   -  :const:`tlp.EventType.TLP_MODIFICATION` : sent to all observers/listeners. That type of event
      is first sent to observers then to listeners.
   -  :const:`tlp.EventType.TLP_INFORMATION` : sent only to listeners.
   -  :const:`tlp.EventType.TLP_INVALID` : never sent, used internally for delaying events.

Implement a class derived from :class:`tlp.Event` to add custom events. Here is a sample code below:

.. code::

   class MyEvent(tlp.Event):

     # sender must be an instance deriving from tlp.Observable
     def __init__(self, sender, data):
       tlp.Event.__init__(self, sender, tlp.EventType.TLP_MODIFICATION)
       self.data = data

     def getData(self):
       return self.data
%End

%TypeHeaderCode
#include <talipot/Observable.h>
%End

%ConvertToSubClassCode
sipType = NULL;
%End


// =======================================================================================================

public:

  Event(const tlp::Observable &sender, tlp::EventType type);

  virtual ~Event();

// =======================================================================================================

  tlp::Observable* sender() const;
%Docstring
tlp.Event.sender()

Returns the object that sent the event.

:rtype:
   :class:`tlp.Observable` or one of its derived type.
%End

// =======================================================================================================

  EventType type() const;
%Docstring
tlp.Event.type()

Returns the type of the event.

:rtype:
   :const:`tlp.EventType.TLP_DELETE` or :const:`tlp.EventType.TLP_MODIFICATION` or
   :const:`tlp.EventType.TLP_INFORMATION` or :const:`tlp.EventType.TLP_INVALID`
%End
};


// =======================================================================================================

class Observable {
%Docstring
The :class:`tlp.Observable` class is the base of Talipot's observation system.

Each class that wishes to send or receive notifications needs to inherit from
:class:`tlp.Observable`.

Talipot has two separate mechanisms of observation : Observers and Listeners. These two mechanisms
work through the same class, the difference lies in the way an Observer or Listener is attached to
the object whose events it will receive.

The Listener is closer to the original pattern, where events are sent directly to the recipient. The
Observer is a twist for performance purposes, it can receive the events in a delayed fashion, and
cannot know if the event was just sent or was delayed.

The purpose of this twist is to allow algorithms that perform a high number of modifications (e.g.
creating a grid to route edges, creating multiple subgraphs with metrics or layouts) to run smoothly
without overcrowding the event queue. As events are sent for every graph modification (e.g. adding a
node, deleting a node, setting a value on a node), the sheer quantity of events sent by these
algorithms would cause a big performance hit.

This is avoided by using the static method :meth:`tlp.Observable.holdObservers`. This holds all
events in a queue and only sends them when the static method :meth:`tlp.Observable.unholdObservers`
is called.

The only exception to this mechanism is the :const:`tlp.EventType.TLP_DELETE` kind of event, that is
never held back. Think of it as an unmaskable POSIX signal; whatever the connection to the object
and the state of holdObserver, this event will get through. This is used to prevent crashes in the
case where and object is listened or observed and is deleted, as it is likely the recipient keeps a
pointer to the now deleted object.

The Listener however is not affected by the use of :meth:`tlp.Observable.holdObservers` and
:meth:`tlp.Observable.unholdObservers`.

The observables Observers and Listeners are internally stocked in a graph structure, allowing to
visualize the connections easily. This eases debugging of observation-related bugs.

Events are always sent to Listeners first, and then to Observers, even when there is no hold.

To receive events, inherit from :class:`tlp.Observable`, and implement one of two virtual functions.

To received events without delay, implement method :meth:`tlp.Observable.treatEvent`. To attach to
an object for receiving events, call its :meth:`tlp.Observable.addListener` method.

To receive events after a delay, implement method :class:`tlp.Observable.treatEvents`. To attach to
an object for receiving events, call its :meth:`tlp.Observable.addObserver` method.
%End

%TypeHeaderCode
#include <talipot/Observable.h>
#include <talipot/TlpTools.h>
%End

%ConvertToSubClassCode
if (dynamic_cast<tlp::Graph *>(sipCpp)) {
  sipType = sipFindType("tlp::Graph");
} else if (dynamic_cast<tlp::PropertyInterface *>(sipCpp)) {
  sipType = sipFindType(tlp::demangleClassName(typeid(*sipCpp).name()).c_str());
} else {
  sipType = NULL;
}
%End

public:

// =======================================================================================================

  static void unholdObservers();
%Docstring
tlp.Observable.unholdObservers()

Static method to send all held events to the Observers.

Listeners are not affected by this function.
%End

// =======================================================================================================

  static void holdObservers();
%Docstring
tlp.Observable.holdObservers()

Holds back all events until :meth:`tlp.Observable.unholObservers` is called. Listeners are not
affected by this function. Once this function is called, all events heading to an Observer will be
held, except :const:`tlp.EventType.TLP_DELETE` events. The events are stored in a queue, and will be
sent once :meth:`tlp.Observable.unholObservers` is called. It is possible to nest calls to
:meth:`tlp.Observable.holObservers` and :meth:`tlp.Observable.unholObservers`, and in this case the
events will only be sent when there have been as many calls to :meth:`tlp.Observable.unholObservers`
as to :meth:`tlp.Observable.holObservers`. It is possible to check whether the events are being help
by checking the :meth:`tlp.Observable.observersHoldCounter` function.
%End


// =======================================================================================================

  static uint observersHoldCounter();
%Docstring
tlp.Observable.observableHoldCounters()

Static method to get the number of times :meth:`tlp.Observable.holdObservers` has been called
without a matching :meth:`tlp.Observable.unholdObservers` call.

:rtype:
   integer
%End

// =======================================================================================================

  void addObserver(tlp::Observable *obs) const;
%Docstring
tlp.Observable.addObserver(observer)

Adds an Observer to this object. The Observer will receive events sent by this object, if there is
no hold applied.

:param observer:
   The object that will receive events.

:type observer:
   :class:`tlp.Observable`
%End

// =======================================================================================================

  void addListener(tlp::Observable *obs) const;
%Docstring
tlp.Observable.addListener(listener)

Adds a Listener to this object. The Listener will receive events regardless of the state of
:meth:`tlp.Observable.holdObservers` and :meth:`tlp.Observable.unholdObservers`.

:param listener:
   The object that will receive events.

:type listener:
   :class:`tlp.Observable`
%End

// =======================================================================================================

  void removeObserver(tlp::Observable *obs) const;
%Docstring
tlp.Observable.removeObserver(observer)

Removes an Observer from this object. The Observer will no longer receive events from this object.

:param observer:
   The Observer to remove from this object.

:type observer:
   :class:`tlp.Observable`
%End

// =======================================================================================================

  void removeListener(tlp::Observable *obs) const;
%Docstring
tlp.Observable.removeListener(listener)

Removes a Listener from this object. The Listener will no longer receive events from this object.

:param listener:
   The Listener to remove from this object.

:type listener:
   :class:`tlp.Observable`
%End

// =======================================================================================================

  uint getSent() const;
%Docstring
tlp.Observable.getSent()

Gets the number of sent events.

:rtype:
   integer
%End

// =======================================================================================================

  uint getReceived() const;
%Docstring
tlp.Observable.getReceived()

Gets the number of received events.

:rtype:
   integer
%End

// =======================================================================================================

  uint countListeners() const;
%Docstring
tlp.Observable.countListeners()

Gets the number of Listeners attached to this object.

:rtype:
   integer
%End

// =======================================================================================================

  uint countObservers() const;
%Docstring
tlp.Observable.countObservers()

Gets the number of Observers attached to this object.

:rtype:
   integer
%End

// =======================================================================================================

protected:

  Observable();
  Observable(const tlp::Observable &);
  virtual ~Observable();

// =======================================================================================================

  void sendEvent(const tlp::Event &);
%Docstring
tlp.Observable.sendEvent(event)

Sends an event to all the Observers/Listeners.

:param event:
   the event to send to the Listeners/Observers.

:type event:
   :class:`tlp.Event`
%End

// =======================================================================================================

  virtual void treatEvents(const std::vector<tlp::Event> &events);
%Docstring
tlp.Observable.treatEvents(events)

This function is called when events are sent to Observers, and Observers only.

It is passed a list of all the events that happened since the last call. If events were on hold,
this vector can be pretty large, and if events were not on hold it is likely it only contains a
single element. It is important to note that custom events can not be caught with that method. Only
instances of base :class:`tlp.Event` type are stored in the list. Moreover when unholding events
through a call to :meth:`tlp.Observable.unholdObservers`, if several events were sent from the same
object, only one instance of :class:`tlp.Event` will be stored in the list for that sender.

:param events:
   The events that happened since the last :meth:`tlp.Observable.unholdObservers` call.

:type events:
   list of :class:`tlp.Event`
%End

// =======================================================================================================

  virtual void treatEvent(const tlp::Event & /NoCopy/);
%Docstring
tlp.Observable.treatEvent(event)

This function is called when events are sent to the Listeners, and Listeners only. Is it passed a
reference to the event that just happened. Implement that method to catch events with custom types
(like for instance :class:`tlp.GraphEvent` or :class:`tlp.PropertyEvent`)

:param event:
   The event that was sent.

:type event:
   :class:`tlp.Event` or types deriving from it
%End

// =======================================================================================================

  bool hasOnlookers() const;
%Docstring
tlp.Observable.hasOnlookers()

Returns whether there are Observers/Listeners attached to this object.

:rtype:
   boolean
%End

// =======================================================================================================

};

};













