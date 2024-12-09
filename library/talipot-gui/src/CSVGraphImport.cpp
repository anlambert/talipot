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

#include <iomanip>

#include "talipot/CSVGraphImport.h"
#include <talipot/TlpQtTools.h>

using namespace tlp;
using namespace std;

CSVImportParameters::CSVImportParameters(uint fromLine, uint toLine,
                                         const vector<CSVColumn *> &columns)
    : fromLine(fromLine), toLine(toLine), columns(columns) {}

CSVImportParameters::~CSVImportParameters() = default;

uint CSVImportParameters::columnNumber() const {
  return columns.size();
}

bool CSVImportParameters::importColumn(uint column) const {
  if (column < columns.size()) {
    return columns[column]->isUsed();
  } else {
    return false;
  }
}

string CSVImportParameters::getColumnName(uint column) const {
  if (column < columns.size()) {
    return columns[column]->name();
  } else {
    return string();
  }
}

string CSVImportParameters::getColumnDataType(uint column) const {
  if (column < columns.size()) {
    return columns[column]->dataType();
  } else {
    return string();
  }
}

char CSVImportParameters::getColumnMultiValueSeparator(uint column) const {
  if (column < columns.size()) {
    return columns[column]->getMultiValueSeparator();
  }
  return 0;
}

CSVColumn::Action CSVImportParameters::getColumnActionForToken(uint column,
                                                               const std::string &token) const {
  if (column < columns.size()) {
    return columns[column]->getActionForToken(token);
  }
  return CSVColumn::Action::SKIP_ROW;
}

bool CSVImportParameters::importRow(uint row) const {
  return row >= fromLine && row <= toLine;
}

uint CSVImportParameters::getFirstLineIndex() const {
  return fromLine;
}
uint CSVImportParameters::getLastLineIndex() const {
  return toLine;
}

AbstractCSVToGraphDataMapping::AbstractCSVToGraphDataMapping(Graph *graph, ElementType type,
                                                             const vector<uint> &colIds,
                                                             const vector<string> &propertyNames)
    : graph(graph), type(type), columnIds(colIds) {
  assert(graph != nullptr);

  for (const auto &propertyName : propertyNames) {
    assert(graph->existProperty(propertyName));
    keyProperties.push_back(graph->getProperty(propertyName));
  }
}

void AbstractCSVToGraphDataMapping::init(uint) {
  // Clean old information.
  valueToId.clear();

  // Fill map with graph values.
  if (type == ElementType::NODE) {
    for (auto n : graph->nodes()) {
      string key;

      for (const auto &keyProperty : keyProperties) {
        key.append(keyProperty->getNodeStringValue(n));
      }

      valueToId[key] = n.id;
    }
  } else {
    for (auto e : graph->edges()) {
      string key;

      for (const auto &keyProperty : keyProperties) {
        key.append(keyProperty->getEdgeStringValue(e));
      }

      valueToId[key] = e.id;
    }
  }
}

pair<ElementType, vector<uint>>
AbstractCSVToGraphDataMapping::getElementsForRow(const vector<vector<string>> &tokens) {
  vector<uint> results(1);

  bool idsOK = true;

  // Check if the ids are available for this line
  for (uint columnId : columnIds) {
    if (columnId >= tokens.size()) {
      idsOK = false;
      break;
    }
  }

  if (idsOK) {
    string key;
    vector<string> keys;

    for (uint columnId : columnIds) {
      for (const string &token : tokens[columnId]) {
        key.append(token);
        keys.push_back(token);
      }
    }

    if (!valueToId.contains(key)) {
      // Try to generate the element
      uint id = buildIndexForRow(0, keys);

      // If the token was correctly generated.
      if (id != UINT_MAX) {
        // Store the id corresponding to the token.
        valueToId[key] = id;
      }

      results[0] = id;
    } else {
      // Use the last found id.
      results[0] = valueToId[key];
    }
  } else {
    results[0] = UINT_MAX;
  }

  return pair<ElementType, vector<uint>>(type, results);
}

CSVToNewNodeIdMapping::CSVToNewNodeIdMapping(Graph *graph) : graph(graph) {}

