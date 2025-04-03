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

#ifndef TALIPOT_WORKSPACE_H
#define TALIPOT_WORKSPACE_H

#include <QWidget>
#include <QMap>
#include <QVector>

#include <talipot/config.h>

class PlaceHolderWidget;

namespace Ui {
class Workspace;
}

class QPushButton;
class QLabel;
class QMimeData;

namespace tlp {
class PluginProgress;
class View;
class WorkspacePanel;
class Graph;
class GraphHierarchiesModel;
class Project;

class TLP_QT_SCOPE Workspace : public QWidget {
    Q_OBJECT

    Ui::Workspace *_ui;
    QList<WorkspacePanel *> _panels;
    int _currentPanelIndex;
    QWidget *_oldWorkspaceMode;
    WorkspacePanel *_focusedPanel;
    bool _focusedPanelHighlighting;

    QMap<QWidget *, QVector<PlaceHolderWidget *>> _modeToSlots;
    QMap<QWidget *, QWidget *> _modeSwitches;
    GraphHierarchiesModel *_model;

    QString panelTitle(WorkspacePanel *) const;

    bool _autoCenterViews;

  public:
    explicit Workspace(QWidget *parent = nullptr);
    ~Workspace() override;

    int addPanel(tlp::View *);
    QList<tlp::View *> panels() const;

  public slots:
    void delView(tlp::View *view);
    void switchToStartupMode();
    void switchToSingleMode();
    void setSingleModeSwitch(QWidget *);
    void switchToSplitMode();
    void setSplitModeSwitch(QWidget *);
    void switchToSplitHorizontalMode();
    void setSplitHorizontalModeSwitch(QWidget *);
    void switchToSplit3Mode();
    void setSplit3ModeSwitch(QWidget *);
    void switchToSplit32Mode();
    void setSplit32ModeSwitch(QWidget *);
    void switchToSplit33Mode();
    void setSplit33ModeSwitch(QWidget *);
    void switchToGridMode();
    void setGridModeSwitch(QWidget *);
    void switchToSixMode();
    void setSixModeSwitch(QWidget *);

    void nextPage();
    void previousPage();
    void setExposeModeSwitch(QPushButton *);
    void expose(bool);
    void showExposeMode();
    void hideExposeMode();
    void closeAll();
    void setActivePanel(tlp::View *);
    void setModel(tlp::GraphHierarchiesModel *);
    tlp::GraphHierarchiesModel *graphModel() const;

    void writeProject(tlp::Project *, QMap<tlp::Graph *, QString>, tlp::PluginProgress *);
    void readProject(tlp::Project *, QMap<QString, tlp::Graph *>, tlp::PluginProgress *);

    void setBottomFrameEnabled(bool);
    void setPageCountLabel(QLabel *l);

    void redrawPanels(bool center = false);

    void setAutoCenterPanelsOnDraw(bool);

    void setFocusedPanelHighlighting(bool);

    void setGraphForFocusedPanel(tlp::Graph *);

  signals:
    void panelFocused(tlp::View *);
    void focusedPanelSynchronized();
    void addPanelRequest(tlp::Graph *g = nullptr);
    void importGraphRequest();

  protected slots:
    void viewNeedsDraw();
    void switchWorkspaceMode(QWidget *page);
    void panelDestroyed(QObject *);
    void updatePageCountLabel();
    void updateAvailableModes();
    void updatePanels();
    void uncheckExposeButton();
    QWidget *suitableMode(QWidget *oldMode);
    WorkspacePanel *panelForScene(QObject *obj);
    void swapPanelsRequested(WorkspacePanel *panel);
    void updateStartupMode();
    void changeFocusedPanelSynchronization(bool);

  protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    bool handleDragEnterEvent(QEvent *e, const QMimeData *mimedata);
    bool handleDropEvent(const QMimeData *mimedata);

    bool eventFilter(QObject *, QEvent *) override;

    QWidget *currentModeWidget() const;
    QVector<PlaceHolderWidget *> currentModeSlots() const;
    uint currentSlotsCount() const;

    void setFocusedPanel(WorkspacePanel *panel);
};
}

#endif // TALIPOT_WORKSPACE_H
