/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_CSV_GRAPH_IMPORT_H
#define TALIPOT_CSV_GRAPH_IMPORT_H

#include <talipot/hash.h>

#include <talipot/CSVContentHandler.h>
#include <talipot/Graph.h>
#include <talipot/config.h>

#include <QMessageBox>

namespace tlp {
class PropertyInterface;

/**
 * @brief Store import parameters for a CSV file column.
 *
 * Contains all the parameters defined by user for a given CSV column (the name of the column, its
 *data type and if user want to import it).
 **/
class TLP_QT_SCOPE CSVColumn {
public:
  CSVColumn(const std::string &columnName = "", const std::string &columnType = "")
      : _used(true), _name(columnName), _type(columnType), _valueSeparator(0) {}

  /**
   * @brief Get the name of the column.
   **/
  const std::string &name() const {
    return _name;
  }

  /**
   * @brief Tells if the property marked for import.
   **/
  bool isUsed() const {
    return _used;
  }

  /**
   * @brief Return the property data type.
   **/
  const std::string &dataType() const {
    return _type;
  }

  bool needMultiValues() const {
    return _valueSeparator != 0;
  }

  char getMultiValueSeparator() const {
    return _valueSeparator;
  }

  // possible actions
  // the two first ones indicate an exception
  enum Action { ASSIGN_NO_VALUE = 0, SKIP_ROW = 1, ASSIGN_VALUE = 2 };

  struct Exception {
    std::string value;
    Action action;
    Exception(const std::string &v, Action a) : value(v), action(a) {}
  };

  void addException(const std::string &value, Action action) {
    _exceptions.push_back(Exception(value, action));
  }

  void clearExceptions() {
    _exceptions.clear();
  }

  // look for a specific exception defined for token
  Action getActionForToken(const std::string &token) {
    for (const Exception &exception : _exceptions) {
      if (exception.value == token) {
        return exception.action;
      }
    }
    return Action::ASSIGN_VALUE;
  }

protected:
  bool _used;
  std::string _name;
  std::string _type;
  char _valueSeparator;
  std::vector<Exception> _exceptions;
};
/**
 * @brief Store all the advanced import parameters for the CSV file.
 *
 * Store the information about columns and rows to import.
 * Use this object to configure the import process of a CSVImportGraph object.
 **/
class TLP_QT_SCOPE CSVImportParameters {
public:
  CSVImportParameters(uint fromLine = 0, uint toLine = UINT_MAX,
                      const std::vector<CSVColumn *> &columns = std::vector<CSVColumn *>());
  virtual ~CSVImportParameters();

  /**
   * @brief Return the number of column.
   **/
  uint columnNumber() const;

  /**
   * @brief return true if the column is marked for import
   **/
  bool importColumn(uint column) const;
  /**
   * @brief Get the column name
   **/
  std::string getColumnName(uint column) const;
  /**
   * @brief Get the column data type
   **/
  std::string getColumnDataType(uint column) const;

  /**
   * @brief Get the column separator for multiple values
   **/
  char getColumnMultiValueSeparator(uint column) const;

  /**
   * @brief Get the column action according to the given token
   **/
  CSVColumn::Action getColumnActionForToken(uint column, const std::string &token) const;

  /**
   * @brief Return the index of the first line to import
   **/
  uint getFirstLineIndex() const;
  /**
   * @brief Return the index of the last line to import
   **/
  uint getLastLineIndex() const;
  /**
   * @brief Return true if the given row is between the first row to import and the last row to
   *import
   **/
  bool importRow(uint row) const;

private:
  uint fromLine;
  uint toLine;
  std::vector<CSVColumn *> columns;
};

/**
 * @brief Interface to map CSV rows to graph elements.
 *
 * To build the mapping user had to parse the CSV file.
 * @code
 * CSVParser *parser;
 * CSVToGraphDataMapping *mapping;
 * parser->parse(mapping);
 * //Now the mapping has been built.
 * //Get the element for the first row.
 * pair<tlp::ElementType,uint> element = mapping->getElementForRow(0);
 * @endcode
 **/
class TLP_QT_SCOPE CSVToGraphDataMapping {
public:
  virtual ~CSVToGraphDataMapping() = default;
  virtual std::pair<tlp::ElementType, std::vector<uint>>
  getElementsForRow(const std::vector<std::vector<std::string>> &tokens) = 0;
  virtual void init(uint rowNumber) = 0;
};

/**
 * @brief Abstract class handling node or edge mapping between a CSV column and a graph property.
 *
 * Be sure there is a property with the given name in the graph or an error will occur.
 * Automatically handle CSV file parsing just implements the buildIndexForRow function to fill the
 *rowToGraphId map with the right graph element.
 **/
class TLP_QT_SCOPE AbstractCSVToGraphDataMapping : public CSVToGraphDataMapping {
public:
  AbstractCSVToGraphDataMapping(tlp::Graph *graph, tlp::ElementType type,
                                const std::vector<uint> &columnIds,
                                const std::vector<std::string> &propertyNames);
  ~AbstractCSVToGraphDataMapping() override = default;

