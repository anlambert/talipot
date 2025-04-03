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

#ifndef TALIPOT_VIEW_TOOL_TIP_AND_URL_MANAGER_H
#define TALIPOT_VIEW_TOOL_TIP_AND_URL_MANAGER_H

class QWidget;

#include <QMenu>

#include <talipot/View.h>
#include <talipot/DataSet.h>

namespace tlp {

class TLP_QT_SCOPE ViewToolTipAndUrlManager : public QObject {
    Q_OBJECT

    View *_view;
    QWidget *_widget;
    bool _tooltips;
    std::string _urlPropName;
    std::string _url, _contextMenuUrl;

  public:
    ViewToolTipAndUrlManager(tlp::View *view, QWidget *widget);

    // the functions below must be called by the associated view
    // when overloading the View class corresponding methods
    void setState(const tlp::DataSet &);
    void state(tlp::DataSet &) const;
    void fillContextMenu(QMenu *menu);
    void fillContextMenu(QMenu *menu, node n);
    void fillContextMenu(QMenu *menu, edge e);

  protected slots:
    void displayToolTips(bool display);
    void setUrlProp(QAction *);
    void openUrl();

  protected:
    bool eventFilter(QObject *, QEvent *e) override;
};
}

#endif // TALIPOT_VIEW_TOOL_TIP_AND_URL_MANAGER_H
