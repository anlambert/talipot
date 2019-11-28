/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_BOOLEAN_PROPERTY_H
#define TALIPOT_BOOLEAN_PROPERTY_H

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/PropertyAlgorithm.h>

namespace tlp {

class PropertyContext;

/**
 * @ingroup Graph
 * @brief A graph property that maps a boolean value to graph elements.
 */
class TLP_SCOPE BooleanProperty : public AbstractProperty<tlp::BooleanType, tlp::BooleanType> {
public:
  BooleanProperty(Graph *g, const std::string &n = "")
      : AbstractProperty<BooleanType, BooleanType>(g, n) {}
  // PropertyInterface inherited methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }

  /**
   * Reverses all values associated to graph elements,
   * i.e true => false, false => true.
   * If sg is nullptr, the graph given when creating the property is considered.
   */
  void reverse(const Graph *sg = nullptr);

  /**
   * Reverses all the direction of edges of the visible graph
   * which are true in this BooleanProperty.
   * * If sg is nullptr, the graph given when creating the property is considered.
   */
  void reverseEdgeDirection(Graph *sg = nullptr);
};

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<bool> value to graph elements.
 */
class TLP_SCOPE BooleanVectorProperty
    : public AbstractVectorProperty<tlp::BooleanVectorType, tlp::BooleanType> {
public:
  BooleanVectorProperty(Graph *g, const std::string &n = "")
      : AbstractVectorProperty<BooleanVectorType, tlp::BooleanType>(g, n) {}
  // PropertyInterface inherited methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
};
#ifdef _MSC_VER
template class AbstractVectorProperty<BooleanVectorType, BooleanType>;
#endif
}

#endif // TALIPOT_BOOLEAN_PROPERTY_H
