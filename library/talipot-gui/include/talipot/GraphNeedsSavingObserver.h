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

#ifndef TALIPOT_GRAPH_NEEDS_SAVING_OBSERVER_H
#define TALIPOT_GRAPH_NEEDS_SAVING_OBSERVER_H

#include <talipot/Observable.h>

#include <QObject>

class QMainWindow;

namespace tlp {

class Graph;

/**
 * @see Observable
 *
 * @brief The GraphNeedsSavingObserver class will observe a graph and tells if it has been modified.
 *
 * @li The constructor. Observe the graph given in parameter for modification
 * @li needsSaving. Returns true is the graph has been modified
 * @li saved. The graph has been saved, and the status of the class must be reset. needsSaving will
 * return false if called after saved().
 * @li savingNeeded. Signal send when the status of the graph evolves.
 *
 */
class TLP_QT_SCOPE GraphNeedsSavingObserver : public QObject, Observable {

  Q_OBJECT

  bool _needsSaving;
  Graph *_graph;
  QMainWindow *_mainWindow;

  void addObserver();
  void removeObservers();

public:
  /**
   * @brief GraphNeedsSavingObserver Class constructor
   * @param graph The graph which needs to be observed for modifications
   * @param mainWindow A QMainWindow object
   */
  GraphNeedsSavingObserver(Graph *graph, QMainWindow *mainWindow = nullptr);

  ~GraphNeedsSavingObserver();

  /**
   * @brief saved If the graph has been saved, one has to call this method to reset the status of
   * the graph (it does not need to be saved).
   * to indicate that the graph does not need to be saved until a new modification.
   */
  void saved();

  /**
   * @brief needsSaving Indicates if the graph has been modified, and thus needs to be saved.
   *
   * @return true if the graph needs to be saved, false otherwise.
   */
  bool needsSaving() const;

  /**
   *
   * @brief forceToSave Even if there is no modification on the graph, this method can be used to
   * force to save the graph.
   */
  void forceToSave();

protected:
  /**
   * @see Listener
   * @see Observer
   * @see Observable
   * @see Observable::treatEvents(const std::vector<Event>&)
   *
   * @brief treatEvents This function is called when events are sent to Observers, and Observers
   * only.
   *
   * @param events The events that happened since the last unHoldObservers().
   */
  void treatEvents(const std::vector<Event> &) override;

signals:

  /**
   * @brief savingNeeded This signal is sent when the graph needs to be saved (it has been
   * modified).
   */

  void savingNeeded();
};
}
#endif // TALIPOT_GRAPH_NEEDS_SAVING_OBSERVER_H
