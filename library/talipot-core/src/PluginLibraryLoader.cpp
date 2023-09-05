/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/PluginLibraryLoader.h>
#include <talipot/PluginsManager.h>

#include <regex>

#ifdef _WIN32
#include <stdio.h>
#include <windows.h>
#else
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cerrno>
#endif

using namespace tlp;

std::string PluginLibraryLoader::_message, PluginLibraryLoader::_pluginPath,
    PluginLibraryLoader::_currentPluginLibrary;

void PluginLibraryLoader::loadPlugins(PluginLoader *loader, const std::string &folder) {
  std::vector<std::string> paths;
  std::stringstream ss(TalipotPluginsPath);
  std::string item;

  while (getline(ss, item, PATH_DELIMITER)) {
    paths.push_back(item);
  }

  // backup current plugin path as the pluginPath variable can be modified as a side effect
  // while loading a plugin that loads plugins
  std::string currentPluginPath = _pluginPath;

  // load the plugins in 'folder' for each path in TalipotPluginsPath (TalipotPluginsPath/folder)
  for (const auto &p : paths) {
    std::string dir = p + "/" + folder;

    PluginsManager::currentLoader = loader;
    _pluginPath = dir;

    // ensure message is empty before plugins directory loading
    _message.clear();

    if (initPluginDir(loader)) {
      if (loader) {
        loader->finished(true, _message);
      }
    }

#ifndef NDEBUG
    else
      tlp::debug() << "loadPlugins info: " << _message.c_str() << std::endl;

#endif

    PluginsManager::currentLoader = nullptr;
  }

  // restore original pluginPath value
  _pluginPath = currentPluginPath;
}

static std::pair<bool, std::string> isTalipotPluginFile(const std::string &libFilename) {
  std::string regExprStr(R"(.*-talipot-([0-9]+)[\._]([0-9]+)[\._]([0-9]+).*\.[so|dylib|dll])");
  std::regex rgx(regExprStr);
  std::smatch smatch;

  std::string mmVersion;
  bool ok = false;

  if (std::regex_search(libFilename, smatch, rgx)) {
    ok = true;
#ifdef _MSC_VER
    mmVersion = smatch[1].str() + "_" + smatch[2].str();
#else
    mmVersion = smatch[1].str() + "." + smatch[2].str();
#endif
  }
  return {ok, mmVersion};
}

void PluginLibraryLoader::loadPluginsFromDir(const std::string &rootPath, PluginLoader *loader,
                                             const std::string &userLocalPath) {
  // backup current plugin path as the pluginPath variable can be modified as a side effect
  // while loading a plugin that loads plugins
  std::string currentPluginPath = _pluginPath;

  PluginsManager::currentLoader = loader;
  _pluginPath = rootPath;

  // ensure message is empty before plugins directory loading
  _message.clear();

  if (initPluginDir(loader, true, userLocalPath)) {
    if (loader) {
      loader->finished(true, _message);
    }
  }

#ifndef NDEBUG
  else
    tlp::debug() << "loadPlugins info: " << _message.c_str() << std::endl;

#endif

  PluginsManager::currentLoader = nullptr;

  // restore original pluginPath value
  _pluginPath = currentPluginPath;
}

#ifdef _WIN32
bool PluginLibraryLoader::loadPluginLibrary(const std::string &filename, PluginLoader *loader) {
  HINSTANCE hDLL = LoadLibrary(filename.c_str());

  if (hDLL == nullptr) {
    if (loader != nullptr) {
      char *msg;
      DWORD dwErrCode = GetLastError();
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    nullptr,   // no source buffer needed
                    dwErrCode, // error code for this message
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    reinterpret_cast<LPTSTR>(&msg), // allocated by fcn
                    0,                              // minimum size of buffer
                    nullptr);                       // no inserts

      if (!msg) {
        char scode[128];
        sprintf(scode, "%s: unable to load(error %d)", filename.c_str(), int(dwErrCode));
        loader->aborted(filename, std::string(scode));
      } else {
        loader->aborted(filename, filename + ": " + msg);
        LocalFree(msg);
      }
    }

    return false;
  }

  return true;
}

#else

