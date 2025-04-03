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

#ifndef TALIPOT_STRING_COLLECTION_H
#define TALIPOT_STRING_COLLECTION_H

#include <string>
#include <vector>

#include <talipot/config.h>

namespace tlp {

/**
 * This class represents a list of selectable string entries that can be used as plugin parameter.
 * The list will appear as a combo box in the Plugin Parameter Dialog from the Tulip GUI.
 */
class TLP_SCOPE StringCollection {

    std::vector<std::string> _data;
    size_t current;

  public:
    /**
     * Initializes an empty string collection.
     */
    StringCollection();

    /**
     * Initializes a string collection with strings stored in a vector.
     *
     * @param vectorParam a vector of strings entries for the string collection
     */
    explicit StringCollection(const std::vector<std::string> &vectorParam);

    /**
     * Initializes a string collection from a semicolon separated values string.
     *
     * @param param  a semicolon separated values string ("entry1;...;entryN"). If an entry need to
     * contain a semicolon, you have to escape it the following way : \; .
     */
    explicit StringCollection(const std::string &param);

    /**
     * Initializes a string collection with strings stored in a vector.
     *
     * @param vectorParam a vector of strings entries for the string collection
     * @param currentParam the index of the current selected string in the vector
     */
    StringCollection(const std::vector<std::string> &vectorParam, const int currentParam);

    /**
     * Initializes a string collection with strings stored in a vector.
     *
     * @param vectorParam a vector of strings entries for the string collection
     * @param currentString the current selected string value from the vector
     */
    StringCollection(const std::vector<std::string> &vectorParam, const std::string &currentString);

    /**
     * Returns all the selectable string entries.
     **/
    const std::vector<std::string> &getValues() const;

    /**
     * Returns the current selected string value.
     */
    const std::string &getCurrentString() const;

    /**
     * Sets the current selected string index.
     * Returns true if the provided index is valid.
     *
     * @param param a valid index in the string collection
     */
    bool setCurrent(const uint param);

    /**
     * Sets the current selected string value.
     * Returns true if the string value exists in the collection.
     *
     * @param param a string value from the collection
     */
    bool setCurrent(const std::string param);

    /**
     * Returns the index of the current selected string.
     */
    int getCurrent() const;

    /**
     * Adds a string value to this string collection.
     *
     * @param element the string to add to the collection
     */
    void push_back(const std::string &element) {
        _data.push_back(element);
    }

    /**
     * Concatenete a string collection to the current collection
     *
     */
    void insert(const tlp::StringCollection &str) {
        _data.insert(_data.end(), str._data.begin(), str._data.end());
    }

    /**
     * Returns true if the collection is empty.
     */
    bool empty() const {
        return _data.empty();
    }
    /**
     * @brief clear the content of the collection.
     */
    void clear() {
        _data.clear();
    }

    /**
     * Returns the string element at a certain index in the collection.
     *
     * @param index a valid index in the collection
     */
    std::string at(const size_t index) const {
        return _data.at(index);
    }

    /**
     * Returns the number of strings in the collection.
     */
    size_t size() const {
        return _data.size();
    }

    std::string operator[](const uint i) const {
        return _data[i];
    }
    std::string &operator[](const uint i) {
        return _data[i];
    }
};
}
#endif // TALIPOT_STRING_COLLECTION_H
