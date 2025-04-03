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

#include <talipot/TlpQtTools.h>
#include <talipot/FontDialog.h>
#include "ui_FontDialog.h"

#include <QFontDatabase>
#include <QPushButton>

using namespace tlp;

FontDialog::FontDialog(QWidget *parent)
    : QDialog(parent), _ui(new Ui::FontDialog), _ok(QDialog::Rejected), _styleUpdate(false) {
    _ui->setupUi(this);

    for (const auto &itFamily : Font::availableFonts()) {
        _ui->nameList->addItem(tlpStringToQString(itFamily.first));
        for (const auto &itStyle : itFamily.second) {
            _fonts[tlpStringToQString(itFamily.first)].append(itStyle.second);
        }
    }

    bool hasFont = _ui->nameList->count() > 0;
    _ui->settingsWidget->setEnabled(hasFont);
    _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(hasFont);

    if (hasFont) {
        _ui->nameList->setCurrentItem(_ui->nameList->item(0));
        _ui->sizeList->setCurrentItem(_ui->sizeList->item(10));
        fontChanged();
    }
}

FontDialog::~FontDialog() {
    delete _ui;
}

Font FontDialog::font() const {
    return _fonts[_ui->nameList->currentItem()->text()][_ui->styleList->currentRow()];
}

void FontDialog::fontChanged() {
    if (sender() == _ui->nameList && _ui->nameList->currentItem() != nullptr) {
        _styleUpdate = true;
        _ui->styleList->clear();
        for (const auto &f : _fonts[_ui->nameList->currentItem()->text()]) {
            _ui->styleList->addItem(tlpStringToQString(f.fontStyle()));
        }
        _styleUpdate = false;
        _ui->styleList->setCurrentItem(_ui->styleList->item(0));
    } else if (sender() == _ui->sizeList) {
        _ui->sizeSpin->setValue(_ui->sizeList->currentItem()->text().toInt());
    }

    if (_styleUpdate || _ui->nameList->currentItem() == nullptr ||
        _ui->styleList->currentItem() == nullptr) {
        return;
    }

    Font selectedFont = font();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QFontDatabase fontDb;
    _ui->preview->setFont(fontDb.font(tlpStringToQString(selectedFont.fontFamily()),
#else
    _ui->preview->setFont(QFontDatabase::font(tlpStringToQString(selectedFont.fontFamily()),
#endif
                                      tlpStringToQString(selectedFont.fontStyle()),
                                      _ui->sizeSpin->value()));
}
int FontDialog::fontSize() const {
    return _ui->sizeSpin->value();
}

void FontDialog::selectFont(const Font &f) {
    QList<QListWidgetItem *> items =
        _ui->nameList->findItems(tlpStringToQString(f.fontFamily()), Qt::MatchExactly);

    if (items.empty()) {
        return;
    }
    _ui->nameList->setCurrentItem(items[0]);
    items = _ui->styleList->findItems(tlpStringToQString(f.fontStyle()), Qt::MatchExactly);
    if (items.empty()) {
        return;
    }
    _ui->styleList->setCurrentItem(items[0]);
}

void FontDialog::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);

    if (parentWidget()) {
        move(parentWidget()->window()->frameGeometry().topLeft() +
             parentWidget()->window()->rect().center() - rect().center());
    }
}
