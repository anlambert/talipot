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

#ifndef TALIPOT_PROPERTY_VALUE_WRAPPERS_H
#define TALIPOT_PROPERTY_VALUE_WRAPPERS_H

namespace tlp {

template <class NodeType, class EdgeType, class PropType>
class PropertyNodeValueWrapper {

public:
  PropertyNodeValueWrapper(AbstractProperty<NodeType, EdgeType, PropType> *prop, node n)
      : _prop(prop), _n(n) {}

  PropertyNodeValueWrapper &operator=(TYPE_CONST_REFERENCE(NodeType) val) {
    _prop->setNodeValue(_n, val);
    return *this;
  }
  PropertyNodeValueWrapper &operator=(const PropertyNodeValueWrapper &pnvw) {
    _prop->setNodeValue(_n, static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw));
    return *this;
  }
  PropertyNodeValueWrapper &operator=(const char *val) {
    _prop->setNodeStringValue(_n, val);
    return *this;
  }
  operator TYPE_CONST_REFERENCE(NodeType)() const {
    return _prop->getNodeValue(_n);
  }

private:
  AbstractProperty<NodeType, EdgeType, PropType> *_prop;
  node _n;
};

template <class NodeType, class EdgeType, class PropType>
class PropertyEdgeValueWrapper {

public:
  PropertyEdgeValueWrapper(AbstractProperty<NodeType, EdgeType, PropType> *prop, edge e)
      : _prop(prop), _e(e) {}

  PropertyEdgeValueWrapper &operator=(TYPE_CONST_REFERENCE(EdgeType) val) {
    _prop->setEdgeValue(_e, val);
    return *this;
  }
  PropertyEdgeValueWrapper &operator=(const PropertyEdgeValueWrapper &pevw) {
    _prop->setEdgeValue(_e, static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw));
    return *this;
  }
  PropertyEdgeValueWrapper &operator=(const char *val) {
    _prop->setEdgeStringValue(_e, val);
    return *this;
  }
  operator TYPE_CONST_REFERENCE(EdgeType)() const {
    return _prop->getEdgeValue(_e);
  }

private:
  AbstractProperty<NodeType, EdgeType, PropType> *_prop;
  edge _e;
};

template <typename T>
struct is_vector {
  static const bool value = false;
};
template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> {
  static const bool value = true;
};

#define DEFINE_EQ_OPERATOR(OP)                                                                   \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,    \
                          const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw2) { \
    return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw)                                     \
        OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw2);                                   \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,    \
                          const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pnew) {  \
    return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw)                                     \
        OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pnew);                                    \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(TYPE_CONST_REFERENCE(NodeType) val,                                    \
                          const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw) {  \
    return val OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw);                             \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,    \
                          TYPE_CONST_REFERENCE(NodeType) val) {                                  \
    return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw) OP val;                             \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,    \
                          const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw2) { \
    return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw)                                     \
        OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw2);                                   \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,    \
                          const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw) {  \
    return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw)                                     \
        OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw);                                    \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(TYPE_CONST_REFERENCE(EdgeType) val,                                    \
                          const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw) {  \
    return val OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw);                             \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,    \
                          TYPE_CONST_REFERENCE(EdgeType) val) {                                  \
    return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw) OP val;                             \
  }

DEFINE_EQ_OPERATOR(==)
DEFINE_EQ_OPERATOR(!=)

