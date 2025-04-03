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

#ifndef TALIPOT_GL_XML_TOOLS_H
#define TALIPOT_GL_XML_TOOLS_H

#include <sstream>
#include <vector>
#include <map>
#include <cassert>

#include <talipot/config.h>

namespace tlp {

/**
 * static tools class use to store/load xml data
 */
class GlEntity;

class TLP_GL_SCOPE GlXMLTools {

  public:
    /**
     * Create and enter into a new data node
     */
    static void beginDataNode(std::string &outString);

    /**
     * End last data node
     */
    static void endDataNode(std::string &outString);

    /**
     * Enter into a data node and set the currentPosition integer to the position next the data tag
     */
    static void enterDataNode(const std::string &inString, uint &currentPosition);

    /**
     * Leave a data node and set the currentPosition integer to the position next the data end tag
     */
    static void leaveDataNode(const std::string &inString, uint &currentPosition);

    /**
     * Create and enter into a new children node with name : name
     */
    static void beginChildNode(std::string &outString, const std::string &name = "children");

    /**
     * End last children node with name : name
     */
    static void endChildNode(std::string &outString, const std::string &name = "children");

    /**
     * Enter into a child node and set the currentPosition integer to the position next the child
     * tag \return child name
     */
    static std::string enterChildNode(const std::string &inString, uint &currentPosition);

    /**
     * Leave into a child node and set the currentPosition integer to the position next the child
     * end tag \return child name
     */
    static void leaveChildNode(const std::string &inString, uint &currentPosition,
                               const std::string &childName);

    /**
     * Create a property with name and value in rootNode
     */
    static void createProperty(std::string &outString, const std::string &name,
                               const std::string &value, const std::string &parent = "");

    /**
     * Get properties associated with the last opened child
     * These properties is returned into a map of string/string
     */
    static std::map<std::string, std::string> getProperties(const std::string &inString,
                                                            uint &currentPosition);

    /**
     * Create a GlEntity with the given name
     */
    static GlEntity *createEntity(const std::string &name);

    /**
     * Get the XML output for a vector of Object
     */
    template <typename Obj>
    static void getXML(std::string &outString, const std::string &name,
                       const typename std::vector<Obj> &vect) {

        std::stringstream str;
        str << "(";
        auto it = vect.begin();
        assert(it != vect.end());
        str << *it;
        ++it;

        for (; it != vect.end(); ++it) {
            str << "," << *it;
        }

        str << ")";
        outString.append("<" + name + ">" + str.str() + "</" + name + ">\n");
    }

    /**
     * Set vector of Object with the given XML
     */
    template <typename Obj>
    static void setWithXML(const std::string &inString, uint &currentPosition,
                           const std::string &name, std::vector<Obj> &vect) {

        goToNextCaracter(inString, currentPosition);

        std::string nameTag = inString.substr(currentPosition, name.size() + 2);
        assert(nameTag == "<" + name + ">");
        currentPosition += name.size() + 2;

        size_t endValuePosition = inString.find("</" + name + ">", currentPosition);
        assert(endValuePosition != std::string::npos);

        std::istringstream is(inString.substr(currentPosition, endValuePosition - currentPosition));
        Obj data;
        char c = is.get();

        while (c != ')') {
            is >> data;
            vect.push_back(data);
            c = is.get();
        }

        currentPosition = endValuePosition + name.size() + 3;
    }

    /**
     * Get the XML output for an Object
     */
    template <typename Obj>
    static void getXML(std::string &outString, const std::string &name, const Obj &value) {
        std::stringstream str;
        str << value;
        applyIndentation(outString);
        outString.append("<" + name + ">" + str.str() + "</" + name + ">\n");
    }

    static bool checkNextXMLtag(const std::string &inString, uint &currentPosition,
                                const std::string &name) {
        uint tmp = currentPosition;
        goToNextCaracter(inString, tmp);
        std::string nameTag = inString.substr(tmp, name.size() + 2);
        return (nameTag == "<" + name + ">");
    }

    /**
     * Set an Object with the given XML
     */
    template <typename Obj>
    static void setWithXML(const std::string &inString, uint &currentPosition,
                           const std::string &name, Obj &value) {

        goToNextCaracter(inString, currentPosition);

        std::string nameTag = inString.substr(currentPosition, name.size() + 2);
        assert(nameTag == "<" + name + ">");
        currentPosition += name.size() + 2;

        size_t endValuePosition = inString.find("</" + name + ">", currentPosition);
        assert(endValuePosition != std::string::npos);

        std::stringstream str(inString.substr(currentPosition, endValuePosition - currentPosition));
        str >> value;
        currentPosition = endValuePosition + name.size() + 3;
    }

    /**
     * Set an Object with the given XML and default value
     */
    template <typename Obj>
    static void setWithXML(const std::string &inString, uint &currentPosition,
                           const std::string &name, Obj &value, const Obj &defValue) {
        goToNextCaracter(inString, currentPosition);

        std::string nameTag = inString.substr(currentPosition, name.size() + 2);

        if (nameTag == "<" + name + ">") {
            currentPosition += name.size() + 2;

            size_t endValuePosition = inString.find("</" + name + ">", currentPosition);
            assert(endValuePosition != std::string::npos);

            std::stringstream str(
                inString.substr(currentPosition, endValuePosition - currentPosition));
            str >> value;
            currentPosition = endValuePosition + name.size() + 3;
        } else {
            value = defValue;
        }
    }

  private:
    static void applyIndentation(std::string &outString);
    static void goToNextCaracter(const std::string &inString, uint &currentPosition);

    static uint indentationNumber;
};
}

#endif // TALIPOT_GL_XML_TOOLS_H
