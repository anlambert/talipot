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

#include <talipot/HeaderFrame.h>

#include "ui_HeaderFrame.h"

using namespace tlp;

void switchToLabel(Ui::HeaderFrame *ui, bool f = true) {
    ui->titleLabel->setVisible(f);
    ui->menusCombo->setVisible(!f);
}

HeaderFrame::HeaderFrame(QWidget *parent)
    : QFrame(parent), _ui(new Ui::HeaderFrame), _expanded(true) {
    _ui->setupUi(this);
    switchToLabel(_ui);
    connect(_ui->menusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            QOverload<int>::of(&HeaderFrame::menuChanged));
    connect(_ui->expandButton, &QAbstractButton::toggled, this, &HeaderFrame::setExpanded);
}

HeaderFrame::~HeaderFrame() {
    delete _ui;
}

QString HeaderFrame::title() const {
    return _ui->titleLabel->text();
}

void HeaderFrame::setTitle(const QString &title) {
    _ui->titleLabel->setText(title);

    if (_ui->menusCombo->count() == 0) {
        switchToLabel(_ui);
    }
}

QStringList HeaderFrame::menus() const {
    QStringList result;

    for (int i = 0; i < _ui->menusCombo->count(); ++i) {
        result << _ui->menusCombo->itemText(i);
    }

    return result;
}

void HeaderFrame::setMenus(const QStringList &menus) {
    _ui->menusCombo->clear();
    switchToLabel(_ui, menus.empty());

    for (const QString &s : menus) {
        _ui->menusCombo->addItem(s);
    }
}

QString HeaderFrame::currentMenu() const {
    return _ui->menusCombo->currentText();
}

int HeaderFrame::currentMenuIndex() const {
    return _ui->menusCombo->currentIndex();
}

bool HeaderFrame::isExpandable() const {
    return _ui->expandButton->isVisible();
}

void HeaderFrame::setExpandable(bool f) {
    _ui->expandButton->setVisible(f);
}

void HeaderFrame::expand(bool e) {
    _ui->expandButton->setChecked(e);
}

void HeaderFrame::setExpanded(bool e) {
    _expanded = e;

    QWidget *pw = parentWidget();

    if (!pw) {
        return;
    }

    for (auto *obj : pw->children()) {
        auto *w = dynamic_cast<QWidget *>(obj);

        if (w && w != this) {
            w->setVisible(e);
        }
    }

    int maxH, minH;

    if (!e) {
        _oldHeightInfo = QPair<int, int>(pw->minimumHeight(), pw->maximumHeight());
        maxH = height();
        minH = height();
    } else {
        minH = _oldHeightInfo.first;
        maxH = _oldHeightInfo.second;
    }

    pw->setMinimumSize(pw->minimumWidth(), minH);
    pw->setMaximumSize(pw->maximumWidth(), maxH);

    _ui->expandButton->setToolTip(e ? "Hide contents" : "Show contents");

    emit expanded(e);
}

bool HeaderFrame::isExpanded() const {
    return _expanded;
}

QWidget *HeaderFrame::expandControl() const {
    return _ui->expandButton;
}

void HeaderFrame::insertWidget(QWidget *w) {
    _ui->horizontalLayout->insertWidget(_ui->horizontalLayout->indexOf(_ui->expandButton), w);
}

void HeaderFrame::menuChanged(int idx) {
    emit menuChanged(_ui->menusCombo->itemText(idx));
}