void CSVToNewNodeIdMapping::init(uint rowNumber) {
  Graph *root = graph->getRoot();
  root->reserveNodes(root->numberOfNodes() + rowNumber);
}

pair<ElementType, vector<uint>>
CSVToNewNodeIdMapping::getElementsForRow(const vector<vector<string>> &) {
  vector<uint> result(1);
  result[0] = graph->addNode().id;
  return make_pair(ElementType::NODE, result);
}

CSVToGraphNodeIdMapping::CSVToGraphNodeIdMapping(Graph *graph, const vector<uint> &colIds,
                                                 const vector<string> &propNames, bool createNode)
    : AbstractCSVToGraphDataMapping(graph, ElementType::NODE, colIds, propNames),
      createMissingNodes(createNode) {}

void CSVToGraphNodeIdMapping::init(uint rowNumber) {
  AbstractCSVToGraphDataMapping::init(rowNumber);

  if (createMissingNodes) {
    Graph *root = graph->getRoot();
    root->reserveNodes(root->numberOfNodes() + rowNumber);
  }
}

uint CSVToGraphNodeIdMapping::buildIndexForRow(uint, const vector<string> &keys) {
  if (createMissingNodes && keys.size() == keyProperties.size()) {
    node newNode = graph->addNode();

    for (uint i = 0; i < keys.size(); ++i) {
      keyProperties[i]->setNodeStringValue(newNode, keys[i]);
    }

    return newNode.id;
  } else {
    return UINT_MAX;
  }
}

CSVToGraphEdgeIdMapping::CSVToGraphEdgeIdMapping(Graph *graph, const vector<uint> &colIds,
                                                 const vector<string> &propNames)
    : AbstractCSVToGraphDataMapping(graph, ElementType::EDGE, colIds, propNames) {}

uint CSVToGraphEdgeIdMapping::buildIndexForRow(uint, const vector<string> &) {
  return UINT_MAX;
}

CSVToGraphEdgeSrcTgtMapping::CSVToGraphEdgeSrcTgtMapping(
    Graph *graph, const vector<uint> &srcColIds, const vector<uint> &tgtColIds,
    const vector<string> &srcPropNames, const vector<string> &tgtPropNames, bool createMissinNodes)
    : graph(graph), srcColumnIds(srcColIds), tgtColumnIds(tgtColIds),
      sameSrcTgtProperties(srcPropNames.size() == tgtPropNames.size()),
      buildMissingElements(createMissinNodes) {
  assert(graph != nullptr);

  for (const auto &srcPropName : srcPropNames) {
    assert(graph->existProperty(srcPropName));
    srcProperties.push_back(graph->getProperty(srcPropName));
  }

  for (uint i = 0; i < tgtPropNames.size(); ++i) {
    assert(graph->existProperty(tgtPropNames[i]));
    tgtProperties.push_back(graph->getProperty(tgtPropNames[i]));
    sameSrcTgtProperties = sameSrcTgtProperties && (tgtPropNames[i] == srcPropNames[i]);
  }
}

void CSVToGraphEdgeSrcTgtMapping::init(uint rowNumber) {
  srcValueToId.clear();
  for (auto n : graph->nodes()) {
    string key;

    for (const auto &srcProperty : srcProperties) {
      key.append(srcProperty->getNodeStringValue(n));
    }

    srcValueToId[key] = n.id;

    if (!sameSrcTgtProperties) {
      key.clear();

      for (const auto &tgtProperty : tgtProperties) {
        key.append(tgtProperty->getNodeStringValue(n));
      }

      tgtValueToId[key] = n.id;
    }
  }

  // Reserve elements
  Graph *root = graph->getRoot();
  root->reserveEdges(root->numberOfEdges() + rowNumber);

  if (buildMissingElements) {
    // Need to reserve for source and target nodes.
    root->reserveNodes(2 * rowNumber + root->numberOfNodes());
  }
}

