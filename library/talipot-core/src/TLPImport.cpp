/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GraphImpl.h>
#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/GraphProperty.h>
#include <talipot/ImportModule.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/TLPParser.h>

#define TLP "tlp"
#define AUTHOR "author"
#define COMMENTS "comments"
#define TLP_VERSION 2.3
#define NODES "nodes"
#define EDGE "edge"
#define NB_NODES "nb_nodes"
#define NB_EDGES "nb_edges"
#define CLUSTER "cluster"
#define CLUSTERNODES "nodes"
#define CLUSTEREDGES "edges"
#define PROPERTY "property"
#define METRIC "metric"
#define METAGRAPH "metagraph"
#define GRAPH "graph"
#define LAYOUT "layout"
#define SIZE "size"
#define COLOR "color"
#define COORD "coord"
#define INT "int"
#define UINT "uint"
#define DOUBLE "double"
#define FLOAT "float"
#define STRING "string"
#define BOOL "bool"
#define SIZE_VECTOR "vector<size>"
#define COLOR_VECTOR "vector<color>"
#define COORD_VECTOR "vector<coord>"
#define INT_VECTOR "vector<int>"
#define DOUBLE_VECTOR "vector<double>"
#define STRING_VECTOR "vector<string>"
#define BOOL_VECTOR "vector<bool>"
#define DATASET "DataSet"
#define NODEVALUE "node"
#define EDGEVALUE "edge"
#define DEFAULTVALUE "default"

#define DISPLAYING "displaying"
#define GLYPH "glyph"
#define OLD_ATTRIBUTES "attributes"
#define ATTRIBUTES "graph_attributes"
#define SCENE "scene"
#define VIEWS "views"
#define CONTROLLER "controller"

using namespace tlp;

static const std::string TalipotBitmapDirSym = "TalipotBitmapDir/";
static const std::string TulipBitmapDirSym = "TulipBitmapDir/";

namespace tlp {
//=================================================================================
struct TLPGraphBuilder : public TLPTrue {
  GraphImpl *_graph;
  Graph *_cluster;
  std::map<int, node> nodeIndex;
  std::map<int, edge> edgeIndex;
  std::map<int, Graph *> clusterIndex;
  DataSet *dataSet;
  bool inTLP;
  double version;

  TLPGraphBuilder(Graph *graph, DataSet *dataSet)
      : _graph(static_cast<GraphImpl *>(graph)), _cluster(nullptr), dataSet(dataSet) {
    clusterIndex[0] = graph;
    inTLP = false;
    version = 0.0;
  }

  ~TLPGraphBuilder() override = default;

  Graph *getSubGraph(int id) {
    if (const auto it = clusterIndex.find(id); it != clusterIndex.end()) {
      return it->second;
    }

    return nullptr;
  }

  bool addString(const std::string &str) override {
    // only used to handle the version of tlp file format
    if (!version) {
      const char *cptr = str.c_str();
      char *endptr;
      version = strtod(cptr, &endptr);
      // check for correctness of version parsing and value
      if ((endptr != cptr) && (version <= TLP_VERSION)) {
        return true;
      }
    }
    parser->errorMsg = "invalid tlp version";
    return false;
  }

