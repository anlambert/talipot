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

///@cond DOXYGEN_HIDDEN

#ifndef TALIPOT_CSV_IMPORT_WIZARD_H
#define TALIPOT_CSV_IMPORT_WIZARD_H

#include <QWizard>
#include <QWizardPage>

#include <talipot/config.h>

namespace Ui {
class CSVImportWizard;
}
namespace tlp {

class Graph;
class CSVParser;
class CSVParserConfigurationWidget;
class CSVTableWidget;
class CSVImportConfigurationWidget;
class CSVToGraphDataMapping;
class CSVGraphMappingConfigurationWidget;
class CSVImportParameters;

/**
 * @brief QWIzardPage encapsulating a CSVParserConfigurationWidget and a preview.
 **/
class CSVParsingConfigurationQWizardPage : public QWizardPage {
  Q_OBJECT
public:
  CSVParsingConfigurationQWizardPage(QWidget *parent = nullptr);
  bool isComplete() const override;
  CSVParser *buildParser(int firstLine = 0) const;
  int getFirstLineIndex() const;

private:
  void updatePreview();
  CSVParserConfigurationWidget *parserConfigurationWidget;
  CSVTableWidget *previewTableWidget;
  unsigned int previewLineNumber;

private slots:
  void parserChanged();
};

class CSVImportConfigurationQWizardPage : public QWizardPage {
  Q_OBJECT
public:
  CSVImportConfigurationQWizardPage(QWidget *parent = nullptr);
  void initializePage() override;
  CSVImportParameters getImportParameters() const;

private:
  CSVImportConfigurationWidget *importConfigurationWidget;
};

class CSVGraphMappingConfigurationQWizardPage : public QWizardPage {
  Q_OBJECT
public:
  CSVGraphMappingConfigurationQWizardPage(QWidget *parent = nullptr);
  void initializePage() override;
  bool isComplete() const override;
  CSVToGraphDataMapping *buildMappingObject() const;

private:
  CSVGraphMappingConfigurationWidget *graphMappingConfigurationWidget;
};

class TLP_QT_SCOPE CSVImportWizard : public QWizard {
  Q_OBJECT

public:
  explicit CSVImportWizard(QWidget *parent = nullptr);
  ~CSVImportWizard() override;

  CSVParsingConfigurationQWizardPage *getParsingConfigurationPage() const;
  CSVImportConfigurationQWizardPage *getImportConfigurationPage() const;
  CSVGraphMappingConfigurationQWizardPage *getMappingConfigurationPage() const;

  static void setGraph(Graph *g) {
    graph = g;
  }

  static Graph *getGraph() {
    return graph;
  }

public slots:
  void accept() override;

private:
  Ui::CSVImportWizard *ui;
  static tlp::Graph *graph;
};
} // namespace tlp
#endif // TALIPOT_CSV_IMPORT_WIZARD_H
///@endcond
