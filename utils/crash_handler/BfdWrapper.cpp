/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include "BfdWrapper.h"

#include <iostream>
#include <vector>
#include <sys/stat.h>

#define INRANGE(foo, bar, baz) (foo(bar)) && ((bar)baz)

#ifdef __FreeBSD__
extern "C" int filename_ncmp(const char *s1, const char *s2, size_t n) {
  return strncmp(s1, s2, n);
}
#endif

using namespace std;

static asymbol **slurp_symtab(bfd *abfd, bool useMini, long *nSymbols, uint *symbolSize,
                              bool *isDynamic) {
  *nSymbols = 0;
  *symbolSize = 0;
  *isDynamic = false;
  asymbol **symbol_table = nullptr;

  if ((bfd_get_file_flags(abfd) & HAS_SYMS) == 0) {
    *nSymbols = 0;
    return symbol_table;
  }

  if (useMini) {
    *nSymbols =
        bfd_read_minisymbols(abfd, false, reinterpret_cast<void **>(&symbol_table), symbolSize);

    if (*nSymbols == 0) {
      *isDynamic = true;
      *nSymbols =
          bfd_read_minisymbols(abfd, true, reinterpret_cast<void **>(&symbol_table), symbolSize);
    }

    if (*nSymbols < 0) {
      cerr << "Error (bfd_read_minisymbols) in " << bfd_get_filename(abfd) << endl;
      return symbol_table;
    }
  } else {

    long storage_needed = bfd_get_symtab_upper_bound(abfd);

    if (storage_needed < 0) {
      cerr << "Error (bfd_get_symtab_upper_bound) slurping symbol table from "
           << bfd_get_filename(abfd) << endl;
      return symbol_table;
    }

    if (storage_needed != 0) {
      symbol_table = reinterpret_cast<asymbol **>(malloc(storage_needed));
    }

    *nSymbols = bfd_canonicalize_symtab(abfd, symbol_table);

    if (*nSymbols < 0) {
      cerr << "Error (bfd_canonicalize_symtab) slurping symbol table from : "
           << bfd_get_filename(abfd) << endl;
      return symbol_table;
    }
  }

  if (*nSymbols == 0) {
    cerr << "No symbols in " << bfd_get_filename(abfd) << endl;
  }

  return symbol_table;
}

int file_exist(const std::string &filename) {
  struct stat buffer;
  return (stat(filename.c_str(), &buffer) == 0);
}

#ifdef __MINGW32__
static DWORD GetModuleBase(DWORD dwAddress) {
  MEMORY_BASIC_INFORMATION Buffer;
#ifndef X86_64
  return VirtualQuery(reinterpret_cast<LPCVOID>(dwAddress), &Buffer, sizeof(Buffer))
             ? reinterpret_cast<DWORD>(Buffer.AllocationBase)
             : 0;
#else
  return VirtualQuery(reinterpret_cast<LPCVOID>(dwAddress), &Buffer, sizeof(Buffer))
             ? reinterpret_cast<DWORD64>(Buffer.AllocationBase)
             : 0;
#endif
}
#else

static void tokenize(const string &str, vector<string> &tokens, const string &delimiters = " ") {
  string::size_type lastPos = 0;
  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos) {
    tokens.push_back(str.substr(lastPos, pos - lastPos));

    if (pos != string::npos) {
      lastPos = pos + 1;
    } else {
      lastPos = string::npos;
    }

    pos = str.find_first_of(delimiters, lastPos);
  }
}

#endif

static bool bfdInit = false;