  bool addNode(int id) {
    if (version < 2.1f) {
      nodeIndex[id] = _graph->addNode();
    } else {
      _graph->addNode();
    }

    return true;
  }
  bool addNodes(int first, int last) {
    // node with id first - 1 must exist
    if (first && !_graph->isElement(node(first - 1))) {
      std::stringstream ess;
      ess << "node with id " << (first - 1) << " must exist";
      parser->errorMsg = ess.str();
      return false;
    }
    _graph->addNodes(last - first + 1);

    if (version < 2.1) {
      auto it = _graph->nodes().begin();

      while (first <= last) {
        nodeIndex[first] = (*it);
        ++first, ++it;
      }
    }

    return true;
  }
  bool reserveNodes(int nbNodes) const {
    _graph->reserveNodes(nbNodes);
    return true;
  }
  bool reserveEdges(int nbEdges) const {
    _graph->reserveEdges(nbEdges);
    return true;
  }
  bool addClusterNodes(const std::vector<node> &nodes) {
    std::vector<node> vNodes;
    vNodes.reserve(nodes.size());
    for (auto n : nodes) {
      if (version < 2.1) {
        n = nodeIndex[n.id];
      }
      vNodes.push_back(n);
      if (!_graph->isElement(n)) {
        std::stringstream ess;
        ess << "node with id " << n.id << " does not exist";
        parser->errorMsg = ess.str();
        return false;
      }
    }
    _cluster->addNodes(vNodes);
    return true;
  }
  bool addClusterEdges(const std::vector<edge> &edges) {
    std::vector<edge> vEdges;
    vEdges.reserve(edges.size());
    for (auto e : edges) {
      if (version < 2.1) {
        e = edgeIndex[e.id];
      }
      vEdges.push_back(e);
      if (!_graph->isElement(e)) {
        std::stringstream ess;
        ess << "edge with id " << e.id << " does not exist";
        parser->errorMsg = ess.str();
        return false;
      }
    }

    _cluster->addEdges(vEdges);
    return true;
  }
  bool addEdge(int id, int idSource, int idTarget) {
    node src(idSource), tgt(idTarget);

    if (version < 2.1) {
      src = nodeIndex[idSource];
      tgt = nodeIndex[idTarget];
    }

    if (!_graph->isElement(src)) {
      std::stringstream ess;
      ess << "node with id " << src << " does not exist";
      parser->errorMsg = ess.str();

      return false;
    }
    if (!_graph->isElement(tgt)) {
      std::stringstream ess;
      ess << "node with id " << tgt << " does not exist";
      parser->errorMsg = ess.str();

      return false;
    }

    edgeIndex[id] = _graph->addEdge(src, tgt);
    return true;
  }
  PropertyInterface *createProperty(int clusterId, const std::string &propertyType,
                                    const std::string &propertyName, bool &isGraphProperty,
                                    bool &isPathViewProperty) {
    Graph *g = clusterId ? getSubGraph(clusterId) : _graph;

    if (g == nullptr) {
      return nullptr;
    }

    if (propertyType == GRAPH || propertyType == METAGRAPH) {
      // METAGRAPH was used in Tulip 2
      isGraphProperty = true;
      return g->getLocalGraphProperty(propertyName);
    }

    if (propertyType == DOUBLE || propertyType == METRIC) {
      // METRIC was used in Tulip 2
      return g->getLocalDoubleProperty(propertyName);
    }

    if (propertyType == LAYOUT) {
      return g->getLocalLayoutProperty(propertyName);
    }

    if (propertyType == SIZE) {
      return g->getLocalSizeProperty(propertyName);
    }

    if (propertyType == COLOR) {
      return g->getLocalColorProperty(propertyName);
    }

    if (propertyType == INT) {
      return g->getLocalIntegerProperty(propertyName);
    }

    if (propertyType == BOOL) {
      return g->getLocalBooleanProperty(propertyName);
    }

    if (propertyType == STRING) {
      isPathViewProperty = ((propertyName == "viewFont") || (propertyName == "viewTexture"));
      return g->getLocalStringProperty(propertyName);
    }

    if (propertyType == SIZE_VECTOR) {
      return g->getLocalSizeVectorProperty(propertyName);
    }

    if (propertyType == COLOR_VECTOR) {
      return g->getLocalColorVectorProperty(propertyName);
    }

    if (propertyType == COORD_VECTOR) {
      return g->getLocalCoordVectorProperty(propertyName);
    }

    if (propertyType == DOUBLE_VECTOR) {
      return g->getLocalDoubleVectorProperty(propertyName);
    }

    if (propertyType == INT_VECTOR) {
      return g->getLocalIntegerVectorProperty(propertyName);
    }

    if (propertyType == BOOL_VECTOR) {
      return g->getLocalBooleanVectorProperty(propertyName);
    }

    if (propertyType == STRING_VECTOR) {
      return g->getLocalStringVectorProperty(propertyName);
    }

    return nullptr;
  }
  bool setNodeValue(int nodeId, PropertyInterface *prop, std::string &value, bool isGraphProperty,
                    bool isPathViewProperty) {
    node n(nodeId);

    if (version < 2.1) {
      n = nodeIndex[nodeId];
    }

    assert(prop->getGraph()->isElement(n));

    if (isPathViewProperty) {
      // if needed replace symbolic path by real path
      if (size_t pos = value.find(TalipotBitmapDirSym); pos != std::string::npos) {
        value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
      }

      if (size_t pos = value.find(TulipBitmapDirSym); pos != std::string::npos) {
        value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
      }

    } else {
      if (isGraphProperty) {
        auto *gProp = static_cast<GraphProperty *>(prop);
        char *endPtr = nullptr;
        const char *startPtr = value.c_str();
        int result = strtol(startPtr, &endPtr, 10);

        if ((endPtr == startPtr) || (!clusterIndex.contains(result))) {
          std::stringstream ess;
          ess << "invalid node value for property " << prop->getName();
          parser->errorMsg = ess.str();
          return false;
        }

        gProp->setNodeValue(n, result ? clusterIndex[result] : nullptr);
        return true;
      }
    }

    return prop->setNodeStringValue(n, value);
  }

