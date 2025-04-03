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

#include <talipot/StringCollection.h>

using namespace std;
using namespace tlp;

StringCollection::StringCollection() : current(0) {}

StringCollection::StringCollection(const std::vector<string> &vectorParam)
    : _data(vectorParam), current(0) {}

StringCollection::StringCollection(const std::string &param) : current(0) {
    string temp;
    bool escapeChar = false;

    for (auto itChar : param) {
        if (escapeChar) {
            // when escaped semi colon can be added to the string
            if (itChar == ';') {
                temp += itChar;
                escapeChar = false;
                continue;
            } else {
                escapeChar = false;
                temp += '\\';
            }
        }

        // check string separator
        if (itChar == ';') {
            escapeChar = false;
            _data.push_back(temp);
            temp.clear();
            continue;
        }

        // check escape char
        if (itChar == '\\') {
            escapeChar = true;
        } else {
            temp += itChar;
        }
    }

    if (!temp.empty()) {
        _data.push_back(temp);
    }
}

StringCollection::StringCollection(const std::vector<string> &vectorParam, const int currentParam)
    : _data(vectorParam) {
    if (currentParam < int(_data.size())) {
        current = currentParam;
    } else {
        current = 0;
    }
}

StringCollection::StringCollection(const std::vector<string> &vectorParam,
                                   const std::string &currentString)
    : _data(vectorParam), current(0) {

    for (const auto &s : _data) {
        if (s == currentString) {
            return;
        }
        ++current;
    }

    current = 0;
}

const std::string &StringCollection::getCurrentString() const {
    if (current < _data.size()) {
        return _data.at(current);
    }

    static std::string emptyString;
    return emptyString;
}

bool StringCollection::setCurrent(const uint param) {
    if (param < _data.size()) {
        current = param;
        return true;
    }

    return false;
}

bool StringCollection::setCurrent(const string param) {
    for (uint i = 0; i < _data.size(); ++i) {
        if (_data.at(i) == param) {
            current = i;
            return true;
        }
    }

    return false;
}

int StringCollection::getCurrent() const {
    return current;
}

const vector<string> &StringCollection::getValues() const {
    return _data;
}
