/**
 *
 * Copyright (C) 2020-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_SINGLETON_H
#define TALIPOT_SINGLETON_H

#include <memory>
#include <mutex>

namespace tlp {

template <typename T>
class Singleton {
private:
  Singleton(const Singleton<T> &) = delete;
  Singleton &operator=(const Singleton<T> &) = delete;

  static std::unique_ptr<T> _instance;
  static std::once_flag _once;

public:
  Singleton() = default;
  virtual ~Singleton() = default;

  static T &instance() {
    std::call_once(_once, []() { _instance.reset(new T); });
    return *(_instance.get());
  }

  template <typename... Args>
  static T &instance(Args &&...args) {
    std::call_once(_once, [&]() { _instance.reset(new T(std::forward<Args>(args)...)); });
    return *(_instance.get());
  }

  void deleteEarly() {
    _instance.reset(nullptr);
  }
};

template <typename T>
std::unique_ptr<T> Singleton<T>::_instance;
template <typename T>
std::once_flag Singleton<T>::_once;

}

#endif // TALIPOT_SINGLETON_H