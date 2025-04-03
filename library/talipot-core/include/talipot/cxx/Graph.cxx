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

#include <talipot/PluginProgress.h>
#include <talipot/PropertyInterface.h>

//================================================================================
template <typename ATTRIBUTETYPE>
bool tlp::Graph::getAttribute(const std::string &name, ATTRIBUTETYPE &value) const {
    return getAttributes().get(name, value);
}
//================================================================================
template <typename ATTRIBUTETYPE>
void tlp::Graph::setAttribute(const std::string &name, const ATTRIBUTETYPE &value) {
    tlp::DataSet &data = getNonConstAttributes();
    notifyBeforeSetAttribute(name);
    data.set(name, value);
    notifyAfterSetAttribute(name);
}
//================================================================================
template <typename PropertyType>
PropertyType *tlp::Graph::getLocalProperty(const std::string &name) {
    if (existLocalProperty(name)) {
        PropertyInterface *prop = getProperty(name);
        assert(dynamic_cast<PropertyType *>(prop) != nullptr);
        return dynamic_cast<PropertyType *>(prop);
    } else {
        auto *prop = new PropertyType(this, name);
        this->addLocalProperty(name, prop);
        return prop;
    }
}
//====================================================================================
template <typename PropertyType>
PropertyType *tlp::Graph::getProperty(const std::string &name) {
    if (existProperty(name)) {
        tlp::PropertyInterface *prop = getProperty(name);
        assert(dynamic_cast<PropertyType *>(prop) != nullptr);
        return dynamic_cast<PropertyType *>(prop);
    } else {
        return getLocalProperty<PropertyType>(name);
    }
}
