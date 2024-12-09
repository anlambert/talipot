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

#ifndef TALIPOT_SIZE_PROPERTY_H
#define TALIPOT_SIZE_PROPERTY_H

#include <talipot/hash.h>

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class PropertyContext;

typedef AbstractProperty<SizeType, SizeType> AbstractSizeProperty;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractProperty<SizeType, SizeType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a Size value to graph elements.
 */
class TLP_SCOPE SizeProperty : public AbstractSizeProperty {

public:
  SizeProperty(Graph *, const std::string &n = "");

  Size getMax(const Graph *sg = nullptr);
  Size getMin(const Graph *sg = nullptr);
  void scale(const Vec3f &, const Graph *sg = nullptr);
  void scale(const Vec3f &, Iterator<node> *, Iterator<edge> *);

  // redefinition of some PropertyInterface methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }

  // redefinition of some AbstractProperty methods
  void setNodeValue(const node n, StoredType<Size>::ConstReference v) override;
  void setAllNodeValue(StoredType<Size>::ConstReference v, const Graph *graph = nullptr) override;

  int compare(const node n1, const node n2) const override;

protected:
  void resetMinMax();

private:
  flat_hash_map<uint, Size> max, min;
  flat_hash_map<uint, bool> minMaxOk;
  void computeMinMax(const Graph *sg = nullptr);
};

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<SizeVectorType, SizeVectorType, VectorPropertyInterface>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractVectorProperty<SizeVectorType, SizeType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<Size> value to graph elements.
 */
class TLP_SCOPE SizeVectorProperty : public AbstractVectorProperty<SizeVectorType, SizeType> {
public:
  SizeVectorProperty(Graph *g, const std::string &n = "")
      : AbstractVectorProperty<SizeVectorType, SizeType>(g, n) {}

  // redefinition of some PropertyInterface methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
};
}
#endif // TALIPOT_SIZE_PROPERTY_H
