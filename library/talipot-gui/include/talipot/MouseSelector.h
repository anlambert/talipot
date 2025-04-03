/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_MOUSE_SELECTOR_H
#define TALIPOT_MOUSE_SELECTOR_H

#include <talipot/GLInteractor.h>

namespace tlp {

class Graph;
class TLP_QT_SCOPE MouseSelector : public GLInteractorComponent {
  public:
    enum SelectionMode { EdgesAndNodes = 0, EdgesOnly, NodesOnly };

    MouseSelector(Qt::MouseButton button = Qt::LeftButton,
                  Qt::KeyboardModifier modifier = Qt::NoModifier,
                  SelectionMode mode = EdgesAndNodes);
    ~MouseSelector() override = default;
    bool draw(GlWidget *) override;
    bool eventFilter(QObject *, QEvent *) override;

  protected:
    Qt::MouseButton mButton;
    Qt::KeyboardModifier kModifier;
    Qt::KeyboardModifiers mousePressModifier;
    uint x, y;
    int w, h;
    bool started;
    Graph *graph;
    SelectionMode _mode;
};
}
#endif // TALIPOT_MOUSE_SELECTOR_H
