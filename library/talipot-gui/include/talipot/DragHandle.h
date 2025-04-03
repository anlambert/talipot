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

#ifndef TALIPOT_DRAG_HANDLE_H
#define TALIPOT_DRAG_HANDLE_H

#include <QLabel>

namespace tlp {

class WorkspacePanel;

class DragHandle : public QLabel {
    Q_OBJECT
  public:
    explicit DragHandle(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

    void setPanel(tlp::WorkspacePanel *panel);

  private:
    tlp::WorkspacePanel *_panel;
    bool _pressed;
    QPoint _clickPosition;
};
}
#endif // TALIPOT_DRAG_HANDLE_H
