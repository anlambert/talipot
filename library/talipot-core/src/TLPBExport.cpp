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

#include <talipot/TLPBExportImport.h>
#include <talipot/GraphProperty.h>

PLUGIN(TLPBExport)

using namespace tlp;
using namespace std;

//================================================================================
void TLPBExport::getSubGraphs(Graph *g, vector<Graph *> &vsg) {
  // get subgraphs in a vector
  for (Graph *sg : g->subGraphs()) {
    vsg.push_back(sg);
    getSubGraphs(sg, vsg);
  }
}
//================================================================================
void TLPBExport::writeAttributes(ostream &os, Graph *g) {
  const DataSet &attributes = g->getAttributes();

  if (!attributes.empty()) {
    // If nodes and edges are stored as graph attributes
    // we need to update their ids before serializing them
    // as nodes and edges have been reindexed

    for (const auto &[key, value] : attributes.getValues()) {
      if (value->getTypeName() == string(typeid(node).name())) {
        node *n = static_cast<node *>(value->value);
        n->id = getNode(*n).id;
      } else if (value->getTypeName() == string(typeid(edge).name())) {
        edge *e = static_cast<edge *>(value->value);
        e->id = getEdge(*e).id;
      } else if (value->getTypeName() == string(typeid(vector<node>).name())) {
        auto *vn = static_cast<vector<node> *>(value->value);

        for (auto &n : *vn) {
          n.id = getNode(n).id;
        }
      } else if (value->getTypeName() == string(typeid(vector<edge>).name())) {
        auto *ve = static_cast<vector<edge> *>(value->value);

        for (auto &e : *ve) {
          e.id = getEdge(e).id;
        }
      }
    }
  }

  uint id = g->getSuperGraph() == g ? 0 : g->getId();
  // write graph id
  os.write(reinterpret_cast<const char *>(&id), sizeof(id));
  // write graph attributes
  DataSet::write(os, attributes);
  // do not forget to write the end marker
  os.put(')');
}
//================================================================================
bool TLPBExport::exportGraph(std::ostream &os) {

  // change graph parent in hierarchy temporarily to itself as
  // it will be the new root of the exported hierarchy
  Graph *superGraph = graph->getSuperGraph();
  graph->setSuperGraph(graph);

  // header
  TLPBHeader header(graph->numberOfNodes(), graph->numberOfEdges());
  // write header
  os.write(reinterpret_cast<const char *>(&header), sizeof(header));
  // loop to write edges
  {
    pluginProgress->setComment("writing edges...");
    // use a vector as buffer
    std::vector<std::pair<node, node>> vEdges(MAX_EDGES_TO_WRITE);
    uint edgesToWrite = 0, nbWrittenEdges = 0;
    for (auto e : graph->edges()) {
      const auto &[src, tgt] = graph->ends(e);
      vEdges[edgesToWrite] = {getNode(src), getNode(tgt)};

      if (++edgesToWrite == MAX_EDGES_TO_WRITE) {
        // write already buffered edges
        os.write(reinterpret_cast<const char *>(vEdges.data()),
                 MAX_EDGES_TO_WRITE * sizeof(vEdges[0]));
        nbWrittenEdges += edgesToWrite;

        if (pluginProgress->progress(nbWrittenEdges, header.numEdges) != TLP_CONTINUE) {
          return pluginProgress->state() != TLP_CANCEL;
        }

        edgesToWrite = 0;
      }
    }

    if (edgesToWrite) {
      // write last buffered edges
      os.write(reinterpret_cast<const char *>(vEdges.data()),
               edgesToWrite * sizeof(std::pair<node, node>));
    }
  }
  // write subgraphs
  std::vector<Graph *> vSubGraphs;
  // get subgraphs in a vector
  getSubGraphs(graph, vSubGraphs);
  uint numSubGraphs = vSubGraphs.size();
  {
    pluginProgress->setComment("writing subgraphs...");
    // write nb subgraphs
    os.write(reinterpret_cast<const char *>(&numSubGraphs), sizeof(numSubGraphs));

    for (uint i = 0; i < numSubGraphs; ++i) {
      Graph *sg = vSubGraphs[i];
      uint parentId = sg->getSuperGraph()->getId();

      if (parentId == graph->getId()) {
        parentId = 0;
      }

      std::pair<uint, uint> ids(sg->getId(), parentId);
      // write ids
      os.write(reinterpret_cast<const char *>(&ids), sizeof(ids));
      // loop to write sg nodes ranges
      {
        // first sort sg nodes
        const std::vector<node> &nodes = sg->nodes();
        uint nbNodes = nodes.size();
        std::vector<node> sgNodes(nbNodes);

        for (uint j = 0; j < nbNodes; ++j) {
          sgNodes[j] = getNode(nodes[j]);
        }

        std::sort(sgNodes.begin(), sgNodes.end());

        // use a vector as buffer
        std::vector<std::vector<std::pair<node, node>>> vRangesVec;
        vRangesVec.push_back(std::vector<std::pair<node, node>>(MAX_RANGES_TO_WRITE));
        std::vector<std::pair<node, node>> &vRanges = vRangesVec.back();

        uint rangesToWrite = 0;
        uint numRanges = 0;

        bool pendingWrite = false;
        node beginNode, lastNode;

        for (uint j = 0; j < nbNodes; ++j) {
          node current = sgNodes[j];
          pendingWrite = true;

          if (!beginNode.isValid()) {
            beginNode = lastNode = current;
          } else {
            if (current.id == lastNode.id + 1) {
              lastNode = current;
            } else {
              vRanges[rangesToWrite++] = std::pair<node, node>(beginNode, lastNode);
              ++numRanges;
              beginNode = lastNode = current;

              if (rangesToWrite == MAX_RANGES_TO_WRITE) {
                vRangesVec.push_back(std::vector<std::pair<node, node>>(MAX_RANGES_TO_WRITE));
                vRanges = vRangesVec.back();
                rangesToWrite = 0;
                pendingWrite = false;
              }
            }
          }
        }

        if (pendingWrite) {
          // insert last range in buffer
          vRanges[rangesToWrite++] = std::pair<node, node>(beginNode, lastNode);
          ++numRanges;
        }

        // mark nb ranges
        os.write(reinterpret_cast<const char *>(&numRanges), sizeof(numRanges));
        // write already buffered ranges
        int numRangesV = numRanges / MAX_RANGES_TO_WRITE;

        for (int j = 0; j < numRangesV; ++j) {
          os.write(reinterpret_cast<const char *>(vRangesVec[j].data()),
                   MAX_RANGES_TO_WRITE * sizeof(vRangesVec[j][0]));
        }

        // write last buffered ranges
        os.write(reinterpret_cast<const char *>(vRanges.data()),
                 rangesToWrite * sizeof(vRanges[0]));
      }
      // loop to write sg edges ranges
      {
        // first sort sg edges
        const std::vector<edge> &edges = sg->edges();
        uint nbEdges = edges.size();
        std::vector<edge> sgEdges(nbEdges);

        for (uint j = 0; j < nbEdges; ++j) {
          sgEdges[j] = getEdge(edges[j]);
        }

        std::sort(sgEdges.begin(), sgEdges.end());

        // use a vector as buffer
        std::vector<std::vector<std::pair<edge, edge>>> vRangesVec;
        vRangesVec.push_back(std::vector<std::pair<edge, edge>>(MAX_RANGES_TO_WRITE));
        std::vector<std::pair<edge, edge>> &vRanges = vRangesVec.back();

        uint rangesToWrite = 0;
        uint numRanges = 0;

        bool pendingWrite = false;
        edge beginEdge, lastEdge;

        for (uint j = 0; j < nbEdges; ++j) {
          edge current = sgEdges[j];
          pendingWrite = true;

          if (!beginEdge.isValid()) {
            beginEdge = lastEdge = current;
          } else {
            if (current.id == lastEdge.id + 1) {
              lastEdge = current;
            } else {
              vRanges[rangesToWrite++] = std::pair<edge, edge>(beginEdge, lastEdge);
              ++numRanges;
              beginEdge = lastEdge = current;

              if (rangesToWrite == MAX_RANGES_TO_WRITE) {
                vRangesVec.push_back(std::vector<std::pair<edge, edge>>(MAX_RANGES_TO_WRITE));
                vRanges = vRangesVec.back();
                rangesToWrite = 0;
                pendingWrite = false;
              }
            }
          }
        }

        if (pendingWrite) {
          // insert last range in buffer
          vRanges[rangesToWrite++] = std::pair<edge, edge>(beginEdge, lastEdge);
          ++numRanges;
        }

        // mark nb ranges
        os.write(reinterpret_cast<const char *>(&numRanges), sizeof(numRanges));
        // write already buffered ranges
        int numRangesV = numRanges / MAX_RANGES_TO_WRITE;

        for (int j = 0; j < numRangesV; ++j) {
          os.write(reinterpret_cast<const char *>(vRangesVec[j].data()),
                   MAX_RANGES_TO_WRITE * sizeof(vRangesVec[j][0]));
        }

        // write last buffered ranges
        os.write(reinterpret_cast<const char *>(vRanges.data()),
                 rangesToWrite * sizeof(vRanges[0]));
      }

      if (pluginProgress->progress(i, numSubGraphs) != TLP_CONTINUE) {
        return pluginProgress->state() != TLP_CANCEL;
      }
    }
  }
  // write properties
  {
    pluginProgress->setComment("writing properties...");
    uint numGraphProperties = 0;
    uint numProperties = 0;
    std::vector<PropertyInterface *> props;
    // get local properties in a vector
    for (PropertyInterface *prop : graph->getObjectProperties()) {
      props.push_back(prop);
      ++numProperties;
      ++numGraphProperties;
    }

    // get subgraphs local properties too
    for (uint i = 0; i < numSubGraphs; ++i) {
      Graph *sg = vSubGraphs[i];
      for (PropertyInterface *prop : sg->getLocalObjectProperties()) {
        props.push_back(prop);
        ++numProperties;
      }
    }

    // write nb properties
    os.write(reinterpret_cast<const char *>(&numProperties), sizeof(numProperties));

    // loop on properties
    for (uint i = 0; i < numProperties; ++i) {
      PropertyInterface *prop = props[i];
      std::string nameOrType = prop->getName();
      uint size = nameOrType.size();
      // write property name
      os.write(reinterpret_cast<const char *>(&size), sizeof(size));
      os.write(reinterpret_cast<const char *>(nameOrType.data()), size);
      // write graph id
      uint propGraphId = prop->getGraph()->getId();

      if (i < numGraphProperties || propGraphId == graph->getId()) {
        propGraphId = 0;
      }

      os.write(reinterpret_cast<const char *>(&propGraphId), sizeof(propGraphId));
      // special treament for pathnames view properties
      bool pnViewProp = (nameOrType == string("viewFont") || nameOrType == string("viewTexture"));
      // write type
      nameOrType = prop->getTypename();
      size = nameOrType.size();
      os.write(reinterpret_cast<const char *>(&size), sizeof(size));
      os.write(reinterpret_cast<const char *>(nameOrType.data()), size);

      if (pnViewProp && !TalipotBitmapDir.empty()) {
        string defVal = prop->getNodeDefaultStringValue();

        if (size_t pos = defVal.find(TalipotBitmapDir); pos != string::npos) {
          defVal.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
        }

        StringType::writeb(os, defVal);

        defVal = prop->getEdgeDefaultStringValue();

        if (size_t pos = defVal.find(TalipotBitmapDir); pos != string::npos) {
          defVal.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
        }

        StringType::writeb(os, defVal);
      } else {
        // write node default value
        prop->writeNodeDefaultValue(os);
        // write edge default value
        prop->writeEdgeDefaultValue(os);
      }

      // write nodes values
      {
        // write nb of non default values
        size = prop->numberOfNonDefaultValuatedNodes(propGraphId ? nullptr : graph);
        os.write(reinterpret_cast<const char *>(&size), sizeof(size));
        // prepare output stream
        stringstream vs;

        // std::basic_streambuf::pubsetbuf is a no-op in libcxx (LLVM implementation of STL)
        // see https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf#L150
        // and https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf#L360
        // and also in STL implementation of Microsoft Visual C++
        // so fallback writing directly to the output stream in these cases
#if defined(_LIBCPP_VERSION) || defined(_MSC_VER)
        bool canUsePubSetBuf = false;
        std::ostream &s = os;
#else
        bool canUsePubSetBuf = true;
        std::ostream &s = vs;
#endif
        char *vBuf = nullptr;
        uint valueSize = prop->nodeValueSize();

        if (valueSize && canUsePubSetBuf) {
          // allocate a special buffer for values
          // this will ease the write of a bunch of values
          vBuf = static_cast<char *>(malloc(MAX_VALUES_TO_WRITE * (sizeof(uint) + valueSize)));
          vs.rdbuf()->pubsetbuf(vBuf, MAX_VALUES_TO_WRITE * (sizeof(uint) + valueSize));
        }

        // loop on nodes
        uint nbValues = 0;
        for (auto n : prop->getNonDefaultValuatedNodes(propGraphId ? nullptr : graph)) {
          size = getNode(n).id;
          s.write(reinterpret_cast<const char *>(&size), sizeof(size));

          if (pnViewProp && !TalipotBitmapDir.empty()) { // viewFont || viewTexture
            string sVal = prop->getNodeStringValue(n);

            if (size_t pos = sVal.find(TalipotBitmapDir); pos != string::npos) {
              sVal.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
            }

            StringType::writeb(s, sVal);
          } else {
            if (propGraphId && // if it is not the real root graph
                prop->getTypename() == GraphProperty::propertyTypename) {
              string tmp = prop->getNodeStringValue(n);
              uint id = strtoul(tmp.c_str(), nullptr, 10);

              // we must check if the pointed subgraph
              // is a descendant of the currently export graph
              if (!graph->getDescendantGraph(id)) {
                uint id = 0;
                UnsignedIntegerType::writeb(s, id);
              }
            } else {
              prop->writeNodeValue(s, n);
            }
          }

          ++nbValues;

          if (nbValues == MAX_VALUES_TO_WRITE && canUsePubSetBuf) {
            // write already buffered values
            if (vBuf) {
              os.write(vBuf, MAX_VALUES_TO_WRITE * (sizeof(uint) + valueSize));
            } else {
              std::string sbuf = vs.str();
              size = uint(vs.tellp());
              // write buffer
              os.write(sbuf.c_str(), size);
            }

            // reset for next write
            vs.seekp(0, ios_base::beg);
            nbValues = 0;
          }
        }

        if (nbValues && canUsePubSetBuf) {
          // write last buffered values
          if (vBuf) {
            os.write(vBuf, nbValues * (sizeof(uint) + valueSize));
          } else {
            std::string sbuf = vs.str();
            size = uint(vs.tellp());
            // write buffer
            os.write(sbuf.c_str(), size);
          }
        }

        if (vBuf) {
          free(vBuf);
        }
      }
      // write edges values
      {
        // write nb of non default values
        size = prop->numberOfNonDefaultValuatedEdges(propGraphId ? nullptr : graph);
        os.write(reinterpret_cast<const char *>(&size), sizeof(size));
        // prepare output stream
        stringstream vs;

        // std::basic_streambuf::pubsetbuf is a no-op in libcxx (LLVM implementation of STL)
        // see https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf#L150
        // and https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf#L360
        // and also in STL implementation of Microsoft Visual C++
        // so fallback writing directly to the output stream in these cases
#if defined(_LIBCPP_VERSION) || defined(_MSC_VER)
        bool canUsePubSetBuf = false;
        ostream &s = os;
#else
        bool canUsePubSetBuf = true;
        ostream &s = vs;
#endif
        char *vBuf = nullptr;
        uint valueSize = prop->edgeValueSize();
        bool isGraphProperty = false;

        if (valueSize && canUsePubSetBuf) {
          // allocate a special buffer for values
          // this will ease the write of a bunch of values
          vBuf = static_cast<char *>(malloc(MAX_VALUES_TO_WRITE * (sizeof(uint) + valueSize)));
          vs.rdbuf()->pubsetbuf(vBuf, MAX_VALUES_TO_WRITE * (sizeof(uint) + valueSize));
        } else {
          if (prop->getTypename() == GraphProperty::propertyTypename) {
            isGraphProperty = true;
          }
        }

        // loop on edges
        uint nbValues = 0;
        for (auto e : prop->getNonDefaultValuatedEdges(propGraphId ? nullptr : graph)) {
          size = getEdge(e).id;
          s.write(reinterpret_cast<const char *>(&size), sizeof(size));

          if (isGraphProperty) {
            // re-index embedded edges
            const set<edge> &edges = (*static_cast<GraphProperty *>(prop))[e];
            set<edge> rEdges;

            for (auto ee : edges) {
              edge rEdge = getEdge(ee);
              // do not export edges that are not elements of the root graph
              if (rEdge.isValid()) {
                rEdges.insert(rEdge);
              }
            }

            // finally save set
            EdgeSetType::writeb(s, rEdges);

          } else {

            if (pnViewProp && !TalipotBitmapDir.empty()) { // viewFont || viewTexture
              string sVal = prop->getEdgeStringValue(e);

              if (size_t pos = sVal.find(TalipotBitmapDir); pos != string::npos) {
                sVal.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
              }

              StringType::writeb(s, sVal);
            } else {
              prop->writeEdgeValue(s, e);
            }
          }

          ++nbValues;

          if (nbValues == MAX_VALUES_TO_WRITE && canUsePubSetBuf) {
            // write already buffered values
            if (vBuf) {
              os.write(vBuf, MAX_VALUES_TO_WRITE * (sizeof(uint) + valueSize));
            } else {
              std::string sbuf = vs.str();
              size = uint(vs.tellp());
              // write buffer
              os.write(sbuf.c_str(), size);
            }

            // reset for next write
            vs.seekp(0, ios_base::beg);
            nbValues = 0;
          }
        }

        if (nbValues && canUsePubSetBuf) {
          // write last buffered values
          if (vBuf) {
            os.write(vBuf, nbValues * (sizeof(uint) + valueSize));
          } else {
            std::string sbuf = vs.str();
            size = uint(vs.tellp());
            // write buffer
            os.write(sbuf.c_str(), size);
          }
        }

        if (vBuf) {
          free(vBuf);
        }
      }

      if (pluginProgress->progress(i, numProperties) != TLP_CONTINUE) {
        return pluginProgress->state() != TLP_CANCEL;
      }
    }
  }
  // write graph and sub graphs attributes
  writeAttributes(os, graph);

  for (uint i = 0; i < numSubGraphs; ++i) {
    writeAttributes(os, vSubGraphs[i]);
  }

  graph->setSuperGraph(superGraph);
  return true;
}
