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

#ifndef TALIPOT_CSV_GRAPH_MAPPING_CONFIGURATION_WIDGET_H
#define TALIPOT_CSV_GRAPH_MAPPING_CONFIGURATION_WIDGET_H

#include <QComboBox>
#include <QPushButton>
#include <QString>

#include <talipot/config.h>

namespace Ui {
class CSVGraphMappingConfigurationWidget;
}

namespace tlp {

class Graph;
class CSVGraphImport;
class CSVImportParameters;
class CSVToGraphDataMapping;

/**
 * @brief Widget generating the CSVToGraphDataMapping object.
 *
 * This widget allow user to configure a CSVToGraphDataMapping object. This object is used during
 *the CSV import process to map CSV columns to graph elements like nodes or edges.
 **/
class TLP_QT_SCOPE CSVGraphMappingConfigurationWidget : public QWidget {
    Q_OBJECT
  public:
    CSVGraphMappingConfigurationWidget(QWidget *parent = nullptr);
    ~CSVGraphMappingConfigurationWidget() override;

    /**
     * @brief Configure the widget with the CSV import parameters.
     **/
    void updateWidget(tlp::Graph *graph, const CSVImportParameters &importParameters);
    /**
     * @brief Build the mapping object between the CSV columns and the graph elements.
     **/
    CSVToGraphDataMapping *buildMappingObject() const;

    /**
     * @brief Check if the values entered by user are valid.
     *
     * If return true you are sure that buildMappingObject never return a nullptr object.
     **/
    bool isValid() const;

  protected:
    tlp::Graph *graph;
    std::vector<std::string> columns;
    std::vector<uint> nodeColumnIds;
    std::vector<std::string> nodeProperties;
    std::vector<uint> edgeColumnIds;
    std::vector<std::string> edgeProperties;
    std::vector<uint> srcColumnIds;
    std::vector<uint> tgtColumnIds;
    std::vector<std::string> srcProperties;
    std::vector<std::string> tgtProperties;

  private:
    Ui::CSVGraphMappingConfigurationWidget *ui;
    void selectColumns(const QString &title, std::vector<uint> &columnIds, QPushButton *button);
    void selectProperties(const QString &title, std::vector<std::string> &selProperties,
                          QPushButton *button);

  private slots:
    void createNewProperty();
    void selectNodeColumns();
    void selectEdgeColumns();
    void selectSrcColumns();
    void selectTgtColumns();
    void selectSrcProperties();
    void selectTgtProperties();
    void selectNodeProperties();
    void selectEdgeProperties();

  signals:
    void mappingChanged();
};
}
#endif // TALIPOT_CSV_GRAPH_MAPPING_CONFIGURATION_WIDGET_H
