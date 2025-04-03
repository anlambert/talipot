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

#ifndef ALGORITHM_RUNNER_ITEM_H
#define ALGORITHM_RUNNER_ITEM_H

#include <QWidget>

#include <talipot/DataSet.h>

namespace Ui {
class AlgorithmRunnerItem;
}
namespace tlp {
class Graph;
class ParameterListModel;
}

class AlgorithmRunnerItem : public QWidget {
    Q_OBJECT
    Ui::AlgorithmRunnerItem *_ui;
    QString _pluginName;
    tlp::Graph *_graph;
    bool _storeResultAsLocal;
    QPointF _dragStartPosition;
    tlp::DataSet _initData;
    static tlp::ParameterListModel *colorMappingModel;

  public:
    explicit AlgorithmRunnerItem(QString pluginName, QWidget *parent = nullptr);
    ~AlgorithmRunnerItem() override;

    QString name() const;
    tlp::Graph *graph() const;
    tlp::DataSet data() const;

    void setFavorite(bool);
    void elideAlgorithmButtonText(int containerWidth);

  protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

  public slots:
    void setGraph(tlp::Graph *);
    void setData(const tlp::DataSet &data);
    void setStoreResultAsLocal(bool);
    void favoriteChanged(int state);

    void run(tlp::Graph *g = nullptr);

  signals:
    void favorized(bool);

  protected slots:
    void afterRun(tlp::Graph *, const tlp::DataSet &);
    void initModel();
};
#endif // ALGORITHM_RUNNER_ITEM_H
