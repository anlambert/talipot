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

#ifndef TALIPOT_CLEARABLE_LINE_EDIT_H
#define TALIPOT_CLEARABLE_LINE_EDIT_H

#include <memory>

#include <QLineEdit>

#include <talipot/config.h>

class TLP_QT_SCOPE ClearableLineEdit : public QLineEdit {
    static std::unique_ptr<QPixmap> CLEAR_PIXMAP;

    bool _clearButtonHovered;
    static void initPixmap();
    QRect pixmapRect();

  public:
    explicit ClearableLineEdit(QWidget *parent = nullptr);

  protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void leaveEvent(QEvent *) override;
};

#endif // TALIPOT_CLEARABLE_LINE_EDIT_H
