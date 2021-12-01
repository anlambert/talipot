/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GraphNeedsSavingObserver.h>
#include <talipot/Graph.h>

#include <deque>

#include <QMainWindow>

using namespace tlp;
using namespace std;

GraphNeedsSavingObserver::GraphNeedsSavingObserver(Graph *graph, QMainWindow *mainWindow)
    : _needsSaving(false), _graph(graph), _mainWindow(mainWindow) {
  addObserver();
}

GraphNeedsSavingObserver::~GraphNeedsSavingObserver() {
  if (_mainWindow) {
    _mainWindow->setWindowModified(true);
  }
}

void GraphNeedsSavingObserver::treatEvents(const vector<Event> &) {
  if (!_needsSaving) {
    forceToSave();
  }
}

void GraphNeedsSavingObserver::saved() {
  _needsSaving = false;
  removeObservers();
  addObserver();

  if (_mainWindow) {
    _mainWindow->setWindowModified(false);
  }
}

bool GraphNeedsSavingObserver::needsSaving() const {
  return _needsSaving;
}

void GraphNeedsSavingObserver::forceToSave() {
  _needsSaving = true;
  // No need to listen to the graph anymore
  removeObservers();

  if (_mainWindow) {
    _mainWindow->setWindowModified(true);
  }

  emit(savingNeeded());
}

/**
 * @brief Listen all the observable objects in the graph (subgraphs, properties).
 **/
void GraphNeedsSavingObserver::addObserver() {
  deque<Graph *> toObserve;
  toObserve.push_back(_graph);

  while (!toObserve.empty()) {
    Graph *current = toObserve.front();
    current->addObserver(this);
    toObserve.pop_front();

    // Listen properties.
    for (PropertyInterface *property : current->getLocalObjectProperties()) {
      property->addObserver(this);
    }

    // Fetch subgraphs
    for (Graph *subgraph : current->subGraphs()) {
      toObserve.push_back(subgraph);
    }
  }
}

/**
 * @brief  Stop listening all the observable objects in the graph (subgraphs, properties).
 **/
void GraphNeedsSavingObserver::removeObservers() {
  deque<Graph *> toUnobserve;
  toUnobserve.push_back(_graph);

  while (!toUnobserve.empty()) {
    Graph *current = toUnobserve.front();
    toUnobserve.pop_front();

    current->removeObserver(this);

    // Stop listening properties.
    for (PropertyInterface *property : current->getLocalObjectProperties()) {
      property->removeObserver(this);
    }

    // Fetch subgraphs
    for (Graph *subgraph : current->subGraphs()) {
      toUnobserve.push_back(subgraph);
    }
  }
}