  bool setEdgeValue(int edgeId, PropertyInterface *prop, std::string &value, bool isGraphProperty,
                    bool isPathViewProperty) {
    edge e(edgeId);

    if (version < 2.1) {
      e = edgeIndex[edgeId];
    }

    assert(prop->getGraph()->isElement(e));

    const std::string &propertyName = prop->getName();

    if (isPathViewProperty) {
      // if needed replace symbolic path by real path
      if (size_t pos = value.find(TalipotBitmapDirSym); pos != std::string::npos) {
        value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
      }

      if (size_t pos = value.find(TulipBitmapDirSym); pos != std::string::npos) {
        value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
      }

    } else if ((version < 2.2) && (propertyName == std::string("viewSrcAnchorShape") ||
                                   propertyName == std::string("viewTgtAnchorShape"))) {
      // If we are in the old edge extremities id system we need to convert the ids in the file.
      return prop->setEdgeStringValue(e, convertOldEdgeExtremitiesValueToNew(value));
    } else {
      if (isGraphProperty) {
        auto *gProp = static_cast<GraphProperty *>(prop);
        std::set<edge> v;
        bool result = EdgeSetType::fromString(v, value);

        if (result) {
          gProp->setEdgeValue(e, v);
        } else {
          std::stringstream ess;
          ess << "invalid edge value for property " << propertyName;
          parser->errorMsg = ess.str();
        }
        return result;
      }
    }

    return prop->setEdgeStringValue(e, value);
  }

  /**
   * @brief Convert the id of a edge extremity glyph from the old numeraotation system to the new
   * numerotation system.
   *
   * We cannot set the new value automatically as one edge extremity id doesn't change.
   * @param oldValue The old glyph value.
   * @return The new glyph value or the old value if no change are needed.
   */
  std::string convertOldEdgeExtremitiesValueToNew(const std::string &oldValue) {
    if (oldValue == std::string("0")) {
      return "-1";
    } else if (oldValue == std::string("1")) {
      return "0";
    } else if (oldValue == std::string("3")) {
      return "2";
    } else if (oldValue == std::string("4")) {
      return "3";
    } else if (oldValue == std::string("5")) {
      return "4";
    } else if (oldValue == std::string("7")) {
      return "6";
    } else if (oldValue == std::string("10")) {
      return "9";
    } else if (oldValue == std::string("13")) {
      return "12";
    } else if (oldValue == std::string("14")) {
      return "13";
    } else if (oldValue == std::string("15")) {
      return "14";
    } else if (oldValue == std::string("16")) {
      return "15";
    } else if (oldValue == std::string("29")) {
      return "28";
    } else {
      return oldValue;
    }
  }

