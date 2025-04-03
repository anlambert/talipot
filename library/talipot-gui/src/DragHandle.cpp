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

#include <QDrag>
#include <QApplication>
#include <QMouseEvent>

#include <talipot/DragHandle.h>
#include <talipot/Mimes.h>
#include <talipot/WorkspacePanel.h>
#include <talipot/View.h>

using namespace tlp;

DragHandle::DragHandle(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f), _panel(nullptr), _pressed(false) {}

void DragHandle::mousePressEvent(QMouseEvent *ev) {
    _pressed = true;
    _clickPosition = ev->pos();
}

void DragHandle::mouseReleaseEvent(QMouseEvent *) {
    _pressed = false;
}

void DragHandle::mouseMoveEvent(QMouseEvent *ev) {
    assert(_panel != nullptr);

    if (!_panel || !_pressed ||
        (ev->pos() - _clickPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    auto *drag = new QDrag(_panel);
    auto *mimedata = new PanelMimeType();
    mimedata->setPanel(_panel);
    drag->setMimeData(mimedata);
    drag->setPixmap(_panel->view()->snapshot(QSize(100, 100)));
    drag->exec(Qt::MoveAction);
}

void DragHandle::setPanel(tlp::WorkspacePanel *panel) {
    _panel = panel;
}