#define DEFINE_CMP_OPERATOR(OP)                                                                  \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,    \
                          const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw2) { \
    if constexpr (!is_vector<NodeType>::value) {                                                 \
      return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw)                                   \
          OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw2);                                 \
    } else {                                                                                     \
      throw std::runtime_error("operator OP not available for type " +                           \
                               demangleClassName<REAL_TYPE(NodeType)>());                        \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,    \
                          const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pnew) {  \
    if constexpr (std::is_same<NodeType, EdgeType>::value && !is_vector<NodeType>::value) {      \
      return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw)                                   \
          OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pnew);                                  \
    } else {                                                                                     \
      if constexpr (is_vector<NodeType>::value) {                                                \
        throw std::runtime_error("operator OP not available for type " +                         \
                                 demangleClassName<REAL_TYPE(NodeType)>());                      \
      } else {                                                                                   \
        throw std::runtime_error("operator OP not available between types " +                    \
                                 demangleClassName<REAL_TYPE(NodeType)>() + " and " +            \
                                 demangleClassName<REAL_TYPE(EdgeType)>());                      \
      }                                                                                          \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(TYPE_CONST_REFERENCE(NodeType) val,                                    \
                          const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw) {  \
    if constexpr (!is_vector<NodeType>::value) {                                                 \
      return val OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw);                           \
    } else {                                                                                     \
      throw std::runtime_error("operator OP not available for type " +                           \
                               demangleClassName<REAL_TYPE(NodeType)>());                        \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,    \
                          TYPE_CONST_REFERENCE(NodeType) val) {                                  \
    if constexpr (!is_vector<NodeType>::value) {                                                 \
      return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw) OP val;                           \
    } else {                                                                                     \
      throw std::runtime_error("operator OP not available for type " +                           \
                               demangleClassName<REAL_TYPE(NodeType)>());                        \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,    \
                          const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw2) { \
    if constexpr (!is_vector<EdgeType>::value) {                                                 \
      return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw)                                   \
          OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw2);                                 \
    } else {                                                                                     \
      throw std::runtime_error("operator OP not available for type " +                           \
                               demangleClassName<REAL_TYPE(EdgeType)>());                        \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,    \
                          const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw) {  \
    if constexpr (std::is_same<NodeType, EdgeType>::value && !is_vector<NodeType>::value) {      \
      return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw)                                   \
          OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw);                                  \
    } else {                                                                                     \
      if constexpr (is_vector<NodeType>::value) {                                                \
        throw std::runtime_error("operator OP not available for type " +                         \
                                 demangleClassName<REAL_TYPE(EdgeType)>());                      \
      } else {                                                                                   \
        throw std::runtime_error("operator OP not available between types " +                    \
                                 demangleClassName<REAL_TYPE(EdgeType)>() + " and " +            \
                                 demangleClassName<REAL_TYPE(NodeType)>());                      \
      }                                                                                          \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(TYPE_CONST_REFERENCE(NodeType) val,                                    \
                          const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw) {  \
    if constexpr (!is_vector<EdgeType>::value) {                                                 \
      return val OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw);                           \
    } else {                                                                                     \
      throw std::runtime_error("operator OP not available for type " +                           \
                               demangleClassName<REAL_TYPE(EdgeType)>());                        \
    }                                                                                            \
  }                                                                                              \
                                                                                                 \
  template <class NodeType, class EdgeType, class PropType>                                      \
  inline bool operator OP(const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,    \
                          TYPE_CONST_REFERENCE(NodeType) val) {                                  \
    if constexpr (!is_vector<EdgeType>::value) {                                                 \
      return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw) OP val;                           \
    } else {                                                                                     \
      throw std::runtime_error("operator OP not available for type " +                           \
                               demangleClassName<REAL_TYPE(EdgeType)>());                        \
    }                                                                                            \
  }

DEFINE_CMP_OPERATOR(<)
DEFINE_CMP_OPERATOR(<=)
DEFINE_CMP_OPERATOR(>)
DEFINE_CMP_OPERATOR(>=)

