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

#include <talipot/ImportModule.h>
#include <talipot/ViewSettings.h>
#include <talipot/TlpQtTools.h>
#include <talipot/StableIterator.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/BooleanProperty.h>

#include <QXmlStreamReader>
#include <QFile>

using namespace tlp;
using namespace std;

/** \file
 *  \brief - Import GEXF format graph file.
 * This plugin imports a graph from a file in GEXF format,<br/>
 * as it is described in the XML Schema 1.2draft (http://gexf.net/format/schema.html). Dynamic mode
 * is not yet supported.
 *  <b>HISTORY</b>
 *
 *  - 04/06/2012 Version 1.0: Initial release
 *
 *  \author Antoine Lambert & Patrick Mary of Tulip Team http://tulip.labri.fr/
 *
 *
 */

static constexpr std::string_view paramHelp[] = {
    // filename
    "This parameter defines the pathname of the GEXF file to import.",

    // curved edges
    "Indicates if Bézier curves should be used to draw the edges."};

class GEXFImport : public ImportModule {

public:
  PLUGININFORMATION("GEXF", "Antoine LAMBERT", "05/05/2010",
                    "<p>Supported extensions: gexf</p><p>Imports a new graph from a file in the "
                    "GEXF input format<br/>as it is described in the XML Schema 1.2 draft "
                    "(http://gexf.net/format/schema.html).<br/>Dynamic mode is not yet "
                    "supported.</p>",
                    "1.0", "File")
  GEXFImport(const PluginContext *context)
      : ImportModule(context), viewLayout(nullptr), viewSize(nullptr), viewColor(nullptr),
        viewLabel(nullptr), viewShape(nullptr), nodesHaveCoordinates(false) {
    // add a file parameter for the plugin
    addInParameter<string>("file::filename", paramHelp[0].data(), "");
    addInParameter<bool>("Curved edges", paramHelp[1].data(), "false");
  }

  ~GEXFImport() override = default;

  std::string icon() const override {
    return ":/talipot/app/icons/32/import_gephi.png";
  }

  std::list<std::string> fileExtensions() const override {
    return {"gexf"};
  }

  // import plugins must implement bool importGraph()
  bool importGraph() override {
    string filename;
    bool curvedEdges = false;
    // get the filename chosen by the user
    dataSet->get<string>("file::filename", filename);
    dataSet->get<bool>("Curved edges", curvedEdges);

    QString qfilename = tlpStringToQString(filename);

    // if wrong extension, abort
    if (!qfilename.endsWith(".gexf")) {
      return false;
    }

    // get Talipot visual attributes properties associated to the empty graph we want to fill
    viewLayout = graph->getLayoutProperty("viewLayout");
    viewLabel = graph->getStringProperty("viewLabel");
    viewSize = graph->getSizeProperty("viewSize");
    viewColor = graph->getColorProperty("viewColor");
    viewShape = graph->getIntegerProperty("viewShape");

    nodesHaveCoordinates = false;

    // Open the GEXF file chosen by the user
    QFile xmlFile(qfilename);

    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      // get error
      pluginProgress->setError(QStringToTlpString(xmlFile.errorString()));
      return false;
    }

    // Instantiate a QXmlStreamReader to parse the file (GEXF is xml)
    QXmlStreamReader xmlReader(&xmlFile);

    // Parse each line of the file
    while (!xmlReader.atEnd()) {
      if (xmlReader.readNextStartElement()) {
        // only static graph are supported
        if (xmlReader.name() == QString("graph")) {
          string mode = QStringToTlpString(xmlReader.attributes().value("mode").toString());

          if (mode == "dynamic") {
            pluginProgress->setError("dynamic graph is not yet supported");
            return false;
          }
        }
        // create Talipot Properties from Gephi attributes
        else if (xmlReader.name() == QString("attributes")) {
          createPropertiesFromAttributes(xmlReader);
        }
        // parse graph node data
        else if (xmlReader.name() == QString("nodes")) {
          createNodes(xmlReader, graph);
        }
        // parse graph edge data
        else if (xmlReader.name() == QString("edges")) {
          createEdges(xmlReader);
        }
      }
    }

    xmlFile.close();

    // Special case : some GEXF files declare edges before nodes
    // so we have to add edges once nodes have been parsed
    for (const auto &[srcId, tgtId] : edgesTmp) {
      graph->addEdge(nodesMap[srcId], nodesMap[tgtId]);
    }

    // nodes shape will be circle
    viewShape->setAllNodeValue(NodeShape::Circle);

    // add subgraph nodes, edges and meta nodes if needed
    Graph *quotientGraph = addSubGraphsNodes();

    if (quotientGraph) {
      addSubGraphsEdges();
      computeMetaNodes(quotientGraph);
    }