  bool setAllNodeValue(PropertyInterface *prop, std::string &value, bool isGraphProperty,
                       bool isPathViewProperty) {
    if (isGraphProperty) {
      auto *gProp = static_cast<GraphProperty *>(prop);
      char *endPtr = nullptr;
      const char *startPtr = value.c_str();
      int result = strtol(startPtr, &endPtr, 10);

      if (endPtr == startPtr) {
        result = 0; // use root graph
      }

      if (!clusterIndex.contains(result)) {
        std::stringstream ess;
        ess << "invalid node value for property " << prop->getName();
        parser->errorMsg = ess.str();
        return false;
      }

      gProp->setAllNodeValue(result ? clusterIndex[result] : nullptr);

      return true;
    }

    if (isPathViewProperty) {
      // if needed replace symbolic path by real path
      if (size_t pos = value.find(TalipotBitmapDirSym); pos != std::string::npos) {
        value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
      }

      if (size_t pos = value.find(TulipBitmapDirSym); pos != std::string::npos) {
        value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
      }
    }

    return prop->setAllNodeStringValue(value);
  }

  bool setAllEdgeValue(PropertyInterface *prop, std::string &value, bool isGraphProperty,
                       bool isPathViewProperty) {
    const std::string &propertyName = prop->getName();

    if (isGraphProperty) {
      auto *gProp = dynamic_cast<GraphProperty *>(prop);
      std::set<edge> v;
      bool result = EdgeSetType::fromString(v, value);

      if (result) {
        gProp->setAllEdgeValue(v);
      } else {
        std::stringstream ess;
        ess << "invalid edge value for property " << propertyName;
        parser->errorMsg = ess.str();
      }

      return result;
    }

    if (dynamic_cast<IntegerProperty *>(prop)) {
      // If we are in the old edge extremities id system we need to convert the ids in the file.
      if (version < 2.2) {
        if (propertyName == std::string("viewSrcAnchorShape") ||
            propertyName == std::string("viewTgtAnchorShape")) {
          value = convertOldEdgeExtremitiesValueToNew(value);
        }
      }
    } else {
      if (isPathViewProperty) {
        // if needed replace symbolic path by real path
        if (size_t pos = value.find(TalipotBitmapDirSym); pos != std::string::npos) {
          value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
        }

        if (size_t pos = value.find(TulipBitmapDirSym); pos != std::string::npos) {
          value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
        }
      }
    }

    return prop->setAllEdgeStringValue(value);
  }
  bool addCluster(int id, const std::string &name, int supergraphId = 0) {
    if (clusterIndex[supergraphId]) {
      _cluster = clusterIndex[id] =
          static_cast<GraphAbstract *>(clusterIndex[supergraphId])->addSubGraph(uint(id));

      if (name.size()) {
        _cluster->setAttribute("name", name);
      }

      return true;
    }

    std::stringstream ess;
    ess << "sub graph with id " << supergraphId << " does not exist.";
    parser->errorMsg = ess.str();
    return false;
  }
  bool addStruct(const std::string &structName, TLPBuilder *&newBuilder) override;
};
//=================================================================================
struct TLPNodeBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  TLPNodeBuilder(TLPGraphBuilder *graphBuilder) : graphBuilder(graphBuilder) {}
  bool addInt(const int id) override {
    return graphBuilder->addNode(id);
  }
  bool addRange(int first, int second) override {
    return graphBuilder->addNodes(first, second);
  }
  bool close() override {
    return true;
  }
};
//=================================================================================
struct TLPNodesBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  TLPNodesBuilder(TLPGraphBuilder *graphBuilder) : graphBuilder(graphBuilder) {}
  bool addInt(const int nbNodes) override {
    return graphBuilder->reserveNodes(nbNodes);
  }
  bool close() override {
    return true;
  }
};
//=================================================================================
struct TLPEdgeBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  int nbParameter;
  std::vector<int> parameters;
  TLPEdgeBuilder(TLPGraphBuilder *graphBuilder) : graphBuilder(graphBuilder), nbParameter(0) {}
  bool addInt(const int id) override {
    if (nbParameter < 3) {
      parameters.push_back(id);
      nbParameter++;
      return true;
    }
    parser->errorMsg = "wrong edge format, must be (edge id src target)";
    return false;
  }
  bool close() override {
    if (nbParameter == 3) {
      return graphBuilder->addEdge(parameters[0], parameters[1], parameters[2]);
    }
    parser->errorMsg = "wrong edge format, must be (edge id src target)";
    return false;
  }
};
//=================================================================================
struct TLPEdgesBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  TLPEdgesBuilder(TLPGraphBuilder *graphBuilder) : graphBuilder(graphBuilder) {}
  bool addInt(const int nbEdges) override {
    return graphBuilder->reserveEdges(nbEdges);
  }
  bool close() override {
    return true;
  }
};
//=================================================================================
struct TLPClusterBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  int clusterId, supergraphId;
  TLPClusterBuilder(TLPGraphBuilder *graphBuilder, int supergraph = 0)
      : graphBuilder(graphBuilder), clusterId(INT_MAX), supergraphId(supergraph) {}
  bool addInt(const int id) override {
    clusterId = id;

    if (graphBuilder->version > 2.2) {
      return graphBuilder->addCluster(id, std::string(), supergraphId);
    }

    return true;
  }
  bool addString(const std::string &str) override {
    if (graphBuilder->version < 2.3) {
      return graphBuilder->addCluster(clusterId, str, supergraphId);
    }

    return true;
  }
  bool addStruct(const std::string &structName, TLPBuilder *&newBuilder) override;
  bool addNodes(const std::vector<node> &nodes) const {
    return graphBuilder->addClusterNodes(nodes);
  }
  bool addEdges(const std::vector<edge> &edges) const {
    return graphBuilder->addClusterEdges(edges);
  }
  bool close() override {
    return true;
  }
};
//=================================================================================
struct TLPClusterNodeBuilder : public TLPFalse {
  TLPClusterBuilder *clusterBuilder;
  std::vector<node> nodes;
  TLPClusterNodeBuilder(TLPClusterBuilder *clusterBuilder) : clusterBuilder(clusterBuilder) {}
  bool addInt(const int id) override {
    nodes.push_back(node(id));
    return true;
  }
  bool addRange(int first, int second) override {
    while (first <= second) {
      nodes.push_back(node(first));
      ++first;
    }
    return true;
  }
  bool close() override {
    clusterBuilder->addNodes(nodes);
    return true;
  }
};
//=================================================================================
struct TLPClusterEdgeBuilder : public TLPFalse {
  TLPClusterBuilder *clusterBuilder;
  std::vector<edge> edges;
  TLPClusterEdgeBuilder(TLPClusterBuilder *clusterBuilder) : clusterBuilder(clusterBuilder) {}
  bool addInt(const int id) override {
    edges.push_back(edge(id));
    return true;
  }
  bool addRange(int first, int second) override {
    while (first <= second) {
      edges.push_back(edge(first));
      ++first;
    }
    return true;
  }
  bool close() override {
    clusterBuilder->addEdges(edges);
    return true;
  }
};
//================================================================================
struct TLPAttributesBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;

  TLPAttributesBuilder(TLPGraphBuilder *graphBuilder) : graphBuilder(graphBuilder) {}
  ~TLPAttributesBuilder() override = default;
  bool close() override {
    return true;
  }
  bool canRead() override {
    return true;
  }
  bool read(std::istream &is) override {
    char c = ' ';

    // go to the first non space char
    while ((is >> c) && isspace(c)) {
    }

    is.unget();
    // to read the id of the graph
    uint id;

    if (!(is >> id)) {
      return false;
    }

    Graph *subgraph = id ? graphBuilder->getSubGraph(id) : graphBuilder->_graph;

    if (subgraph == nullptr) {
      std::stringstream ess;
      ess << "sub graph with id " << id << " does not exist.";
      parser->errorMsg = ess.str();
      return false;
    }

    return DataSet::read(is, const_cast<DataSet &>(subgraph->getAttributes()));
  }
};
//================================================================================
struct TLPDataSetBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  DataSet dataSet;
  DataSet *currentDataSet;
  char *dataSetName;

  TLPDataSetBuilder(TLPGraphBuilder *graphBuilder)
      : graphBuilder(graphBuilder),
        currentDataSet(const_cast<DataSet *>(&(graphBuilder->_graph->getAttributes()))),
        dataSetName(nullptr) {}
  TLPDataSetBuilder(TLPGraphBuilder *graphBuilder, char *name)
      : graphBuilder(graphBuilder), currentDataSet(graphBuilder->dataSet), dataSetName(name) {
    graphBuilder->dataSet->get(dataSetName, dataSet);
    currentDataSet = &dataSet;
  }
  TLPDataSetBuilder(TLPGraphBuilder *graphBuilder, DataSet *currentDataSet)
      : graphBuilder(graphBuilder), currentDataSet(currentDataSet), dataSetName(nullptr) {}
  ~TLPDataSetBuilder() override = default;
  bool close() override {
    return true;
  }
  bool canRead() override {
    return true;
  }
  bool read(std::istream &is) override {
    return DataSet::read(is, *currentDataSet);
  }
};
//================================================================================
struct TLPFileInfoBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  std::string name;

  TLPFileInfoBuilder(TLPGraphBuilder *graphBuilder, const std::string &infoName)
      : graphBuilder(graphBuilder), name(infoName) {}
  ~TLPFileInfoBuilder() override = default;

  bool addString(const std::string &str) override {
    std::string theStr(str);

    if (name == AUTHOR) {
      graphBuilder->dataSet->set(AUTHOR, theStr);
    } else if (name == COMMENTS) {
      graphBuilder->dataSet->set("text::comments", theStr);
    }

    return true;
  }
  bool close() override {
    return true;
  }
};
//================================================================================
struct TLPSceneBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;

  TLPSceneBuilder(TLPGraphBuilder *graphBuilder) : graphBuilder(graphBuilder) {}
  ~TLPSceneBuilder() override = default;

  bool addString(const std::string &str) override {
    graphBuilder->dataSet->set(SCENE, str);
    return true;
  }
  bool close() override {
    return true;
  }
};
//=================================================================================
bool TLPClusterBuilder::addStruct(const std::string &structName, TLPBuilder *&newBuilder) {
  if (structName == CLUSTERNODES) {
    newBuilder = new TLPClusterNodeBuilder(this);
  } else if (structName == CLUSTEREDGES) {
    newBuilder = new TLPClusterEdgeBuilder(this);
  } else if (structName == CLUSTER) {
    newBuilder = new TLPClusterBuilder(graphBuilder, clusterId);
  } else {
    newBuilder = new TLPFalse();
    return false;
  }

  return true;
}
//=================================================================================
struct TLPPropertyBuilder : public TLPFalse {
  TLPGraphBuilder *graphBuilder;
  int clusterId;
  std::string propertyType, propertyName;
  PropertyInterface *property;
  bool isGraphProperty;
  bool isPathViewProperty;