pair<ElementType, vector<uint>>
CSVToGraphEdgeSrcTgtMapping::getElementsForRow(const vector<vector<string>> &tokens) {

  vector<uint> results;

  vector<node> srcs;
  vector<node> tgts;

  bool idsOK = true;

  // Check if the src ids are available for this line
  for (uint srcColumnId : srcColumnIds) {
    if (srcColumnId >= tokens.size()) {
      idsOK = false;
      break;
    }
  }

  if (idsOK) {
    vector<vector<string>> keyTokens;

    for (uint srcColumnId : srcColumnIds) {
      const vector<string> &currentTokens = tokens[srcColumnId];

      // merge current tokens with previous ones
      if (keyTokens.empty()) {
        keyTokens.resize(currentTokens.size());

        for (uint j = 0; j < currentTokens.size(); ++j) {
          keyTokens[j].push_back(currentTokens[j]);
        }
      } else {
        vector<vector<string>> previousTokens(keyTokens);
        keyTokens.clear();
        keyTokens.resize(previousTokens.size() * currentTokens.size());

        for (uint j = 0; j < previousTokens.size(); ++j) {
          for (uint k = 0; k < currentTokens.size(); ++k) {
            keyTokens[j * currentTokens.size() + k] = previousTokens[j];
            keyTokens[j * currentTokens.size() + k].push_back(currentTokens[k]);
          }
        }
      }
    }

    for (const auto &keyToken : keyTokens) {
      // because column values may be of type vector
      // we can have several source entities
      string key;

      for (const auto &s : keyToken) {
        key.append(s);
      }

      auto it = srcValueToId.find(key);

      // token exists in the map
      if (it != srcValueToId.end()) {
        srcs.push_back(node(it->second));
      } else if (buildMissingElements && srcProperties.size() == keyToken.size()) {
        node src = graph->addNode();
        srcs.push_back(src);

        for (uint j = 0; j < keyToken.size(); ++j) {
          srcProperties[j]->setNodeStringValue(src, keyToken[j]);
        }

        srcValueToId[key] = src.id;
      }
    }
  }

  // Check if the target ids are available for this line
  for (uint tgtColumnId : tgtColumnIds) {
    if (tgtColumnId >= tokens.size()) {
      idsOK = false;
      break;
    }
  }

  if (idsOK) {
    vector<vector<string>> keyTokens;

    for (uint tgtColumnId : tgtColumnIds) {
      const vector<string> &currentTokens = tokens[tgtColumnId];

      // merge current tokens with previous ones
      if (keyTokens.empty()) {
        keyTokens.resize(currentTokens.size());

        for (uint j = 0; j < currentTokens.size(); ++j) {
          keyTokens[j].push_back(currentTokens[j]);
        }
      } else {
        vector<vector<string>> previousTokens(keyTokens);
        keyTokens.clear();
        keyTokens.resize(previousTokens.size() * currentTokens.size());

        for (uint j = 0; j < previousTokens.size(); ++j) {
          for (uint k = 0; k < currentTokens.size(); ++k) {
            keyTokens[j * currentTokens.size() + k] = previousTokens[j];
            keyTokens[j * currentTokens.size() + k].push_back(currentTokens[k]);
          }
        }
      }
    }

    flat_hash_map<string, uint> &valueToId = sameSrcTgtProperties ? srcValueToId : tgtValueToId;

    for (const auto &keyToken : keyTokens) {
      // because column values may be of type vector
      // we can have several target entities
      string key;

      for (const auto &s : keyToken) {
        key.append(s);
      }

      auto it = valueToId.find(key);

      // token exists in the map
      if (it != valueToId.end()) {
        tgts.push_back(node(it->second));
      } else if (buildMissingElements && tgtProperties.size() == keyToken.size()) {
        node tgt = graph->addNode();
        tgts.push_back(tgt);

        for (uint j = 0; j < keyToken.size(); ++j) {
          tgtProperties[j]->setNodeStringValue(tgt, keyToken[j]);
        }

        valueToId[key] = tgt.id;
      }
    }
  }

  // we create as much edges as we can build
  // of valid source-target entities couple
  for (auto src : srcs) {
    for (auto tgt : tgts) {
      if (src.isValid() && tgt.isValid()) {
        results.push_back(graph->addEdge(src, tgt).id);
      }
    }
  }

  return make_pair(ElementType::EDGE, results);
}

