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

#include "ReadGraph.h"
#include "ExportInterface.h"

#include <talipot/DrawingTools.h>
#include <talipot/GlEdge.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/GlGraphRenderingParameters.h>

using namespace std;
using namespace tlp;

static bool treatEdges(Graph *graph, tlp::PluginProgress *pp, ExportInterface &exportint,
                       unsigned &i, const int nb_elements, tlp::SizeProperty *sizes,
                       tlp::ColorProperty *colors, tlp::LayoutProperty *layout,
                       tlp::IntegerProperty *shape, tlp::IntegerProperty *srcanchorshape,
                       tlp::IntegerProperty *tgtanchorshape, tlp::StringProperty *label,
                       tlp::ColorProperty *labelcolor, bool edge_color_interpolation,
                       bool edge_size_interpolation, bool edge_extremities, const bool edge_labels,
                       IntegerProperty *fontsize, StringProperty *iconName) {
  pp->setComment("Exporting edges...");
  bool ret(true);
  ret = exportint.groupEdge();

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when starting edge export");
    }

    return false;
  }

  uint id_src_shape = 0;
  uint id_tgt_shape = 0;
  uint id_src_grad = 0;
  uint id_tgt_grad = 0;
  GlGraphRenderingParameters rp;
  GlGraphInputData inputData(graph, &rp);
  for (auto e : graph->edges()) {
    if ((++i % 100) == 0) {
      pp->progress(i, nb_elements);
    }

    const auto &[src, tgt] = graph->ends(e);
    ret = exportint.startEdge(e.id);

    if (!ret) {
      if (pp->getError().empty()) {
        stringstream str;
        str << "Error when starting to export edge " << e;
        pp->setError(str.str());
      }

      return false;
    }

    GlEdge glEdge(e, graph);
    Coord srcCoord, tgtCoord;
    Size srcSize, tgtSize;
    vector<Coord> edgeVertices;
    auto nbVertices = glEdge.getVertices(&inputData, e, src, tgt, srcCoord, tgtCoord, srcSize,
                                         tgtSize, edgeVertices);

    // nothing to do if current edge is a loop with no bends
    if (!nbVertices) {
      continue;
    }

    // Edges extremities
    EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type = EdgeExtremityShape::None;
    EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type = EdgeExtremityShape::None;
    bool ret(true);

    if (edge_extremities) {
      src_anchor_shape_type =
          static_cast<EdgeExtremityShape::EdgeExtremityShapes>(srcanchorshape->getEdgeValue(e));
      tgt_anchor_shape_type =
          static_cast<EdgeExtremityShape::EdgeExtremityShapes>(tgtanchorshape->getEdgeValue(e));
    }

    if (src_anchor_shape_type != EdgeExtremityShape::None ||
        tgt_anchor_shape_type != EdgeExtremityShape::None) {
      ret = exportint.exportEdgeExtremity(id_src_shape, id_tgt_shape, src_anchor_shape_type,
                                          tgt_anchor_shape_type, colors->getEdgeValue(e),
                                          id_src_grad, id_tgt_grad, iconName->getEdgeValue(e));

      if (!ret) {
        if (pp->getError().empty()) {
          stringstream str;
          str << "Error when exporting edge extremity for edge " << e;
          pp->setError(str.str());
        }

        return false;
      }
    }

    double width = 0;

    if (edge_size_interpolation) {
      // svg only handles a width for each edge
      width = std::min(sizes->getNodeValue(src)[0] / 8, sizes->getNodeValue(tgt)[0] / 8);
    } else {
      width = std::min(sizes->getEdgeValue(e)[0], sizes->getEdgeValue(e)[1]) + 1;
    }

    // Get edge type
    if (!edge_color_interpolation) {
      ret = exportint.exportEdge(static_cast<EdgeShape::EdgeShapes>(shape->getEdgeValue(e)),
                                 layout->getEdgeValue(e), colors->getEdgeValue(e), width,
                                 src_anchor_shape_type, id_src_shape, tgt_anchor_shape_type,
                                 id_tgt_shape, edgeVertices);
    } else {
      ret = exportint.exportEdge(e.id, static_cast<EdgeShape::EdgeShapes>(shape->getEdgeValue(e)),
                                 layout->getEdgeValue(e), colors->getNodeValue(src),
                                 colors->getNodeValue(tgt), width, src_anchor_shape_type,
                                 id_src_shape, tgt_anchor_shape_type, id_tgt_shape, edgeVertices);
    }

    if (!ret) {
      if (pp->getError().empty()) {
        stringstream str;
        str << "Error when exporting edge " << e;
        pp->setError(str.str());
      }

      return false;
    }

    if (edge_labels) {
      Coord c = edgeVertices[edgeVertices.size() / 2] + edgeVertices[edgeVertices.size() / 2 - 1];
      ret = exportint.addLabel("edge", label->getEdgeValue(e), labelcolor->getEdgeValue(e), c /= 2,
                               fontsize->getEdgeValue(e), sizes->getEdgeValue(e));

      if (!ret) {
        if (pp->getError().empty()) {
          stringstream str;
          str << "Error when exporting label for edge " << e;
          pp->setError(str.str());
        }

        return false;
      }
    }

    ret = exportint.endEdge();

    if (!ret) {
      if (pp->getError().empty()) {
        stringstream str;
        str << "Error when terminating export of edge " << e;
        pp->setError(str.str());
      }

      return false;
    }

    if (src_anchor_shape_type != EdgeExtremityShape::None) {
      ++id_src_shape;
    }

    if (tgt_anchor_shape_type != EdgeExtremityShape::None) {
      ++id_tgt_shape;
    }

    if (src_anchor_shape_type == EdgeExtremityShape::Sphere) {
      ++id_src_grad;
    }

    if (src_anchor_shape_type == EdgeExtremityShape::GlowSphere) {
      id_src_grad += 2;
    }

    if (tgt_anchor_shape_type == EdgeExtremityShape::Sphere) {
      ++id_tgt_grad;
    }

    if (tgt_anchor_shape_type == EdgeExtremityShape::GlowSphere) {
      id_tgt_grad += 2;
    }
  }

  // Ending the group of edges
  ret = exportint.endGroupEdge();

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when ending edge export");
    }

    return false;
  }

  return true;
}

