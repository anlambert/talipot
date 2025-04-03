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

#ifndef ALGORITHM_RUNNER_H
#define ALGORITHM_RUNNER_H

#include <QWidget>

#include <talipot/PluginModel.h>
#include <talipot/Algorithm.h>

#include "AlgorithmRunnerItem.h"

namespace Ui {
class AlgorithmRunner;
class AlgorithmRunnerItem;
} // namespace Ui

class QToolButton;

namespace tlp {
class Graph;
}

class AlgorithmRunner : public QWidget {
    Q_OBJECT

    Ui::AlgorithmRunner *_ui;
    tlp::Graph *_graph;
    QToolButton *_storeResultAsLocalButton;
    QAction *_resultAsLocalPropAction;

    QList<AlgorithmRunnerItem *> _favorites;

    void refreshTreeUi(QWidget *w);
    void buildTreeUi(QWidget *w, tlp::PluginModel<tlp::Algorithm> *model, const QModelIndex &parent,
                     bool root = false);
    void insertItem(QWidget *w, const QString &name) const;

  public:
    explicit AlgorithmRunner(QWidget *parent = nullptr);
    ~AlgorithmRunner() override;

  signals:
    void setStoreResultAsLocal(bool);

  public slots:
    void setGraph(tlp::Graph *);
    void findPlugins();
    void buildPluginsList();
    void refreshPluginsList();

  protected slots:
    void setFilter(QString);
    void addFavorite(const QString &algName, const tlp::DataSet &data = tlp::DataSet());
    void removeFavorite(const QString &algName);
    void favorized(bool);
    void setStoreResultAsLocal(QAction *);
    void expanded(bool);

  protected:
    bool eventFilter(QObject *, QEvent *) override;
    void resizeEvent(QResizeEvent *) override;
};

#endif // ALGORITHM_RUNNER_H