CSVImportColumnToGraphPropertyMappingProxy::CSVImportColumnToGraphPropertyMappingProxy(
    Graph *graph, const CSVImportParameters &importParameters, QWidget *parent)
    : graph(graph), importParameters(importParameters), parent(parent) {}

PropertyInterface *
CSVImportColumnToGraphPropertyMappingProxy::generateApproximateProperty(const std::string &name,
                                                                        const std::string &type) {
  // loop to generate a non existing approximate name
  std::ostringstream nameBuf;
  uint nb = 1;
  while (true) {
    nameBuf << name << '_' << setfill('0') << setw(2) << nb;
    if (!graph->existProperty(nameBuf.str())) {
      return graph->getProperty(nameBuf.str(), type);
    }
    nameBuf.seekp(0);
    ++nb;
  }
  return nullptr;
}

PropertyInterface *
CSVImportColumnToGraphPropertyMappingProxy::getPropertyInterface(uint column, const string &) {
  auto it = propertiesBuffer.find(column);

  // No properties
  if (it == propertiesBuffer.end()) {
    string propertyType = importParameters.getColumnDataType(column);
    string propertyName = importParameters.getColumnName(column);

    // If auto detection fail set to default type : string.
    if (propertyType.empty()) {
      qWarning() << __PRETTY_FUNCTION__ << " No type for the column " << propertyName
                 << " set to string";
      propertyType = "string";
    }

    PropertyInterface *interf = nullptr;

    // If the property already exists.
    // we need to check the compatibility
    if (graph->existProperty(propertyName)) {
      PropertyInterface *existingProperty = graph->getProperty(propertyName);

      // If the properties are compatible query if we had to use existing.
      if (existingProperty->getTypename() == propertyType) {
        if (overwritePropertiesButton != QMessageBox::YesToAll &&
            overwritePropertiesButton != QMessageBox::NoToAll) {
          overwritePropertiesButton = QMessageBox::question(
              parent, parent->tr("Property already exists"),
              parent->tr("A property named \"") + tlpStringToQString(propertyName) +
                  parent->tr("\" already exists.\nDo you want to use it ?\nIf not, a property with "
                             "an approximate name will be generated."),
              QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll,
              QMessageBox::Yes);
        }

        if (overwritePropertiesButton == QMessageBox::NoToAll ||
            overwritePropertiesButton == QMessageBox::No) {
          interf = generateApproximateProperty(propertyName, propertyType);
        } else {
          interf = graph->getProperty(propertyName);
        }
      } else {
        // If the properties are not compatible
        // generate a new property with an approximate name
        QMessageBox::critical(parent, parent->tr("Property already existing"),
                              parent->tr("A property named \"") + tlpStringToQString(propertyName) +
                                  parent->tr("\" already exists with a different type. A property "
                                             "with an approximate name will be generated."));
        interf = generateApproximateProperty(propertyName, propertyType);
      }
    } else {
      interf = graph->getProperty(propertyName, propertyType);
    }

    propertiesBuffer[column] = interf;
    return interf;
  } else {
    return it->second;
  }
}

CSVGraphImport::CSVGraphImport(CSVToGraphDataMapping *mapping,
                               CSVImportColumnToGraphPropertyMapping *properties,
                               const CSVImportParameters &importParameters)
    : mapping(mapping), propertiesManager(properties), importParameters(importParameters) {}
CSVGraphImport::~CSVGraphImport() = default;
bool CSVGraphImport::begin() {
  mapping->init(importParameters.getLastLineIndex() - importParameters.getFirstLineIndex() + 1);
  return true;
}