#define DEFINE_ARITHMETIC_OPERATOR(OP)                                                             \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(NodeType) operator OP(                                                          \
      const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,                          \
      const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw2) {                       \
    if constexpr (!is_vector<NodeType>::value &&                                                   \
                  !std::is_same<REAL_TYPE(NodeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(NodeType), bool>::value) {                               \
      return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw)                                     \
          OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw2);                                   \
    } else {                                                                                       \
      throw std::runtime_error("operator OP in PropertyNodeValueWrapper not available for type " + \
                               demangleClassName<REAL_TYPE(NodeType)>());                          \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(NodeType) operator OP(                                                          \
      const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,                          \
      const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pnew) {                        \
    if constexpr (std::is_same<NodeType, EdgeType>::value && !is_vector<NodeType>::value &&        \
                  !std::is_same<REAL_TYPE(NodeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(NodeType), bool>::value) {                               \
      return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw)                                     \
          OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pnew);                                    \
    } else {                                                                                       \
      if constexpr (is_vector<NodeType>::value) {                                                  \
        throw std::runtime_error(                                                                  \
            "operator OP in PropertyNodeValueWrapper not available for type " +                    \
            demangleClassName<REAL_TYPE(NodeType)>());                                             \
      } else {                                                                                     \
        throw std::runtime_error(                                                                  \
            "operator OP in PropertyNodeValueWrapper not available between types " +               \
            demangleClassName<REAL_TYPE(NodeType)>() + " and " +                                   \
            demangleClassName<REAL_TYPE(EdgeType)>());                                             \
      }                                                                                            \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(NodeType) operator OP(                                                          \
      TYPE_CONST_REFERENCE(NodeType) val,                                                          \
      const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw) {                        \
    if constexpr (!is_vector<NodeType>::value &&                                                   \
                  !std::is_same<REAL_TYPE(NodeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(NodeType), bool>::value) {                               \
      return val OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw);                             \
    } else {                                                                                       \
      throw std::runtime_error("operator OP in PropertyNodeValueWrapper not available for type " + \
                               demangleClassName<REAL_TYPE(NodeType)>());                          \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(NodeType) operator OP(                                                          \
      const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw,                          \
      TYPE_CONST_REFERENCE(NodeType) val) {                                                        \
    if constexpr (!is_vector<NodeType>::value &&                                                   \
                  !std::is_same<REAL_TYPE(NodeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(NodeType), bool>::value) {                               \
      return static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw) OP val;                             \
    } else {                                                                                       \
      throw std::runtime_error("operator OP in PropertyNodeValueWrapper not available for type " + \
                               demangleClassName<REAL_TYPE(NodeType)>());                          \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(EdgeType) operator OP(                                                          \
      const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,                          \
      const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw2) {                       \
    if constexpr (!is_vector<EdgeType>::value &&                                                   \
                  !std::is_same<REAL_TYPE(EdgeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(EdgeType), bool>::value) {                               \
      return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw)                                     \
          OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw2);                                   \
    } else {                                                                                       \
      throw std::runtime_error("operator OP in PropertyEdgeValueWrapper not available for type " + \
                               demangleClassName<REAL_TYPE(EdgeType)>());                          \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(EdgeType) operator OP(                                                          \
      const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,                          \
      const PropertyNodeValueWrapper<NodeType, EdgeType, PropType> &pnvw) {                        \
    if constexpr (std::is_same<NodeType, EdgeType>::value && !is_vector<NodeType>::value &&        \
                  !std::is_same<REAL_TYPE(EdgeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(EdgeType), bool>::value) {                               \
      return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw)                                     \
          OP static_cast<TYPE_CONST_REFERENCE(NodeType)>(pnvw);                                    \
    } else {                                                                                       \
      if constexpr (is_vector<NodeType>::value) {                                                  \
        throw std::runtime_error(                                                                  \
            "operator OP in PropertyEdgeValueWrapper not available for type " +                    \
            demangleClassName<REAL_TYPE(EdgeType)>());                                             \
      } else {                                                                                     \
        throw std::runtime_error(                                                                  \
            "operator OP in PropertyEdgeValueWrapper not available between types " +               \
            demangleClassName<REAL_TYPE(EdgeType)>() + " and " +                                   \
            demangleClassName<REAL_TYPE(NodeType)>());                                             \
      }                                                                                            \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(EdgeType) operator OP(                                                          \
      TYPE_CONST_REFERENCE(NodeType) val,                                                          \
      const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw) {                        \
    if constexpr (!is_vector<EdgeType>::value &&                                                   \
                  !std::is_same<REAL_TYPE(EdgeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(EdgeType), bool>::value) {                               \
      return val OP static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw);                             \
    } else {                                                                                       \
      throw std::runtime_error("operator OP in PropertyEdgeValueWrapper not available for type " + \
                               demangleClassName<REAL_TYPE(EdgeType)>());                          \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  template <class NodeType, class EdgeType, class PropType>                                        \
  inline REAL_TYPE(EdgeType) operator OP(                                                          \
      const PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> &pevw,                          \
      TYPE_CONST_REFERENCE(NodeType) val) {                                                        \
    if constexpr (!is_vector<EdgeType>::value &&                                                   \
                  !std::is_same<REAL_TYPE(EdgeType), std::string>::value &&                        \
                  !std::is_same<REAL_TYPE(EdgeType), bool>::value) {                               \
      return static_cast<TYPE_CONST_REFERENCE(EdgeType)>(pevw) OP val;                             \
    } else {                                                                                       \
      throw std::runtime_error("operator OP in PropertyEdgeValueWrapper not available for type " + \
                               demangleClassName<REAL_TYPE(EdgeType)>());                          \
    }                                                                                              \
  }

DEFINE_ARITHMETIC_OPERATOR(+)
DEFINE_ARITHMETIC_OPERATOR(-)
DEFINE_ARITHMETIC_OPERATOR(*)
DEFINE_ARITHMETIC_OPERATOR(/)

}

#endif // TALIPOT_PROPERTY_VALUE_WRAPPERS_H