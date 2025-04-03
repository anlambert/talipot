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

#include <QFileDialog>
#include <QImageReader>

#include <talipot/TextureFileDialog.h>

using namespace tlp;

TextureFileDialog::TextureFileDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TextureFileDialog()) {
    ui->setupUi(this);
    connect(ui->chooseFileOrDirButton, &QAbstractButton::clicked, this, &TextureFileDialog::browse);
}

TextureFileDialog::~TextureFileDialog() {
    delete ui;
}

void TextureFileDialog::done(int res) {
    if (res) {
        if (ui->localFilesystemRB->isChecked()) {
            _data.texturePath = ui->fileOrDirLineEdit->text();
        } else {
            _data.texturePath = ui->urlLineEdit->text();
        }
    }

    QDialog::done(res);
}

void TextureFileDialog::setData(const TextureFile &tf) {
    _data = tf;
    setWindowTitle("Choose a texture file");

    if (tf.texturePath.startsWith("http")) {
        ui->httpUrlRB->setChecked(true);
        ui->urlLineEdit->setText(tf.texturePath);
    } else {
        ui->localFilesystemRB->setChecked(true);
        ui->fileOrDirLineEdit->setText(tf.texturePath);
    }

    setModal(true);
    move(QCursor::pos() - QPoint(250, 40));
}

void TextureFileDialog::browse() {
    QString filter = "Images (";
    for (auto f : QImageReader::supportedImageFormats()) {
        filter += QString(" *.%1").arg(QString(f).toLower());
    }
    filter += ')';
    QString result = QFileDialog::getOpenFileName(parentWidget(), "Choose a texture file",
                                                  _data.texturePath, filter);

    if (!result.isEmpty()) {
        ui->fileOrDirLineEdit->setText(result);
    }
}

void TextureFileDialog::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);

    if (parentWidget()) {
        move(parentWidget()->window()->frameGeometry().topLeft() +
             parentWidget()->window()->rect().center() - rect().center());
    }
}
