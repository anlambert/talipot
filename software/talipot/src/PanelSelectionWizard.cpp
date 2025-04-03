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

#include "PanelSelectionWizard.h"
#include "ui_PanelSelectionWizard.h"

#include <QAbstractButton>

#include <talipot/View.h>
#include <talipot/MetaTypes.h>
#include <talipot/GraphHierarchiesModel.h>
#include <talipot/PluginModel.h>

using namespace tlp;
using namespace std;

PanelSelectionWizard::PanelSelectionWizard(GraphHierarchiesModel *model, QWidget *parent)
    : QWizard(parent), _ui(new Ui::PanelSelectionWizard), _model(model), _view(nullptr) {
    _ui->setupUi(this);
    setWizardStyle(QWizard::ClassicStyle);

    connect(this, &QWizard::currentIdChanged, this, &PanelSelectionWizard::pageChanged);
    _ui->graphCombo->setModel(_model);
    _ui->graphCombo->selectIndex(_model->indexOf(_model->currentGraph()));

    list<string> plugins = PluginsManager::availablePlugins<tlp::View>();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    _ui->panelList->setModel(
        new SimplePluginListModel(QList<string>(plugins.begin(), plugins.end()), _ui->panelList));
#else
    _ui->panelList->setModel(
        new SimplePluginListModel(QList<string>::fromStdList(plugins), _ui->panelList));
#endif
    connect(_ui->panelList->selectionModel(), &QItemSelectionModel::currentChanged, this,
            &PanelSelectionWizard::panelSelected);
    connect(_ui->panelList, &QAbstractItemView::doubleClicked, button(QWizard::FinishButton),
            &QAbstractButton::click);
    _ui->panelList->setCurrentIndex(_ui->panelList->model()->index(0, 0));
}

PanelSelectionWizard::~PanelSelectionWizard() {
    delete _ui;
}

void PanelSelectionWizard::panelSelected(const QModelIndex &index) {
    _currentItem = index.data().toString();
    _ui->panelDescription->setHtml(
        PluginsManager::pluginInformation(QStringToTlpString(_currentItem)).info().c_str());
    // NexButton is temporarily hidden
    // QWizard::HaveNextButtonOnLastPage has been removed
    // from options property in PanelSelectionWizard.ui
    button(QWizard::NextButton)->setEnabled(true);
}

tlp::Graph *PanelSelectionWizard::graph() const {
    return _model->data(_ui->graphCombo->selectedIndex(), Model::GraphRole).value<tlp::Graph *>();
}

void PanelSelectionWizard::setSelectedGraph(tlp::Graph *g) {
    _ui->graphCombo->selectIndex(_model->indexOf(g));
}

tlp::View *PanelSelectionWizard::panel() const {
    return _view;
}

void PanelSelectionWizard::createView() {
    _view = PluginsManager::getPluginObject<View>(QStringToTlpString(_currentItem));
    _view->setupUi();
    _view->setGraph(graph());
    _view->setState(DataSet());
}

void PanelSelectionWizard::clearView() {
    delete _view;
    _view = nullptr;

    for (auto id : pageIds()) {
        if (id == startId() || id == currentId()) {
            continue;
        }

        QWizardPage *p = page(id);
        removePage(id);
        delete p;
    }

    _ui->placeHolder = new QWizardPage();
    addPage(_ui->placeHolder);
}

void PanelSelectionWizard::done(int result) {
    if (result == QDialog::Accepted && _view == nullptr) {
        createView();
    } else if (result == QDialog::Rejected) {
        clearView();
    }

    QWizard::done(result);
}

void PanelSelectionWizard::pageChanged(int id) {
    // temporarily display OK instead of Finish
    setButtonText(QWizard::FinishButton, "OK");

    if (id == startId()) {
        clearView();
        button(QWizard::FinishButton)->setEnabled(true);
    }

    if (page(id) == _ui->placeHolder) {
        createView();
        bool inPlaceHolder = true;

        for (auto *w : _view->configurationWidgets()) {
            QWizardPage *p;

            if (inPlaceHolder) {
                p = _ui->placeHolder;
                inPlaceHolder = false;
            } else {
                p = new QWizardPage;
                addPage(p);
            }

            p->setLayout(new QVBoxLayout);
            p->layout()->addWidget(w);
        }
    }

    // NexButton is temporarily hidden
    // QWizard::HaveNextButtonOnLastPage has been removed
    // from options property in PanelSelectionWizard.ui
    button(QWizard::NextButton)->setEnabled(nextId() != -1);
}
