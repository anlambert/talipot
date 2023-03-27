/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef IMPORT_WIZARD_H
#define IMPORT_WIZARD_H

#include <QWizard>
#include <QModelIndex>
#include <talipot/WithParameter.h>

namespace Ui {
class ImportWizard;
}

namespace tlp {
class GraphHierarchiesModel;
}

class QSortFilterProxyModel;

class ImportWizard : public QWizard {
  Q_OBJECT

  Ui::ImportWizard *_ui;
  QSortFilterProxyModel *_filterModel;

public:
  explicit ImportWizard(QWidget *parent = nullptr);
  ~ImportWizard() override;

  QString algorithm() const;
  tlp::DataSet parameters() const;

protected slots:
  void algorithmSelected(const QModelIndex &index);
  void updateFinishButton();
  void filterTextChanged(const QString &);
};

#endif // IMPORT_WIZARD_H
