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

#ifndef TALIPOT_GRAPH_PROPERTY_H
#define TALIPOT_GRAPH_PROPERTY_H

#include <set>

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class PropertyContext;
class GraphAbstract;

typedef AbstractProperty<GraphType, EdgeSetType> AbstractGraphProperty;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractProperty<GraphType, EdgeSetType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a Graph* value to graph elements.
 *
 * @warning This property is mainly used into the meta node engine. Using GraphProperty outside of
 * this system is strongly discouraged since it could lead to unwanted behavior.
 */
class TLP_SCOPE GraphProperty : public AbstractGraphProperty {
  friend class GraphAbstract;

public:
  GraphProperty(Graph *, const std::string &n = "");
  ~GraphProperty() override;
  // override Observable::treatEvent
  void treatEvent(const Event &) override;

  // redefinition of some PropertyInterface methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  bool setNodeStringValue(const node n, const std::string &v) override;
  bool setAllNodeStringValue(const std::string &v, const Graph *graph = nullptr) override;
  bool setEdgeStringValue(const edge e, const std::string &v) override;
  bool setAllEdgeStringValue(const std::string &v, const Graph *graph = nullptr) override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }

  // redefinition of some AbstractProperty methods
  void setNodeValue(const node n, StoredType<GraphType::RealType>::ConstReference g) override;
  void setAllNodeValue(StoredType<GraphType::RealType>::ConstReference g,
                       const Graph *graph = nullptr) override;
  bool readNodeDefaultValue(std::istream &iss) override;
  bool readNodeValue(std::istream &iss, node n) override;
  // GraphType encapsulates a Graph pointer but that is the graph id
  // that gets serialized when using the TLPB format
  uint nodeValueSize() const override {
    return sizeof(uint);
  }
  uint edgeValueSize() const override {
    return 0;
  }

  // for optimizations purpose
  bool hasNonDefaultValue(const node n) const {
    return nodeProperties.hasNonDefaultValue(n.id);
  }
  bool hasNonDefaultValue(const edge e) const {
    return !edgeProperties.get(e.id).empty();
  }

private:
  flat_hash_map<Graph *, std::set<node>> referencedGraph;
  const std::set<edge> &getReferencedEdges(const edge) const;
};
}
#endif // TALIPOT_GRAPH_PROPERTY_H
