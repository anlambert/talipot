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

#include <locale>
#include <codecvt>

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <filesystem>
#ifdef _MSC_VER
#include <dbghelp.h>
#endif
#ifdef __GLIBCXX__
#include <ext/stdio_filebuf.h>
#endif
#else
#include <dirent.h>
#include <dlfcn.h>
#endif

#include <gzstream.h>
#include <zststream.h>

#include <talipot/Exception.h>
#include <talipot/Plugin.h>
#include <talipot/PropertyTypes.h>
#if defined(_OPENMP) && defined(__APPLE__)
#include <talipot/ParallelTools.h>
#endif

using namespace std;
using namespace tlp;

static const char *TALIPOT_PLUGINS_PATH_VARIABLE = "TLP_PLUGINS_PATH";

// the relative path (a string), from the install dir
// of the directory where the talipot libraries are installed
#define TALIPOT_INSTALL_LIBDIR_STR STRINGIFY(TALIPOT_INSTALL_LIBDIR)

string tlp::TalipotLibDir;
string tlp::TalipotPluginsPath;
string tlp::TalipotBitmapDir;
string tlp::TalipotShareDir;
#ifdef _WIN32
const char tlp::PATH_DELIMITER = ';';
#else
const char tlp::PATH_DELIMITER = ':';
#endif

inline std::string getTalipotLibName() {
#ifdef _WIN32
#ifdef __MINGW32__
  return "libtalipot-core-" + getMajor(TALIPOT_VERSION) + "." + getMinor(TALIPOT_VERSION) + ".dll";
#else
  return "talipot-core-" + getMajor(TALIPOT_VERSION) + "_" + getMinor(TALIPOT_VERSION) + ".dll";
#endif
#else
  std::string talipotLibName =
      "libtalipot-core-" + getMajor(TALIPOT_VERSION) + "." + getMinor(TALIPOT_VERSION);
#ifdef __APPLE__
  return talipotLibName + ".dylib";
#else
  return talipotLibName + ".so";
#endif
#endif
}
// A function that retrieves the Talipot libraries directory based on
// the path of the loaded shared library libtalipot-core-X.Y.[dll, so, dylib]
extern "C" {
const char *getTalipotLibDir() {
  std::string libTalipotName = getTalipotLibName();
  std::string libTalipotPath;

#ifdef _WIN32

  HMODULE hmod = GetModuleHandle(libTalipotName.c_str());

  if (hmod != nullptr) {
    TCHAR szPath[512 + 1];
    DWORD dwLen = GetModuleFileName(hmod, szPath, 512);

    if (dwLen > 0) {
      libTalipotPath = szPath;
      std::replace(libTalipotPath.begin(), libTalipotPath.end(), '\\', '/');
    }
  }

#else

  void *ptr = dlopen(libTalipotName.c_str(), RTLD_LAZY);

  if (ptr != nullptr) {
    void *symbol = dlsym(ptr, "getTalipotLibDir");

    if (symbol != nullptr) {
      Dl_info info;
      if (dladdr(symbol, &info)) {
        libTalipotPath = info.dli_fname;
      }
    }
    dlclose(ptr);
  }

#endif

  static std::string talipotLibDir;
  talipotLibDir =
      libTalipotPath.substr(0, libTalipotPath.rfind('/') + 1) + "../" + TALIPOT_INSTALL_LIBDIR_STR;
  return talipotLibDir.c_str();
}
}

// throw an exception if an expected directory does not exist
static void checkDirectory(std::string dir, bool tlpDirSet, bool throwEx) {
  // remove ending / separator if any
  // bug detected on Windows
  if (dir[dir.length() - 1] == '/') {
    dir.erase(dir.length() - 1);
  }

  if (!pathExists(dir)) {
    std::stringstream ess;
    ess << "Error - " << dir << ":" << std::endl << strerror(errno) << std::endl;
    if (tlpDirSet) {
      ess << std::endl << "Check your TLP_DIR environment variable";
    }
    if (throwEx) {
      throw Exception(ess.str());
    } else if ( // output only if not in a python installed wheel
        (dir.find("/talipot/native/") == string::npos) &&
        // and not building talipot-core bindings
        (dir.find("library/talipot-core/src") == string::npos)) {
      tlp::error() << ess.str();
    }
  }
}

