/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_MODEL_H
#define TALIPOT_MODEL_H

#include <QAbstractItemModel>

#include <talipot/config.h>

namespace tlp {

class TLP_QT_SCOPE Model : public QAbstractItemModel {
  Q_OBJECT
  Q_ENUMS(TalipotRole)

public:
  enum TalipotRole {
    GraphRole = Qt::UserRole + 1,
    PropertyRole = Qt::UserRole + 2,
    IsNodeRole = Qt::UserRole + 3,
    StringRole = Qt::UserRole + 4,
    MandatoryRole = Qt::UserRole + 5,
    ElementIdRole = Qt::UserRole + 6,
    ElementSelectedRole = Qt::UserRole + 7,
  };

  explicit Model(QObject *parent = nullptr);
  ~Model() override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

signals:
  void checkStateChanged(QModelIndex, Qt::CheckState);
};
}

#endif // TALIPOT_MODEL_H
