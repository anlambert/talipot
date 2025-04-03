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

#ifndef TALIPOT_RANGE_SLIDER_H
#define TALIPOT_RANGE_SLIDER_H

#include <QSlider>
#include <QStyle>
#include <QStylePainter>

#include <talipot/config.h>

namespace tlp {

/*
   RangeSlider is a slider with two handles. It is
   handy for letting user to choose a range of integers between min/max.
   This class is totally inspired by the QxtSpanSlider class
   of the no longer maintainer Qxt code
   see https://bitbucket.org/libqxt/libqxt/wiki/Home for more details
*/
class TLP_QT_SCOPE RangeSlider : public QSlider {
    Q_OBJECT
    Q_PROPERTY(int lowerValue READ lowerValue WRITE setLowerValue)
    Q_PROPERTY(int upperValue READ upperValue WRITE setUpperValue)
    Q_PROPERTY(
        HandleMovementMode handleMovementMode READ handleMovementMode WRITE setHandleMovementMode)
    Q_ENUMS(HandleMovementMode)

  public:
    explicit RangeSlider(QWidget *parent = nullptr);
    explicit RangeSlider(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~RangeSlider() override = default;

    enum HandleMovementMode { FreeMovement, NoCrossing, NoOverlapping };

    HandleMovementMode handleMovementMode() const;
    void setHandleMovementMode(HandleMovementMode mode);

    int lowerValue() const;
    int upperValue() const;

    int lowerPosition() const;
    int upperPosition() const;

  public slots:
    void setLowerValue(int lower);
    void setUpperValue(int upper);
    void setRange(int lower, int upper);

    void setLowerPosition(int lower);
    void setUpperPosition(int upper);

  signals:
    void rangeChanged(int lower, int upper);
    void lowerValueChanged(int lower);
    void upperValueChanged(int upper);

    void lowerPositionChanged(int lower);
    void upperPositionChanged(int upper);

  protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

  private:
    enum RangeHandle { NoHandle, LowerHandle, UpperHandle };

    Q_PROPERTY(int lowerPosition READ lowerPosition WRITE setLowerPosition)
    Q_PROPERTY(int upperPosition READ upperPosition WRITE setUpperPosition)

    void initStyle(QStyleOptionSlider *option, RangeHandle handle = UpperHandle) const;
    int pick(const QPoint &pt) const {
        return orientation() == Qt::Horizontal ? pt.x() : pt.y();
    }
    int pixelPosToRangeValue(int pos) const;
    void handleMousePress(const QPoint &pos, QStyle::SubControl &control, int value,
                          RangeHandle handle);
    void drawHandle(QStylePainter *painter, RangeHandle handle) const;
    void setupPainter(QPainter *painter, Qt::Orientation orientation, qreal x1, qreal y1, qreal x2,
                      qreal y2) const;
    void drawRange(QStylePainter *painter, const QRect &rect) const;
    void triggerAction(QAbstractSlider::SliderAction action, bool main);
    void swapControls();

    int lower;
    int upper;
    int lowerPos;
    int upperPos;
    int offset;
    int position;
    RangeHandle lastPressed;
    RangeHandle mainControl;
    QStyle::SubControl lowerPressed;
    QStyle::SubControl upperPressed;
    HandleMovementMode movement;
    bool firstMovement;
    bool blockTracking;

  public slots:
    void updateRange(int min, int max);
    void movePressedHandle();
};
}

#endif // TALIPOT_RANGE_SLIDER_H
