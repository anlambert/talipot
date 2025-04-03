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

#include <QPainter>

#include "ProgressWidgetGraphicsProxy.h"
#include "ui_ProgressWidget.h"

using namespace tlp;

ProgressWidget::ProgressWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::ProgressWidget), cancelClicked(true) {
    _ui->setupUi(this);
    connect(_ui->cancelButton, &QAbstractButton::clicked, this,
            &ProgressWidget::cancelButtonClicked);
}

ProgressWidget::~ProgressWidget() {
    delete _ui;
}

void ProgressWidget::setComment(const QString &comment) {
    _ui->commentLabel->setText(comment);
}

void ProgressWidget::setProgress(int value, int max) {
    _ui->progressBar->setMaximum(max);
    _ui->progressBar->setValue(value);
}

void ProgressWidget::showEvent(QShowEvent *event) {
    cancelClicked = false;
    QWidget::showEvent(event);
}

void ProgressWidget::cancelButtonClicked() {
    cancelClicked = true;
}

ProgressWidgetGraphicsProxy::ProgressWidgetGraphicsProxy() : frameColor(Qt::green) {
    progressWidget = new ProgressWidget();
    setWidget(progressWidget);
    setWindowFlags(Qt::Window);
    setWindowFrameMargins(0, 0, 0, 0);
    setOpacity(0.8);
}

void ProgressWidgetGraphicsProxy::setComment(const QString &comment) {
    progressWidget->setComment(comment);
}

void ProgressWidgetGraphicsProxy::setProgress(int value, int max) {
    progressWidget->setProgress(value, max);
}

const float offset = 10.;

void ProgressWidgetGraphicsProxy::paintWindowFrame(QPainter *painter,
                                                   const QStyleOptionGraphicsItem *, QWidget *) {
    QRectF widgetGeometry = windowFrameRect();
    QPainterPath path;
    path.addRect(widgetGeometry);
    path.moveTo(widgetGeometry.topLeft() + QPointF(0, -offset));
    path.quadTo(widgetGeometry.topLeft() + QPointF(-offset, -offset),
                widgetGeometry.topLeft() + QPointF(-offset, 0));
    path.lineTo(widgetGeometry.bottomLeft() + QPointF(-offset, 0));
    path.quadTo(widgetGeometry.bottomLeft() + QPointF(-offset, offset),
                widgetGeometry.bottomLeft() + QPointF(0, offset));
    path.lineTo(widgetGeometry.bottomRight() + QPointF(0, offset));
    path.quadTo(widgetGeometry.bottomRight() + QPointF(offset, offset),
                widgetGeometry.bottomRight() + QPointF(offset, 0));
    path.lineTo(widgetGeometry.topRight() + QPointF(offset, 0));
    path.quadTo(widgetGeometry.topRight() + QPointF(offset, -offset),
                widgetGeometry.topRight() + QPointF(0, -offset));
    path.lineTo(widgetGeometry.topLeft() + QPointF(0, -offset));

    painter->setPen(Qt::black);
    painter->setBrush(frameColor);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPath(path);
    painter->setRenderHint(QPainter::Antialiasing, false);
}

bool ProgressWidgetGraphicsProxy::cancelRequested() const {
    return progressWidget->cancelRequested();
}
