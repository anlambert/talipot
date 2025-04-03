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

#ifndef TALIPOT_HEADER_FRAME_H
#define TALIPOT_HEADER_FRAME_H

#include <talipot/config.h>

#include <QFrame>

namespace Ui {
class HeaderFrame;
}

namespace tlp {

class TLP_QT_SCOPE HeaderFrame : public QFrame {
    Q_OBJECT

    QPair<int, int> _oldHeightInfo;

    Ui::HeaderFrame *_ui;
    Q_PROPERTY(QString title READ title WRITE setTitle)
    QString _title;

    Q_PROPERTY(bool expandable READ isExpandable WRITE setExpandable)

    Q_PROPERTY(bool expanded READ isExpanded WRITE expand)
    bool _expanded;

    Q_PROPERTY(QStringList menus READ menus WRITE setMenus)

  public:
    explicit HeaderFrame(QWidget *parent = nullptr);
    ~HeaderFrame() override;

    QString title() const;
    QStringList menus() const;
    QString currentMenu() const;
    int currentMenuIndex() const;

    bool isExpandable() const;
    bool isExpanded() const;

    QWidget *expandControl() const;

  public slots:
    void setTitle(const QString &title);
    void setMenus(const QStringList &menus);
    void setExpandable(bool f);

    void expand(bool e);
    void insertWidget(QWidget *);
    void menuChanged(int);

  signals:
    void menuChanged(const QString &);
    void expanded(bool);

  protected slots:
    void setExpanded(bool e);
};

}

#endif // TALIPOT_HEADER_FRAME_H
