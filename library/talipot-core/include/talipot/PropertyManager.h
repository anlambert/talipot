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

#ifndef TALIPOT_PROPERTY_MANAGER_H
#define TALIPOT_PROPERTY_MANAGER_H

#include <map>
#include <string>

namespace tlp {

template <class itType>
struct Iterator;

class PropertyInterface;
class Graph;

class PropertyManager {

  private:
    std::map<std::string, PropertyInterface *> localProperties;
    std::map<std::string, PropertyInterface *> inheritedProperties;

  public:
    Graph *graph;
    explicit PropertyManager(Graph *);
    ~PropertyManager();
    //======================================================================================
    bool existProperty(const std::string &) const;
    bool existLocalProperty(const std::string &) const;
    bool existInheritedProperty(const std::string &) const;
    void setLocalProperty(const std::string &, PropertyInterface *);
    bool renameLocalProperty(PropertyInterface *, const std::string &);
    PropertyInterface *getProperty(const std::string &) const;
    PropertyInterface *getLocalProperty(const std::string &) const;
    PropertyInterface *getInheritedProperty(const std::string &) const;
    void delLocalProperty(const std::string &);
    void notifyBeforeDelInheritedProperty(const std::string &);
    void erase(const node);
    void erase(const edge);
    //======================================================================================
    Iterator<std::string> *getLocalProperties();
    Iterator<std::string> *getInheritedProperties();
    Iterator<PropertyInterface *> *getLocalObjectProperties();
    Iterator<PropertyInterface *> *getInheritedObjectProperties();

  protected:
    void setInheritedProperty(const std::string &, PropertyInterface *);
};
}

#endif // TALIPOT_PROPERTY_MANAGER_H
