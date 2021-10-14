/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifdef _WIN32
#include <time.h>
#endif

#include <talipot/ExportModule.h>
#include <talipot/YajlFacade.h>
#include <talipot/JsonTokens.h>
#include <talipot/GraphProperty.h>

using namespace std;
using namespace tlp;

/**
 * @brief Exports a Talipot Graph to a JSON format.
 *
 * In order to maintain full capabilities of exporting to older format versions, the export of the
 *data is decomposed in two parts:
 * * The metadata
 * * The Graph Hierarchy
 *
 * The metadata is exported by saveMetaData() and the graph hierarchy saved (recursively) by
 *saveGraph().
 *
 * These functions are suffixed by the format version they export to (e.g. saveGraph_V4() as of
 *version 4.0 of the format).
 * Under no circumstances should these functions be modified for anything besides a simple bugfix.
 *
 * Any feature addition should be done by writing a new version of saveMetaData and saveGraph, and
 *switching on the version number in the main function.
 *
 **/
class TlpJsonExport : public ExportModule {
public:
  PLUGININFORMATION("JSON Export", "Charles Huet", "18/05/2011",
                    "<p>Supported extensions: json</p><p>Exports a graph in a file using a "
                    "JSON format.</p>",
                    "1.0", "File")

  std::string fileExtension() const override {
    return "json";
  }

  std::string icon() const override {
    return ":/talipot/gui/icons/json32x32.png";
  }

  /**
   * @brief Mandatory constructor to initialize the AlgorithmContext.
   *
   * @param context The context this export algorithm will be initialized with.
   **/
  TlpJsonExport(tlp::PluginContext *context) : ExportModule(context) {
    addInParameter<bool>("Beautify JSON string",
                         "If true, generate a JSON string with indentation and line breaks.",
                         "false");
  }

  bool exportGraph(ostream &fileOut) override {

    if (dataSet && dataSet->exists("Beautify JSON string")) {
      bool beautify = false;
      dataSet->get("Beautify JSON string", beautify);
      _writer.beautifyString(beautify);
    }

    // the export only works for the root graph
    Graph *superGraph = graph->getSuperGraph();
    graph->setSuperGraph(graph);

    _writer.writeMapOpen(); // top-level map

    _writer.writeString("version");
    _writer.writeString("4.0");

    saveMetaData_V4();

    _writer.writeString(GraphToken);
    _writer.writeMapOpen(); // graph hierarchy map
    saveGraph_V4(graph);
    _writer.writeMapClose(); // graph hierarchy map

    _writer.writeMapClose(); // top-level map

    fileOut << _writer.generatedString();

    graph->setSuperGraph(superGraph);

    return true;
  }

  /**
   * @brief Saves the metadata of the graph, such as date and comment.
   * This version does not save anything else.
   *
   * @return void
   **/
  void saveMetaData_V4() {
    time_t ostime = time(nullptr);
    // get local time
    struct tm *currTime = localtime(&ostime);
    // format date
    char currDate[32];
    strftime(currDate, 32, "%Y-%m-%d", currTime);
    _writer.writeString("date");
    _writer.writeString(currDate);

    std::string comment;
    dataSet->get<string>("comment", comment);
    _writer.writeString("comment");
    _writer.writeString(comment);
  }