struct sortNodes {
  LayoutProperty *layout;
  sortNodes(LayoutProperty *ly) : layout(ly) {}
  bool operator()(const node i, const node j) const {
    return layout->getNodeValue(i)[2] < layout->getNodeValue(j)[2];
  }
};

static bool treatNodes(Graph *graph, tlp::PluginProgress *pp, ExportInterface &exportint,
                       unsigned &i, const int nb_elements, tlp::SizeProperty *sizes,
                       tlp::ColorProperty *colors, tlp::LayoutProperty *layout,
                       tlp::IntegerProperty *shape, tlp::DoubleProperty *rotation,
                       tlp::DoubleProperty *borderwidth, tlp::StringProperty *label,
                       tlp::ColorProperty *labelcolor, tlp::ColorProperty *bordercolor,
                       std::vector<tlp::node> &metanodeVertices, const bool node_labels,
                       IntegerProperty *fontsize, StringProperty *iconName) {
  pp->setComment("Exporting nodes...");
  bool ret = exportint.groupNode();

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when starting node export");
    }

    return false;
  }

  /*z-ordering is handled in SVG.
  from the SVG 1.1 spec:
    3.3 Rendering Order
   Elements in an SVG document fragment have an implicit drawing order, with the first elements in
  the SVG document
  fragment getting "painted" first. Subsequent elements are painted on top of previously painted
  elements.

  So order nodes following their z coordinates before exporting them
  */

  vector<node> nodes = graph->nodes();
  sortNodes srt(layout);
  std::sort(nodes.begin(), nodes.end(), srt);

  for (auto n : nodes) {

    if (graph->isMetaNode(n)) {
      metanodeVertices.push_back(n);
    }

    Coord c = layout->getNodeValue(n);
    Size s = sizes->getNodeValue(n);

    if ((++i % 100) == 0) {
      pp->progress(i, nb_elements);
    }

    // Never change the call order of the methods below
    ret = exportint.startNode(n.id);

    if (!ret) {
      if (pp->getError().empty()) {
        stringstream str;
        str << "Error when starting to export node " << n;
        pp->setError(str.str());
      }

      return false;
    }

    if (rotation->getNodeValue(n) != 0) {
      ret = exportint.addRotation(rotation->getNodeValue(n), c);

      if (!ret) {
        if (pp->getError().empty()) {
          stringstream str;
          str << "Error when exporting rotation for node " << n;
          pp->setError(str.str());
        }

        return false;
      }
    }

    ret = exportint.addShape(static_cast<NodeShape::NodeShapes>(shape->getNodeValue(n)), c, s,
                             bordercolor->getNodeValue(n), borderwidth->getNodeValue(n),
                             colors->getNodeValue(n), iconName->getNodeValue(n));

    if (!ret) {
      if (pp->getError().empty()) {
        stringstream str;
        str << "Error when exporting shape for node " << n;
        pp->setError(str.str());
      }

      return false;
    }

    if (node_labels) {
      ret = exportint.addLabel("node", label->getNodeValue(n), labelcolor->getNodeValue(n), c,
                               fontsize->getNodeValue(n), s);

      if (!ret) {
        if (pp->getError().empty()) {
          stringstream str;
          str << "Error when exporting label for node " << n;
          pp->setError(str.str());
        }

        return false;
      }
    }

    ret = exportint.endNode();

    if (!ret) {
      if (pp->getError().empty()) {
        stringstream str;
        str << "Error when finishing to export node " << n;
        pp->setError(str.str());
      }

      return false;
    }
  }

  // Ending the group of nodes
  ret = exportint.endGroupNode();

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when finishing to export nodes");
    }

    return false;
  }

  ret = exportint.writeEndGraph();

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when finishing to export nodes");
    }

    return false;
  }

  return true;
}