bool PluginLibraryLoader::loadPluginLibrary(const std::string &filename, PluginLoader *loader) {
  void *handle = dlopen(filename.c_str(), RTLD_NOW);

  if (!handle) {
    if (loader != nullptr) {
      loader->aborted(filename, std::string(dlerror()));
    }

    return false;
  }

  return true;
}

// accepts only file whose name matches *.so or *.dylib
#ifdef __FreeBSD__
#include <sys/param.h>
#if (__FreeBSD_version < 900000 && __FreeBSD_version >= 800501) || (__FreeBSD_version >= 900006)
int __talipot_select_libs(const struct dirent *ent) {
#else
int __talipot_select_libs(struct dirent *ent) {
#endif /* __FreeBSD_version */
#else  /* __FreeBSD__ */
int __talipot_select_libs(struct dirent *ent) {
#endif
#if !defined(__APPLE__)
  const char *suffix = ".so";
  const unsigned long suffix_len = 3;
#else
  const char *suffix = ".dylib";
  const unsigned long suffix_len = 6;
#endif
  int idx = strlen(ent->d_name) - suffix_len;

  if (idx < 0) {
    return 0;
  }

  for (unsigned long i = 0; i < suffix_len; ++i) {
    if ((ent->d_name[idx + i]) != suffix[i]) {
      return 0;
    }
  }

  return 1;
}

// accepts only sub-directories
#ifdef __FreeBSD__
#include <sys/param.h>
#if (__FreeBSD_version < 900000 && __FreeBSD_version >= 800501) || (__FreeBSD_version >= 900006)
int __talipot_select_dirs(const struct dirent *ent) {
#else
int __talipot_select_dirs(struct dirent *ent) {
#endif /* __FreeBSD_version */
#else  /* __FreeBSD__ */
int __talipot_select_dirs(struct dirent *ent) {
#endif

  std::string name(ent->d_name);

  if (ent->d_type == DT_DIR && name != "." && name != "..") {
    return 1;
  } else {
    return 0;
  }
}

#endif

bool PluginLibraryLoader::initPluginDir(PluginLoader *loader, bool recursive,
                                        const std::string &userPluginsPath) {
  std::string talipot_mm_version(TALIPOT_MM_VERSION);
  std::string talipot_version(TALIPOT_VERSION);

#if defined(_WIN32)

  HANDLE hFind;
  WIN32_FIND_DATA findData;
  TCHAR currentDirectory[FILENAME_MAX];
  DWORD dwRet;
  dwRet = GetCurrentDirectory(FILENAME_MAX, currentDirectory);

  if (dwRet == 0) {
    _message = _pluginPath + " - Scandir error";
    return false;
  } else {
    // first check is pluginPath exists
    DWORD fileAttr = GetFileAttributes(_pluginPath.c_str());

    if (fileAttr == 0xFFFFFFFF) {
      _message += std::string("Directory not found: ") + _pluginPath.c_str();
      return false;
    }

    if (loader != nullptr) {
      loader->start(_pluginPath.c_str());
    }

    SetCurrentDirectory(_pluginPath.c_str());
    hFind = FindFirstFile("*.dll", &findData);

    if (loader != nullptr) {
      // count files loop
      unsigned long nbFiles = 0;

      if (hFind != INVALID_HANDLE_VALUE) {
        nbFiles = 1;
      }

      while (FindNextFile(hFind, &findData)) {
        ++nbFiles;
      }

      if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
      }

      loader->numberOfFiles(nbFiles);
      hFind = FindFirstFile("*.dll", &findData);
    }

    BOOL success = hFind != INVALID_HANDLE_VALUE;

    while (success) {
      _currentPluginLibrary = _pluginPath + "/" + findData.cFileName;
      std::string lib(findData.cFileName);

      // don't print error messages when trying to load Talipot Python
      // binary modules
      if (lib[0] == '_' || lib.find("sip") != std::string::npos) {
        success = FindNextFile(hFind, &findData);
        continue;
      }

      // looking for a suffix matching -talipot-X.Y.Z.dll
      auto [ok, mmVersion] = isTalipotPluginFile(lib);

      if (ok) {
        if (mmVersion == talipot_mm_version) {
          // if a user local plugin with the same name exists
          // we do not try to load the current one
          if (!userPluginsPath.empty()) {
            std::string userPluginLibrary = userPluginsPath + "/" + findData.cFileName;
            if (GetFileAttributes(userPluginLibrary.c_str()) != 0xFFFFFFFF) {
              success = FindNextFile(hFind, &findData);
              continue;
            }
          }
          if (loader) {
            loader->loading(findData.cFileName);
          }

          loadPluginLibrary(_currentPluginLibrary, loader);
        } else if (loader) {
          loader->aborted(_currentPluginLibrary, _currentPluginLibrary +
                                                     " is not compatible with Talipot " +
                                                     TALIPOT_VERSION);
        }
      }
      success = FindNextFile(hFind, &findData);
    }

    if (hFind != INVALID_HANDLE_VALUE) {
      FindClose(hFind);
    }

    if (recursive) {

      hFind = FindFirstFile("*", &findData);

      std::string rootPath = _pluginPath;

      while (FindNextFile(hFind, &findData)) {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
          std::string dir(findData.cFileName);

          if (dir != "." && dir != "..") {
            _pluginPath = rootPath + "/" + dir;
            initPluginDir(loader, true, userPluginsPath);
            _pluginPath = rootPath;
          }
        }
      }

      if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
      }
    }

    SetCurrentDirectory(currentDirectory);
  }

#else

  struct dirent **namelist;
  int n = scandir(_pluginPath.c_str(), &namelist,
#if !(defined(__APPLE__) || defined(__FreeBSD__)) || \
    (defined(__APPLE__) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 1080)
                  reinterpret_cast<int (*)(const dirent *)>(__talipot_select_libs),
#else
                  __talipot_select_libs,
#endif
                  alphasort);

  if (loader != nullptr) {
    loader->numberOfFiles(n);
  }

  if (n < 0) {
    _message += _pluginPath + " - " + std::string(strerror(errno));
    return false;
  }

  if (loader != nullptr) {
    loader->start(_pluginPath);
  }

  while (n > 0) {
    n--;
    std::string lib(namelist[n]->d_name);
    free(namelist[n]);

    if (n == 0) {
      free(namelist);
    }

    // don't print error messages when trying to load Talipot Python
    // binary modules
    if (lib[0] == '_' || lib.find("sip") != std::string::npos) {
      continue;
    }

    _currentPluginLibrary = _pluginPath + "/" + lib;
    // looking for a suffix matching -talipot-X.Y.Z.(so/dylib)
    auto [ok, mmVersion] = isTalipotPluginFile(lib);

    if (ok) {
      if (mmVersion == talipot_mm_version) {
        // if a user local plugin with the same name exists
        // we do not try to load the current one
        if (!userPluginsPath.empty()) {
          std::string userPluginLibrary = userPluginsPath + "/" + lib;
          struct stat slib;
          if (stat(userPluginLibrary.c_str(), &slib) == 0) {
            continue;
          }
        }
        if (loader != nullptr) {
          loader->loading(lib);
        }

        loadPluginLibrary(_currentPluginLibrary, loader);
        continue;
      } else {
        if (loader) {
          loader->aborted(_currentPluginLibrary, _currentPluginLibrary +
                                                     " is not compatible with Talipot " +
                                                     TALIPOT_VERSION);
          return n > 0;
        }
      }
    }
  }

  if (recursive) {

    n = scandir(_pluginPath.c_str(), &namelist,
#if !(defined(__APPLE__) || defined(__FreeBSD__)) || \
    (defined(__APPLE__) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 1080)
                reinterpret_cast<int (*)(const dirent *)>(__talipot_select_dirs),
#else
                __talipot_select_dirs,
#endif
                alphasort);

    std::string rootPath = _pluginPath;

    while (n > 0) {
      n--;
      std::string dir(namelist[n]->d_name);
      free(namelist[n]);

      _pluginPath = rootPath + "/" + dir;

      initPluginDir(loader, true, userPluginsPath);

      _pluginPath = rootPath;

      if (n == 0) {
        free(namelist);
      }
    }
  }

#endif
  return true;
}