  /**
   * @brief Saves the graph recursively.
   *
   * @param graph The graph to save.
   * @return void
   **/
  void saveGraph_V4(Graph *g) {
    node n;
    edge e;

    _writer.writeString(GraphIDToken);

    if (g->getSuperGraph() == g) {
      _writer.writeInteger(0);
    } else {
      _writer.writeInteger(g->getId());
    }

    // we need to save all nodes and edges on the root graph
    if (g->getSuperGraph() == g) {
      // saving nodes only requires knowing how many of them there are
      _writer.writeString(NodesNumberToken);
      _writer.writeInteger(g->numberOfNodes());
      // saving the number of edges will speed up the import phase
      // because the space needed to store the edges will be
      // allocated in one call
      _writer.writeString(EdgesNumberToken);
      _writer.writeInteger(g->numberOfEdges());
      // saving edges requires writing source and target for every edge
      _writer.writeString(EdgesToken);
      _writer.writeArrayOpen();

      for (auto e : g->edges()) {
        const auto &[src, tgt] = g->ends(e);
        _writer.writeArrayOpen();
        _writer.writeInteger(graph->nodePos(src));
        _writer.writeInteger(graph->nodePos(tgt));
        _writer.writeArrayClose();
      }

      _writer.writeArrayClose();
    } else {
      // only saving relevant nodes and edges
      const std::vector<node> &nodes = g->nodes();
      uint nbElts = nodes.size();
      std::vector<uint> pos(nbElts);

      for (uint i = 0; i < nbElts; ++i) {
        pos[i] = graph->nodePos(nodes[i]);
      }

      std::sort(pos.begin(), pos.end());
      writeInterval(NodesIDsToken, pos);

      const std::vector<edge> &edges = g->edges();
      pos.resize(nbElts = edges.size());

      for (uint i = 0; i < nbElts; ++i) {
        pos[i] = graph->edgePos(edges[i]);
      }

      std::sort(pos.begin(), pos.end());
      writeInterval(EdgesIDsToken, pos);
    }

    _writer.writeString(PropertiesToken);
    _writer.writeMapOpen();
    // saving properties
    Iterator<PropertyInterface *> *itP = nullptr;

    if (g->getSuperGraph() == g) {
      itP = g->getObjectProperties();
    } else {
      itP = g->getLocalObjectProperties();
    }

    for (PropertyInterface *property : itP) {
      _writer.writeString(property->getName());
      _writer.writeMapOpen();

      _writer.writeString(TypeToken);
      _writer.writeString(property->getTypename());

      _writer.writeString(NodeDefaultToken);
      bool writingPathViewProperty = (property->getName() == string("viewFont") ||
                                      property->getName() == string("viewTexture"));

      string dsValue = property->getNodeDefaultStringValue();

      if (writingPathViewProperty && !TalipotBitmapDir.empty()) {
        if (size_t pos = dsValue.find(TalipotBitmapDir); pos != string::npos) {
          dsValue.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
        }
      }

      _writer.writeString(dsValue);

      _writer.writeString(EdgeDefaultToken);
      dsValue = property->getEdgeDefaultStringValue();

      if (writingPathViewProperty && !TalipotBitmapDir.empty()) {
        if (size_t pos = dsValue.find(TalipotBitmapDir); pos != string::npos) {
          dsValue.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
        }
      }

      _writer.writeString(dsValue);

      if (property->hasNonDefaultValuatedNodes()) {
        _writer.writeString(NodesValuesToken);
        _writer.writeMapOpen();
        for (auto n : property->getNonDefaultValuatedNodes(g)) {

          string sValue = property->getNodeStringValue(n);

          if (g->getId() != 0 && // if it is not the real root graph
              property->getTypename() == GraphProperty::propertyTypename) {
            uint id = strtoul(sValue.c_str(), nullptr, 10);

            // we must check if the pointed subgraph
            // is a descendant of the currently exported graph
            if (!graph->getDescendantGraph(id)) {
              continue;
            }
          }

          stringstream temp;
          temp << graph->nodePos(n);
          _writer.writeString(temp.str());

          if (writingPathViewProperty && !TalipotBitmapDir.empty()) {
            if (size_t pos = sValue.find(TalipotBitmapDir); pos != string::npos) {
              sValue.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
            }
          }

          _writer.writeString(sValue);
        }
        _writer.writeMapClose();
      }

      if (property->hasNonDefaultValuatedEdges()) {
        _writer.writeString(EdgesValuesToken);
        _writer.writeMapOpen();
        for (auto e : property->getNonDefaultValuatedEdges(g)) {

          string sValue;

          if (property->getTypename() == GraphProperty::propertyTypename) {

            // for GraphProperty we must ensure the reindexing
            // of embedded edges
            const set<edge> &edges = (*static_cast<GraphProperty *>(property))[e];
            set<edge> rEdges;
            for (auto ee : edges) {
              edge rEdge = edge(graph->edgePos(ee));
              // do not export edges that are not elements of the root graph
              if (rEdge.isValid()) {
                rEdges.insert(rEdge);
              }
            }

            if (rEdges.empty()) {
              continue;
            }

            sValue = EdgeSetType::toString(rEdges);
          } else {

            sValue = property->getEdgeStringValue(e);

            if (writingPathViewProperty && !TalipotBitmapDir.empty()) {
              if (size_t pos = sValue.find(TalipotBitmapDir); pos != string::npos) {
                sValue.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
              }
            }
          }

          stringstream temp;
          temp << graph->edgePos(e);
          _writer.writeString(temp.str());
          _writer.writeString(sValue);
        }
        _writer.writeMapClose();
      }

      _writer.writeMapClose();
    }
    _writer.writeMapClose();

    _writer.writeString(AttributesToken);
    _writer.writeMapOpen();
    // saving attributes
    DataSet attributes = g->getAttributes();
    for (const auto &[key, value] : attributes.getValues()) {
      // If nodes and edges are stored as graph attributes
      // we need to update their id before serializing them
      // as nodes and edges have been reindexed
      if (value->getTypeName() == string(typeid(node).name())) {
        node *n = static_cast<node *>(value->value);
        n->id = graph->nodePos(*n);
      } else if (value->getTypeName() == string(typeid(edge).name())) {
        edge *e = static_cast<edge *>(value->value);
        e->id = g->edgePos(*e);
      } else if (value->getTypeName() == string(typeid(vector<node>).name())) {
        auto *vn = static_cast<vector<node> *>(value->value);

        for (auto &n : *vn) {
          n.id = graph->nodePos(n);
        }
      } else if (value->getTypeName() == string(typeid(vector<edge>).name())) {
        auto *ve = static_cast<vector<edge> *>(value->value);

        for (auto &e : *ve) {
          e.id = graph->edgePos(e);
        }
      }

      DataTypeSerializer *serializer = DataSet::typenameToSerializer(value->getTypeName());
      _writer.writeString(key);
      _writer.writeArrayOpen();
      _writer.writeString(serializer->outputTypeName);

      stringstream temp;
      serializer->writeData(temp, value);
      _writer.writeString(temp.str());
      _writer.writeArrayClose();
    }
    _writer.writeMapClose();

    // saving subgraphs
    _writer.writeString(SubgraphsToken);
    _writer.writeArrayOpen();
    for (Graph *sub : g->subGraphs()) {
      _writer.writeMapOpen();
      saveGraph_V4(sub);
      _writer.writeMapClose();
    }
    _writer.writeArrayClose();
  }

