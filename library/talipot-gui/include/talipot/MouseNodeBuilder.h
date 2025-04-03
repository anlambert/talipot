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

#ifndef TALIPOT_MOUSE_NODE_BUILDER_H
#define TALIPOT_MOUSE_NODE_BUILDER_H

#include <talipot/InteractorComposite.h>

#include <QEvent>

namespace tlp {
class GlWidget;

class TLP_QT_SCOPE MouseNodeBuilder : public InteractorComponent {

  public:
    MouseNodeBuilder(QEvent::Type eventType = QEvent::MouseButtonPress)
        : _eventType(eventType), glWidget(nullptr) {}
    ~MouseNodeBuilder() override = default;
    bool eventFilter(QObject *, QEvent *) override;
    void clear() override;

  private:
    QEvent::Type _eventType;
    GlWidget *glWidget;
};
}
#endif // TALIPOT_MOUSE_NODE_BUILDER_H
