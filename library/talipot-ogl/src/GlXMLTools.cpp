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

#include <talipot/GlXMLTools.h>

#include <talipot/Gl2DRect.h>
#include <talipot/GlComposite.h>
#include <talipot/GlBox.h>
#include <talipot/GlCircle.h>
#include <talipot/GlGrid.h>
#include <talipot/GlLabel.h>
#include <talipot/GlLine.h>
#include <talipot/GlSphere.h>
#include <talipot/GlCurve.h>
#include <talipot/GlComplexPolygon.h>
#include <talipot/GlQuad.h>

using namespace std;

uint tlp::GlXMLTools::indentationNumber = 0;

namespace tlp {

void GlXMLTools::applyIndentation(string &outString) {
    for (uint i = 0; i < indentationNumber; ++i) {
        outString.append("  ");
    }
}

void GlXMLTools::goToNextCaracter(const string &inString, uint &currentPosition) {
    while (inString[currentPosition] == ' ' || inString[currentPosition] == '\n') {
        currentPosition++;
    }
}

void GlXMLTools::beginDataNode(string &outString) {
    applyIndentation(outString);
    outString.append("<data>\n");
    indentationNumber++;
}

void GlXMLTools::endDataNode(string &outString) {
    indentationNumber--;
    applyIndentation(outString);
    outString.append("</data>\n");
}

void GlXMLTools::enterDataNode(const string &inString, uint &currentPosition) {
    goToNextCaracter(inString, currentPosition);
    assert(inString.substr(currentPosition, 6) == "<data>");
    currentPosition += 6;
}

void GlXMLTools::leaveDataNode(const string &inString, uint &currentPosition) {
    goToNextCaracter(inString, currentPosition);
    assert(inString.substr(currentPosition, 7) == "</data>");
    currentPosition += 7;
}

void GlXMLTools::beginChildNode(string &outString, const string &name) {
    applyIndentation(outString);
    outString.append("<" + name + ">\n");
    indentationNumber++;
}

void GlXMLTools::endChildNode(string &outString, const string &name) {
    indentationNumber--;
    applyIndentation(outString);
    outString.append("</" + name + ">\n");
}

string GlXMLTools::enterChildNode(const string &inString, uint &currentPosition) {
    goToNextCaracter(inString, currentPosition);
    uint beginPosition = currentPosition + 1;
    size_t endPosition = inString.find('>', currentPosition);

    if (inString.substr(beginPosition - 1, endPosition - beginPosition).find("</") !=
        string::npos) {
        return "";
    }

    size_t childNameEndPosition = inString.find(' ', currentPosition);
    currentPosition = endPosition + 1;

    if (childNameEndPosition < endPosition) {
        return inString.substr(beginPosition, childNameEndPosition - beginPosition);
    } else {
        return inString.substr(beginPosition, endPosition - beginPosition);
    }
}

void GlXMLTools::leaveChildNode(const string &inString, uint &currentPosition,
                                const string &childName) {
    currentPosition = inString.find("</" + childName + ">", currentPosition) + 3 + childName.size();
}

void GlXMLTools::createProperty(string &outString, const string &name, const string &value,
                                const string &parent) {
    if (parent.empty()) {
        outString = outString.substr(0, outString.size() - 2);
        outString.append(" " + name + "=\"" + value + "\">\n");
    } else {
        size_t beginPosition = outString.rfind("<" + parent);
        beginPosition = outString.find('>', beginPosition);
        string end = outString.substr(beginPosition);
        outString.resize(beginPosition);
        outString.append(" " + name + "=\"" + value + "\"");
        outString.append(end);
    }
}

map<string, string> GlXMLTools::getProperties(const string &inString, uint &currentPosition) {
    map<string, string> properties;
    size_t beginPosition = inString.rfind('<', currentPosition);
    size_t propertyPosition = inString.find('=', beginPosition);

    while (propertyPosition < currentPosition) {
        size_t propertyNameBegin = inString.rfind(' ', propertyPosition) + 1;
        size_t propertyValueEnd = inString.find('\"', propertyPosition + 2);
        string name = inString.substr(propertyNameBegin, propertyPosition - propertyNameBegin);
        string value =
            inString.substr(propertyPosition + 2, propertyValueEnd - propertyPosition - 2);
        properties[name] = value;
        propertyPosition = inString.find('=', propertyPosition + 1);
    }

    return properties;
}

GlEntity *GlXMLTools::createEntity(const string &name) {
    if (name == "GlBox") {
        return new GlBox();
    } else if (name == "GlCircle") {
        return new GlCircle();
    } else if (name == "GlComplexPolygon") {
        return new GlComplexPolygon();
    } else if (name == "GlComposite") {
        return new GlComposite();
    } else if (name == "GlCurve") {
        return new GlCurve();
    } else if (name == "GlGraph" || name == "GlGraphComposite") {
    } else if (name == "GlGrid") {
        return new GlGrid();
    } else if (name == "GlLabel") {
        return new GlLabel();
    } else if (name == "GlLine") {
        return new GlLine();
    } else if (name == "GlPolygon") {
        return new GlPolygon();
    } else if (name == "GlQuad") {
        return new GlQuad();
    } else if (name == "GlRect") {
        return new GlRect();
    } else if (name == "GlRectTextured") {
        return new Gl2DRect();
    } else if (name == "Gl2DRect") {
        return new Gl2DRect();
    } else if (name == "GlSphere") {
        return new GlSphere();
    } else {
        tlp::debug() << "Unknown entity type : " << name << ". Can't create it !" << endl;
    }

    return nullptr;
}
}
