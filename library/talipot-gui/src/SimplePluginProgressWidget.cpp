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

#include <talipot/SimplePluginProgressWidget.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include "ui_SimplePluginProgressWidget.h"

#include <QCloseEvent>

using namespace tlp;

SimplePluginProgressWidget::SimplePluginProgressWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f), _ui(new Ui::SimplePluginProgressWidget),
      _lastUpdate(QTime::currentTime()), _state(tlp::ProgressState::TLP_CONTINUE) {
  _ui->setupUi(this);
  _ui->cancelButton->setIcon(FontIcon::icon(MaterialDesignIcons::Cancel, QColor(50, 50, 50), 0.8));
  _ui->stopButton->setIcon(FontIcon::icon(MaterialDesignIcons::Stop, QColor(50, 50, 50), 1.4));
  connect(_ui->cancelButton, &QAbstractButton::clicked, this,
          &SimplePluginProgressWidget::cancelClicked);
  connect(_ui->stopButton, &QAbstractButton::clicked, this,
          &SimplePluginProgressWidget::stopClicked);
}

SimplePluginProgressWidget::~SimplePluginProgressWidget() {
  delete _ui;
}

void SimplePluginProgressWidget::checkLastUpdate() {
  if (_lastUpdate.msecsTo(QTime::currentTime()) > 50) {
    QApplication::processEvents();
    _lastUpdate = QTime::currentTime();
  }
}

void SimplePluginProgressWidget::setComment(const std::string &s) {
  setComment(tlpStringToQString(s));
}

void SimplePluginProgressWidget::setComment(const QString &s) {
  _ui->comment->setText("<b>" + s + "</b>");
  checkLastUpdate();
}

void SimplePluginProgressWidget::setComment(const char *s) {
  setComment(tlpStringToQString(s));
}

ProgressState SimplePluginProgressWidget::progress(int step, int max_step) {
  PluginProgress::progress(step, max_step);
  _ui->progressBar->setValue(step);
  _ui->progressBar->setMaximum(max_step);
  checkLastUpdate();
  return _state;
}

void SimplePluginProgressWidget::cancel() {
  _state = tlp::ProgressState::TLP_CANCEL;
}

void SimplePluginProgressWidget::stop() {
  _state = tlp::ProgressState::TLP_STOP;
}

bool SimplePluginProgressWidget::isPreviewMode() const {
  return _ui->previewBox->isChecked();
}

void SimplePluginProgressWidget::setPreviewMode(bool drawPreview) {
  _ui->previewBox->setChecked(drawPreview);
}

void SimplePluginProgressWidget::showPreview(bool showPreview) {
  _ui->previewBox->setVisible(showPreview);
  checkLastUpdate();
}

void SimplePluginProgressWidget::showStops(bool showButtons) {
  _ui->cancelButton->setVisible(showButtons);
  _ui->stopButton->setVisible(showButtons);
  if (showButtons) {
    setComment("");
  } else {
    setComment("Processing in progress...");
  }
}

ProgressState SimplePluginProgressWidget::state() const {
  return _state;
}

std::string SimplePluginProgressWidget::getError() {
  return _error;
}

void SimplePluginProgressWidget::setError(const std::string &error) {
  _error = error;
}

void SimplePluginProgressWidget::closeEvent(QCloseEvent *ev) {
  ev->ignore();
}

void SimplePluginProgressWidget::setCancelButtonVisible(bool v) {
  _ui->cancelButton->setVisible(v);
}

void SimplePluginProgressWidget::setStopButtonVisible(bool v) {
  _ui->stopButton->setVisible(v);
}

void SimplePluginProgressWidget::cancelClicked() {
  cancel();
}

void SimplePluginProgressWidget::stopClicked() {
  stop();
}

// ===================
// DIALOG
// ===================

SimplePluginProgressDialog::SimplePluginProgressDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowTitleHint | Qt::CustomizeWindowHint), _painted(false),
      _progress(new SimplePluginProgressWidget(this)) {
  setModal(true);
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(_progress);
  setWindowIcon(QIcon(":/talipot/gui/icons/talipot.ico"));
  resize(500, height());
}

SimplePluginProgressDialog::~SimplePluginProgressDialog() {
  delete _progress;
}

void SimplePluginProgressDialog::setComment(const std::string &s) {
  _progress->setComment(s);
}

void SimplePluginProgressDialog::setComment(const QString &s) {
  _progress->setComment(s);
}

void SimplePluginProgressDialog::setComment(const char *s) {
  _progress->setComment(s);
}

void SimplePluginProgressDialog::setTitle(const std::string &title) {
  setWindowTitle(tlpStringToQString(title));
  QApplication::processEvents();
}

ProgressState SimplePluginProgressDialog::progress(int step, int max_step) {
  PluginProgress::progress(step, max_step);
  return _progress->progress(step, max_step);
}

void SimplePluginProgressDialog::cancel() {
  _progress->cancel();
}

void SimplePluginProgressDialog::stop() {
  _progress->stop();
}

bool SimplePluginProgressDialog::isPreviewMode() const {
  return _progress->isPreviewMode();
}

void SimplePluginProgressDialog::setPreviewMode(bool drawPreview) {
  _progress->setPreviewMode(drawPreview);
}

void SimplePluginProgressDialog::showPreview(bool showPreview) {
  _progress->showPreview(showPreview);
  if (_painted) {
    _painted = false;
    update();
    while (!_painted) {
      QApplication::processEvents();
    }
    QApplication::processEvents();
  }
}

void SimplePluginProgressDialog::showStops(bool showButtons) {
  _progress->showStops(showButtons);
  _painted = false;
  update();
  while (!_painted) {
    QApplication::processEvents();
  }
  QApplication::processEvents();
}

ProgressState SimplePluginProgressDialog::state() const {
  return _progress->state();
}

std::string SimplePluginProgressDialog::getError() {
  return _progress->getError();
}

void SimplePluginProgressDialog::setError(const std::string &error) {
  _progress->setError(error);
}

void SimplePluginProgressDialog::paintEvent(QPaintEvent *ev) {
  QWidget::paintEvent(ev);
  _painted = true;
}

void SimplePluginProgressDialog::closeEvent(QCloseEvent *ev) {
  ev->ignore();
}

void SimplePluginProgressDialog::setCancelButtonVisible(bool v) {
  _progress->setCancelButtonVisible(v);
}

void SimplePluginProgressDialog::setStopButtonVisible(bool v) {
  _progress->setStopButtonVisible(v);
}