bool ReadGraph::readGraph(Graph *graph, tlp::DataSet *ds, tlp::PluginProgress *pp,
                          ExportInterface &exportint) {
  LayoutProperty *layout = graph->getLayoutProperty("viewLayout");
  ColorProperty *colors = graph->getColorProperty("viewColor");
  IntegerProperty *shape = graph->getIntegerProperty("viewShape");
  IntegerProperty *srcanchorshape = graph->getIntegerProperty("viewSrcAnchorShape");
  IntegerProperty *tgtanchorshape = graph->getIntegerProperty("viewTgtAnchorShape");
  SizeProperty *sizes = graph->getSizeProperty("viewSize");
  StringProperty *label = graph->getStringProperty("viewLabel");
  ColorProperty *labelcolor = graph->getColorProperty("viewLabelColor");
  ColorProperty *bordercolor = graph->getColorProperty("viewBorderColor");
  DoubleProperty *borderwidth = graph->getDoubleProperty("viewBorderWidth");
  DoubleProperty *rotation = graph->getDoubleProperty("viewRotation");
  IntegerProperty *fontsize = graph->getIntegerProperty("viewFontSize");
  StringProperty *fontIconName = graph->getStringProperty("viewIcon");
  bool edge_color_interpolation = false;
  bool edge_extremities = false;
  bool edge_size_interpolation = true;
  bool edge_labels(false), node_labels(true), metanode_labels(false);
  Color background = Color::White;
  bool noBackground = false;

  if (ds != nullptr) {
    ds->get("Edge color interpolation", edge_color_interpolation);
    ds->get("Edge size interpolation", edge_size_interpolation);
    ds->get("Edge extremities", edge_extremities);
    ds->get("Background color", background);
    ds->get("No background", noBackground);
    ds->get("Export node labels", node_labels);
    ds->get("Export edge labels", edge_labels);
    ds->get("Export metanode labels", metanode_labels);
  }

  // Finding graph size
  BoundingBox graphbb = tlp::computeBoundingBox(graph, layout, sizes, rotation);

  // Writing the header of the file
  bool ret = exportint.writeHeader(graphbb);

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when exporting graph header");
    }

    return false;
  }

  ret = exportint.writeGraph(graphbb, background, noBackground);

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when starting to export graph");
    }

    return false;
  }

  // We will start by analysing edges for a better display of the image
  int nb_elements = graph->numberOfEdges() + graph->numberOfNodes();
  unsigned i = 0;

  // Analysing edges
  ret = treatEdges(graph, pp, exportint, i, nb_elements, sizes, colors, layout, shape,
                   srcanchorshape, tgtanchorshape, label, labelcolor, edge_color_interpolation,
                   edge_size_interpolation, edge_extremities, edge_labels, fontsize, fontIconName);

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when exporting edges");
    }

    return false;
  }

  // Analysing nodes
  std::vector<tlp::node> metanodeVertices;
  ret = treatNodes(graph, pp, exportint, i, nb_elements, sizes, colors, layout, shape, rotation,
                   borderwidth, label, labelcolor, bordercolor, metanodeVertices, node_labels,
                   fontsize, fontIconName);

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when exporting nodes");
    }

    return false;
  }

  // Analysing meta-nodes
  while (!metanodeVertices.empty()) {
    vector<int> transformationVertices;
    int indice_Transform = 0;
    unsigned sizeFirstVertice = 2 * metanodeVertices.size();
    vector<tlp::node> subMetanodeVertices;

    for (auto metanode : metanodeVertices) {
      Graph *metagraph = graph->getNodeMetaInfo(metanode);
      BoundingBox metagraphbb = tlp::computeBoundingBox(metagraph, layout, sizes, rotation);
      Coord coord_meta_node = layout->getNodeValue(metanode);
      Size size_meta_node = sizes->getNodeValue(metanode);

      // We compute the scale
      float scale = min(size_meta_node.width() / (metagraphbb.width() * 1.4),
                        size_meta_node.height() / (metagraphbb.height() * 1.4));

      if (scale >= 1) {
        scale *= 0.64f;
      }

      // We compute the transformation on X and add him to our vertice
      transformationVertices.push_back(-graphbb.center().getX() + graphbb.width() / 2 -
                                       metagraphbb.center().getX() * (scale - 1) +
                                       coord_meta_node.getX() - metagraphbb.center().getX());
      // We compute the transformation on Y and add him to our vertice
      transformationVertices.push_back(graphbb.center().getY() + graphbb.height() / 2 +
                                       metagraphbb.center().getY() * (scale - 1) -
                                       coord_meta_node.getY() + metagraphbb.center().getY());

      ret = exportint.writeMetaGraph(transformationVertices[indice_Transform],
                                     transformationVertices[indice_Transform + 1], scale);

      if (!ret) {
        if (pp->getError().empty()) {
          pp->setError("Error when exporting a metanode");
        }

        return false;
      }

      indice_Transform += 2;

      // Analysing edges in the metanode
      ret = treatEdges(metagraph, pp, exportint, i, nb_elements, sizes, colors, layout, shape,
                       srcanchorshape, tgtanchorshape, label, labelcolor, edge_color_interpolation,
                       edge_size_interpolation, edge_extremities, metanode_labels, fontsize,
                       fontIconName);

      if (!ret) {
        stringstream str;
        str << pp->getError() << "-- metanode " << metanode;
        pp->setError(str.str());
        return false;
      }

      // Analysing nodes in the metanode
      ret = treatNodes(metagraph, pp, exportint, i, nb_elements, sizes, colors, layout, shape,
                       rotation, borderwidth, label, labelcolor, bordercolor, subMetanodeVertices,
                       metanode_labels, fontsize, fontIconName);

      if (!ret) {
        stringstream str;
        str << pp->getError() << "-- metanode " << metanode;
        pp->setError(str.str());
        return false;
      }

      if (transformationVertices.size() > sizeFirstVertice) {
        cerr << "Metanode in a metanode not working properly" << endl;
      }
    }

    indice_Transform = 0;
    metanodeVertices = subMetanodeVertices;
  }

  // Writing the end of the file
  ret = exportint.writeEnd();

  if (!ret) {
    if (pp->getError().empty()) {
      pp->setError("Error when ending graph export");
    }

    return false;
  }

  return true;
}
