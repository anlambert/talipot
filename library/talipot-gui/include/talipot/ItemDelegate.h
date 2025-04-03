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

#ifndef TALIPOT_ITEM_DELEGATE_H
#define TALIPOT_ITEM_DELEGATE_H

#include <QStyledItemDelegate>

#include <talipot/ItemEditorCreators.h>
#include <talipot/MetaTypes.h>

namespace tlp {

class TLP_QT_SCOPE ItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

    QMap<int, ItemEditorCreator *> _creators;

    QObject *_currentMonitoredChild;
    QComboBox *_currentMonitoredCombo;

  public:
    static QVariant showEditorDialog(tlp::ElementType, tlp::PropertyInterface *, tlp::Graph *,
                                     ItemDelegate *, QWidget *dialogParent = nullptr,
                                     uint id = UINT_MAX);

    explicit ItemDelegate(QObject *parent = nullptr);
    ~ItemDelegate() override;

    template <typename T>
    void registerCreator(tlp::ItemEditorCreator *);

    void unregisterCreator(tlp::ItemEditorCreator *);

    template <typename T>
    void unregisterCreator();

    template <typename T>
    tlp::ItemEditorCreator *creator() const;

    tlp::ItemEditorCreator *creator(int) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool eventFilter(QObject *object, QEvent *event) override;

  protected slots:
    void comboDataChanged();
};
}

#include "cxx/ItemDelegate.cxx"

#endif // TALIPOT_ITEM_DELEGATE_H