  ~TLPPropertyBuilder() override = default;
  TLPPropertyBuilder(TLPGraphBuilder *graphBuilder)
      : graphBuilder(graphBuilder), clusterId(INT_MAX), propertyType(std::string()),
        propertyName(std::string()), property(nullptr), isGraphProperty(false),
        isPathViewProperty(false) {}
  bool getProperty() {
    assert(property == nullptr);
    property = graphBuilder->createProperty(clusterId, propertyType, propertyName, isGraphProperty,
                                            isPathViewProperty);
    return property != nullptr;
  }
  bool addInt(const int id) override {
    assert(id != INT_MAX);
    clusterId = id;

    if (!propertyType.empty() && !propertyName.empty()) {
      return getProperty();
    }

    return true;
  }
  bool addString(const std::string &str) override {
    if (propertyType.empty()) {
      propertyType = str;
    } else if (propertyName.empty()) {
      propertyName = str;

      if (clusterId != INT_MAX) {
        return getProperty();
      }
    } else {
      parser->errorMsg = "invalid property format";
      return false;
    }

    return true;
  }
  bool setNodeValue(int nodeId, const std::string &value) {
    return property ? graphBuilder->setNodeValue(nodeId, property, const_cast<std::string &>(value),
                                                 isGraphProperty, isPathViewProperty)
                    : false;
  }
  bool setEdgeValue(int edgeId, const std::string &value) {
    return property ? graphBuilder->setEdgeValue(edgeId, property, const_cast<std::string &>(value),
                                                 isGraphProperty, isPathViewProperty)
                    : false;
  }
  bool setAllNodeValue(const std::string &value) {
    return property ? graphBuilder->setAllNodeValue(property, const_cast<std::string &>(value),
                                                    isGraphProperty, isPathViewProperty)
                    : false;
  }
  bool setAllEdgeValue(const std::string &value) {
    return property ? graphBuilder->setAllEdgeValue(property, const_cast<std::string &>(value),
                                                    isGraphProperty, isPathViewProperty)
                    : false;
  }
  bool addStruct(const std::string &structName, TLPBuilder *&newBuilder) override;
  bool close() override {
    return property != nullptr;
  }
};
//=================================================================================
struct TLPNodePropertyBuilder : public TLPFalse {
  TLPPropertyBuilder *propertyBuilder;
  int nodeId;
  TLPNodePropertyBuilder(TLPPropertyBuilder *propertyBuilder)
      : propertyBuilder(propertyBuilder), nodeId(INT_MAX) {}
  bool addInt(const int id) override {
    nodeId = id;
    return true;
  }
  bool addString(const std::string &val) override {
    return propertyBuilder->setNodeValue(nodeId, val);
  }
  bool close() override {
    return true;
  }
};
//=================================================================================
struct TLPEdgePropertyBuilder : public TLPFalse {
  TLPPropertyBuilder *propertyBuilder;
  int edgeId;
  std::string nodeValue;
  TLPEdgePropertyBuilder(TLPPropertyBuilder *propertyBuilder)
      : propertyBuilder(propertyBuilder), edgeId(INT_MAX) {}
  ~TLPEdgePropertyBuilder() override = default;
  bool addInt(const int id) override {
    edgeId = id;
    return true;
  }
  bool addString(const std::string &val) override {
    return propertyBuilder->setEdgeValue(edgeId, val);
  }
  bool close() override {
    return true;
  }
};
struct TLPDefaultPropertyBuilder : public TLPFalse {
  TLPPropertyBuilder *propertyBuilder;
  int edgeId;
  std::string nodeValue;
  int i;
  TLPDefaultPropertyBuilder(TLPPropertyBuilder *propertyBuilder)
      : propertyBuilder(propertyBuilder), edgeId(INT_MAX), i(0) {}
  ~TLPDefaultPropertyBuilder() override = default;
  bool addString(const std::string &val) override {
    if (i == 0) {
      i++;
      return propertyBuilder->setAllNodeValue(val);
    }

    if (i == 1) {
      i++;
      return propertyBuilder->setAllEdgeValue(val);
    }
    parser->errorMsg = "invalid property default value format";
    return false;
  }
  bool close() override {
    return true;
  }
};

}
//=================================================================================
bool TLPPropertyBuilder::addStruct(const std::string &structName, TLPBuilder *&newBuilder) {
  if (structName == DEFAULTVALUE) {
    newBuilder = new TLPDefaultPropertyBuilder(this);
    return true;
  } else if (structName == NODEVALUE) {
    newBuilder = new TLPNodePropertyBuilder(this);
    return true;
  } else if (structName == EDGEVALUE) {
    newBuilder = new TLPEdgePropertyBuilder(this);
    return true;
  }

  return false;
}
//=================================================================================
bool TLPGraphBuilder::addStruct(const std::string &structName, TLPBuilder *&newBuilder) {
  if (structName == TLP) {
    inTLP = true;
    newBuilder = this;
  } else if (structName == NODES) {
    newBuilder = new TLPNodeBuilder(this);
  } else if (structName == NB_NODES) {
    newBuilder = new TLPNodesBuilder(this);
  } else if (structName == NB_EDGES) {
    newBuilder = new TLPEdgesBuilder(this);
  } else if (structName == EDGE) {
    newBuilder = new TLPEdgeBuilder(this);
  } else if (structName == CLUSTER) {
    newBuilder = new TLPClusterBuilder(this);
  } else if (structName == PROPERTY) {
    newBuilder = new TLPPropertyBuilder(this);
  } else if (structName == DISPLAYING) {
    newBuilder = new TLPDataSetBuilder(this, const_cast<char *>(DISPLAYING));
  } else if (structName == OLD_ATTRIBUTES) {
    newBuilder = new TLPDataSetBuilder(this);
  } else if (structName == ATTRIBUTES) {
    newBuilder = new TLPAttributesBuilder(this);
  } else if (structName == SCENE) {
    newBuilder = new TLPSceneBuilder(this);
  } else if (structName == VIEWS) {
    newBuilder = new TLPDataSetBuilder(this, const_cast<char *>(VIEWS));
  } else if (structName == CONTROLLER) {
    newBuilder = new TLPDataSetBuilder(this, const_cast<char *>(CONTROLLER));
  } else {
    newBuilder = new TLPFileInfoBuilder(this, structName);
  }

  return true;
}

