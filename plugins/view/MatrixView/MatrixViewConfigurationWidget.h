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

#ifndef MATRIX_VIEW_CONFIGURATION_WIDGET_H
#define MATRIX_VIEW_CONFIGURATION_WIDGET_H

#include <QWidget>

#include <talipot/Color.h>

namespace Ui {
class MatrixViewConfigurationWidget;
}

namespace tlp {
class Graph;

enum GridDisplayMode { SHOW_ALWAYS = 0, SHOW_NEVER = 1, SHOW_ON_ZOOM = 2 };

class MatrixViewConfigurationWidget : public QWidget {
    Q_OBJECT

    Ui::MatrixViewConfigurationWidget *_ui;

  public:
    MatrixViewConfigurationWidget(QWidget *parent = nullptr);
    void setGraph(tlp::Graph *);
    ~MatrixViewConfigurationWidget() override;

    GridDisplayMode gridDisplayMode() const;
    void setgridmode(int index);
    void setBackgroundColor(const QColor &);
    Color getBackgroundColor() const;
    void setDisplayEdges(const bool state);
    bool displayGraphEdges() const;
    void setEdgeColorInterpolation(const bool state);
    bool isEdgeColorInterpolation() const;
    int orderingProperty() const;
    void setOrderingProperty(int index);
    void setAscendingOrder(const bool state);
    bool ascendingOrder() const;
    void setOriented(const bool state);

  protected slots:
    void orderingMetricComboIndexChanged(int i);
    void orderingDirectionChanged();

  signals:
    void metricSelected(std::string);
    void changeBackgroundColor(QColor);
    void setGridDisplayMode();
    void showEdges(bool);
    void enableEdgeColorInterpolation(bool);
    void updateOriented(bool);

  private:
    bool _modifyingMetricList;
};
}
#endif // MATRIX_VIEW_CONFIGURATION_WIDGET_H
