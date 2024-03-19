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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include <cstring>

#include <cgraph.h>

#include <talipot/PluginHeaders.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

extern bool decodeGraphvizColor(Color &outColor, const string &inValue);
extern bool getCoordFromGraphvizPos(Coord &outCoord, const string &inValue);
extern bool getCoordsFromGraphvizPos(vector<Coord> &outCoords, const string &inValue);

static constexpr std::string_view paramHelp[] = {
    // filename
    "Graphviz file to import written in DOT language."};

/**
 * This plugin enables to import a graph written in the DOT language.
 *
 * File format: http://graphviz.org/doc/info/lang.html
 *
 * The plugin will import graph elements and their labels but also subgraphs.
 *
 * For each node or edge attribute, a talipot string property will be
 * created and raw values will be stored in it.
 *
 * Some visual encodings stored in those attributes will also be converted to
 * talipot view properties, notably:
 *   - node border color
 *   - node color
 *   - node label color
 *   - node shape
 *   - node position
 *   - node size
 *   - edge color
 *   - edge border color
 *   - edge spline control points
 *
 */
class GraphvizImport : public ImportModule {
public:
  PLUGININFORMATION("Graphviz", "Antoine Lambert", "19/01/2021",
                    "<p>Supported extensions: dot, gv</p>"
                    "<p>Imports a new graph from a file in the DOT language from Graphviz</p>",
                    "1.0", "File")

  list<string> fileExtensions() const override {
    return {"dot", "gv"};
  }

  GraphvizImport(PluginContext *context) : ImportModule(context) {
    addInParameter<string>("file::filename", paramHelp[0].data(), "");
  }

  ~GraphvizImport() override = default;

  string icon() const override {
    return ":/talipot/app/icons/32/import_graphviz.png";
  }

