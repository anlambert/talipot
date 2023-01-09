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

#ifndef GEOGRAPHIC_VIEW_INTERACTORS_H
#define GEOGRAPHIC_VIEW_INTERACTORS_H

#include <talipot/GLInteractor.h>
#include <talipot/MouseInteractors.h>
#include "GeographicView.h"
#include <talipot/NodeLinkDiagramViewInteractor.h>
#include <QCursor>

using namespace tlp;

class GeographicViewInteractor : public NodeLinkDiagramViewInteractor {

public:
  GeographicViewInteractor(const QIcon &icon, const QString &text, uint priority = 0);

  bool isCompatible(const std::string &viewName) const override;
};

class GeographicViewNavigator : public MouseNKeysNavigator {

public:
  GeographicViewNavigator();
  ~GeographicViewNavigator() override;

  bool eventFilter(QObject *, QEvent *) override;

  void viewChanged(View *) override;

protected:
  int x, y;
  bool inRotation;
};

class GeographicViewInteractorNavigation : public GeographicViewInteractor {

public:
  PLUGININFORMATION("InteractorNavigationGeographicView", "Tulip Team", "01/04/2009",
                    "Geographic View Navigation Interactor", "1.0", "Navigation")

  GeographicViewInteractorNavigation(const PluginContext *);

  void construct() override;

  QWidget *configurationWidget() const override;
};

class GeographicViewInteractorSelection : public GeographicViewInteractor {

public:
  PLUGININFORMATION("InteractorSelectionGeographicView", "Tulip Team", "01/12/2018",
                    "Geographic View Selection Interactor", "1.0", "Selection")

  GeographicViewInteractorSelection(const PluginContext *);

  void construct() override;

  QWidget *configurationWidget() const override;

  QCursor cursor() const override;
};

class GeographicViewInteractorSelectionEditor : public GeographicViewInteractor {

public:
  PLUGININFORMATION("InteractorSelectionEditorGeographicView", "Tulip Team", "01/12/2018",
                    "Geographic View Selection Editor Interactor", "1.0", "Selection")

  GeographicViewInteractorSelectionEditor(const PluginContext *);

  void construct() override;

  QWidget *configurationWidget() const override;

  QCursor cursor() const override;
};

class GeographicViewInteractorAddEdges : public GeographicViewInteractor {

public:
  PLUGININFORMATION("InteractorAddEdgesGeographicView", "Tulip Team", "02/06/2017",
                    "Geographic View Add Edges Interactor", "1.0", "Modification")

  GeographicViewInteractorAddEdges(const PluginContext *);

  void construct() override;

  QCursor cursor() const override;
};

class GeographicViewInteractorEditEdgeBends : public GeographicViewInteractor {

public:
  PLUGININFORMATION("InteractorEditEdgeBendsGeographicView", "Tulip Team", "02/06/2017",
                    "Geographic View Edit Edge Bends Interactor", "1.0", "Modification")

  GeographicViewInteractorEditEdgeBends(const PluginContext *);

  void construct() override;
};

class GeographicViewInteractorRectangleZoom : public GeographicViewInteractor {

public:
  PLUGININFORMATION("InteractorRectangleZoomGeographicView", "Tulip Team", "14/11/2022",
                    "Geographic View Rectangle Zoom Interactor", "1.0", "Navigation")

  GeographicViewInteractorRectangleZoom(const PluginContext *);

  void construct() override;
};

#endif // GEOGRAPHIC_VIEW_INTERACTORS_H
