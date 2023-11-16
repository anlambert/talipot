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

#ifndef TALIPOT_TLP_TOOLS_H
#define TALIPOT_TLP_TOOLS_H

#include <iostream>
#include <climits>
#include <cstring>
#include <random>
#include <string_view>

#include <sys/types.h>
#include <sys/stat.h>
#include <talipot/config.h>

#ifdef WIN32
typedef struct _stat tlp_stat_t;
#else
typedef struct stat tlp_stat_t;
#endif

// The hash_combine function from the boost library
// Call it repeatedly to incrementally create a hash value from several variables.

// Explanation of the formula from StackOverflow
// (http://stackoverflow.com/questions/4948780/magic-numbers-in-boosthash-combine) :
// The magic number 0x9e3779b9 = 2^32 / ((1 + sqrt(5)) / 2) is the reciprocal of the golden ratio.
// It is supposed to be 32 random bits, where each is equally likely to be 0 or 1, and with no
// simple correlation between the bits.
// So including this number "randomly" changes each bit of the seed; as you say, this means that
// consecutive values will be far apart.
// Including the shifted versions of the old seed makes sure that, even if hash_value() has a fairly
// small range of values,
// differences will soon be spread across all the bits.
namespace std {
template <class T>
inline void tlp_hash_combine(std::size_t &seed, const T &v) {
  hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace std

namespace tlp {
extern TLP_SCOPE const char PATH_DELIMITER;
extern TLP_SCOPE std::string TalipotLibDir;
extern TLP_SCOPE std::string TalipotPluginsPath;
extern TLP_SCOPE std::string TalipotBitmapDir;
extern TLP_SCOPE std::string TalipotShareDir;

/**
 * @ingroup Plugins
 *
 * @brief Initializes the Talipot library.
 * Looks for the Talipot plug-ins directory.
 * The plug-ins directory can be defined in different ways, given by order of prevalence :
 * 1. the TLP_DIR environment variable, if it has a value
 * 2. the appDirPath parameter, if it is not nullptr
 * 3. at that point, the Talipot paths will be retrieved from the path of the loaded Talipot shared
 * library
 *  (you must dispose of a standard Talipot installation for that feature to work).
 * 4. a fallback value of 'C:/Talipot/lib/' on windows, or '/usr/local/lib/' on Unix.
 */
extern TLP_SCOPE void initTalipotLib(const char *appDirPath = nullptr);

/**
 * @ingroup Plugins
 *
 * @brief Demangles the name of a C++ class
 * @param className The mangled name of a class
 * @param hideTlp a flag to indicate if the 'tlp::' prefix
 * @return string The demangled name of a Talipot C++ class.
 */
TLP_SCOPE std::string demangleClassName(const std::string &className, bool hideTlp = false);

template <typename T>
std::string demangleClassName(bool hideTlp = false) {
  return demangleClassName(typeid(T).name(), hideTlp);
}

/**
 * @ingroup Plugins
 *
 * @brief Demangles the name of a C++ class defined in the tlp namespace.
 * @param className The mangled name of a class
 * @return string The demangled name of a Talipot C++ class
 * without the tlp:: prefix
 */
inline std::string demangleTlpClassName(const std::string &className) {
  return demangleClassName(className, true);
}

template <typename T>
std::string demangleTlpClassName() {
  return demangleTlpClassName(typeid(T).name());
}

/**
 * @brief Gives the value of the seed used for further initialization
 * of a random sequence (with further calls to rand() or random()).
 * @param seed the value of the seed.
 * Set seed to UINT_MAX if you need a random choice of the seed.
 */
TLP_SCOPE void setSeedOfRandomSequence(uint seed = UINT_MAX);

/**
 * @brief Returns the value of the seed used for further initialization
 * of a random sequence
 */
TLP_SCOPE uint getSeedOfRandomSequence();

/**
 * @brief Initializes a random sequence with the seed previously set
 * Further calls to rand() or random() will return the elements of
 * that sequence
 */
TLP_SCOPE void initRandomSequence();

/**
 * @brief Returns C++11 random number generator.
 */
TLP_SCOPE std::mt19937 &getRandomNumberGenerator();

/**
 * @brief Returns a random integer in the range [0, max] if max is positive or in the range [max, 0]
 * if max is negative
 */
TLP_SCOPE int randomNumber(int max);

/**
 * @brief Returns a random unsigned integer in the range [0, max]
 */
TLP_SCOPE uint randomNumber(uint max);

/**
 * @brief Returns a random unsigned long in the range [0, max]
 */
TLP_SCOPE ulong randomNumber(ulong max);

/**
 * @brief Returns a random unsigned long long in the range [0, max]
 */
TLP_SCOPE ullong randomNumber(ullong max);

/**
 * @brief Returns a random double in the range [0, max]
 */
TLP_SCOPE double randomNumber(double max = 1.0);

/**
 * @brief Cross-platform function to stat a path on a filesystem.
 *
 * Its purpose is to support paths on windows platform containing non-ascii characters.
 *
 * @param pathname an utf-8 encoded string containing the path name to stat
 * @param buf a pointer to a tlp_stat_t structure (a typedef for struct stat)
 */
TLP_SCOPE int statPath(const std::string &pathname, tlp_stat_t *buf);

/**
 * @brief Cross-platform function to check if a path exists on a filesystem.
 *
 * Its purpose is to support paths on windows platform containing non-ascii characters.
 *
 * @param pathname an utf-8 encoded string containing the path to check
 */
TLP_SCOPE bool pathExists(const std::string &pathname);

/**
 * @brief Cross-platform function to get an input file stream.
 *
 * Its purpose is to support paths on windows platform containing non-ascii characters.
 *
 * @param filename an utf-8 encoded string containing the path of the file to open
 * for performing input operation
 * @param mode the stream opening mode flags (bitwise combination of
 * std::ios_base::openmode constants)
 */
TLP_SCOPE std::istream *getInputFileStream(const std::string &filename,
                                           std::ios_base::openmode mode = std::ios::in |
                                                                          std::ios::binary);

/**
 * @brief Cross-platform function to get an output file stream.
 *
 * Its purpose is to support paths on windows platform containing non-ascii characters.
 *
 * @param filename an utf-8 encoded string containing the path of the file to open
 * for performing output operations
 * @param mode the stream opening mode flags (bitwise combination of
 * std::ios_base::openmode constants)
 */
TLP_SCOPE std::ostream *getOutputFileStream(const std::string &filename,
                                            std::ios_base::openmode mode = std::ios::out |
                                                                           std::ios::binary);

/**
 * @brief Returns an input stream to read from a zlib compressed file
 * (uses gzstream lib, a C++ stream wrapper around zlib).
 *
 * The stream has to be deleted after use.
 *
 * @param filename the name of the file to read from
 * @return input stream for a zlib compressed file
 */
TLP_SCOPE std::istream *getZlibInputFileStream(const std::string &filename);

/**
 * @brief Returns an output stream to write to a zlib compressed file
 * (uses gzstream lib, a C++ stream wrapper around zlib).
 *
 * The stream has to be deleted after use.
 *
 * @param filename the name of the file to write to
 * @return output stream for a zlib compressed file
 */
TLP_SCOPE std::ostream *getZlibOutputFileStream(const std::string &filename);

/**
 * @brief Returns an input stream to read from a Zstandard compressed file
 * (uses ZstdStream, a C++ stream wrapper around zstd).
 *
 * The stream has to be deleted after use.
 *
 * @param filename the name of the file to read from
 * @return input stream for a Zstandard compressed file
 */
TLP_SCOPE std::istream *getZstdInputFileStream(const std::string &filename);

/**
 * @brief Returns an output stream to write to a Zstandard compressed file
 * (uses ZstdStream, a C++ stream wrapper around zstd).
 *
 * The stream has to be deleted after use.
 *
 * @param filename the name of the file to write to
 * @param compressionLevel zstd compression level to used (1 to 22)
 * @return output stream for a Zstandard compressed file
 */
TLP_SCOPE std::ostream *getZstdOutputFileStream(const std::string &filename,
                                                int compressionLevel = 3);

TLP_SCOPE std::string utf32to8(const std::u32string &s);

TLP_SCOPE std::wstring utf8to16(const std::string &s);

TLP_SCOPE std::vector<std::string> tokenize(const std::string &str,
                                            const std::string &delimiter = " ");

// Utility constexpr template function to get a typename at compile time.
// This is useful to discard execution branches for specific types in template implementation.
// https://stackoverflow.com/questions/35941045/can-i-obtain-c-type-names-in-a-constexpr-way

template <typename T>
constexpr std::string_view type_name();

template <>
constexpr std::string_view type_name<void>() {
  return "void";
}

using type_name_prober = void;

template <typename T>
constexpr std::string_view wrapped_type_name() {
#if defined(__clang__) || defined(__GNUC__)
  return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
  return __FUNCSIG__;
#else
#error "wrapped_type_name: Unsupported compiler"
#endif
}

constexpr std::size_t wrapped_type_name_prefix_length() {
  return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>());
}

constexpr std::size_t wrapped_type_name_suffix_length() {
  return wrapped_type_name<type_name_prober>().length() - wrapped_type_name_prefix_length() -
         type_name<type_name_prober>().length();
}

template <typename T>
constexpr std::string_view type_name() {
  constexpr auto wrapped_name = wrapped_type_name<T>();
  constexpr auto prefix_length = wrapped_type_name_prefix_length();
  constexpr auto suffix_length = wrapped_type_name_suffix_length();
  constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
  return wrapped_name.substr(prefix_length, type_name_length);
}

struct LesserString {
  bool operator()(const char *lhs, const char *rhs) const {
    return std::strcmp(lhs, rhs) < 0;
  }
};

struct HashString {
  std::size_t operator()(const char *arg) const {
    return std::hash<std::string>()(arg);
  }
};

struct EqualString {
  bool operator()(const char *lhs, const char *rhs) const {
    return !std::strcmp(lhs, rhs);
  }
};

}
#endif // TALIPOT_TLP_TOOLS_H
