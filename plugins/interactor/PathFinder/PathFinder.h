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

#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include <talipot/GLInteractor.h>

#include "../../utils/StandardInteractorPriority.h"
#include "PathAlgorithm.h"

#define NO_METRIC "None"
#define DEFAULT_ORIENTATION PathAlgorithm::Undirected
#define DEFAULT_PATHS_TYPE PathAlgorithm::OneShortest

class QPushButton;

namespace tlp {
class PathFinderComponent;
class PathHighlighter;
class PathFinderConfigurationWidget;

class StringsListSelectionWidget;
class BooleanProperty;

/** \file
 *  \brief PathFinder helps you finding paths between nodes in your graph.
 *
 * This plug-in offers several possibilities to highlight different kinds of paths between two nodes
 * in a graph.
 * You can thus display one or several shortest paths between two selected nodes.
 */
class PathFinder : public tlp::GLInteractorComposite {
  Q_OBJECT
public:
  PLUGININFORMATION("PathFinder", "Tulip Team", "03/24/2010", "Path finding interactor", "1.1",
                    "Information")

  PathFinder(const tlp::PluginContext *);
  ~PathFinder() override;
  void construct() override;
  uint priority() const override {
    return tlp::StandardInteractorPriority::PathSelection;
  }
  QWidget *configurationWidget() const override;

  /**
   * @return The name of the property used to get the weight values over the edges.
   */
  std::string getWeightMetricName() const {
    return weightMetric;
  }

  /**
   * @return The edge orientation used when computing the path.
   * @see PathAlgorithm::EdgeOrientation
   */
  PathAlgorithm::EdgeOrientation getEdgeOrientation() const {
    return edgeOrientation;
  }

  /**
   * @return the type of path the user wants to select.
   * @see PathAlgorithm::PathType
   */
  PathAlgorithm::PathType getPathsType() const {
    return pathsTypes;
  }

  /**
   * @return The active path highlighters
   */
  std::vector<std::string> getActiveHighlighters();

  /**
   * @return The inactive path highlighters
   */
  std::vector<std::string> getInactiveHighlighters();

  /**
   * @return All the installed path highlighters
   */
  std::vector<std::string> getHighlighters();

  bool isCompatible(const std::string &viewName) const override;

public slots:

  void setEdgeOrientation(const QString &orientation);
  void setPathsType(const QString &pathType);
  void setWeightMetric(const QString &metric);
  void configureHighlighterButtonPressed();

private:
  PathFinderComponent *getPathFinderComponent();

  std::string weightMetric;
  PathAlgorithm::EdgeOrientation edgeOrientation;
  PathAlgorithm::PathType pathsTypes;

  // Used for GUI interaction.
  std::map<PathAlgorithm::EdgeOrientation, std::string> edgeOrientationLabels;
  std::map<PathAlgorithm::PathType, std::string> pathsTypesLabels;

  // GUI elements.
  PathFinderConfigurationWidget *_configurationWidget;
  tlp::StringsListSelectionWidget *highlightersListWidget;
  QPushButton *configureHighlighterBtn;
};
}
#endif // PATH_FINDER_H
