/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/PropertyCreationDialog.h"
#include "ui_PropertyCreationDialog.h"

#include <QMessageBox>
#include <QPushButton>

#include <talipot/Graph.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

using namespace tlp;

PropertyCreationDialog::PropertyCreationDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PropertyCreationDialog), _createPropertyButton(nullptr),
      _graph(nullptr), _createdProperty(nullptr) {
  initGui();
}

PropertyCreationDialog::PropertyCreationDialog(Graph *graph, QWidget *parent,
                                               const std::string &selectedType)
    : QDialog(parent), ui(new Ui::PropertyCreationDialog), _graph(graph),
      _createdProperty(nullptr) {
  initGui();

  if (!selectedType.empty()) {
    int selectedIndex =
        ui->propertyTypeComboBox->findText(propertyTypeToPropertyTypeLabel(selectedType));

    if (selectedIndex != -1) {
      ui->propertyTypeComboBox->setCurrentIndex(selectedIndex);
    }
  }
}

void PropertyCreationDialog::initGui() {
  ui->setupUi(this);
  QStringList labels;
  // Init properties list
  labels << propertyTypeToPropertyTypeLabel("bool");
  labels << propertyTypeToPropertyTypeLabel("color");
  labels << propertyTypeToPropertyTypeLabel("double");
  labels << propertyTypeToPropertyTypeLabel("int");
  labels << propertyTypeToPropertyTypeLabel("layout");
  labels << propertyTypeToPropertyTypeLabel("size");
  labels << propertyTypeToPropertyTypeLabel("string");
  labels << propertyTypeToPropertyTypeLabel("vector<bool>");
  labels << propertyTypeToPropertyTypeLabel("vector<color>");
  labels << propertyTypeToPropertyTypeLabel("vector<coord>");
  labels << propertyTypeToPropertyTypeLabel("vector<double>");
  labels << propertyTypeToPropertyTypeLabel("vector<int>");
  labels << propertyTypeToPropertyTypeLabel("vector<size>");
  labels << propertyTypeToPropertyTypeLabel("vector<string>");
  ui->propertyTypeComboBox->addItems(labels);
  _createPropertyButton = ui->buttonBox->addButton("Create", QDialogButtonBox::AcceptRole);
  _createPropertyButton->setIcon(FontIcon::icon(MaterialDesignIcons::Check));
  ui->errorIconLabel->setPixmap(
      QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(16, 16));
  connect(ui->propertyNameLineEdit, &QLineEdit::textChanged, this,
          &PropertyCreationDialog::checkValidity);
  checkValidity();
}

PropertyCreationDialog::~PropertyCreationDialog() {
  delete ui;
}

void PropertyCreationDialog::setGraph(Graph *graph) {
  _graph = graph;
  checkValidity();
}

void PropertyCreationDialog::accept() {
  bool error = false;

  if (_graph == nullptr) {
    QMessageBox::warning(this, "Failed to create property", "The parent graph is invalid",
                         QMessageBox::Ok, QMessageBox::Ok);
    error = true;
  }

  QString propertyName = ui->propertyNameLineEdit->text();

  if (propertyName.isEmpty()) {
    QMessageBox::warning(this, "Failed to create property",
                         "You cannot create a property with an empty name", QMessageBox::Ok,
                         QMessageBox::Ok);
    error = true;
  }

  if (_graph->existLocalProperty(QStringToTlpString(propertyName))) {
    QMessageBox::warning(this, "Failed to create property",
                         "A property with the same name already exists", QMessageBox::Ok,
                         QMessageBox::Ok);
    error = true;
  }

  if (!error) {
    _graph->push();
    _createdProperty = _graph->getLocalProperty(
        QStringToTlpString(propertyName),
        propertyTypeLabelToPropertyType(ui->propertyTypeComboBox->currentText()));
  }

  QDialog::accept();
}

PropertyInterface *PropertyCreationDialog::createNewProperty(tlp::Graph *graph, QWidget *parent,
                                                             const std::string &selectedType) {
  auto *dialog = new PropertyCreationDialog(graph, parent, selectedType);
  PropertyInterface *result = nullptr;

  if (dialog->exec() == QDialog::Accepted) {
    result = dialog->createdProperty();
  }

  dialog->deleteLater();
  return result;
}

void PropertyCreationDialog::checkValidity() {
  QString propertyName = ui->propertyNameLineEdit->text();

  if (_graph == nullptr) {
    ui->errorLabel->setText("You need to specify a parent graph");
    ui->errorNotificationWidget->setVisible(true);
    _createPropertyButton->setEnabled(false);
    return;
  } else if (propertyName.isEmpty()) {
    ui->errorLabel->setText("You cannot create a property with an empty name");
    _createPropertyButton->setEnabled(false);
    ui->errorNotificationWidget->setVisible(true);
    return;
  } else if (_graph->existLocalProperty(QStringToTlpString(propertyName))) {
    ui->errorLabel->setText("A property with the same name already exists");
    _createPropertyButton->setEnabled(false);
    ui->errorNotificationWidget->setVisible(true);
    return;
  }

  ui->errorNotificationWidget->setVisible(false);
  _createPropertyButton->setEnabled(true);
}
