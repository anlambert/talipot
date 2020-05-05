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

#include <QGraphicsView>

#include <talipot/ViewActionsManager.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/SnapshotDialog.h>
#include <talipot/TlpQtTools.h>

using namespace tlp;

ViewActionsManager::ViewActionsManager(View *view, GlMainWidget *widget, bool keepRatio)
    : _view(view), _glMainWidget(widget), _keepSizeRatio(keepRatio),
      _advAntiAliasingAction(nullptr) {
  // create actions and add them to _view->graphicsView()
  // to enable their keyboard shortcut
  _forceRedrawAction = new QAction("Force redraw", widget);
  setToolTipWithCtrlShortcut(_forceRedrawAction, "Redraw the current view", "Shift+R");
  connect(_forceRedrawAction, &QAction::triggered, this, &ViewActionsManager::redraw);
  _forceRedrawAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
  _forceRedrawAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  _view->graphicsView()->addAction(_forceRedrawAction);

  _centerViewAction = new QAction("Center view", widget);
  setToolTipWithCtrlShortcut(_centerViewAction,
                             "Make the view to fully display and center its contents", "Shif+C");
  connect(_centerViewAction, &QAction::triggered, this, &ViewActionsManager::centerView);
  _centerViewAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
  _centerViewAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  _view->graphicsView()->addAction(_centerViewAction);

  _snapshotAction = new QAction("Take a snapshot", widget);
  setToolTipWithCtrlShortcut(
      _snapshotAction, "Show a dialog to save a snapshot of the current view display", "Shift+P");
  connect(_snapshotAction, &QAction::triggered, this, &ViewActionsManager::openSnapshotDialog);
  _snapshotAction->setShortcut(QKeySequence("Ctrl+Shift+P"));
  _snapshotAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  _view->graphicsView()->addAction(_snapshotAction);
}

void ViewActionsManager::centerView() {
  _view->centerView();
}

void ViewActionsManager::redraw() {
  _view->refresh();
}

void ViewActionsManager::openSnapshotDialog() {
  SnapshotDialog dlg(_view, _view->graphicsView()->window());
  dlg.setSnapshotHasViewSizeRatio(_keepSizeRatio);
  dlg.exec();
}

void ViewActionsManager::setAntiAliasing(bool aa) {
  OpenGlConfigManager::setAntiAliasing(aa);
  if (_advAntiAliasingAction) {
    _advAntiAliasingAction->setVisible(aa);
    if (_advAntiAliasingAction->isChecked()) {
      _advAntiAliasingAction->setChecked(false);
    } else {
      _view->draw();
    }
  } else {
    _view->draw();
  }
}

void ViewActionsManager::fillContextMenu(QMenu *menu) {
  menu->addAction("View")->setEnabled(false);
  menu->addSeparator();
  menu->addAction(_forceRedrawAction);
  menu->addAction(_centerViewAction);

  QAction *action = menu->addAction("Anti-aliasing");
  action->setToolTip("Improve rendering quality");
  action->setCheckable(true);
  action->setChecked(OpenGlConfigManager::antiAliasing());
  connect(action, &QAction::triggered, this, &ViewActionsManager::setAntiAliasing);

  if (_advAntiAliasingAction) {
    menu->addAction(_advAntiAliasingAction);
  }

  menu->addAction(_snapshotAction);
}
