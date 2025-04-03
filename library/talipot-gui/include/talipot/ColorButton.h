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

#ifndef TALIPOT_COLOR_BUTTON_H
#define TALIPOT_COLOR_BUTTON_H

#include <QPushButton>

#include <talipot/config.h>
#include <talipot/Color.h>
#include <talipot/MetaTypes.h>

namespace tlp {

class TLP_QT_SCOPE ChooseColorButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(Color talipotColor READ talipotColor WRITE setColor)

  public:
    explicit ChooseColorButton(QWidget *parent = nullptr);

    QColor color() const;
    QString text() const;
    Color talipotColor() const;

  protected:
    QColor _color;
    QString _text;
    QString _dialogTitle;

  signals:
    void colorChanged(QColor);
    void talipotColorChanged(Color);

  public slots:
    void setColor(const QColor &);
    void setText(const QString &text);
    void setColor(const Color &);
    void setDialogTitle(const QString &);

  protected slots:
    void chooseColor();
};

class TLP_QT_SCOPE ColorButton : public ChooseColorButton {
    Q_OBJECT

  public:
    explicit ColorButton(QWidget *parent = nullptr) : ChooseColorButton(parent) {}

  protected:
    void paintEvent(QPaintEvent *) override;
};
}

#endif // TALIPOT_COLOR_BUTTON_H