bool CSVGraphImport::line(uint row, const vector<string> &lineTokens) {
  // Check if user wants to import the line.
  if (!importParameters.importRow(row)) {
    return true;
  }

  // build vector of property interface and vector of input tokens
  vector<PropertyInterface *> props(lineTokens.size(), nullptr);
  vector<std::vector<std::string>> tokens(lineTokens.size());

  for (size_t column = 0; column < lineTokens.size(); ++column) {
    if (importParameters.importColumn(column)) {
      PropertyInterface *property = props[column] =
          propertiesManager->getPropertyInterface(column, lineTokens[column]);
      const string &token = lineTokens[column];

      // If the property does not exists or
      // if the token is empty no need to import the value
      if (property != nullptr && !token.empty()) {
        CSVColumn::Action action = CSVColumn::Action::ASSIGN_VALUE;
        bool isVectorProperty = (property->getTypename().find("vector") == 0);

        if (isVectorProperty) {
          string tokenCopy = token;
          // check if the list of values is enclosed
          // between an openChar and a closeChar
          size_t first = token.find_first_not_of(" \t\f\v");

          if (first != string::npos) {
            char closeChar = '\0';
            // get the openChar and
            // the possible closeChar
            switch (token[first]) {
            case '(':
              closeChar = ')';
              break;

            case '[':
              closeChar = ']';
              break;

            case '{':
              closeChar = '}';
              break;

            case '<':
              closeChar = '>';
              break;

            default:
              break;
            }

            if (closeChar) {
              // check if we find the closeChar at the end of token
              size_t last = token.find_last_not_of(" \t\f\v");

              if (token[last] == closeChar) {
                // remove closeChar
                tokenCopy.resize(last);
                // and openChar
                tokenCopy.erase(0, first + 1);
              }
            }
          }
          static_cast<VectorPropertyInterface *>(property)->tokenize(
              tokenCopy, tokens[column], '\0',
              importParameters.getColumnMultiValueSeparator(column), '\0');
          // check tokens actions
          for (const std::string &tok : tokens[column]) {
            auto tokAction = importParameters.getColumnActionForToken(column, tok);
            if (tokAction == CSVColumn::Action::SKIP_ROW) {
              action = tokAction;
              break;
            } else if (tokAction != CSVColumn::Action::ASSIGN_VALUE) {
              action = tokAction;
            }
          }
        } else {
          action = importParameters.getColumnActionForToken(column, token);
          tokens[column].push_back(token);
        }
        if (action == CSVColumn::Action::SKIP_ROW) {
          return true;
        }
        if (action == CSVColumn::Action::ASSIGN_NO_VALUE) {
          tokens[column].clear();
        }
      }
    }
  }

  // Compute the element id associated to the line
  pair<ElementType, vector<uint>> elements = mapping->getElementsForRow(tokens);

  for (size_t column = 0; column < lineTokens.size(); ++column) {
    PropertyInterface *property = props[column];

    // If the property does not exists or
    // if the token is empty no need to import the value
    if (property != nullptr && !tokens[column].empty()) {
      bool isVectorProperty = (property->getTypename().find("vector") == 0);
      if (elements.first == ElementType::NODE) {
        for (uint i : elements.second) {
          if (i == UINT_MAX) {
            continue;
          }

          if (!(isVectorProperty
                    ? static_cast<VectorPropertyInterface *>(property)->setNodeStringValueAsVector(
                          node(i), tokens[column])
                    : property->setNodeStringValue(node(i), tokens[column][0]))) {
            // We add one to the row number as in the configuration widget we start from row 1 not
            // row 0
            qWarning() << __PRETTY_FUNCTION__ << ":" << __LINE__ << " error when importing token \""
                       << lineTokens[column] << "\" in property \"" << property->getName()
                       << "\" of type \"" << property->getTypename() << "\" at line " << row + 1;
          }
        }
      } else {
        for (uint i : elements.second) {
          if (!(isVectorProperty
                    ? static_cast<VectorPropertyInterface *>(property)->setEdgeStringValueAsVector(
                          edge(i), tokens[column])
                    : property->setEdgeStringValue(edge(i), tokens[column][0]))) {
            // We add one to the row number as in the configuration widget we start from row 1 not
            // row 0
            qWarning() << __PRETTY_FUNCTION__ << ":" << __LINE__ << " error when importing token \""
                       << lineTokens[column] << "\" in property \"" << property->getName()
                       << "\" of type \"" << property->getTypename() << "\" at line " << row + 1;
          }
        }
      }
    }
  }

  return true;
}

bool CSVGraphImport::end(uint, uint) {
  return true;
}
