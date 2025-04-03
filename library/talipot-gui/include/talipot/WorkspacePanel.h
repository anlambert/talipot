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

#ifndef TALIPOT_WORKSPACE_PANEL_H
#define TALIPOT_WORKSPACE_PANEL_H

#include <QFrame>
#include <QAction>
#include <QDialog>

#include <talipot/config.h>

class QDragEnterEvent;
class QGraphicsObject;
class QPropertyAnimation;
class QGraphicsProxyWidget;
class QGraphicsRectItem;
class QMimeData;
class QVBoxLayout;
class QTabWidget;

namespace Ui {
class WorkspacePanel;
}

namespace tlp {
class Graph;
class View;
class Interactor;
class GraphHierarchiesModel;
class InteractorConfigWidget;

class TLP_QT_SCOPE WorkspacePanel : public QFrame {
    Q_OBJECT

    Ui::WorkspacePanel *_ui;
    InteractorConfigWidget *_interactorConfigWidget;
    tlp::View *_view;
    QMap<QAction *, QWidget *> _actionTriggers;
    QGraphicsRectItem *_overlayRect;

    QTabWidget *_viewConfigurationTabWidget;
    QGraphicsProxyWidget *_viewConfigurationTabWidgetProxy;
    bool _viewConfigurationExpanded;

    QPointF configurationTabPosition(bool expanded) const;
    void setConfigurationTabExpanded(bool expanded, bool animate = true);

  public:
    explicit WorkspacePanel(tlp::View *view, QWidget *parent = nullptr);
    ~WorkspacePanel() override;

    bool eventFilter(QObject *, QEvent *) override;

    tlp::View *view() const;
    QString viewName() const;

  public slots:
    void setView(tlp::View *view);
    void setCurrentInteractor(tlp::Interactor *);
    void setGraphsModel(tlp::GraphHierarchiesModel *);
    void viewGraphSet(tlp::Graph *);
    void setOverlayMode(bool);
    void setHighlightMode(bool);
    bool isGraphSynchronized() const;
    void resetInteractorsScrollButtonsVisibility();
    void updateCurrentInteractorButtonText();

  signals:
    void drawNeeded();
    void swapWithPanels(WorkspacePanel *panel);
    void changeGraphSynchronization(bool);

  protected slots:
    void hideConfigurationTab();
    void interactorActionTriggered();
    void viewDestroyed();
    void graphComboIndexChanged();
    void setCurrentInteractorConfigurationVisible(bool);
    void actionChanged();

    void scrollInteractorsRight();
    void scrollInteractorsLeft();

    void refreshInteractorsToolbar();
    void toggleSynchronization(bool);

  protected:
    void resizeEvent(QResizeEvent *) override;

    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;
    void dragLeaveEvent(QDragLeaveEvent *) override;

    bool handleDragEnterEvent(QEvent *e, const QMimeData *mimedata);
    bool handleDropEvent(const QMimeData *mimedata);
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

}

#endif // TALIPOT_WORKSPACE_PANEL_H
