/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

///@cond DOXYGEN_HIDDEN

#ifndef TALIPOT_VIEW_ACTIONS_MANAGER_H
#define TALIPOT_VIEW_ACTIONS_MANAGER_H

#include <QMenu>
#include <QAction>

#include <talipot/View.h>
#include <talipot/DataSet.h>
#include <talipot/GlMainWidget.h>

namespace tlp {
class TLP_QT_SCOPE ViewActionsManager : public QObject {
  Q_OBJECT

  View *_view;
  GlMainWidget *_glMainWidget;
  bool _keepSizeRatio;
  QAction *_centerViewAction;
  QAction *_forceRedrawAction;
  QAction *_advAntiAliasingAction;
  QAction *_snapshotAction;

public:
  ViewActionsManager(View *view, GlMainWidget *widget, bool keepRatio);

  // the function below must be called by the associated view
  // when overloading the View class corresponding method
  void fillContextMenu(QMenu *menu);

protected slots:
  void centerView();
  void redraw();
  void openSnapshotDialog();
  void setAntiAliasing(bool);
};
}

#endif // TALIPOT_VIEW_ACTIONS_MANAGER_H

///@endcond
