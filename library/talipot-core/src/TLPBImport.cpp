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

#include <fstream>
#include <cerrno>
#include <talipot/TLPBExportImport.h>
#include <talipot/TlpTools.h>
#include <talipot/GraphAbstract.h>
#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/GraphProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>

PLUGIN(TLPBImport)

using namespace tlp;
using namespace std;

static const string TalipotBitmapDirSym = "TalipotBitmapDir/";
static const string TulipBitmapDirSym = "TulipBitmapDir/";

// a template class to iterate into a range of nodes or edges
template <typename T>
struct RangeIterator : public Iterator<T> {
  T first, last, current;

  RangeIterator(T begin, T end) : Iterator<T>(), first(begin), last(end), current(begin) {
    assert(begin.id <= last.id);
  }
  ~RangeIterator() override {}
  T next() override {
    T tmp = current;
    ++(current.id);
    return tmp;
  }

  bool hasNext() override {
    return current.id <= last.id;
  }
};

//================================================================================
TLPBImport::TLPBImport(tlp::PluginContext *context) : ImportModule(context) {
  addInParameter<std::string>("file::filename", "The pathname of the TLPB file to import.", "");
}
//================================================================================
bool TLPBImport::importGraph() {
  auto inputData = getInputData();

  if (!inputData.valid()) {
    return false;
  }

  pluginProgress->showPreview(false);
  pluginProgress->setComment(std::string("Loading ") + inputData.filename + "...");

  // header
  TLPBHeader header;

  // read header
  if (!bool(inputData.is->read(reinterpret_cast<char *>(&header), sizeof(header)))) {
    return false;
  }

  if (!header.checkCompatibility()) {
    pluginProgress->setError("file is not in TLPB format.");
    tlp::error() << pluginProgress->getError() << std::endl;
    return false;
  }

  // add nodes
  graph->addNodes(header.numNodes);

  // loop to read edges
  {
    // we can use a buffer to limit the disk reads
    std::vector<std::pair<node, node>> vEdges(MAX_EDGES_TO_READ);
    unsigned int nbEdges = header.numEdges;
    pluginProgress->setComment(inputData.filename + ": reading edges...");

    while (nbEdges) {
      unsigned int edgesToRead = nbEdges > MAX_EDGES_TO_READ ? MAX_EDGES_TO_READ : nbEdges;
      vEdges.resize(edgesToRead);

      // read a bunch of edges
      if (!bool(inputData.is->read(reinterpret_cast<char *>(vEdges.data()),
                                   edgesToRead * sizeof(vEdges[0])))) {
        return false;
      }

      if (pluginProgress->progress(header.numEdges - nbEdges, header.numEdges) != TLP_CONTINUE) {
        return pluginProgress->state() != TLP_CANCEL;
      }

      // add edges in the graph
      graph->addEdges(vEdges);
      // decrement nbEdges
      nbEdges -= edgesToRead;
    }
  }
  // read subgraphs
  unsigned int numSubGraphs = 0;
  MutableContainer<Graph *> subgraphs;
  subgraphs.set(0, graph);
  {
    // read the number of subgraphs
    if (!bool(inputData.is->read(reinterpret_cast<char *>(&numSubGraphs), sizeof(numSubGraphs)))) {
      return false;
    }

    // read loop for subgraphs
    pluginProgress->setComment(inputData.filename + ": reading subgraphs...");

    for (unsigned int i = 0; i < numSubGraphs; ++i) {
      std::pair<unsigned int, unsigned int> ids;

      // read subgraph id and parent id
      if (!bool(inputData.is->read(reinterpret_cast<char *>(&ids), sizeof(ids)))) {
        return false;
      }

      const auto &[sgId, parentId] = ids;

      // add subgraph
      Graph *parent = subgraphs.get(parentId);
      Graph *sg = static_cast<GraphAbstract *>(parent)->addSubGraph(sgId);
      // record sg
      subgraphs.set(sgId, sg);
      // read sg nodes ranges
      {
        unsigned int numRanges = 0;

        // read the number of nodes ranges
        if (!bool(inputData.is->read(reinterpret_cast<char *>(&numRanges), sizeof(numRanges)))) {
          return false;
        }

        // we can use a buffer to limit the disk reads
        std::vector<std::pair<node, node>> vRanges(MAX_RANGES_TO_READ);

        // loop to read ranges
        while (numRanges) {
          unsigned int rangesToRead =
              numRanges > MAX_RANGES_TO_READ ? MAX_RANGES_TO_READ : numRanges;
          vRanges.resize(rangesToRead);

          // read a bunch of ranges
          if (!bool(inputData.is->read(reinterpret_cast<char *>(vRanges.data()),
                                       rangesToRead * sizeof(vRanges[0])))) {
            return false;
          }

          // loop to add nodes
          for (unsigned int i = 0; i < rangesToRead; ++i) {
            const auto &[n1, n2] = vRanges[i];
            sg->addNodes(new RangeIterator<node>(n1, n2));
          }

          numRanges -= rangesToRead;
        }
      }
      // read sg edges ranges
      {
        unsigned int numRanges = 0;

        // read the number of edges ranges
        if (!bool(inputData.is->read(reinterpret_cast<char *>(&numRanges), sizeof(numRanges)))) {
          return false;
        }

        // loop to read ranges
        std::vector<std::pair<edge, edge>> vRanges(MAX_RANGES_TO_READ);

        while (numRanges) {
          unsigned int rangesToRead =
              numRanges > MAX_RANGES_TO_READ ? MAX_RANGES_TO_READ : numRanges;
          vRanges.resize(rangesToRead);

          // read a bunch of ranges
          if (!bool(inputData.is->read(reinterpret_cast<char *>(vRanges.data()),
                                       rangesToRead * sizeof(vRanges[0])))) {
            return false;
          }

          // loop to add edges
          for (unsigned int i = 0; i < rangesToRead; ++i) {
            const auto &[e1, e2] = vRanges[i];
            sg->addEdges(new RangeIterator<edge>(e1, e2));
          }

          numRanges -= rangesToRead;
        }
      }

      if (pluginProgress->progress(i + 1, numSubGraphs) != TLP_CONTINUE) {
        return pluginProgress->state() != TLP_CANCEL;
      }
    }
  }
  // read properties
  {
    unsigned int numProperties = 0;

    // read number of properties
    if (!bool(
            inputData.is->read(reinterpret_cast<char *>(&numProperties), sizeof(numProperties)))) {
      return false;
    }

    // read loop on properties
    pluginProgress->setComment(inputData.filename + ": reading properties...");

    for (unsigned int i = 0; i < numProperties; ++i) {
      unsigned int size = 0;

      // read name length
      if (!bool(inputData.is->read(reinterpret_cast<char *>(&size), sizeof(size)))) {
        return false;
      }

      // + 1 to ensure a null terminated string
      std::string propName(size + 1, 0);

      // read name
      if (!inputData.is->read(const_cast<char *>(propName.data()), size)) {
        return false;
      }

      propName.resize(size);

      // special treament for pathnames view properties
      bool pnViewProp = (propName == string("viewFont") || propName == string("viewTexture"));

      // read graph id
      if (!bool(inputData.is->read(reinterpret_cast<char *>(&size), sizeof(size)))) {
        return false;
      }

      // get corresponding graph
      Graph *g = subgraphs.get(size);
      assert(g);

      if (g == nullptr) {
        return false;
      }

      // read type length
      if (!bool(inputData.is->read(reinterpret_cast<char *>(&size), sizeof(size)))) {
        return false;
      }

      // + 1 to ensure a null terminated string
      std::string propType(size + 1, 0);

      // read type
      if (!inputData.is->read(const_cast<char *>(propType.data()), size)) {
        return false;
      }

      propType.resize(size);
      PropertyInterface *prop = nullptr;

      // create property
      if (propType == GraphProperty::propertyTypename) {
        prop = g->getLocalGraphProperty(propName);
      } else if (propType == DoubleProperty::propertyTypename) {
        prop = g->getLocalDoubleProperty(propName);
      } else if (propType == LayoutProperty::propertyTypename) {
        prop = g->getLocalLayoutProperty(propName);
      } else if (propType == SizeProperty::propertyTypename) {
        prop = g->getLocalSizeProperty(propName);
      } else if (propType == ColorProperty::propertyTypename) {
        prop = g->getLocalColorProperty(propName);
      } else if (propType == IntegerProperty::propertyTypename) {
        prop = g->getLocalIntegerProperty(propName);
      } else if (propType == BooleanProperty::propertyTypename) {
        prop = g->getLocalBooleanProperty(propName);
      } else if (propType == StringProperty::propertyTypename) {
        prop = g->getLocalStringProperty(propName);
      } else if (propType == SizeVectorProperty::propertyTypename) {
        prop = g->getLocalSizeVectorProperty(propName);
      } else if (propType == ColorVectorProperty::propertyTypename) {
        prop = g->getLocalColorVectorProperty(propName);
      } else if (propType == CoordVectorProperty::propertyTypename) {
        prop = g->getLocalCoordVectorProperty(propName);
      } else if (propType == DoubleVectorProperty::propertyTypename) {
        prop = g->getLocalDoubleVectorProperty(propName);
      } else if (propType == IntegerVectorProperty::propertyTypename) {
        prop = g->getLocalIntegerVectorProperty(propName);
      } else if (propType == BooleanVectorProperty::propertyTypename) {
        prop = g->getLocalBooleanVectorProperty(propName);
      } else if (propType == StringVectorProperty::propertyTypename) {
        prop = g->getLocalStringVectorProperty(propName);
      }

      assert(prop);

      if (prop == nullptr) {
        return false;
      }

      if (pnViewProp) {
        std::string value;
        StringType::readb(*inputData.is, value);
        // if needed replace symbolic path by real path
        size_t pos = value.find(TalipotBitmapDirSym);

        if (pos != std::string::npos) {
          value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
        }

        pos = value.find(TulipBitmapDirSym);

        if (pos != std::string::npos) {
          value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
        }

        static_cast<StringProperty *>(prop)->setAllNodeValue(value);

        StringType::readb(*inputData.is, value);
        // if needed replace symbolic path by real path
        pos = value.find(TalipotBitmapDirSym);

        if (pos != std::string::npos) {
          value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
        }

        pos = value.find(TulipBitmapDirSym);

        if (pos != std::string::npos) {
          value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
        }

        static_cast<StringProperty *>(prop)->setAllEdgeValue(value);
      } else {
        // read and set property node default value
        if (!prop->readNodeDefaultValue(*inputData.is)) {
          return false;
        }

        // read and set property edge default value
        if (!prop->readEdgeDefaultValue(*inputData.is)) {
          return false;
        }
      }

      // nodes / edges values
      {
        unsigned int numValues = 0;

        // read the number of nodes values
        if (!bool(inputData.is->read(reinterpret_cast<char *>(&numValues), sizeof(numValues)))) {
          return false;
        }

        // std::basic_streambuf::pubsetbuf is a no-op in libcxx (LLVM implementation of STL)
        // see https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf#L150
        // and https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf#L360
        // and also in STL implementation of Microsoft Visual C++
        // so fallback writing directly to the output stream in these cases
#if defined(_LIBCPP_VERSION) || defined(_MSC_VER)
        bool canUsePubSetBuf = false;
#else
        bool canUsePubSetBuf = true;
#endif

        // loop on nodes values
        size = prop->nodeValueSize();

        // for backward compatibility with TLPB format <= 1.1, (see sourceforge commit #11536)
        if (header.major == 1 && header.minor <= 1 && propType == GraphProperty::propertyTypename) {
          size = sizeof(tlp::Graph *);
        }

        if (size && canUsePubSetBuf) {
          // as the size of any value is fixed
          // we can use a buffer to limit the number of disk reads
          unique_ptr<char[]> vBuf;

          if (numValues < MAX_VALUES_TO_READ) {
            vBuf.reset(new char[numValues * (sizeof(unsigned int) + size)]);
          } else {
            vBuf.reset(new char[MAX_VALUES_TO_READ * (sizeof(unsigned int) + size)]);
          }

          while (numValues) {
            // read a bunch of <node, prop_value>
            unsigned int valuesToRead =
                (numValues > MAX_VALUES_TO_READ) ? MAX_VALUES_TO_READ : numValues;

            if (!inputData.is->read(reinterpret_cast<char *>(vBuf.get()),
                                    valuesToRead * (sizeof(unsigned int) + size))) {
              return false;
            }

            // use a stringstream to read nodes and properties
            stringstream vs;
            // set read buffer of stringstream to vBuf
            vs.rdbuf()->pubsetbuf(reinterpret_cast<char *>(vBuf.get()),
                                  valuesToRead * (sizeof(unsigned int) + size));

            for (unsigned int i = 0; i < valuesToRead; ++i) {
              node n;

              // read node id
              if (!bool(vs.read(reinterpret_cast<char *>(&n.id), sizeof(unsigned int)))) {
                return false;
              }

              // read and set node value
              assert(g->isElement(n));

              if (!prop->readNodeValue(vs, n)) {
                return false;
              }
            }

            numValues -= valuesToRead;
          }
        } else {
          // we cannot predict the size of property values
          // so the loop is simpler but with more disk reads
          for (unsigned int i = 0; i < numValues; ++i) {
            node n;

            // read node id
            if (!bool(
                    inputData.is->read(reinterpret_cast<char *>(&(n.id)), sizeof(unsigned int)))) {
              return false;
            }

            assert(g->isElement(n));

            if (pnViewProp) {
              std::string value;

              // must ensure ascendant compatibility
              // after clang bug fix in commit #11142
              if (header.major == 1 && header.minor == 0) {
                StringType::read(*inputData.is, value);
              } else {
                StringType::readb(*inputData.is, value);
              }

              // if needed replace symbolic path by real path
              size_t pos = value.find(TalipotBitmapDirSym);

              if (pos != std::string::npos) {
                value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
              }

              pos = value.find(TulipBitmapDirSym);

              if (pos != std::string::npos) {
                value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
              }

              static_cast<StringProperty *>(prop)->setNodeValue(n, value);
            } else {
              // read and set node value
              if (!prop->readNodeValue(*inputData.is, n)) {
                return false;
              }
            }
          }
        }

        // read the number of edges values
        if (!bool(inputData.is->read(reinterpret_cast<char *>(&numValues), sizeof(numValues)))) {
          return false;
        }

        // loop on edges values
        size = prop->edgeValueSize();

        if (size && canUsePubSetBuf) {
          // as the size of any value is fixed
          // we can use a buffer to limit the number of disk reads
          unique_ptr<char[]> vBuf;

          if (numValues < MAX_VALUES_TO_READ) {
            vBuf.reset(new char[numValues * (sizeof(unsigned int) + size)]);
          } else {
            vBuf.reset(new char[MAX_VALUES_TO_READ * (sizeof(unsigned int) + size)]);
          }

          while (numValues) {
            // read a bunch of <edge, prop_value> in vBuf
            unsigned int valuesToRead =
                (numValues > MAX_VALUES_TO_READ) ? MAX_VALUES_TO_READ : numValues;

            if (!inputData.is->read(reinterpret_cast<char *>(vBuf.get()),
                                    valuesToRead * (sizeof(unsigned int) + size))) {
              return false;
            }

            // use a stringstream to read edges and properties
            stringstream vs;
            // set read buffer of stringstream to vBuf
            vs.rdbuf()->pubsetbuf(vBuf.get(), valuesToRead * (sizeof(unsigned int) + size));

            for (unsigned int i = 0; i < valuesToRead; ++i) {
              edge e;

              // read edge id
              if (!bool(vs.read(reinterpret_cast<char *>(&e.id), sizeof(unsigned int)))) {
                return false;
              }

              assert(g->isElement(e));

              if (pnViewProp) {
                std::string value;

                // must ensure ascendant compatibility
                // after clang bug fix in commit #11142
                if (header.major == 1 && header.minor == 0) {
                  StringType::read(*inputData.is, value);
                } else {
                  StringType::readb(*inputData.is, value);
                }

                // if needed replace symbolic path by real path
                size_t pos = value.find(TalipotBitmapDirSym);

                if (pos != std::string::npos) {
                  value.replace(pos, TalipotBitmapDirSym.size(), TalipotBitmapDir);
                }

                pos = value.find(TulipBitmapDirSym);

                if (pos != std::string::npos) {
                  value.replace(pos, TulipBitmapDirSym.size(), TalipotBitmapDir);
                }

                static_cast<StringProperty *>(prop)->setEdgeValue(e, value);
              } else

                  // read and set edge value
                  if (!prop->readEdgeValue(vs, e)) {
                return false;
              }
            }

            numValues -= valuesToRead;
          }
        } else {
          // we cannot predict the size of property values
          // so the loop is simpler but with more disk reads
          for (unsigned int i = 0; i < numValues; ++i) {
            edge e;

            // read edge id
            if (!bool(inputData.is->read(reinterpret_cast<char *>(&e.id), sizeof(unsigned int)))) {
              return false;
            }

            // read and set edge value
            assert(g->isElement(e));

            if (!prop->readEdgeValue(*inputData.is, e)) {
              return false;
            }
          }
        }
      }

      if (pluginProgress->progress(i + 1, numProperties) != TLP_CONTINUE) {
        return pluginProgress->state() != TLP_CANCEL;
      }
    }
  }
  // read graphs (root graph + subgraphs) attributes
  pluginProgress->setComment(inputData.filename + ": reading attributes of graphs...");

  for (unsigned int i = 0; i < numSubGraphs + 1; ++i) {
    unsigned int id = 0;

    // read graph id
    if (!bool(inputData.is->read(reinterpret_cast<char *>(&id), sizeof(id)))) {
      return false;
    }

    Graph *g = id ? subgraphs.get(id) : graph;
    assert(g);

    if (g == nullptr) {
      return false;
    }

    // read graph attributes
    DataSet::read(*inputData.is, const_cast<DataSet &>(g->getAttributes()));
    // do not forget to read the end marker
    char c = '\0';
    inputData.is->get(c);
    assert(c == ')');

    if (c != ')') {
      return false;
    }

    if (pluginProgress->progress(i + 1, numSubGraphs + 1) != TLP_CONTINUE) {
      return pluginProgress->state() != TLP_CANCEL;
    }
  }

  return true;
}