  /**
   * @brief Writes a set of identifiers as contiguous intervals (defined by arrays containing lower
   *and higher bounds).
   * e.g. the set {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 15, 17} will be saved as the array:
   * [ [0, 7], [9, 11], 15, 17]
   *
   * @param intervalName The name of the interval.
   * @param iterator An iterator over the values to save.
   * @return void
   **/
  void writeInterval(const std::string &intervalName, std::vector<uint> &pos) {
    _writer.writeString(intervalName);
    _writer.writeArrayOpen();
    uint intervalBegin = UINT_MAX;
    uint intervalEnd = UINT_MAX;
    uint previousId = UINT_MAX;
    uint currentId = UINT_MAX;
    uint nbElts = pos.size();

    for (uint i = 0; i < nbElts; ++i) {
      currentId = pos[i];

      // we don't need/want to do all this on the first time we loop
      if (previousId != UINT_MAX) {

        // if the ID are continuous, define an interval, otherwise write the IDs (either intervals
        // or single)
        if (currentId == previousId + 1) {
          // if we have no interval being defined, set the lower bound to the previous edge ID
          // if an interval is being defined, set its end to the current edge ID
          if (intervalBegin == UINT_MAX) {
            intervalBegin = previousId;
          }

          intervalEnd = currentId;
        } else {
          // if an interval is defined, write it
          if (intervalBegin != UINT_MAX) {
            _writer.writeArrayOpen();
            _writer.writeInteger(intervalBegin);
            _writer.writeInteger(intervalEnd);
            _writer.writeArrayClose();
            intervalBegin = UINT_MAX;
            intervalEnd = UINT_MAX;
          } else {
            _writer.writeInteger(previousId);
          }
        }

        if (i == (nbElts - 1)) {
          if (intervalBegin != UINT_MAX) {
            _writer.writeArrayOpen();
            _writer.writeInteger(intervalBegin);
            _writer.writeInteger(intervalEnd);
            _writer.writeArrayClose();
          } else {
            _writer.writeInteger(currentId);
          }
        }
      }

      previousId = currentId;
    }

    // handle the case where there is only one id to write
    if (nbElts == 1) {
      _writer.writeInteger(currentId);
    }

    _writer.writeArrayClose();
  }

protected:
  YajlWriteFacade _writer;
};

PLUGIN(TlpJsonExport)
