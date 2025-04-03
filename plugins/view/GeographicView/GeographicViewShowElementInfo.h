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

#ifndef GEOGRAPHIC_VIEW_SHOW_ELEMENT_INFO_H
#define GEOGRAPHIC_VIEW_SHOW_ELEMENT_INFO_H

#include <talipot/InteractorComposite.h>

#include "GeographicView.h"

class QTableView;

namespace tlp {

class View;
class GlComplexPolygonItemEditor;

/**
 * We define a specific interactor to show element graph info in eltProperties
 */
class GeographicViewShowElementInfo : public InteractorComponent {
    Q_OBJECT
  public:
    GeographicViewShowElementInfo();
    ~GeographicViewShowElementInfo() override;
    bool eventFilter(QObject *widget, QEvent *e) override;

    void viewChanged(View *) override;

    void clear() override;

    virtual bool pick(int x, int y, SelectedEntity &selectedEntity);

  protected:
    GeographicView *_view;
    QWidget *_informationWidget;
    QGraphicsProxyWidget *_informationWidgetItem;
    GlComplexPolygonItemEditor *_editor;

    QTableView *tableView() const;

  private slots:
    void hideInfos();
};
}

#endif // GEOGRAPHIC_VIEW_SHOW_ELEMENT_INFO_H
