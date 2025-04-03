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

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;
#define DEGTORAD(x) (M_PI / 180) * x

static constexpr std::string_view paramHelp[] = {
    // width
    "Grid node width.",

    // height
    "Grid node height.",

    // Connectivity
    "Connectivity number of each node.",

    // tore
    "If true, opposite nodes on each side of the grid are connected. In a 4 connectivity the "
    "resulting object is a torus.",

    // Line spacing
    "Spacing between nodes."};

/// Grid - Import of a grid.
/** This plugin enables to create a grid.
 *
 *  User can specify the connectivity of each nodes in the grid, spacing between nodes and if
 * opposite nodes are connected.
 */
class Grid : public ImportModule {
  public:
    PLUGININFORMATION("Grid", "Jonathan Dubois", "02/12/2003",
                      "Imports a new grid structured graph.", "2.0", "Graph")
    Grid(tlp::PluginContext *context) : ImportModule(context) {
        addInParameter<uint>("width", paramHelp[0].data(), "10");
        addInParameter<uint>("height", paramHelp[1].data(), "10");
        addInParameter<StringCollection>("connectivity", paramHelp[2].data(), "4;6;8", true,
                                         "<b>4</b> <br> <b>6</b> <br> <b>8</b>");
        addInParameter<bool>("oppositeNodesConnected", paramHelp[3].data(), "false");
        addInParameter<double>("spacing", paramHelp[4].data(), "1.0");
    }
    ~Grid() override = default;

    void buildRow(const vector<node> &nodes, vector<pair<node, node>> &ends, uint iRow,
                  unsigned width, int conn, bool isTore, double spacing) {
        LayoutProperty *layout = graph->getLayoutProperty("viewLayout");

        // Used for conn == 6
        double r = 0.5;
        double h = sqrt((r * r) - ((r / 2) * (r / 2)));
        double hHeight = cos(DEGTORAD(60)) * r;
        double shift = 0;

        // If iRow is even introduce a shift
        if (iRow % 2 == 0) {
            shift += h;
        } else {
            shift += 0;
        }

        uint iBegin = iRow * width;
        node previous, current;

        for (uint i = 0; i < width; ++i) {
            current = nodes[iBegin + i];

            if (conn == 6) {
                layout->setNodeValue(current, Coord(i * 2 * h + shift + i * spacing,
                                                    iRow * (1.0 - hHeight + spacing), 0));
            } else {
                layout->setNodeValue(current,
                                     Coord(i * (1.0 + spacing), iRow * (1.0 + spacing), 0));
            }

            if (previous.isValid()) {
                ends.push_back(pair<node, node>(previous, current));
            }

            previous = current;
        }

        if (isTore) {
            ends.push_back(pair<node, node>(current, nodes[iBegin]));
        }
    }

    void connectRow(const vector<node> &nodes, vector<pair<node, node>> &ends, uint row1, uint row2,
                    uint width, int conn, bool isTore) {
        uint row1Begin = row1 * width;
        uint row2Begin = row2 * width;

        for (uint i = 0; i < width; ++i) {
            ends.push_back(pair<node, node>(nodes[row1Begin + i], nodes[row2Begin + i]));

            if (conn == 8) {
                if (i > 0) {
                    ends.push_back(
                        pair<node, node>(nodes[row1Begin + i], nodes[row2Begin + i - 1]));
                } else if (isTore) {
                    ends.push_back(
                        pair<node, node>(nodes[row1Begin + i], nodes[row2Begin + width - 1]));
                }

                if (i < width - 1) {
                    ends.push_back(
                        pair<node, node>(nodes[row1Begin + i], nodes[row2Begin + i + 1]));
                } else if (isTore) {
                    ends.push_back(pair<node, node>(nodes[row1Begin + i], nodes[row2Begin]));
                }
            }

            if (conn == 6) {
                // In this case row1 must be even in order to ensure right connectivity in the
                // hexagonal grid.
                if (row1 % 2 == 0) {
                    if (i < width - 1) {
                        ends.push_back(
                            pair<node, node>(nodes[row1Begin + i], nodes[row2Begin + i + 1]));
                    } else if (isTore) {
                        ends.push_back(pair<node, node>(nodes[row1Begin + i], nodes[row2Begin]));
                    }
                } else {
                    if (i > 0) {
                        ends.push_back(
                            pair<node, node>(nodes[row1Begin + i], nodes[row2Begin + i - 1]));
                    } else if (isTore) {
                        ends.push_back(
                            pair<node, node>(nodes[row1Begin + i], nodes[row2Begin + width - 1]));
                    }
                }
            }
        }
    }

    bool importGraph() override {
        uint width = 10;
        uint height = 10;
        bool isTore = false;
        int conn = 4;
        StringCollection connectivity;
        double spacing = 1.0;

        if (dataSet != nullptr) {
            dataSet->get("width", width);
            dataSet->get("height", height);
            dataSet->get("oppositeNodesConnected", isTore);
            dataSet->get("spacing", spacing);
            dataSet->get("connectivity", connectivity);
        }

        if (width == 0) {
            if (pluginProgress) {
                pluginProgress->setError(string("Error: width cannot be null"));
            }

            return false;
        }

        if (height == 0) {
            if (pluginProgress) {
                pluginProgress->setError(string("Error: height cannot be null"));
            }

            return false;
        }

        if (spacing < 0.0) {
            if (pluginProgress) {
                pluginProgress->setError(string("Error: spacing must be strictly positive"));
            }

            return false;
        }

        if (connectivity.getCurrentString() == "4") {
            conn = 4;
        } else if (connectivity.getCurrentString() == "6") {
            conn = 6;

            if (isTore && height % 2 == 1) {
                if (pluginProgress) {
                    pluginProgress->setError("Error : cannot connect opposite nodes in an "
                                             "hexagonal grid with odd height");
                } else {
                    tlp::warning() << __PRETTY_FUNCTION__ << ":" << __LINE__
                                   << " Error : cannot connect opposite nodes in an hexagonal grid "
                                      "with odd height"
                                   << std::endl;
                }

                return false;
            }
        } else {
            conn = 8;
        }

        // graph is predimensioned according the parameters
        graph->addNodes(height * width);

        // compute nb edges
        uint nbEdges = height * (width - 1);

        if (isTore) {
            nbEdges += height;
        }

        // add the between rows connections to the in rows connections
        nbEdges += width * (height - 1);

        // more between rows connections
        if (conn >= 6) {
            nbEdges += (height - 1) * (width - 1);

            if (isTore) {
                nbEdges += height - 1;
            }
        }

        // more between row connections
        if (conn == 8) {
            nbEdges += (height - 1) * (width - 1);

            if (isTore) {
                nbEdges += height - 1;
            }
        }

        vector<pair<node, node>> ends;
        ends.reserve(nbEdges);
        graph->reserveEdges(nbEdges);

        const vector<node> &nodes = graph->nodes();
        buildRow(nodes, ends, 0, width, conn, isTore, spacing);

        for (uint i = 1; i < height; ++i) {
            buildRow(nodes, ends, i, width, conn, isTore, spacing);
            connectRow(nodes, ends, i - 1, i, width, conn, isTore);
        }

        if (isTore) {
            connectRow(nodes, ends, height - 1, 0, width, conn, isTore);
        }

        graph->addEdges(ends);

        return true;
    }
};

PLUGIN(Grid)