//=========================================================
void tlp::initTalipotLib(const char *appDirPath) {
  if (!TalipotShareDir.empty()) { // already initialized
    return;
  }

  char *getEnvTlp;
  string::size_type pos;
  // we use curDir to ease debugging
  // Talipot..Dir global variables may be invisible
  // when using gdb
  std::string curDir;

  getEnvTlp = getenv("TLP_DIR");

  if (getEnvTlp == nullptr) {
    if (appDirPath) {
#ifdef _WIN32
      curDir = std::string(appDirPath) + "/../" + TALIPOT_INSTALL_LIBDIR_STR;
#else
      // one dir up to initialize the lib dir
      curDir.append(appDirPath, strlen(appDirPath) - strlen(strrchr(appDirPath, '/') + 1));
      curDir.append(TALIPOT_INSTALL_LIBDIR_STR);

#endif
    } else {
      // if no appDirPath is provided, retrieve dynamically the Talipot lib dir
      curDir = getTalipotLibDir();
    }
  } else {
    curDir = string(getEnvTlp);
  }

#ifdef _WIN32
  // ensure it is a unix-style path
  pos = curDir.find('\\', 0);

  while (pos != string::npos) {
    curDir[pos] = '/';
    pos = curDir.find('\\', pos);
  }

#endif

  // ensure it is '/' terminated
  if (curDir[curDir.length() - 1] != '/') {
    curDir += '/';
  }

  bool tlpDirSet = (getEnvTlp != nullptr);
  bool throwExOnCheck = appDirPath != nullptr;

  TalipotLibDir = curDir;
  if (!TalipotLibDir.empty() && TalipotLibDir != "/") {
    checkDirectory(TalipotLibDir, tlpDirSet, throwExOnCheck);
  }

  getEnvTlp = getenv(TALIPOT_PLUGINS_PATH_VARIABLE);

  if (getEnvTlp != nullptr) {
    curDir = string(getEnvTlp);
#ifdef _WIN32
    // ensure it is a unix-style path
    pos = curDir.find('\\', 0);

    while (pos != string::npos) {
      curDir[pos] = '/';
      pos = curDir.find('\\', pos);
    }

#endif
    curDir = TalipotLibDir + "talipot" + PATH_DELIMITER + curDir;
  } else {
    curDir = TalipotLibDir + "talipot";
  }
  TalipotPluginsPath = curDir;

  // one dir up to initialize the share dir
  pos = TalipotLibDir.length() - 2;
  pos = TalipotLibDir.rfind("/", pos);
  curDir = TalipotLibDir.substr(0, pos + 1) + "share/talipot/";

#ifndef _WIN32
  // special case for Debian when Talipot install prefix is /usr
  // as libraries are installed in <prefix>/lib/<arch>
  if (!pathExists(curDir) != 0) {
    pos = TalipotLibDir.rfind("/", pos - 1);
    curDir = TalipotLibDir.substr(0, pos + 1) + "share/talipot/";
  }
#endif

#if defined(_OPENMP) && defined(__APPLE__)
  // Register an exit handler to prevent using OpenMP locks
  // when application shutdowns as some crashes have been observed
  // on some MacOS runtimes (10.12 for instance)
  OpenMPLock::registerExitHandler();
#endif

  TalipotShareDir = curDir;
  if (!TalipotLibDir.empty() && TalipotLibDir != "/") {
    checkDirectory(TalipotShareDir, tlpDirSet, throwExOnCheck);
  }

  TalipotBitmapDir = TalipotShareDir + "bitmaps/";

  if (!TalipotLibDir.empty() && TalipotLibDir != "/") {
    checkDirectory(TalipotBitmapDir, tlpDirSet, throwExOnCheck);
  }

  // initialize serializers
  initTypeSerializers();

  // initialize pseudo random number generator seed
  initRandomSequence();
}
//=========================================================

// tlp class names demangler
#if defined(__GNUC__)
#include <cxxabi.h>
std::string tlp::demangleClassName(const std::string &className, bool hideTlp) {
  int status;
  std::unique_ptr<char, decltype(free) *> demangledNamePtr(
      abi::__cxa_demangle(className.c_str(), 0, 0, &status), free);
  std::string demangledName = demangledNamePtr.get();

  // skip tlp::
  if (hideTlp && demangledName.find("tlp::") == 0) {
    demangledName = demangledName.substr(5);
  }

  return demangledName;
}
#elif defined(_MSC_VER)
// With Visual Studio, typeid(tlp::T).name() does not return a mangled type name
// but a human readable type name in the form "class tlp::T"
// so just remove the first 11 characters to return T
std::string tlp::demangleClassName(const std::string &className, bool hideTlp) {
  std::string demangledName = className;
  if (demangledName.find("class ") == 0) {
    demangledName = demangledName.substr(6);
  }

  if (hideTlp && demangledName.find("tlp::") == 0) {
    demangledName = demangledName.substr(5);
  }

  return demangledName;
}
#else
#error define symbols demangling function
#endif

// random sequence management
//=========================================================

