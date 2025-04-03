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

#ifndef PROGRESS_WIDGET_GRAPHICS_PROXY_H
#define PROGRESS_WIDGET_GRAPHICS_PROXY_H

#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QColor>

class QPainter;
class QStyleOptionGraphicsItem;

namespace Ui {
class ProgressWidget;
}

namespace tlp {

class ProgressWidget : public QWidget {

    Q_OBJECT

    Ui::ProgressWidget *_ui;

  public:
    ProgressWidget(QWidget *parent = nullptr);
    ~ProgressWidget() override;

    void setComment(const QString &comment);

    void setProgress(int value, int max);

    bool cancelRequested() const {
        return cancelClicked;
    }

  protected:
    void showEvent(QShowEvent *event) override;

  private slots:

    void cancelButtonClicked();

  private:
    bool cancelClicked;
};

class ProgressWidgetGraphicsProxy : public QGraphicsProxyWidget {

  public:
    ProgressWidgetGraphicsProxy();

    void setComment(const QString &comment);

    void setProgress(int value, int max);

    void setFrameColor(const QColor &frameColor) {
        this->frameColor = frameColor;
    }

    bool cancelRequested() const;

  protected:
    void paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option,
                          QWidget *widget = nullptr) override;

  private:
    ProgressWidget *progressWidget;
    QColor frameColor;
};
}

#endif // PROGRESS_WIDGET_GRAPHICS_PROXY_H
