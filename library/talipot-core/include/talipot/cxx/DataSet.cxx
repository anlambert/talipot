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

//=======================================================================
// DataSet implementation
template <typename T>
bool tlp::DataSet::get(const std::string &str, T &value) const {

    if (const auto it = data.find(str); it != data.end()) {
        value = *(static_cast<T *>(it->second->value));
        return true;
    }

    return false;
}

template <typename T>
bool tlp::DataSet::getAndFree(const std::string &str, T &value) {
    if (const auto it = data.find(str); it != data.end()) {
        value = *(static_cast<T *>(it->second->value));
        delete it->second;
        data.erase(it);
        return true;
    }
    return false;
}

template <typename T>
void tlp::DataSet::set(const std::string &key, const T &value) {
    TypedData<T> dtc(new T(value));
    setData(key, &dtc);
}
//=======================================================================