static uint randomSeed = UINT_MAX;
// uniformly-distributed integer random number generator that produces non-deterministic random
// numbers
static std::random_device rd;
// Mersenne Twister pseudo-random generator of 32-bit numbers
static std::mt19937 mt;

void tlp::setSeedOfRandomSequence(uint seed) {
  randomSeed = seed;
}

uint tlp::getSeedOfRandomSequence() {
  return randomSeed;
}

void tlp::initRandomSequence() {
  // init seed from random sequence with std::random_device
  if (randomSeed == UINT_MAX) {
    mt.seed(rd());
  } else {
    mt.seed(randomSeed);
  }
}

TLP_SCOPE std::mt19937 &tlp::getRandomNumberGenerator() {
  return mt;
}

int tlp::randomInteger(int max) {
  if (max == 0) {
    return 0;
  } else if (max > 0) {
    std::uniform_int_distribution<int> dist(0, max);
    return dist(mt);
  } else {
    std::uniform_int_distribution<int> dist(max, 0);
    return dist(mt);
  }
}

uint tlp::randomUnsignedInteger(uint max) {
  if (max == 0) {
    return 0;
  } else {
    std::uniform_int_distribution<uint> dist(0, max);
    return dist(mt);
  }
}

double tlp::randomDouble(double max) {
  std::uniform_real_distribution<double> dist(0, std::nextafter(max, DBL_MAX));
  return dist(mt);
}

// files management
//=========================================================

int tlp::statPath(const std::string &pathname, tlp_stat_t *buf) {
#ifndef WIN32
  return stat(pathname.c_str(), buf);
#else
  std::wstring utf16pathname = utf8to16(pathname);
  return _wstat(utf16pathname.c_str(), buf);
#endif
}

bool tlp::pathExists(const std::string &pathname) {
  tlp_stat_t infoEntry;
  return statPath(pathname, &infoEntry) == 0;
}

// file streams management
//=========================================================

std::istream *tlp::getInputFileStream(const std::string &filename, std::ios_base::openmode mode) {
#ifdef WIN32
#ifdef _MSC_VER
  return new std::ifstream(filesystem::u8path(filename), mode);
#else
  return new std::ifstream(filesystem::path(filename), mode);
#endif
#else
  return new std::ifstream(filename, mode);
#endif
}

//=========================================================

std::ostream *tlp::getOutputFileStream(const std::string &filename, std::ios_base::openmode mode) {
#ifdef WIN32
#ifdef _MSC_VER
  return new std::ofstream(filesystem::u8path(filename), mode);
#else
  return new std::ofstream(filesystem::path(filename), mode);
#endif
#else
  return new std::ofstream(filename, mode);
#endif
}

//=========================================================

std::istream *tlp::getZlibInputFileStream(const std::string &filename) {
#if defined(WIN32) && ZLIB_VERNUM >= 0x1270
  std::wstring utf16filename = utf8to16(filename);
  return new igzstream(utf16filename.c_str(), ios::in | ios::binary);
#else
  return new igzstream(filename.c_str(), ios::in | ios::binary);
#endif
}

//=========================================================

std::ostream *tlp::getZlibOutputFileStream(const std::string &filename) {
#if defined(WIN32) && ZLIB_VERNUM >= 0x1270
  std::wstring utf16filename = utf8to16(filename);
  return new ogzstream(utf16filename.c_str(), ios::out | ios::binary);
#else
  return new ogzstream(filename.c_str(), ios::out | ios::binary);
#endif
}

//=========================================================

std::istream *tlp::getZstdInputFileStream(const std::string &filename) {
  return new zstd::ZstdIStream(getInputFileStream(filename));
}

//=========================================================

std::ostream *tlp::getZstdOutputFileStream(const std::string &filename, int compressionLevel) {
  return new zstd::ZstdOStream(getOutputFileStream(filename), compressionLevel);
}

//=========================================================

// silent not critical codecvt deprecation warnings
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

string tlp::utf32to8(const u32string &s) {
  wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
  return conv.to_bytes(s);
}

//=========================================================

wstring tlp::utf8to16(const string &s) {
  wstring_convert<codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
  return conv.from_bytes(s);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//=========================================================

vector<string> tlp::tokenize(const string &str, const string &delimiter) {
  vector<string> tokens;
  string::size_type lastPos = 0;
  string::size_type pos = str.find_first_of(delimiter, lastPos);

  while (string::npos != pos || string::npos != lastPos) {
    tokens.push_back(str.substr(lastPos, pos - lastPos));

    if (pos != string::npos) {
      lastPos = pos + 1;
    } else {
      lastPos = string::npos;
    }

    pos = str.find_first_of(delimiter, lastPos);
  }
  return tokens;
}
