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

#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QWidget>

namespace Ui {
class SearchWidget;
}

class QComboBox;
class SearchOperator;

namespace tlp {
class GraphHierarchiesModel;
class Graph;
class PropertyInterface;
}

class SearchWidget : public QWidget {
    Q_OBJECT

    QVector<SearchOperator *> NUMERIC_OPERATORS;
    QVector<SearchOperator *> STRING_OPERATORS;
    QVector<SearchOperator *> NOCASE_STRING_OPERATORS;

    Ui::SearchWidget *_ui;
    tlp::Graph *_graph;

    tlp::PropertyInterface *term(QComboBox *combo);

    SearchOperator *searchOperator();
    bool isNumericComparison();

  public:
    explicit SearchWidget(QWidget *parent = nullptr);
    ~SearchWidget() override;

  signals:

  public slots:
    void setModel(tlp::GraphHierarchiesModel *model);
    void currentGraphChanged(tlp::Graph *g);
    void setGraph(tlp::Graph *g);
    void search();

  protected slots:

    void graphIndexChanged();
    void termAChanged();
    void termBChanged();
    void updateOperators(tlp::PropertyInterface *a, tlp::PropertyInterface *b);
    void updateOperators(tlp::PropertyInterface *a, const QString &b);
    void setNumericOperatorsEnabled(bool e);
    void updateEditorWidget();
    void selectionModeChanged(int index);

  protected:
    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;
};

#endif // SEARCH_WIDGET_H