BfdWrapper::BfdWrapper(const char *dsoName)
    : filePath(dsoName), abfd(nullptr), textSection(nullptr), symbolTable(nullptr), nSymbols(0),
      symbolSize(0), isMini(true), isDynamic(false), scratchSymbol(nullptr), relocationOffset(-1) {

#ifndef __MINGW32__

  // try to find the absolute path of the shared library or executable
  if (!file_exist(filePath)) {

    // if no / character in the filename, it surely is an executable
    // whose location is in the PATH environment variable
    if (filePath.find('/') == string::npos) {
      const string PATH = getenv("PATH");
      vector<string> paths;

      tokenize(PATH, paths, ":");

      bool absolutePathFound = false;

      for (const auto &path : paths) {
        if (file_exist((path + "/" + filePath))) {
          filePath = path + "/" + filePath;
          absolutePathFound = true;
          break;
        }
      }

      if (!absolutePathFound) {
        cerr << "Can't find " << filePath << " in $PATH\n" << endl;
        return;
      }
    }

    string pwd(getenv("PWD"));

    // maybe it's a relative path otherwise
    if (file_exist(pwd + "/" + filePath)) {
      filePath = pwd + "/" + filePath;
    }
  }

#endif

  if (!bfdInit) {
    bfd_init();
    bfdInit = true;
  }

  abfd = bfd_openr(filePath.c_str(), nullptr);

  if (abfd == nullptr) {
    cerr << "Can't open file " << filePath << endl;
    return;
  }

  if (!bfd_check_format(abfd, bfd_object)) {
    cerr << "Can't open file " << bfd_get_filename(abfd) << endl;
    bfd_close(abfd);
    return;
  }

  textSection = bfd_get_section_by_name(abfd, ".text");

  if (textSection == nullptr) {
    cerr << "Can't find .text section in " << bfd_get_filename(abfd) << endl;
    bfd_close(abfd);
    return;
  }

  isMini = true;
  nSymbols = 0;
  symbolSize = 0;
  isDynamic = false;
  symbolTable = slurp_symtab(abfd, isMini, &nSymbols, &symbolSize, &isDynamic);

#ifdef bfd_get_section_flags
  if ((bfd_get_section_flags(abfd, textSection) & SEC_ALLOC) == 0) {
#else
  if ((bfd_section_flags(textSection) & SEC_ALLOC) == 0) {
#endif
    cerr << "SEC_ALLOC flag not set on .text section (whatever that means) in "
         << bfd_get_filename(abfd) << endl;
    free(symbolTable);
    bfd_close(abfd);
    return;
  }

  asymbol *scratchSymbol = bfd_make_empty_symbol(abfd);

  if (scratchSymbol == nullptr) {
    cerr << "Error (bfd_make_empty_symbol) in " << bfd_get_filename(abfd) << endl;
    free(symbolTable);
    bfd_close(abfd);
    return;
  }
}

BfdWrapper::~BfdWrapper() {
  if (symbolTable != nullptr) {
    free(symbolTable);
  }

  if (abfd != nullptr) {
    bfd_close(abfd);
  }
}

#ifndef __MINGW32__

pair<const char *, uint> BfdWrapper::getFileAndLineForAddress(const char *mangledSymbol,
                                                              const int64_t runtimeAddr,
                                                              const int64_t runtimeOffset) {
  pair<const char *, uint> ret = make_pair("", 0);

  if (!abfd || !isMini || symbolSize == 0) {
    return ret;
  }

  auto *from = reinterpret_cast<bfd_byte *>(symbolTable);
  bfd_byte *fromend = from + nSymbols * symbolSize;

  for (; from < fromend; from += symbolSize) {
    asymbol *sym = bfd_minisymbol_to_symbol(abfd, isDynamic, from, scratchSymbol);

    if (sym == nullptr) {
      cerr << "Error (bfd_minisymbol_to_symbol) in " << bfd_get_filename(abfd) << endl;
      return ret;
    }

    symbol_info syminfo;
    bfd_get_symbol_info(abfd, sym, &syminfo);

    if (syminfo.type == 'T' || syminfo.type == 'W') {
      bool matched = string(syminfo.name) == string(mangledSymbol);

      if (matched) {

        int64_t relocatedSymbolAddress = runtimeAddr - runtimeOffset;
        int64_t unrelocatedSymbolAddress = syminfo.value;

        relocationOffset = relocatedSymbolAddress - unrelocatedSymbolAddress;

        int64_t relocatedAddr = runtimeAddr;
        int64_t unrelocatedAddr = relocatedAddr;

        if (relocationOffset != -1) {
          unrelocatedAddr -= relocationOffset;
        }

        const char *funcName = nullptr;
        const char *fileName = nullptr;
        uint lineno = 0;

#ifdef bfd_get_section_vma
        bfd_vma textSection_vma = bfd_get_section_vma(abfd, textSection);
#else
        bfd_vma textSection_vma = bfd_section_vma(textSection);
#endif

#ifdef bfd_section_size
        bfd_size_type textSection_size = bfd_section_size(abfd, textSection);
#else
        bfd_size_type textSection_size = bfd_section_size(textSection);
#endif

        if (!INRANGE(static_cast<int64_t>(textSection_vma) <=, unrelocatedAddr,
                     <= static_cast<int64_t>(textSection_vma + textSection_size))) {
          cerr << "Trying to look up an address that's outside of the range of the text section of "
               << filePath
               << "... usually this means the executable or DSO in question has "
                  "changed since the stack trace was generated"
               << endl;
          return ret;
        }

        if (!bfd_find_nearest_line(abfd, textSection, symbolTable,
                                   unrelocatedAddr - textSection_vma - 1, &fileName, &funcName,
                                   &lineno)) {
          cerr << "Can't find line for address " << hex
               << static_cast<unsigned long long>(relocatedAddr) << " <- "
               << static_cast<unsigned long long>(unrelocatedAddr) << dec << endl;
          return ret;
        } else {
          if (fileName) {
            ret = make_pair(fileName, lineno);
          }

          return ret;
        }
      }
    }
  }

  return ret;
}

#else

pair<const char *, uint> BfdWrapper::getFileAndLineForAddress(const int64_t runtimeAddr) {

  pair<const char *, uint> ret = make_pair("", 0);

  if (!abfd)
    return ret;

  const char *funcName = "";
  const char *fileName = "";
  uint lineno = 0;

  int64_t symbolOffset = runtimeAddr - GetModuleBase(runtimeAddr) - 0x1000 - 1;
#ifdef bfd_section_size
  bfd_size_type textSection_size = bfd_section_size(abfd, textSection);
#else
  bfd_size_type textSection_size = bfd_section_size(textSection);
#endif

  if (!INRANGE(static_cast<int64_t>(0) <=, symbolOffset,
               <= static_cast<int64_t>(textSection_size))) {
    cerr << "Trying to look up an address that's outside of the range of the text section of "
         << filePath
         << "... usually this means the executable or DSO in question has changed "
            "since the stack trace was generated"
         << endl;
  } else {
    bfd_find_nearest_line(abfd, textSection, symbolTable, symbolOffset, &fileName, &funcName,
                          &lineno);
  }

  ret = make_pair(fileName, lineno);
  return ret;
}

const char *BfdWrapper::getFunctionForAddress(const int64_t runtimeAddr) {

  const char *funcName = "";
  const char *fileName = "";
  uint lineno = 0;

  if (!abfd)
    return funcName;

  int64_t symbolOffset = runtimeAddr - GetModuleBase(runtimeAddr) - 0x1000 - 1;
#ifdef bfd_section_size
  bfd_size_type textSection_size = bfd_section_size(abfd, textSection);
#else
  bfd_size_type textSection_size = bfd_section_size(textSection);
#endif

  if (!INRANGE(static_cast<int64_t>(0) <=, symbolOffset,
               <= static_cast<int64_t>(textSection_size))) {
    cerr << "Trying to look up an address that's outside of the range of the text section of "
         << filePath
         << "... usually this means the executable or DSO in question has changed "
            "since the stack trace was generated"
         << endl;
    return funcName;
  }

  bfd_find_nearest_line(abfd, textSection, symbolTable, symbolOffset, &fileName, &funcName,
                        &lineno);
  return funcName;
}

#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