    // Set edges to be rendered as Cubic Bézier curves and
    // compute curve control points for each edge
    if (curvedEdges && nodesHaveCoordinates) {
      viewShape->setAllEdgeValue(EdgeShape::BezierCurve);
      curveGraphEdges();
    }

    return true;
  }

  // Create a set of Talipot Properties from the attributes declared in the GEXF file
  // according to data types
  void createPropertiesFromAttributes(QXmlStreamReader &xmlReader) {
    bool nodeProperties = xmlReader.attributes().value("class") == QString("node");
    flat_hash_map<string, PropertyInterface *> &propertiesMap =
        nodeProperties ? nodePropertiesMap : edgePropertiesMap;

    while (!(xmlReader.isEndElement() && xmlReader.name() == QString("attributes"))) {
      xmlReader.readNext();

      // create a Talipot property and store mapping between attribute id and property
      if (xmlReader.isStartElement() && xmlReader.name() == QString("attribute")) {
        string attributeId = QStringToTlpString(xmlReader.attributes().value("id").toString());
        string attributeName = QStringToTlpString(xmlReader.attributes().value("title").toString());
        string attributeType = QStringToTlpString(xmlReader.attributes().value("type").toString());

        if (attributeType == "string") {
          propertiesMap[attributeId] = graph->getStringProperty(attributeName);
        } else if (attributeType == "float" || attributeType == "double") {
          propertiesMap[attributeId] = graph->getDoubleProperty(attributeName);
        } else if (attributeType == "integer") {
          propertiesMap[attributeId] = graph->getIntegerProperty(attributeName);
        } else if (attributeType == "boolean") {
          propertiesMap[attributeId] = graph->getBooleanProperty(attributeName);
        }
      }
    }
  }

  // create nodes
  void createNodes(QXmlStreamReader &xmlReader, Graph *g) {
    while (!(xmlReader.isEndElement() && xmlReader.name() == QString("nodes"))) {
      xmlReader.readNext();

      // must be a node
      if (xmlReader.isStartElement() && xmlReader.name() == QString("node")) {
        parseNode(xmlReader, g);
      }
    }
  }

  // create edges
  void createEdges(QXmlStreamReader &xmlReader) {
    while (!(xmlReader.isEndElement() && xmlReader.name() == QString("edges"))) {
      xmlReader.readNext();

      // must be an edge
      if (xmlReader.isStartElement() && xmlReader.name() == QString("edge")) {
        parseEdge(xmlReader);
      }
    }
  }

  Graph *addInParent(node n, const string &pid) {
    // get the parent node
    // to find the corresponding sub graph
    // in which the current node must be added
    node pn;

    if (!nodesMap.contains(pid)) {
      // create a fake meta node
      pn = graph->addNode();
      nodesMap[pid] = pn;
    } else {
      pn = nodesMap[pid];
    }

    Graph *sg = nullptr;
    if (const auto it = nodeToSubgraph.find(pn); it != nodeToSubgraph.end()) {
      sg = it->second;
    }

    if (sg == nullptr) {
      // add a subgraph for the fake meta node
      sg = graph->addSubGraph();
      // record pn as its fake meta node
      sg->setAttribute<node>("meta-node", pn);
      // and vice-versa
      nodeToSubgraph[pn] = sg;
    }

    // add n in the subgraph found
    sg->addNode(n);

    return sg;
  }

  // Parse node data
  void parseNode(QXmlStreamReader &xmlReader, Graph *g) {
    node n;
    string nodeId = QStringToTlpString(xmlReader.attributes().value("id").toString());

    if (!nodesMap.contains(nodeId)) {
      // if needed, add a node in the graph we are building
      n = g->addNode();
      // save mapping between gexf node id and created Talipot node
      nodesMap[nodeId] = n;
    } else {
      n = nodesMap[nodeId];
    }

    // parse node label
    if (xmlReader.attributes().hasAttribute("label")) {
      string nodeLabel = QStringToTlpString(xmlReader.attributes().value("label").toString());
      viewLabel->setNodeValue(n, nodeLabel);
    }

    // parse node pid
    if (xmlReader.attributes().hasAttribute("pid")) {
      string pid = QStringToTlpString(xmlReader.attributes().value("pid").toString());

      if (g == graph) {
        g = addInParent(n, pid);
      } else {
        cerr << "multiple parents are not supported: " << pid.c_str()
             << " will be not added as parent of " << nodeId.c_str() << endl;
      }
    }

    xmlReader.readNext();

    while (!(xmlReader.isEndElement() && xmlReader.name() == QString("node"))) {
      // parse node color
      if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("viz:color")) {
        uint r = xmlReader.attributes().value("r").toString().toUInt();
        uint g = xmlReader.attributes().value("g").toString().toUInt();
        uint b = xmlReader.attributes().value("b").toString().toUInt();
        float a = 1.0f;

        if (xmlReader.attributes().hasAttribute("a")) {
          a = xmlReader.attributes().value("a").toString().toFloat();
        }

        viewColor->setNodeValue(n, Color(uchar(r), uchar(g), uchar(b), uchar(a * 255)));
      }
      // parse node coordinates
      else if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("viz:position")) {
        nodesHaveCoordinates = true;
        float x = xmlReader.attributes().value("x").toString().toFloat();
        float y = xmlReader.attributes().value("y").toString().toFloat();
        float z = xmlReader.attributes().value("z").toString().toFloat();
        viewLayout->setNodeValue(n, Coord(x, y, z));
      }
      // parse node size
      else if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("viz:size")) {
        float size = xmlReader.attributes().value("value").toString().toFloat();
        viewSize->setNodeValue(n, Size(size, size, size));
      }
      // parse node attributes
      else if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("attvalue")) {
        string attributeId;

        if (xmlReader.attributes().hasAttribute("id")) {
          attributeId = QStringToTlpString(xmlReader.attributes().value("id").toString());
        } else if (xmlReader.attributes().hasAttribute("for")) {
          attributeId = QStringToTlpString(xmlReader.attributes().value("for").toString());
        }

        string attributeStrValue =
            QStringToTlpString(xmlReader.attributes().value("value").toString());

        if (nodePropertiesMap.contains(attributeId)) {
          nodePropertiesMap[attributeId]->setNodeStringValue(n, attributeStrValue);
        }
      }
      // check for subgraph
      else if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("nodes")) {
        Graph *sg = nullptr;
        if (const auto it = nodeToSubgraph.find(n); it != nodeToSubgraph.end()) {
          sg = it->second;
        }

        if (sg == nullptr) {
          // add subgraph
          sg = graph->addSubGraph();
          // record the current node as its fake meta node
          sg->setAttribute<node>("meta-node", n);
          // and vice-versa
          nodeToSubgraph[n] = sg;
        }

        // create its nodes
        createNodes(xmlReader, sg);
      } else if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("edges")) {
        // create its edges
        createEdges(xmlReader);
      } else if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("parents")) {
        while (!(xmlReader.isEndElement() && xmlReader.name() == QString("parents"))) {
          xmlReader.readNext();

          // must be a parent
          if (xmlReader.isStartElement() && xmlReader.name() == QString("parent")) {
            string pid = QStringToTlpString(xmlReader.attributes().value("for").toString());

            if (g == graph) {
              g = addInParent(n, pid);
            } else {
              cerr << "multiple parents are not supported: " << pid.c_str()
                   << " will be not added as parent of " << nodeId.c_str() << endl;
            }
          }
        }
      }

      xmlReader.readNext();
    }
  }

  // Parse edge data
  void parseEdge(QXmlStreamReader &xmlReader) {
    // parse the source node id and target node id
    string srcId = QStringToTlpString(xmlReader.attributes().value("source").toString());
    string tgtId = QStringToTlpString(xmlReader.attributes().value("target").toString());

    // Check if nodes have been parsed
    if (!nodesMap.empty()) {
      // add an edge in the graph we are building
      edge e = graph->addEdge(nodesMap[srcId], nodesMap[tgtId]);

      if (xmlReader.attributes().hasAttribute("label")) {
        string edgeLabel = QStringToTlpString(xmlReader.attributes().value("label").toString());
        viewLabel->setEdgeValue(e, edgeLabel);
      }

      xmlReader.readNext();

      while (!(xmlReader.isEndElement() && xmlReader.name() == QString("edge"))) {
        // parse edge attribute
        if (xmlReader.isStartElement() && xmlReader.qualifiedName() == QString("attvalue")) {
          string attributeId;

          if (xmlReader.attributes().hasAttribute("id")) {
            attributeId = QStringToTlpString(xmlReader.attributes().value("id").toString());
          } else if (xmlReader.attributes().hasAttribute("for")) {
            attributeId = QStringToTlpString(xmlReader.attributes().value("for").toString());
          }

          string attributeStrValue =
              QStringToTlpString(xmlReader.attributes().value("value").toString());

          if (edgePropertiesMap.contains(attributeId)) {
            edgePropertiesMap[attributeId]->setEdgeStringValue(e, attributeStrValue);
          }
        }

        xmlReader.readNext();
      }

      // Store edge extremities information to add them to the graph once nodes will be parsed
    } else {
      edgesTmp.push_back(make_pair(srcId, tgtId));
    }
  }

  Graph *addSubGraphsNodes() {
    // quotientGraph will be created
    // as soon as there is a subgraph
    Graph *quotientGraph = nullptr;
    // iterate on each subgraph of graph
    // and add missing nodes
    for (Graph *sg : stableIterator(graph->getSubGraphs())) {
      if (!quotientGraph) {
        quotientGraph = graph->addCloneSubGraph("quotient graph");
      }

      // iterate on nodes
      for (auto n : stableIterator(sg->getNodes())) {
        Graph *msg = nullptr;
        if (const auto it = nodeToSubgraph.find(n); it != nodeToSubgraph.end()) {
          msg = it->second;
        }

        if (msg) {
          // if the current node is a fake meta node
          // add the nodes of the pointed subgraph
          for (auto msn : msg->nodes()) {
            sg->addNode(msn);
            // the nodes in a pointed subgraph have to be removed
            // from quotientGraph
            quotientGraph->delNode(msn);
          }
        }
      }
    }

    return quotientGraph;
  }

  void addSubGraphsEdges() {
    // iterate on each subgraph of graph
    // and add needed edges
    for (Graph *sg : graph->subGraphs()) {
      // iterate on nodes
      for (auto n : sg->nodes()) {
        // add its out edges
        for (auto e : graph->getOutEdges(n)) {
          if (sg->isElement(graph->target(e))) {
            sg->addEdge(e);
          }
        }
      }
    }
  }

  void computeMetaNodes(Graph *quotientGraph) {
    // iterate on each subgraph of g
    // and add needed meta nodes

    for (Graph *sg : graph->subGraphs()) {
      // iterate on nodes
      vector<node> sgNodes(sg->nodes());
      for (auto n : sgNodes) {
        Graph *msg = nullptr;
        if (const auto it = nodeToSubgraph.find(n); it != nodeToSubgraph.end()) {
          msg = it->second;
        }

        if (msg != nullptr) {
          // n is a fake meta node
          // create the real meta node
          // pointing to msg
          node mn = sg->createMetaNode(msg);
          // set its label as the name
          // of the pointed subgraph
          string label = viewLabel->getNodeValue(n);

          if (!label.empty()) {
            msg->setName(label);
          }

          // set meta node properties values to the ones
          // of the fake meta node
          for (PropertyInterface *prop : graph->getObjectProperties()) {
            prop->copy(mn, n, prop, true);
          }

          // add it to quotient graph if needed
          if (sg != quotientGraph) {
            quotientGraph->addNode(mn);
          }

          // replace n by mn
          for (auto e : graph->incidence(n)) {
            const auto &[src, tgt] = graph->ends(e);

            if (src == n) {
              graph->setEnds(e, mn, tgt);

              if ((sg != quotientGraph) && quotientGraph->isElement(tgt)) {
                quotientGraph->addEdge(e);
              }
            } else {
              graph->setEnds(e, src, mn);

              if ((sg != quotientGraph) && quotientGraph->isElement(src)) {
                quotientGraph->addEdge(e);
              }
            }
          }
          // remove fake meta node
          graph->delNode(n);
          msg->removeAttribute("meta-node");
          nodeToSubgraph[n] = nullptr;
        }
      }
    }
  }

  // Methods which compute Cubic Bézier control points for each edge
  void curveGraphEdges() {
    for (auto e : graph->edges()) {
      const auto &[src, tgt] = graph->ends(e);
      const Coord &srcCoord = viewLayout->getNodeValue(src);
      const Coord &tgtCoord = viewLayout->getNodeValue(tgt);
      Coord dir = tgtCoord - srcCoord;
      dir /= dir.norm();
      float length = srcCoord.dist(tgtCoord);
      float factor = 0.2f * length;
      Coord normal = Coord(dir[1], -dir[0]);
      normal *= factor;

      Coord p1 = dir;
      p1 *= factor;
      p1 += srcCoord;
      p1 += normal;

      Coord p2 = dir;
      p2 *= -factor;
      p2 += tgtCoord;
      p2 += normal;

      // Set the second and third Cubic Bézier curve control points as edge bends
      vector<Coord> bends;
      bends.push_back(p1);
      bends.push_back(p2);

      viewLayout->setEdgeValue(e, bends);
    }
  }

private:
  // maps associating attribute id to Talipot property
  flat_hash_map<string, PropertyInterface *> nodePropertiesMap;
  flat_hash_map<string, PropertyInterface *> edgePropertiesMap;

  // map associating GEXF node id to Talipot node
  flat_hash_map<string, node> nodesMap;

  // vector to store edge information in case edges are declared before nodes in GEXF file
  vector<pair<string, string>> edgesTmp;

  // Visual attributes properties pointers to the graph we are building
  LayoutProperty *viewLayout;
  SizeProperty *viewSize;
  ColorProperty *viewColor;
  StringProperty *viewLabel;
  IntegerProperty *viewShape;
  // to register the subgraph corresponding
  // to a fake meta node
  flat_hash_map<node, Graph *> nodeToSubgraph;

  bool nodesHaveCoordinates;
};

// Macro for declaring import plugin in Talipot, it will then be accessible since the File -> Import
// -> File menu entry in Talipot
PLUGIN(GEXFImport)
