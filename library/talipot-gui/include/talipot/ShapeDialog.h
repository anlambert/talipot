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

#ifndef TALIPOT_SHAPE_DIALOG_H
#define TALIPOT_SHAPE_DIALOG_H

#include <talipot/config.h>

#include <QDialog>
#include <QString>
#include <QPixmap>

namespace Ui {
class ShapeDialog;
}

namespace tlp {

class TLP_QT_SCOPE ShapeDialog : public QDialog {

    Q_OBJECT

    Ui::ShapeDialog *_ui;
    QString _selectedShapeName;
    std::list<std::pair<QString, QPixmap>> shapes;

  public:
    ShapeDialog(std::list<std::pair<QString, QPixmap>> &nodeShapes, QWidget *parent = nullptr);
    ~ShapeDialog() override;

    QString getSelectedShapeName() const;

    void setSelectedShapeName(const QString &shapeName);

    void accept() override;

    void showEvent(QShowEvent *) override;

  protected slots:
    void updateShapeList();
};
}

#endif // TALIPOT_SHAPE_DIALOG_H
