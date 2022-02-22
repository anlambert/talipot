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

#ifndef TALIPOT_CSV_PARSER_CONFIGURATION_WIDGET_H
#define TALIPOT_CSV_PARSER_CONFIGURATION_WIDGET_H

#include <climits>

#include <QWidget>

#include <talipot/config.h>

namespace Ui {
class CSVParserConfigurationWidget;
}

namespace tlp {

class CSVParser;

class TLP_QT_SCOPE CSVParserConfigurationWidget : public QWidget {
  Q_OBJECT
public:
  CSVParserConfigurationWidget(QWidget *parent = nullptr);
  ~CSVParserConfigurationWidget() override;
  /**
   * @brief Generate a csv parser in function of the parameters in the widget. User define the
   *range of line to import.
   * The user takes the ownership of the widget.
   **/
  CSVParser *buildParser(uint firstLine = 0, uint lastLine = UINT_MAX) const;

  std::string getFile() const;
  bool isValid() const;
  QString getSeparator() const;
  char getTextSeparator() const;
  char getDecimalMark() const;
  std::string getEncoding() const;
  bool invertMatrix() const;
  bool getMergeSeparator() const;
  void initWithLastOpenedFile();
  int getFirstLineIndex() const;
  void setNbIgnoredLines(int);
  void clearFile();

public slots:
  void setFileToOpen(const QString &fileToOpen);

protected:
  void fillEncodingComboBox();

protected slots:
  void changeFileNameButtonPressed();
  void encodingChanged();
  void changeSeparator(int index);
  void ignoreFirstLines(int checkState);

private:
  QString getSeparator(int index) const;

  Ui::CSVParserConfigurationWidget *ui;
  static QString lastOpenedFile;

signals:
  void parserChanged();
};
}
#endif // TALIPOT_CSV_PARSER_CONFIGURATION_WIDGET_H
