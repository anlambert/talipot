/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <QApplication>
#include <talipot/MouseNodeBuilder.h>
#include <talipot/MouseEdgeBuilder.h>
#include <talipot/MouseSelector.h>
#include <talipot/MouseSelectionEditor.h>
#include <talipot/MouseEdgeBendEditor.h>

#include "GeographicViewInteractors.h"

#include "../../utils/StandardInteractorPriority.h"
#include "../../utils/PluginNames.h"
#include "../../utils/InteractorIcons.h"

using namespace std;
using namespace tlp;

GeographicViewInteractor::GeographicViewInteractor(const QIcon &icon, const QString &text)
    : GLInteractorComposite(icon, text) {}

bool GeographicViewInteractor::isCompatible(const std::string &viewName) const {
  return (viewName == ViewName::GeographicViewName);
}

GeographicViewInteractorNavigation::GeographicViewInteractorNavigation(const PluginContext *)
    : GeographicViewInteractor(interactorIcon(InteractorType::Navigation), "Navigate in view") {}

unsigned int GeographicViewInteractorNavigation::priority() const {
  return StandardInteractorPriority::Navigation;
}

void GeographicViewInteractorNavigation::construct() {
  push_back(new GeographicViewNavigator);
}

QWidget *GeographicViewInteractorNavigation::configurationWidget() const {
  return nullptr;
}

GeographicViewInteractorSelection::GeographicViewInteractorSelection(const PluginContext *)
    : GeographicViewInteractor(interactorIcon(InteractorType::Selection), "selection in view") {}

void GeographicViewInteractorSelection::construct() {
  push_back(new GeographicViewNavigator);
  push_back(new MouseSelector);
}

QWidget *GeographicViewInteractorSelection::configurationWidget() const {
  return nullptr;
}

QCursor GeographicViewInteractorSelection::cursor() const {
  return Qt::CrossCursor;
}

unsigned int GeographicViewInteractorSelection::priority() const {
  return StandardInteractorPriority::RectangleSelection;
}

PLUGIN(GeographicViewInteractorSelection)

GeographicViewInteractorSelectionEditor::GeographicViewInteractorSelectionEditor(
    const PluginContext *)
    : GeographicViewInteractor(interactorIcon(InteractorType::SelectionModifier),
                               "selection edition in view") {}

void GeographicViewInteractorSelectionEditor::construct() {
  push_back(new GeographicViewNavigator);
  push_back(new MouseSelector);
  push_back(new MouseSelectionEditor);
}

QWidget *GeographicViewInteractorSelectionEditor::configurationWidget() const {
  return nullptr;
}

QCursor GeographicViewInteractorSelectionEditor::cursor() const {
  return Qt::CrossCursor;
}

unsigned int GeographicViewInteractorSelectionEditor::priority() const {
  return StandardInteractorPriority::RectangleSelectionModifier;
}

PLUGIN(GeographicViewInteractorSelectionEditor)

GeographicViewNavigator::GeographicViewNavigator() : x(0), y(0), inRotation(false) {}

GeographicViewNavigator::~GeographicViewNavigator() {}

void GeographicViewNavigator::viewChanged(View *) {}

void trans(Coord &c1, Coord &c2, float angle1, float angle2) {
  float rho1 = sqrt(c1[0] * c1[0] + c1[1] * c1[1] + c1[2] * c1[2]);
  float theta1 = acos(c1[2] / rho1);
  float phi1 = acos(c1[0] / sqrt(c1[0] * c1[0] + c1[1] * c1[1]));

  float rho2 = sqrt(c2[0] * c2[0] + c2[1] * c2[1] + c2[2] * c2[2]);
  float theta2 = acos(c2[2] / rho2);
  float phi2 = acos(c2[0] / sqrt(c2[0] * c2[0] + c2[1] * c2[1]));

  if (c1[1] < 0) {
    phi1 = 2 * M_PI - phi1;
  }

  if (c1[0] == 0 && c1[1] == 0) {
    phi1 = 0;
  }

  if (c2[1] < 0) {
    phi2 = 2 * M_PI - phi2;
  }

  if (c2[0] == 0 && c2[1] == 0) {
    phi2 = 0;
  }

  if (theta1 + angle1 > 0.001 && theta1 + angle1 < M_PI && theta2 + angle1 > 0.001 &&
      theta2 + angle1 < M_PI) {
    theta1 += angle1;
    theta2 += angle1;

    if (theta2 > theta1) {
      float tmp = theta1;
      theta1 = theta2;
      theta2 = tmp;
    }
  }

  phi2 += angle2;
  phi1 = phi2;

  c1[0] = rho1 * sin(theta1) * cos(phi1);
  c1[1] = rho1 * sin(theta1) * sin(phi1);
  c1[2] = rho1 * cos(theta1);

  c2[0] = rho2 * sin(theta2) * cos(phi2);
  c2[1] = rho2 * sin(theta2) * sin(phi2);
  c2[2] = rho2 * cos(theta2);
}

