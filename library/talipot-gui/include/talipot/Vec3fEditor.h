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

#ifndef TALIPOT_COORD_EDITOR_H
#define TALIPOT_COORD_EDITOR_H

#include <QDialog>

#include <talipot/Coord.h>
#include <talipot/config.h>

namespace Ui {
class Vec3fEditor;
}

namespace tlp {
/**
 * @brief Simple Coord edition widget.
 **/
class TLP_QT_SCOPE Vec3fEditor : public QDialog {
    Q_OBJECT
    Q_PROPERTY(Vec3f vec3f READ vec3f WRITE setVec3f NOTIFY vecChanged)

    Ui::Vec3fEditor *ui;
    Vec3f currentVec;

  public:
    explicit Vec3fEditor(QWidget *parent = nullptr, bool editSize = false);
    ~Vec3fEditor() override;

    tlp::Vec3f vec3f() const;

    // redefinition to ensure it is shown in the center of its parent
    void showEvent(QShowEvent *ev) override;

  public slots:
    void setVec3f(const tlp::Vec3f &vec);
    // redefinition to ensure to catch the end of input
    void done(int r) override;

  signals:
    void vecChanged(tlp::Vec3f vec);

  private slots:
    void vecUpdated();
};
}

#endif // TALIPOT_COORD_EDITOR_H
