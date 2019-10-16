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

#include "ExportWizard.h"
#include "ui_ExportWizard.h"

#include <QAbstractButton>
#include <QFileDialog>
#include <QMessageBox>

#include <talipot/ItemDelegate.h>
#include <talipot/ParameterListModel.h>
#include <talipot/ExportModule.h>

#include <talipot/GraphHierarchiesModel.h>
#include <talipot/PluginModel.h>

using namespace tlp;
using namespace std;

ExportWizard::ExportWizard(Graph *g, const QString &exportFile, QWidget *parent)
    : QWizard(parent), _ui(new Ui::ExportWizard), _graph(g) {
  _ui->setupUi(this);
  button(QWizard::FinishButton)->setEnabled(false);

  PluginModel<tlp::ExportModule> *model = new PluginModel<tlp::ExportModule>(_ui->exportModules);

  _ui->exportModules->setModel(model);
  _ui->exportModules->setRootIndex(model->index(0, 0));
  _ui->exportModules->expandAll();
  connect(_ui->exportModules->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
          this, SLOT(algorithmSelected(QModelIndex)));

  _ui->parametersList->setItemDelegate(new ItemDelegate(_ui->parametersList));
  _ui->parametersList->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  connect(_ui->exportModules, SIGNAL(doubleClicked(QModelIndex)), button(QWizard::FinishButton),
          SLOT(click()));

  // display OK instead of Finish
  setButtonText(QWizard::FinishButton, "OK");
  _ui->parametersFrame->hide();
  updateFinishButton();

  _ui->pathEdit->setText(exportFile);
}

ExportWizard::~ExportWizard() {
  delete _ui->parametersList->model();
  delete _ui;
}

void ExportWizard::algorithmSelected(const QModelIndex &index) {
  QString alg(index.data().toString());
  string algs(tlp::QStringToTlpString(alg));
  _ui->parametersFrame->setVisible(!alg.isEmpty());
  QAbstractItemModel *oldModel = _ui->parametersList->model();
  QAbstractItemModel *newModel = nullptr;

  if (PluginsManager::pluginExists(algs)) {
    newModel = new ParameterListModel(PluginsManager::getPluginParameters(algs), _graph);
  }

  _ui->parametersList->setModel(newModel);

  QString parametersText("<b>Parameters</b>");
  parametersText += "&nbsp;<font size=-2>[" + alg + "]</font>";
  _ui->parametersLabel->setText(parametersText);

  delete oldModel;
  updateFinishButton();
}

QString ExportWizard::algorithm() const {
  if (_ui->exportModules->selectionModel()->hasSelection())
    return _ui->exportModules->selectionModel()->selectedIndexes()[0].data().toString();

  return QString();
}

tlp::DataSet ExportWizard::parameters() const {
  ParameterListModel *model = dynamic_cast<ParameterListModel *>(_ui->parametersList->model());

  if (model == nullptr)
    return DataSet();

  return model->parametersValues();
}

QString ExportWizard::outputFile() const {
  return _ui->pathEdit->text();
}

void ExportWizard::updateFinishButton() {
  button(QWizard::FinishButton)->setEnabled(_ui->parametersList->model() != nullptr);
}

void ExportWizard::pathChanged(QString s) {
  QString selectedExport;
  _ui->algFrame->setEnabled(!s.isEmpty());
  button(QWizard::FinishButton)->setEnabled(!s.isEmpty());

  std::list<std::string> modules = PluginsManager::availablePlugins<ExportModule>();

  for (std::list<std::string>::iterator itm = modules.begin(); itm != modules.end(); ++itm) {
    ExportModule *p = PluginsManager::getPluginObject<ExportModule>(*itm);
    std::list<std::string> extension = p->allFileExtensions();

    for (list<string>::const_iterator extit = extension.begin(); extit != extension.end();
         ++extit) {
      if (s.endsWith((*extit).c_str())) {
        selectedExport = itm->c_str();
        delete p;
        break;
      }
    }

    if (!selectedExport.isEmpty())
      break;
  }

  if (selectedExport.isEmpty()) {
    _ui->exportModules->clearSelection();
    return;
  }

  PluginModel<tlp::ExportModule> *model =
      static_cast<PluginModel<tlp::ExportModule> *>(_ui->exportModules->model());
  QModelIndexList results = model->match(_ui->exportModules->rootIndex(), Qt::DisplayRole,
                                         selectedExport, 1, Qt::MatchExactly | Qt::MatchRecursive);

  if (results.empty())
    return;

  _ui->exportModules->setCurrentIndex(results[0]);
}

void ExportWizard::browseButtonClicked() {
  QString filter;
  QString all = "all supported formats (";
  const std::list<std::string> modules = PluginsManager::availablePlugins<ExportModule>();

  for (std::list<std::string>::const_iterator itm = modules.begin(); itm != modules.end(); ++itm) {
    ExportModule *p = PluginsManager::getPluginObject<ExportModule>(*itm);
    const std::list<std::string> extension = p->allFileExtensions();
    filter += tlpStringToQString(p->name()) + " (";

    for (list<string>::const_iterator it = extension.begin(); it != extension.end(); ++it) {
      filter += (*it).c_str() + QString(" ");
      all += (*it).c_str() + QString(" ");
    }

    filter.resize(filter.length() - 1);
    filter += ");;";
    delete p;
  }

  filter.resize(filter.length() - 2);
  all.resize(all.length() - 1);
  all = all + ");;" + filter;
  QString exportFile =
      QFileDialog::getSaveFileName(this, "Export file", _ui->pathEdit->text(), all, nullptr
// on MacOSX selectedFilter is ignored by the
// native dialog
#ifdef __APPLE__
                                   ,
                                   QFileDialog::DontUseNativeDialog
#endif
      );

  if (!exportFile.isEmpty()) {
    _ui->pathEdit->setText(exportFile);
  }
}

bool ExportWizard::validateCurrentPage() {
  QString exportFile = outputFile();

  // check correct extension
  ExportModule *p =
      PluginsManager::getPluginObject<ExportModule>(tlp::QStringToTlpString(algorithm()));
  std::list<std::string> extension;

  if (p != nullptr)
    extension = p->allFileExtensions();

  bool extok(false);
  QString ext;

  for (list<string>::const_iterator it = extension.begin(); it != extension.end(); ++it) {
    ext += tlp::tlpStringToQString(*it) + ", ";

    if (exportFile.endsWith(tlp::tlpStringToQString(*it))) {
      extok = true;
    }
  }

  delete p;

  if (!extok) {
    if (extension.size() == 1)
      _ui->pathEdit->setText(exportFile + "." + tlp::tlpStringToQString(*extension.begin()));
    else {
      ext.resize(ext.length() - 2);
      QString msg = "Filename does not terminate with a valid extension. ";

      if (!algorithm().isEmpty())
        msg += "Please add one.<br>Valid extensions for " + algorithm() + " are: " + ext;

      QMessageBox::warning(parentWidget(), "Filename not valid", msg);
      return false;
    }
  }

  // if file exists and is valid, check if user wants to overwrite it
  return (!exportFile.isEmpty() &&
          (!QFile::exists(exportFile) ||
           (QMessageBox::question(
                parentWidget(), "Overwriting an existing file",
                "The export file already exists.<br/>Do you really want to overwrite it?",
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)));
}