  void init(uint rowNumber) override;
  std::pair<tlp::ElementType, std::vector<uint>>
  getElementsForRow(const std::vector<std::vector<std::string>> &tokens) override;

protected:
  /**
   * @brief Create a new element if no elements for the given row was found.
   * @return Return the graph element id or UINT_MAX if no new element is created.
   **/
  virtual uint buildIndexForRow(uint row, const std::vector<std::string> &keys) = 0;

protected:
  flat_hash_map<std::string, uint> valueToId;
  tlp::Graph *graph;
  tlp::ElementType type;
  std::vector<uint> columnIds;
  std::vector<tlp::PropertyInterface *> keyProperties;
};
/**
 * @brief Map each row of the CSV file on a new node.
 **/
class TLP_QT_SCOPE CSVToNewNodeIdMapping : public CSVToGraphDataMapping {
public:
  CSVToNewNodeIdMapping(tlp::Graph *graph);
  void init(uint rowNumber) override;
  std::pair<tlp::ElementType, std::vector<uint>>
  getElementsForRow(const std::vector<std::vector<std::string>> &tokens) override;

private:
  tlp::Graph *graph;
};

/**
 * @brief Try to map CSV file rows to nodes according to value between a CSV column and a graph
 *property.
 *
 * Be sure there is a property with the given name in the graph before using it.
 **/
class TLP_QT_SCOPE CSVToGraphNodeIdMapping : public AbstractCSVToGraphDataMapping {
public:
  /**
   * @param graph The graph where the nodes will be searched.
   * @param columnIndex The index of the column with the ids in the CSV file.
   * @param propertyName The name of the property to search ids.
   * @param firstRow The first row to search ids.
   * @param lastRow The last row to search ids.
   * @param createNode If set to true if there is no node for an id in the CSV file a new node will
   *be created for this id.
   **/
  CSVToGraphNodeIdMapping(tlp::Graph *graph, const std::vector<uint> &columnIds,
                          const std::vector<std::string> &propertyNames, bool createNode = false);
  void init(uint rowNumber) override;

protected:
  uint buildIndexForRow(uint row, const std::vector<std::string> &keys) override;

private:
  bool createMissingNodes;
};
/**
 * @brief Try to map CSV file rows to edges according to value between a CSV column and a graph
 *property.
 *
 * Be sure there is a property with the given name in the graph before using it.
 **/
class TLP_QT_SCOPE CSVToGraphEdgeIdMapping : public AbstractCSVToGraphDataMapping {
public:
  /**
   * @param graph The graph where the edges will be searched.
   * @param columnIndex The index of the column with the ids in the CSV file.
   * @param propertyName The name of the property to search ids.
   * @param firstRow The first row to search ids.
   * @param lastRow The last row to search ids.
   **/
  CSVToGraphEdgeIdMapping(tlp::Graph *graph, const std::vector<uint> &columnIds,
                          const std::vector<std::string> &propertyNames);

protected:
  uint buildIndexForRow(uint row, const std::vector<std::string> &keys) override;
};

/**
 * @brief Try to map CSV file rows to edges according to edge source and destination.
 *
 * For each row in the CSV file create an edge in the graph between source and destination nodes.
 *Find source node by comparing id in the source CSV column and destination node by comparing id in
 *the destination CSV column.
 **/
class TLP_QT_SCOPE CSVToGraphEdgeSrcTgtMapping : public CSVToGraphDataMapping {
public:
  /**
   * @param graph The graph where the edges will be searched.
   * @param srcColumnIndex The index of the column with the source node id in the CSV file.
   * @param tgtColumnIndex The index of the column with the taret node id in the CSV file.
   * @param srcPropertyName The name of the property to search source node id.
   * @param tgtPropertyName The name of the property to search target node id.
   * @param firstRow The first row to search ids.
   * @param lastRow The last row to search ids.
   * @param createMissinElements If true create source node, destination node if one of them is not
   *found in the graph.
   **/
  CSVToGraphEdgeSrcTgtMapping(tlp::Graph *graph, const std::vector<uint> &srcColumnIds,
                              const std::vector<uint> &tgtColumnIds,
                              const std::vector<std::string> &srcPropNames,
                              const std::vector<std::string> &tgtPropNames,
                              bool createMissinElements = false);
  std::pair<tlp::ElementType, uint> getElementForRow(uint row);
  void init(uint lineNumbers) override;
  std::pair<tlp::ElementType, std::vector<uint>>
  getElementsForRow(const std::vector<std::vector<std::string>> &tokens) override;

private:
  tlp::Graph *graph;
  flat_hash_map<std::string, uint> srcValueToId;
  flat_hash_map<std::string, uint> tgtValueToId;
  std::vector<uint> srcColumnIds;
  std::vector<uint> tgtColumnIds;
  std::vector<tlp::PropertyInterface *> srcProperties;
  std::vector<tlp::PropertyInterface *> tgtProperties;
  bool sameSrcTgtProperties;
  bool buildMissingElements;
};

/**
 * @brief Interface to perform mapping between CSV columns and graph properties during the CSV
 *import process.
 *
 **/
class TLP_QT_SCOPE CSVImportColumnToGraphPropertyMapping {
public:
  virtual ~CSVImportColumnToGraphPropertyMapping() = default;
  /**
   * @brief Return the property corresponding to the column index.
   * @param column The index of the column.
   * @param token The current token. May be needed to determine column data type.
   *
   * The token parameter is used to guess property type if needed.
   **/
  virtual tlp::PropertyInterface *getPropertyInterface(uint column, const std::string &token) = 0;
};

/**
 * @brief Proxy to handle all the properties operations like access, creation, data type detection
 *during the CSV parsing process.
 *
 * Try to guess the type of the property in function of the first token
 * if user don't tell which type the property is.
 **/
class TLP_QT_SCOPE CSVImportColumnToGraphPropertyMappingProxy
    : public CSVImportColumnToGraphPropertyMapping {
public:
  CSVImportColumnToGraphPropertyMappingProxy(tlp::Graph *graph,
                                             const CSVImportParameters &importParameters,
                                             QWidget *parent = nullptr);
  ~CSVImportColumnToGraphPropertyMappingProxy() override = default;
  tlp::PropertyInterface *getPropertyInterface(uint column, const std::string &token) override;

private:
  tlp::Graph *graph;
  CSVImportParameters importParameters;
  flat_hash_map<uint, tlp::PropertyInterface *> propertiesBuffer;
  QMessageBox::StandardButton overwritePropertiesButton;
  QWidget *parent;
  PropertyInterface *generateApproximateProperty(const std::string &name, const std::string &type);
};

/**
 * @brief Manage all the CSV import process. Use the mapping object to find the graph element in
 *function of the row and the propertiesManager to find the property corresponding to the column.
 * The import parameters are used to filter the rows and the columns to import.
 **/
class TLP_QT_SCOPE CSVGraphImport : public tlp::CSVContentHandler {
public:
  CSVGraphImport(CSVToGraphDataMapping *mapping,
                 CSVImportColumnToGraphPropertyMapping *propertiesManager,
                 const CSVImportParameters &importParameters);
  ~CSVGraphImport() override;
  bool begin() override;
  bool line(uint row, const std::vector<std::string> &lineTokens) override;
  bool end(uint rowNumber, uint columnNumber) override;

protected:
  CSVToGraphDataMapping *mapping;
  CSVImportColumnToGraphPropertyMapping *propertiesManager;
  CSVImportParameters importParameters;
};
}
#endif // TALIPOT_CSV_GRAPH_IMPORT_H