  bool importGraph() override {

    // Open input file stream
    string fn;
    dataSet->get("file::filename", fn);
#ifndef WIN32
    FILE *fd = fopen(fn.c_str(), "r");
#else
    wstring wfn = winPath(fn);
    FILE *fd = _wfopen(reinterpret_cast<const wchar_t *>(wfn.c_str()), L"r");
#endif

    // Abort if error
    if (!fd) {
      if (pluginProgress) {
        pluginProgress->setError(strerror(errno));
      }

      return false;
    }

    // Parse Graphviz file
    Agraph_t *g = agread(fd, 0);

    // Abort if error
    if (!g) {
      if (pluginProgress) {
        pluginProgress->setError("Graphviz file could not be read.");
      }
      fclose(fd);
      return false;
    }

    graph->setName(agnameof(g));

    // Get talipot view properties
    auto *viewLabel = graph->getStringProperty("viewLabel");
    auto *viewFontSize = graph->getIntegerProperty("viewFontSize");
    auto *viewColor = graph->getColorProperty("viewColor");
    auto *viewBorderColor = graph->getColorProperty("viewBorderColor");
    auto *viewBorderWidth = graph->getDoubleProperty("viewBorderWidth");
    auto *viewLabelColor = graph->getColorProperty("viewLabelColor");
    auto *viewLayout = graph->getLayoutProperty("viewLayout");
    auto *viewSize = graph->getSizeProperty("viewSize");
    auto *viewShape = graph->getIntegerProperty("viewShape");

    // Set some visual defaults
    Color defaultNodeColor;
    decodeGraphvizColor(defaultNodeColor, "lightgrey");
    viewColor->setAllNodeValue(defaultNodeColor);
    viewColor->setAllEdgeValue(Color::Black);
    viewBorderWidth->setAllNodeValue(2);
    viewShape->setAllEdgeValue(EdgeShape::CubicBSplineCurve);

    // Graphviz shape to talipot shape mapping
    unordered_map<string, NodeShape::NodeShapes> shapeMap = {
        {"circle", NodeShape::Circle},     {"box", NodeShape::Square},
        {"ellipse", NodeShape::Circle},    {"oval", NodeShape::Circle},
        {"triangle", NodeShape::Triangle}, {"diamond", NodeShape::Diamond},
        {"pentagon", NodeShape::Pentagon}, {"hexagon", NodeShape::Hexagon},
        {"rect", NodeShape::Square},       {"rectangle", NodeShape::Square},
        {"square", NodeShape::Square},     {"star", NodeShape::Star},
        {"cylinder", NodeShape::Cylinder}, {"box3d", NodeShape::Cube}};

    // Define utility functions to set graph properties values
    using SetNodePropertyFunc = function<void(node, const string &)>;
    using SetEdgePropertyFunc = function<void(edge, const string &)>;

    auto setNodePropertyFunc = [&](PropertyInterface *prop) {
      auto func = [prop](node n, const string &nodeVal) { prop->setNodeStringValue(n, nodeVal); };
      return func;
    };

    auto setNodeColorPropertyFunc = [&](ColorProperty *colorProp) {
      auto func = [colorProp](node n, const string &nodeVal) {
        Color color;
        if (decodeGraphvizColor(color, nodeVal)) {
          colorProp->setNodeValue(n, color);
        }
      };
      return func;
    };

    auto setEdgeColorPropertyFunc = [&](ColorProperty *colorProp) {
      auto func = [colorProp](edge e, const string &edgeVal) {
        Color color;
        if (decodeGraphvizColor(color, edgeVal)) {
          colorProp->setEdgeValue(e, color);
        }
      };
      return func;
    };

    auto setNodeLayoutFunc = [&](node n, const string &nodeVal) {
      Coord coord;
      if (getCoordFromGraphvizPos(coord, nodeVal)) {
        viewLayout->setNodeValue(n, coord);
      }
    };

    auto setNodeShapeFunc = [&](node n, const string &nodeVal) {
      if (shapeMap.contains(nodeVal)) {
        viewShape->setNodeValue(n, shapeMap[nodeVal]);
      }
    };

    auto setEdgeLayoutFunc = [&](edge e, const string &edgeVal) {
      vector<Coord> bends;
      if (getCoordsFromGraphvizPos(bends, edgeVal)) {
        viewLayout->setEdgeValue(e, bends);
      }
    };

    auto setNodeSizePropertyFunc = [&](int i) {
      auto setNodeSizeFunc = [i, viewSize](node n, const string &nodeVal) {
        Size size = viewSize->getNodeValue(n);
        size[i] = atof(nodeVal.c_str()) * 72;
        viewSize->setNodeValue(n, size);
      };
      return setNodeSizeFunc;
    };

    unordered_map<string, SetNodePropertyFunc> setNodePropertyMap;
    setNodePropertyMap["label"] = setNodePropertyFunc(viewLabel);
    setNodePropertyMap["fontsize"] = setNodePropertyFunc(viewFontSize);
    setNodePropertyMap["color"] = setNodeColorPropertyFunc(viewBorderColor);
    setNodePropertyMap["fillcolor"] = setNodeColorPropertyFunc(viewColor);
    setNodePropertyMap["fontcolor"] = setNodeColorPropertyFunc(viewLabelColor);
    setNodePropertyMap["pos"] = setNodeLayoutFunc;
    setNodePropertyMap["width"] = setNodeSizePropertyFunc(0);
    setNodePropertyMap["height"] = setNodeSizePropertyFunc(1);
    setNodePropertyMap["shape"] = setNodeShapeFunc;

    unordered_map<string, SetEdgePropertyFunc> setEdgePropertyMap;
    setEdgePropertyMap["pos"] = setEdgeLayoutFunc;
    setEdgePropertyMap["color"] = setEdgeColorPropertyFunc(viewBorderColor);
    setEdgePropertyMap["fillcolor"] = setEdgeColorPropertyFunc(viewColor);
    setEdgePropertyMap["fontcolor"] = setEdgeColorPropertyFunc(viewLabelColor);

    // Copy graph attributes
    Agsym_t *sym = 0;
    while ((sym = agnxtattr(g, AGRAPH, sym))) {
      string attrName = sym->name;
      string attrValue = agxget(g, sym);
      if (!attrValue.empty()) {
        graph->setAttribute(attrName, attrValue);
      }
    }

    // Utility function to add a node
    unordered_map<Agnode_t *, node> nodeMap;
    auto getTlpNode = [&](Agnode_t *n) {
      if (!nodeMap.contains(n)) {
        node nTlp;
        nodeMap[n] = nTlp = graph->addNode();
        viewLabel->setNodeValue(nTlp, agnameof(n));
        Agsym_t *sym = 0;
        while ((sym = agnxtattr(g, AGNODE, sym))) {
          string attrName = sym->name;
          string attrDefaultValue = sym->defval;
          string attrValue = agxget(n, sym);

          auto *prop = graph->getStringProperty(attrName);
          prop->setNodeDefaultValue(attrDefaultValue);
          prop->setNodeValue(nTlp, attrValue);

          if (!attrValue.empty() && setNodePropertyMap.contains(attrName)) {
            setNodePropertyMap[attrName](nTlp, attrValue);
          }
        }
      }
      return nodeMap[n];
    };

    // Iterate on nodes and edges, create talipot ones, set property values
    for (Agnode_t *n = agfstnode(g); n; n = agnxtnode(g, n)) {
      node src = getTlpNode(n);
      for (Agedge_t *e = agfstout(g, n); e; e = agnxtout(g, e)) {
        node tgt = getTlpNode(aghead(e));
        edge eTlp = graph->addEdge(src, tgt);

        Agsym_t *sym = 0;
        while ((sym = agnxtattr(g, AGEDGE, sym))) {
          string attrName = sym->name;
          string attrDefaultValue = sym->defval;
          string attrValue = agxget(e, sym);

          auto *prop = graph->getStringProperty(attrName);
          prop->setEdgeDefaultValue(attrDefaultValue);
          prop->setEdgeValue(eTlp, attrValue);

          if (!attrValue.empty() && setEdgePropertyMap.contains(attrName)) {
            setEdgePropertyMap[attrName](eTlp, attrValue);
          }
        }
      }
    }

    // Iterate on subgraphs and create talipot ones
    function<void(Agraph_t *, Graph *)> addSubgraphs = [&](Agraph_t *root, Graph *g) {
      for (Agraph_t *sg = agfstsubg(root); sg; sg = agnxtsubg(sg)) {
        Graph *sgTlp = g->addSubGraph(agnameof(sg));
        for (Agnode_t *n = agfstnode(sg); n; n = agnxtnode(sg, n)) {
          node src = getTlpNode(n);
          sgTlp->addNode(src);
          for (Agedge_t *e = agfstout(sg, n); e; e = agnxtout(sg, e)) {
            node tgt = getTlpNode(aghead(e));
            sgTlp->addNode(tgt);
            sgTlp->addEdge(graph->existEdge(src, tgt));
          }
        }
        addSubgraphs(sg, sgTlp);
      }
    };
    addSubgraphs(g, graph);

    // Cleanup
    agclose(g);
    fclose(fd);

    return true;
  }
};

PLUGIN(GraphvizImport)

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
