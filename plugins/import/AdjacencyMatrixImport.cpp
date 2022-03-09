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

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;

enum ValType { TLP_DOUBLE = 0, TLP_STRING = 1, TLP_NOVAL = 2, TLP_NOTHING = 3, TLP_AND = 4 };

static constexpr std::string_view paramHelp[] = {
    // filename
    "This parameter defines the pathname of the file to import."};

/** This plugin enables to import a graph coded with a matrix
 *
 *  File format:
 *
 *  The input format of this plugin is an ascii file where each
 *  line represents a row of the matrix.
 *  In each row, cells must be separated by a space.
 *
 *  Let M(i,j) be a cell of the matrix :
 *       - if i==j we define the value of a node.
 *       - if i!=j  we define a directed edge between node[i] and node[j]
 *
 *  If M(i,j) is real value (0, .0, -1, -1.0), it is stored in the
 *  viewMetric property of the graph. <br>
 *  If M(i,j) is a string, it is stored in the
 *  viewLabel property of the graph. <br>
 *  Use & to set the viewMetric and viewLabel properties of a node or edge in the same time.
 *  If M(i,j) == @ an edge will be created without value <br>
 *  If M(i,j) == # no edge will be created between node[i] and node[j]
 *
 *  EXAMPLE 1 :
 *  <br>A
 *  <br># B
 *  <br># # C
 *  <br>Defines a graph with 3 nodes (with labels A B C) and without edge.
 *
 *  EXAMPLE 2 :
 *  <br>A
 *  <br>@ B
 *  <br>@ @ C
 *  <br>Defines a simple complete graph with 3 nodes (with labels A B C) and no label (or value) on
 * its edges
 *
 *  EXAMPLE 3 :
 *  <br>A # E & 5
 *  <br>@ B
 *  <br># @ C
 *  <br>Defines a graph with 3 nodes and 3 edges, the edge between A and C is named E and has the
 * value 5
 *
 */
class AdjacencyMatrixImport : public ImportModule {
public:
  PLUGININFORMATION("Adjacency Matrix", "Auber David", "05/09/2008",
                    "Imports a graph from a file coding an adjacency matrix.<br/>File format:<br/>\
The input format of this plugin is an ascii file where each line represents a row of the matrix.\
In each row, cells must be separated by a space.<br/>Let M(i,j) be a cell of the matrix :<br/>\
     - if i==j we define the value of a node.<br/>\
     - if i!=j  we define a directed edge between node[i] and node[j]<br/>\
If M(i,j) is real value (0, .0, -1, -1.0), it is stored in the viewMetric property of the graph.<br/>\
If M(i,j) is a string, it is stored in the viewLabel property of the graph.<br/>\
Use & to set the viewMetric and viewLabel properties of a node or edge in the same time.<br/>\
If M(i,j) == @ an edge will be created without value<br/>\
If M(i,j) == # no edge will be created between node[i] and node[j]<br/>\
EXAMPLE 1 :<br/>\
A<br/>\
# B<br/>\
# # C<br/>\
Defines a graph with 3 nodes (with labels A B C) and without edge.<br/>\
EXAMPLE 2 :<br/>\
A<br/>\
@ B<br/>\
@ @ C<br/>\
Defines a simple complete graph with 3 nodes (with labels A B C) and no label (or value) on its edges<br/>\
EXAMPLE 3 :<br/>\
A # E & 5<br/>\
@ B<br/># @ C<br/>\
Defines a graph with 3 nodes and 3 edges, the edge between A and C is named E and has the value 5",
                    "1.2", "File")
  AdjacencyMatrixImport(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<string>("file::filename", paramHelp[0].data(), "");
  }
  ~AdjacencyMatrixImport() override = default;
  vector<node> nodes;

  std::string icon() const override {
    return ":/talipot/app/icons/32/import_adj_mat.png";
  }

  bool formatError(const char *s, int curLine) {
    std::stringstream ess;
    ess << "Error parsing '" << s << "' at line :" << curLine + 1;
    pluginProgress->setError(ess.str());
    tlp::warning() << pluginProgress->getError() << std::endl;
    return false;
  }

  bool importGraph() override {
    auto inputData = getInputData();

    if (!inputData.valid()) {
      return false;
    }

    std::istream *in = inputData.is.get();
    uint curLine = 0;
    DoubleProperty *metric = graph->getDoubleProperty("viewMetric");
    StringProperty *stringP = graph->getStringProperty("viewLabel");

    std::string line;

    while (!in->eof() && std::getline(*in, line)) {
      stringstream lines(line);
      uint curNode = 0;
      edge e;
      bool itemFound = false;
      bool andFound = false;

      while (lines.good()) {
        string valString;

        if (lines >> valString) {
          const char *start = valString.c_str();
          char *res;
          double valDouble = strtod(start, &res);
          ValType type;

          if (res != start) {
            type = TLP_DOUBLE;
            itemFound = true;
          } else if (valString == "&") {
            if (!itemFound) {
              return formatError(valString.c_str(), curLine);
            }

            type = TLP_AND;
            --curNode;
            andFound = true;
            itemFound = false;
            continue;
          } else if (valString == "@") {
            if (andFound) {
              return formatError(valString.c_str(), curLine);
            }

            type = TLP_NOVAL;
            itemFound = false;
          } else if (valString == "#") {
            if (andFound) {
              return formatError(valString.c_str(), curLine);
            }

            type = TLP_NOTHING;
            itemFound = false;
          } else {
            type = TLP_STRING;
            itemFound = true;
          }

          if (curNode >= nodes.size() || curLine >= nodes.size()) {
            nodes.push_back(graph->addNode());
          }

          if (curNode == curLine) {
            switch (type) {
            case TLP_DOUBLE:
              metric->setNodeValue(nodes[curNode], valDouble);
              break;

            case TLP_STRING:
              stringP->setNodeValue(nodes[curNode], valString);
              break;

            default:
              return formatError(valString.c_str(), curLine);
            }
          } else {
            switch (type) {
            case TLP_DOUBLE:

              if (!andFound) {
                e = graph->addEdge(nodes[curLine], nodes[curNode]);
              }

              metric->setEdgeValue(e, valDouble);
              break;

            case TLP_STRING:

              if (!andFound) {
                e = graph->addEdge(nodes[curLine], nodes[curNode]);
              }

              stringP->setEdgeValue(e, valString);
              break;

            case TLP_NOVAL:
              e = graph->addEdge(nodes[curLine], nodes[curNode]);
              break;

            case TLP_NOTHING:
              break;

            default:
              return formatError(valString.c_str(), curLine);
            }
          }

          ++curNode;
        }

        andFound = false;
      }

      if (andFound) {
        return formatError("&", curLine);
      }

      ++curLine;
    }

    // final check:
    // number of lines must be equal to number of nodes
    if (curLine == nodes.size()) {
      return true;
    }

    pluginProgress->setError(std::string("The number of lines in file ") + inputData.filename +
                             "\n is different from the number of found nodes.");
    tlp::warning() << pluginProgress->getError() << std::endl;
    return false;
  }
};

PLUGIN(AdjacencyMatrixImport)
