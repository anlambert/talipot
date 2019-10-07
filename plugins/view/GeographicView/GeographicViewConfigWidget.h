/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef GEOGRAPHIC_VIEW_CONFIG_WIDGET_H
#define GEOGRAPHIC_VIEW_CONFIG_WIDGET_H

#include <QWidget>

#include <talipot/DataSet.h>

namespace Ui {
class GeographicViewConfigWidgetData;
}

namespace tlp {
class GeographicViewConfigWidget : public QWidget {

  Q_OBJECT

  Ui::GeographicViewConfigWidgetData *_ui;

public:
  enum PolyFileType { None = -1, Default = 0, CsvFile, PolyFile };

  GeographicViewConfigWidget(QWidget *parent = nullptr);
  ~GeographicViewConfigWidget() override;

  bool useSharedLayoutProperty() const;
  bool useSharedSizeProperty() const;
  bool useSharedShapeProperty() const;

  PolyFileType polyFileType() const;
  void setPolyFileType(PolyFileType &fileType);
  QString getCsvFile() const;
  QString getPolyFile() const;

  bool polyOptionsChanged();

  QString getCustomTileLayerUrl() const;

  void setState(const DataSet &dataSet);

  DataSet state() const;

signals:

  void mapToPolygonSignal();

public slots:
  void openCsvFileBrowser();
  void openPolyFileBrowser();
  void openCsvHelp();
  void openPolyHelp();

  void mapToPolygonSlot() {
    emit mapToPolygonSignal();
  }

protected:
  PolyFileType _oldPolyFileType;
  std::string _oldFileLoaded;
};
}

#endif // GEOGRAPHIC_VIEW_CONFIG_WIDGET_H