bool GeographicViewNavigator::eventFilter(QObject *widget, QEvent *e) {
  GeographicView *geoView = static_cast<GeographicView *>(view());
  GlMainWidget *g = static_cast<GlMainWidget *>(widget);
  QMouseEvent *qMouseEv = dynamic_cast<QMouseEvent *>(e);
  QWheelEvent *qWheelEv = dynamic_cast<QWheelEvent *>(e);

  if (geoView->viewType() == GeographicView::OpenStreetMap ||
      geoView->viewType() == GeographicView::EsriSatellite ||
      geoView->viewType() == GeographicView::EsriTerrain ||
      geoView->viewType() == GeographicView::EsriGrayCanvas ||
      geoView->viewType() == GeographicView::LeafletCustomTileLayer) {
    return false;
  } else if (geoView->viewType() == GeographicView::Globe) {
    if (e->type() == QEvent::Wheel && qWheelEv->orientation() == Qt::Vertical) {
#define WHEEL_DELTA 120
      g->getScene()->zoomXY(qWheelEv->delta() / WHEEL_DELTA, g->width() / 2., g->height() / 2.);
      view()->draw();
      return true;
    }

    if (e->type() == QEvent::MouseButtonPress && !inRotation) {
      if (qMouseEv->button() == Qt::LeftButton) {
        x = qMouseEv->x();
        y = qMouseEv->y();
        inRotation = true;
        return true;
      }
    }

    if (e->type() == QEvent::MouseButtonRelease) {
      if (qMouseEv->button() == Qt::LeftButton) {
        inRotation = false;
        return true;
      }
    }

    if (e->type() == QEvent::MouseMove && inRotation) {

      Camera &camera = g->getScene()->getGraphCamera();
      Coord c1 = camera.getEyes() - camera.getCenter();
      Coord c2 = camera.getEyes() - camera.getCenter() + camera.getUp();
      trans(c1, c2, -0.005 * (qMouseEv->y() - y), -0.005 * (qMouseEv->x() - x));
      camera.setCenter(Coord(0, 0, 0));
      camera.setEyes(c1);
      camera.setUp(c2 - camera.getEyes());

      x = qMouseEv->x();
      y = qMouseEv->y();

      view()->draw();
      return true;
    }

    if (e->type() == QEvent::KeyPress) {

      float angle1 = 0;
      float angle2 = 0;

      switch (static_cast<QKeyEvent *>(e)->key()) {
      case Qt::Key_Left:
        angle2 = -0.05f;
        break;

      case Qt::Key_Right:
        angle2 = 0.05f;
        break;

      case Qt::Key_Up:
        angle1 = 0.05f;
        break;

      case Qt::Key_Down:
        angle1 = -0.05f;
        break;
      }

      Camera &camera = g->getScene()->getGraphCamera();
      Coord c1 = camera.getEyes() - camera.getCenter();
      Coord c2 = camera.getEyes() - camera.getCenter() + camera.getUp();
      trans(c1, c2, angle1, angle2);
      camera.setCenter(Coord(0, 0, 0));
      camera.setEyes(c1);
      camera.setUp(c2 - camera.getEyes());

      view()->draw();

      return true;
    }

    return false;
  } else {
    return MouseNKeysNavigator::eventFilter(widget, e);
  }
}

PLUGIN(GeographicViewInteractorNavigation)

GeographicViewInteractorAddEdges::GeographicViewInteractorAddEdges(const PluginContext *)
    : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::AddEdge), "Add nodes/edges",
                                    StandardInteractorPriority::AddNodesOrEdges) {}

void GeographicViewInteractorAddEdges::construct() {
  setConfigurationWidgetText("<h3>Add nodes/edges</h3>To add a node: <b>Mouse left</b> click "
                             "outside any node.<br/>To add an edge: <b>Mouse left</b> click on the "
                             "source node,<br/>then <b>Mouse left</b> click on the target "
                             "node.<br/>Any <b>Mouse left</b> click outside a node before the "
                             "click on the target node will add an edge bend,<br/><b>Mouse "
                             "middle</b> click will cancel the current edge construction.");
  push_back(new GeographicViewNavigator);
  push_back(new MouseNodeBuilder);
  push_back(new MouseEdgeBuilder);
}

QCursor GeographicViewInteractorAddEdges::cursor() const {
  return QCursor(Qt::PointingHandCursor);
}

bool GeographicViewInteractorAddEdges::isCompatible(const std::string &viewName) const {
  return (viewName == ViewName::GeographicViewName);
}

PLUGIN(GeographicViewInteractorAddEdges)

GeographicViewInteractorEditEdgeBends::GeographicViewInteractorEditEdgeBends(const PluginContext *)
    : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::EditEdgeBends),
                                    "Edit edge bends", StandardInteractorPriority::EditEdgeBends) {}

void GeographicViewInteractorEditEdgeBends::construct() {
  push_back(new GeographicViewNavigator);
  push_back(new MouseSelector);
  push_back(new MouseEdgeBendEditor);
}

bool GeographicViewInteractorEditEdgeBends::isCompatible(const std::string &viewName) const {
  return (viewName == ViewName::GeographicViewName);
}

PLUGIN(GeographicViewInteractorEditEdgeBends)
