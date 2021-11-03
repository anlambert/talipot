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

#include "ParallelCoordsGlEntitiesSelector.h"
#include "ParallelCoordinatesView.h"

using namespace std;

namespace tlp {

bool ParallelCoordsGlEntitiesSelector::eventFilter(QObject *widget, QEvent *e) {

  auto *parallelView = static_cast<ParallelCoordinatesView *>(view());
  auto *glWidget = static_cast<GlWidget *>(widget);

  if (e->type() == QEvent::MouseButtonPress) {

    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (qMouseEv->buttons() == Qt::LeftButton) {

      if (!started) {
        x = qMouseEv->pos().x();
        y = qMouseEv->pos().y();
        w = 0;
        h = 0;
        started = true;
        graph = glWidget->getGlGraphInputData()->graph();
      }

      return true;
    }
  }

  if (e->type() == QEvent::MouseMove) {

    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (qMouseEv->buttons() & Qt::LeftButton && started) {
      if ((qMouseEv->pos().x() > 0) && (qMouseEv->pos().x() < glWidget->width())) {
        w = qMouseEv->pos().x() - x;
      }

      if ((qMouseEv->pos().y() > 0) && (qMouseEv->pos().y() < glWidget->height())) {
        h = qMouseEv->pos().y() - y;
      }

      parallelView->refresh();
      return true;
    }
  }

  if (e->type() == QEvent::MouseButtonRelease) {

    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (started) {
      Observable::holdObservers();
      bool boolVal = true; // add to selection

      if (qMouseEv->modifiers() != Qt::ControlModifier) {
        if (qMouseEv->modifiers() !=
#if defined(__APPLE__)
            Qt::AltModifier
#else
            Qt::ShiftModifier
#endif
        ) {

          unselectAllEntitiesHandler(parallelView);

        } else
          boolVal = false; // remove from selection
      }

      if ((w == 0) && (h == 0)) {

        selectedEntitiesHandler(parallelView, x, y, boolVal);

      } else {

        if (w < 0) {
          w *= -1;
          x -= w;
        }

        if (h < 0) {
          h *= -1;
          y -= h;
        }

        selectedEntitiesHandler(parallelView, x, y, w, h, boolVal);
      }

      started = false;
      Observable::unholdObservers();
      return true;
    }
  }

  return false;
}
}
