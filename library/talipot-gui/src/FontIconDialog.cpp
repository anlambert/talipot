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

#include <talipot/FontIconDialog.h>
#include <talipot/FontIcon.h>
#include <talipot/FontAwesome.h>
#include <talipot/MaterialDesignIcons.h>

#include <QDesktopServices>
#include <QRegularExpression>
#include <QUrl>

#include "ui_FontIconDialog.h"

using namespace tlp;

FontIconDialog::FontIconDialog(QWidget *parent) : QDialog(parent), _ui(new Ui::FontIconDialog) {

  _ui->setupUi(this);

  _ui->iconsCreditLabel->setText(
      QString("<html><head/><body><p><span style=\" font-size:8pt;\">Special credit for the design "
              "of icons goes to:</span><br/><span style=\" font-size:8pt; font-weight:600;\">Font "
              "Awesome </span><span style=\"font-size:8pt; color:#0000ff;\"><a "
              "href=\"http://fontawesome.com\">http://fontawesome.com</a></span><span style=\" "
              "font-size:8pt;\"> (v%1)</span><br/><span style=\"font-size:8pt; "
              "font-weight:600;\">Material Design Icons </span><span "
              "style=\"font-size:8pt;color:#0000ff;\"><a "
              "href=\"https://materialdesignicons.com\">https://materialdesignicons.com</a></"
              "span><span style=\" font-size:8pt;\"> (v%2)</span></p></body></html>")
          .arg(FontAwesome::getVersion().c_str())
          .arg(MaterialDesignIcons::getVersion().c_str()));
  connect(_ui->iconNameFilterLineEdit, &QLineEdit::textChanged, this,
          &FontIconDialog::updateIconList);
  connect(_ui->iconsCreditLabel, &QLabel::linkActivated, this, &FontIconDialog::openUrlInBrowser);

  updateIconList();
}

void FontIconDialog::updateIconList() {
  _ui->iconListWidget->clear();

  QRegularExpression regexp(_ui->iconNameFilterLineEdit->text());

  std::vector<std::string> iconNames = FontAwesome::getSupportedIcons();

  for (const auto &ic : iconNames) {
    QString iconName = tlpStringToQString(ic);

    if (iconName.indexOf(regexp) != -1) {
      _ui->iconListWidget->addItem(new QListWidgetItem(FontIcon::icon(iconName), iconName));
    }
  }

  iconNames = MaterialDesignIcons::getSupportedIcons();

  for (const auto &ic : iconNames) {
    QString iconName = tlpStringToQString(ic);

    if (iconName.indexOf(regexp) != -1) {
      _ui->iconListWidget->addItem(new QListWidgetItem(FontIcon::icon(iconName), iconName));
    }
  }

  if (_ui->iconListWidget->count() > 0) {
    _ui->iconListWidget->sortItems();
    _ui->iconListWidget->setCurrentRow(0);
  }
}

QString FontIconDialog::getSelectedIconName() const {
  return _selectedIconName;
}

void FontIconDialog::setSelectedIconName(const QString &iconName) {
  QList<QListWidgetItem *> items = _ui->iconListWidget->findItems(iconName, Qt::MatchExactly);

  if (!items.isEmpty()) {
    _ui->iconListWidget->setCurrentItem(items.at(0));
    _selectedIconName = iconName;
  }
}

void FontIconDialog::accept() {
  if (_ui->iconListWidget->count() > 0) {
    _selectedIconName = _ui->iconListWidget->currentItem()->text();
  }

  QDialog::accept();
}

void FontIconDialog::showEvent(QShowEvent *ev) {
  QDialog::showEvent(ev);

  _selectedIconName = _ui->iconListWidget->currentItem()->text();

  if (parentWidget()) {
    move(parentWidget()->window()->frameGeometry().topLeft() +
         parentWidget()->window()->rect().center() - rect().center());
  }
}

void FontIconDialog::openUrlInBrowser(const QString &url) {
  QDesktopServices::openUrl(QUrl(url));
}
