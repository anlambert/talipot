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

#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <talipot/ViewWidget.h>
#include <talipot/BooleanProperty.h>
#include <QModelIndex>

namespace Ui {
class TableView;
}

namespace tlp {
class GraphModel;
}

class TableViewConfiguration;
class PropertiesEditor;

class TableView : public tlp::ViewWidget {
  Q_OBJECT

  Ui::TableView *_ui;
  PropertiesEditor *propertiesEditor;
  tlp::GraphModel *_model;
  bool isNewGraph;
  bool filteringColumns;
  tlp::Graph *previousGraph;
  int minFontSize;

public:
  PLUGININFORMATION("Spreadsheet", "Tulip Team", "04/17/2012", "Spreadsheet view for raw data",
                    "4.0", "")

  TableView(tlp::PluginContext *);
  ~TableView() override;
  std::string icon() const override {
    return ":/spreadsheet_view.png";
  }
  tlp::DataSet state() const override;
  void setState(const tlp::DataSet &) override;
  void setupWidget() override;
  QList<QWidget *> configurationWidgets() const override;
  bool getNodeOrEdgeAtViewportPos(int x, int y, tlp::node &n, tlp::edge &e) const override;

  void draw() override;
public slots:
  void readSettings();
  void setPropertyVisible(tlp::PropertyInterface *, bool);
  void filterChanged();
  tlp::BooleanProperty *getFilteringProperty() const;
  bool hasEffectiveFiltering();

protected:
  void graphChanged(tlp::Graph *) override;
  void graphDeleted(tlp::Graph *) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

protected slots:
  void delHighlightedRows();
  void toggleHighlightedRows();
  void selectHighlightedRows();
  bool setAllHighlightedRows(tlp::PropertyInterface *);
  bool setCurrentValue(tlp::PropertyInterface *, uint);
  void setLabelsOfHighlightedRows(tlp::PropertyInterface *);
  void setColumnsFilter(const QString &);
  void setColumnsFilterCase();
  void setPropertiesFilter(const QString &);
  void mapToGraphSelection();

  void columnsInserted(const QModelIndex &, int, int);
  void showCustomContextMenu(const QPoint &pos);
  void showHorizontalHeaderCustomContextMenu(const QPoint &pos);
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
  void setZoomLevel(int);
};

#endif // TABLE_VIEW_H
