/**
 *
 * Copyright (C) 2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PROPERTY_PROXY_H
#define TALIPOT_PROPERTY_PROXY_H

#include <string>
#include <variant>

#include <talipot/config.h>
#include <talipot/Edge.h>
#include <talipot/Node.h>
#include <talipot/PropertyTypes.h>

// Utility classes to allow some syntactic sugar when working with graph properties.
// Basically, the following code snippets are equivalent, the latter being shorter
// to write and less verbose.
//
// graph->getDoubleProperty("metric")->setAllNodeValue(1.0);
// graph->getDoubleProperty("metric")->setNodeValue(n, 2.0);
// graph->getDoubleProperty("metric")->setAllEdgeValue(0.0);
// graph->getDoubleProperty("metric")->setEdgeValue(e, 3.0);
//
// (*graph)["metric"].setAllNodeValue(1.0);
// (*graph)["metric"][n] = 2.0;
// (*graph)["metric"].setAllEdgeValue(0.0);
// (*graph)["metric"][e] = 3.0;

namespace tlp {

class Color;
class Graph;

typedef std::variant<
    REAL_TYPE(BooleanType), REAL_TYPE(ColorType), REAL_TYPE(DoubleType), REAL_TYPE(IntegerType),
    REAL_TYPE(LineType), REAL_TYPE(PointType), REAL_TYPE(SizeType), REAL_TYPE(StringType),
    REAL_TYPE(BooleanVectorType), REAL_TYPE(ColorVectorType), REAL_TYPE(DoubleVectorType),
    REAL_TYPE(IntegerVectorType), REAL_TYPE(SizeVectorType), REAL_TYPE(StringVectorType)>

    WrappedValue;

#define DECLARE_PVW_METHODS(PROP_TYPE)                                  \
  PropertyValueWrapper &operator=(TYPE_CONST_REFERENCE(PROP_TYPE) val); \
  operator TYPE_CONST_REFERENCE(PROP_TYPE)() const;

class TLP_SCOPE PropertyValueWrapper {

public:
  PropertyValueWrapper() = default;
  PropertyValueWrapper(Graph *graph, const std::string &propertyName, node n, edge e)
      : _graph(graph), _propertyName(propertyName), _n(n), _e(e){};
  ~PropertyValueWrapper() = default;

  DECLARE_PVW_METHODS(BooleanType)
  DECLARE_PVW_METHODS(ColorType)
  DECLARE_PVW_METHODS(DoubleType)
  DECLARE_PVW_METHODS(IntegerType)
  DECLARE_PVW_METHODS(LineType)
  DECLARE_PVW_METHODS(PointType)
  DECLARE_PVW_METHODS(SizeType)
  DECLARE_PVW_METHODS(StringType)
  DECLARE_PVW_METHODS(BooleanVectorType)
  DECLARE_PVW_METHODS(ColorVectorType)
  DECLARE_PVW_METHODS(DoubleVectorType)
  DECLARE_PVW_METHODS(IntegerVectorType)
  DECLARE_PVW_METHODS(SizeVectorType)
  DECLARE_PVW_METHODS(StringVectorType)
  PropertyValueWrapper &operator=(const char *val);

  PropertyValueWrapper &operator=(const PropertyValueWrapper &pvw);

  bool operator==(const PropertyValueWrapper &pvw) const;
  bool operator!=(const PropertyValueWrapper &pvw) const;
  bool operator<(const PropertyValueWrapper &pvw) const;
  bool operator<=(const PropertyValueWrapper &pvw) const;
  bool operator>(const PropertyValueWrapper &pvw) const;
  bool operator>=(const PropertyValueWrapper &pvw) const;

private:
  WrappedValue getValue() const;

  Graph *_graph;
  std::string _propertyName;
  node _n;
  edge _e;
};

#define DECLARE_PP_NODE_METHODS(PROP_TYPE)                                                     \
  Iterator<node> *getNodesEqualTo(TYPE_CONST_REFERENCE(PROP_TYPE) v, const Graph *g = nullptr) \
      const;                                                                                   \
  void setAllNodeValue(TYPE_CONST_REFERENCE(PROP_TYPE) v, const Graph *graph = nullptr);       \
  void setNodeDefaultValue(TYPE_CONST_REFERENCE(PROP_TYPE) v);

#define DECLARE_PP_EDGE_METHODS(PROP_TYPE)                                                     \
  Iterator<edge> *getEdgesEqualTo(TYPE_CONST_REFERENCE(PROP_TYPE) v, const Graph *g = nullptr) \
      const;                                                                                   \
  void setAllEdgeValue(TYPE_CONST_REFERENCE(PROP_TYPE) v, const Graph *graph = nullptr);       \
  void setEdgeDefaultValue(TYPE_CONST_REFERENCE(PROP_TYPE) v);

#define DECLARE_PP_METHODS(PROP_TYPE) \
  DECLARE_PP_NODE_METHODS(PROP_TYPE)  \
  DECLARE_PP_EDGE_METHODS(PROP_TYPE)

class TLP_SCOPE PropertyProxy {
public:
  PropertyProxy(Graph *graph, const std::string &propertyName)
      : _graph(graph), _propertyName(propertyName){};
  ~PropertyProxy() = default;

  DECLARE_PP_METHODS(BooleanType)
  DECLARE_PP_METHODS(ColorType)
  DECLARE_PP_METHODS(DoubleType)
  DECLARE_PP_METHODS(IntegerType)
  DECLARE_PP_METHODS(LineType)
  DECLARE_PP_NODE_METHODS(PointType)
  DECLARE_PP_METHODS(SizeType)
  DECLARE_PP_METHODS(StringType)
  DECLARE_PP_METHODS(BooleanVectorType)
  DECLARE_PP_METHODS(ColorVectorType)
  DECLARE_PP_METHODS(DoubleVectorType)
  DECLARE_PP_METHODS(IntegerVectorType)
  DECLARE_PP_METHODS(SizeVectorType)
  DECLARE_PP_METHODS(StringVectorType)
  Iterator<node> *getNodesEqualTo(const char *v, const Graph *g = nullptr) const;
  Iterator<edge> *getEdgesEqualTo(const char *v, const Graph *g = nullptr) const;
  void setAllNodeValue(const char *v, const Graph *graph = nullptr);
  void setAllEdgeValue(const char *v, const Graph *graph = nullptr);
  void setNodeDefaultValue(const char *v);
  void setEdgeDefaultValue(const char *v);

  Iterator<node> *getNonDefaultValuatedNodes(const Graph *g = nullptr) const;
  bool hasNonDefaultValuatedNodes(const Graph *g = nullptr) const;
  unsigned int numberOfNonDefaultValuatedNodes(const Graph *g = nullptr) const;
  Iterator<edge> *getNonDefaultValuatedEdges(const Graph *g = nullptr) const;
  bool hasNonDefaultValuatedEdges(const Graph *g = nullptr) const;
  unsigned int numberOfNonDefaultValuatedEdges(const Graph * = nullptr) const;

  PropertyValueWrapper operator[](node n) const {
    return PropertyValueWrapper(_graph, _propertyName, n, edge());
  }

  PropertyValueWrapper operator[](edge e) const {
    return PropertyValueWrapper(_graph, _propertyName, node(), e);
  }

private:
  Graph *_graph;
  std::string _propertyName;
};

#define DECLARE_EQUAL_OPERATORS(TYPE)                                                       \
  inline bool operator==(TYPE_CONST_REFERENCE(TYPE) val, const PropertyValueWrapper &pvw) { \
    return val == static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw);                             \
  }                                                                                         \
  inline bool operator==(const PropertyValueWrapper &pvw, TYPE_CONST_REFERENCE(TYPE) val) { \
    return val == pvw;                                                                      \
  }

#define DECLARE_COMP_OPERATORS(TYPE)                                                        \
  inline bool operator<(TYPE_CONST_REFERENCE(TYPE) val, const PropertyValueWrapper &pvw) {  \
    return val < static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw);                              \
  }                                                                                         \
  inline bool operator<(const PropertyValueWrapper &pvw, TYPE_CONST_REFERENCE(TYPE) val) {  \
    return static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw) < val;                              \
  }                                                                                         \
  inline bool operator<=(TYPE_CONST_REFERENCE(TYPE) val, const PropertyValueWrapper &pvw) { \
    return val <= static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw);                             \
  }                                                                                         \
  inline bool operator<=(const PropertyValueWrapper &pvw, TYPE_CONST_REFERENCE(TYPE) val) { \
    return static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw) <= val;                             \
  }                                                                                         \
  inline bool operator>(TYPE_CONST_REFERENCE(TYPE) val, const PropertyValueWrapper &pvw) {  \
    return val > static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw);                              \
  }                                                                                         \
  inline bool operator>(const PropertyValueWrapper &pvw, TYPE_CONST_REFERENCE(TYPE) val) {  \
    return static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw) > val;                              \
  }                                                                                         \
  inline bool operator>=(TYPE_CONST_REFERENCE(TYPE) val, const PropertyValueWrapper &pvw) { \
    return val >= static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw);                             \
  }                                                                                         \
  inline bool operator>=(const PropertyValueWrapper &pvw, TYPE_CONST_REFERENCE(TYPE) val) { \
    return static_cast<TYPE_CONST_REFERENCE(TYPE)>(pvw) >= val;                             \
  }

DECLARE_EQUAL_OPERATORS(BooleanType)
DECLARE_EQUAL_OPERATORS(BooleanVectorType)
DECLARE_EQUAL_OPERATORS(ColorType)
DECLARE_EQUAL_OPERATORS(ColorVectorType)
DECLARE_EQUAL_OPERATORS(DoubleType)
DECLARE_EQUAL_OPERATORS(DoubleVectorType)
DECLARE_EQUAL_OPERATORS(IntegerType)
DECLARE_EQUAL_OPERATORS(IntegerVectorType)
DECLARE_EQUAL_OPERATORS(LineType)
DECLARE_EQUAL_OPERATORS(PointType)
DECLARE_EQUAL_OPERATORS(SizeType)
DECLARE_EQUAL_OPERATORS(SizeVectorType)
DECLARE_EQUAL_OPERATORS(StringType)
DECLARE_EQUAL_OPERATORS(StringVectorType)

DECLARE_COMP_OPERATORS(BooleanType)
DECLARE_COMP_OPERATORS(ColorType)
DECLARE_COMP_OPERATORS(DoubleType)
DECLARE_COMP_OPERATORS(IntegerType)
DECLARE_COMP_OPERATORS(PointType)
DECLARE_COMP_OPERATORS(SizeType)
DECLARE_COMP_OPERATORS(StringType)

}

#endif // TALIPOT_PROPERTY_PROXY_H
