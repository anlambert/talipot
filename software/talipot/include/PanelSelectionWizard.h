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

#ifndef PANEL_SELECTION_WIZARD_H
#define PANEL_SELECTION_WIZARD_H

#include <QModelIndex>
#include <QWizard>

namespace tlp {
class Graph;
class Plugin;
class GraphHierarchiesModel;
class View;
}

namespace Ui {
class PanelSelectionWizard;
}

class PanelSelectionWizard : public QWizard {
    Q_OBJECT

    QWizardPage *_placeHolder;
    Ui::PanelSelectionWizard *_ui;
    tlp::GraphHierarchiesModel *_model;
    tlp::View *_view;
    QString _currentItem;

    void createView();
    void clearView();

  public:
    explicit PanelSelectionWizard(tlp::GraphHierarchiesModel *model, QWidget *parent = nullptr);
    ~PanelSelectionWizard() override;

    tlp::Graph *graph() const;
    QString panelName() const;
    tlp::View *panel() const;
    void setSelectedGraph(tlp::Graph *);

  protected slots:
    void pageChanged(int);
    void panelSelected(const QModelIndex &index);

  protected:
    void done(int result) override;
};

#endif // PANEL_SELECTION_WIZARD_H
