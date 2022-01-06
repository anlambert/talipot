/**
 *
 * Copyright (C) 2020-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef INTERACTOR_ICONS_H
#define INTERACTOR_ICONS_H

#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

namespace tlp {

enum InteractorType {
  AddEdge,
  DeleteElement,
  EditEdgeBends,
  Fisheye,
  GetInformation,
  Lasso,
  MagnifyingGlass,
  Navigation,
  NeighborhoodHighlighting,
  PathFinding,
  RectangleZoom,
  Selection,
  SelectionModifier,
};

static QIcon interactorIcon(InteractorType interactorType, const QColor &iconColor = Qt::white) {
  QIcon icon;
  switch (interactorType) {
  case AddEdge: {
    icon = FontIcon::icon(MaterialDesignIcons::VectorPolyline, iconColor);
    break;
  }
  case DeleteElement: {
    icon = FontIcon::icon(MaterialDesignIcons::DeleteOutline, iconColor);
    break;
  }
  case EditEdgeBends: {
    icon = FontIcon::icon(MaterialDesignIcons::VectorCurve, iconColor, 1, 90);
    break;
  }
  case Fisheye: {
    icon = FontIcon::icon(MaterialDesignIcons::Web, iconColor);
    break;
  }
  case GetInformation: {
    QIcon backIcon =
        FontIcon::icon(MaterialDesignIcons::CursorDefault, iconColor, 0.9, 0, QPointF(-20, 0));
    QIcon frontIcon =
        FontIcon::icon(MaterialDesignIcons::Help, iconColor, 0.6, 0, QPointF(40, -20));
    icon = FontIcon::stackIcons(backIcon, frontIcon);
    break;
  }
  case Lasso: {
    icon = FontIcon::icon(MaterialDesignIcons::VectorPolygon, iconColor);
    break;
  }
  case MagnifyingGlass: {
    icon = FontIcon::icon(MaterialDesignIcons::Magnify, iconColor);
    break;
  }
  case Navigation: {
    icon = FontIcon::icon(MaterialDesignIcons::CursorPointer, iconColor);
    break;
  }
  case NeighborhoodHighlighting: {
    icon = FontIcon::icon(MaterialDesignIcons::GoogleCirclesExtended, iconColor);
    break;
  }
  case PathFinding: {
    icon = FontIcon::icon(MaterialDesignIcons::Routes, iconColor);
    break;
  }
  case RectangleZoom: {
    QIcon backIcon = FontIcon::icon(MaterialDesignIcons::Selection, iconColor);
    QIcon frontIcon = FontIcon::icon(MaterialDesignIcons::MagnifyPlus, iconColor, 0.6);
    icon = FontIcon::stackIcons(backIcon, frontIcon);
    break;
  }
  case Selection: {
    icon = FontIcon::icon(MaterialDesignIcons::Selection, iconColor);
    break;
  }
  case SelectionModifier: {
    QIcon backIcon = FontIcon::icon(MaterialDesignIcons::Selection, iconColor);
    QIcon frontIcon = FontIcon::icon(MaterialDesignIcons::ArrowAll, iconColor, 0.6);
    icon = FontIcon::stackIcons(backIcon, frontIcon);
    break;
  }
  default:
    break;
  }
  return icon;
}

}
#endif // INTERACTOR_ICONS_H