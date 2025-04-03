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

#ifndef TALIPOT_SNAPSHOT_DIALOG_H
#define TALIPOT_SNAPSHOT_DIALOG_H

#include <QDialog>

#include <talipot/config.h>

namespace Ui {
class SnapshotDialog;
}

class QGraphicsScene;
class QGraphicsPixmapItem;
class QAbstractButton;
class QLabel;

namespace tlp {

class LinkWidget;
class View;

class TLP_QT_SCOPE SnapshotDialog : public QDialog {

    Q_OBJECT

  public:
    SnapshotDialog(const View *v, QWidget *parent = nullptr);
    ~SnapshotDialog() override;

    void setSnapshotHasViewSizeRatio(bool snapshotHasViewSizeRatio);

  public slots:

    void accept() override;

  protected slots:

    void widthSpinBoxValueChanged(int value);
    void heightSpinBoxValueChanged(int value);
    void clicked(QAbstractButton *b);

  protected:
    void resizeEvent(QResizeEvent *) override;

    void sizeSpinBoxValueChanged();

    Ui::SnapshotDialog *ui;
    const View *view;

    LinkWidget *linkWidget;

    float ratio;

    bool inSizeSpinBoxValueChanged;
};
}

#endif // TALIPOT_SNAPSHOT_DIALOG_H
