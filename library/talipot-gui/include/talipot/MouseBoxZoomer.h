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

#ifndef TALIPOT_MOUSE_BOX_ZOOMER_H
#define TALIPOT_MOUSE_BOX_ZOOMER_H

#include <talipot/GLInteractor.h>

class QMouseEvent;
class QKeyEvent;

namespace tlp {

class Graph;

class TLP_QT_SCOPE MouseBoxZoomer : public GLInteractorComponent {

public:
  MouseBoxZoomer(Qt::MouseButton button = Qt::LeftButton,
                 Qt::KeyboardModifier modifier = Qt::NoModifier);
  ~MouseBoxZoomer() override;
  bool draw(GlWidget *) override;
  bool eventFilter(QObject *, QEvent *) override;

protected:
  Qt::MouseButton mButton;
  Qt::KeyboardModifier kModifier;
  uint x, y;
  int w, h;
  bool started;
  Graph *graph;
};
}
#endif // TALIPOT_MOUSE_BOX_ZOOMER_H