//================================================================================

namespace tlp {

/**
 * This plugin imports a Tulip graph structure recorded in a file
 * using the TLP format.
 * TLP is the Tulip Software Graph Format.
 *
 */
class TLPImport : public ImportModule {
public:
  PLUGININFORMATION("TLP Import", "Auber", "16/02/2001",
                    "<p>Supported extensions: tlp, tlpz (compressed), tlp.gz "
                    "(compressed)</p><p>Imports a graph recorded in a file using the TLP format "
                    "(Tulip Software Graph Format).<br/>See "
                    "https://tulip.labri.fr/site/?q=tlp-file-format for description.</p>",
                    "1.0", "File")
  std::list<std::string> fileExtensions() const override {
    return {"tlp"};
  }

  TLPImport(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<std::string>("file::filename", "The pathname of the TLP file to import.", "");
  }
  ~TLPImport() override = default;

  std::string icon() const override {
    return ":/talipot/gui/icons/tlp32x32.png";
  }

  bool importGraph() override {

    auto inputData = getInputData();

    if (!inputData.valid()) {
      return false;
    }

    pluginProgress->showPreview(false);
    pluginProgress->setComment(std::string("Loading ") + inputData.filename + "...");
    TLPParser myParser(*inputData.is, new TLPGraphBuilder(graph, dataSet), pluginProgress,
                       inputData.size);
    bool result = myParser.parse();

    if (!result) {
      pluginProgress->setError(inputData.filename + ": " + pluginProgress->getError());
    }

    return result;
  }
};
PLUGIN(TLPImport)
}
