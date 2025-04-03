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

#include <talipot/StringEditor.h>
#include <QVBoxLayout>
#include <QDialogButtonBox>

using namespace tlp;

StringEditor::StringEditor(QWidget *parent) : QDialog(parent) {
    auto *layout = new QVBoxLayout;
    setLayout(layout);
    edit = new QTextEdit(this);
    layout->addWidget(edit);
    auto *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal);
    layout->addWidget(buttonBox);
    QWidget::setTabOrder(edit, buttonBox);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    setString(QString());
    setModal(true);
    setWindowTitle("Set string value");
}

QString StringEditor::getString() const {
    return currentString;
}

void StringEditor::setString(const QString &qstr) {
    currentString = qstr;
    blockSignals(true);
    edit->setPlainText(qstr);
    blockSignals(false);
}

void StringEditor::done(int r) {
    if (r == QDialog::Accepted) {
        currentString = edit->toPlainText();
    }

    QDialog::done(r);
}

// to ensure it is shown in the center of its parent
void StringEditor::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);

    if (parentWidget()) {
        move(parentWidget()->window()->frameGeometry().topLeft() +
             parentWidget()->window()->rect().center() - rect().center());
    }
}
