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

#include <talipot/SystemDefinition.h>
#include <talipot/Release.h>
#include <talipot/config.h>

#include <fstream>

#include "CrashHandler.h"
#include "StackWalker.h"

using namespace std;

static std::string TALIPOT_DUMP_FILE;

void CrashHandler::setDumpPath(string s) {
  TALIPOT_DUMP_FILE = s;
}

#ifdef _MSC_VER

static std::string SYMBOLS_SEARCH_PATHS = "";

void CrashHandler::setExtraSymbolsSearchPaths(const std::string &searchPaths) {
  SYMBOLS_SEARCH_PATHS = searchPaths;
}

#endif

static void dumpStackTrace(StackWalker &sw) {
  std::ostream *os = &std::cerr;
  std::ofstream ofs;

  if (!TALIPOT_DUMP_FILE.empty()) {
    ofs.open(TALIPOT_DUMP_FILE.c_str());

    if (!ofs.is_open()) {
      std::cerr << "Could not open " << TALIPOT_DUMP_FILE << std::endl;
    } else {
      os = &ofs;
      std::cerr << "Writing dump stack to " << TALIPOT_DUMP_FILE << std::endl;
    }
  }

  *os << TLP_PLATEFORM_HEADER << " " << OS_PLATFORM << std::endl
      << TLP_ARCH_HEADER << " " << OS_ARCHITECTURE << std::endl
      << TLP_COMPILER_HEADER << " " << OS_COMPILER << std::endl
      << TLP_VERSION_HEADER << " " << TALIPOT_VERSION << std::endl;
  *os << TLP_STACK_BEGIN_HEADER << std::endl;
  sw.printCallStack(*os, 50);
  *os << TLP_STACK_END_HEADER << std::endl;
  *os << std::flush;

  if (ofs.is_open()) {
    ofs.close();
  }
}

/*
  Linux/MacOS-specific handling
 */
#if defined(__unix__) || defined(__APPLE__)

#if defined(__i386__) || defined(__amd64__) || defined(__arm64__)

#include "UnixSignalInterposer.h"

#if defined(__APPLE__)
#include <sys/ucontext.h>
#elif defined(__FreeBSD__)
#include <sys/ucontext.h>
typedef ucontext_t sig_ucontext_t;
#else

// This structure mirrors the one found in /usr/include/asm/ucontext.h
typedef struct _sig_ucontext {
  unsigned long uc_flags;
  struct ucontext *uc_link;
  stack_t uc_stack;
  struct sigcontext uc_mcontext;
  sigset_t uc_sigmask;
} sig_ucontext_t;

#endif

void dumpStack(int sig, siginfo_t *, void *ucontext) {

  // Get the address at the time the signal was raised from the EIP (x86) or RIP (x86_64)

#ifndef __APPLE__

  auto *uc = reinterpret_cast<sig_ucontext_t *>(ucontext);
#if defined(__i386__)
#ifdef __FreeBSD__
  void *callerAddress = reinterpret_cast<void *>(uc->uc_mcontext.mc_eip); // x86 specific;
#else
  void *callerAddress = reinterpret_cast<void *>(uc->uc_mcontext.eip); // x86 specific;
#endif
#else
#ifdef __FreeBSD__
  void *callerAddress = reinterpret_cast<void *>(uc->uc_mcontext.mc_rip); // x86_64 specific;
#else
  void *callerAddress = reinterpret_cast<void *>(uc->uc_mcontext.rip); // x86_64 specific;
#endif
#endif

#else

  ucontext_t *uc = reinterpret_cast<ucontext_t *>(ucontext);
#if defined(__i386__)
  void *callerAddress = reinterpret_cast<void *>(uc->uc_mcontext->__ss.__eip);
#elif defined(__amd64__)
  void *callerAddress = reinterpret_cast<void *>(uc->uc_mcontext->__ss.__rip);
#elif defined(__arm64__)
  void *callerAddress = reinterpret_cast<void *>(uc->uc_mcontext->__ss.__lr);
#endif

#endif

  StackWalkerGCC sw;
  sw.setCallerAddress(callerAddress);

  dumpStackTrace(sw);

  installSignalHandler(sig, SIG_DFL);
}

extern void installSignalHandlers(void);

void CrashHandler::install() {
  installSignalHandler(SIGSEGV, &dumpStack);
  installSignalHandler(SIGABRT, &dumpStack);
  installSignalHandler(SIGFPE, &dumpStack);
  installSignalHandler(SIGILL, &dumpStack);
  installSignalHandler(SIGBUS, &dumpStack);
}

#else
// architecture not supported
void CrashHandler::install() {}
#endif

/*
  MinGW-specific handling
 */
#elif defined(__MINGW32__)

static LONG WINAPI exception_filter(LPEXCEPTION_POINTERS info) {
  StackWalkerMinGW sw;
  sw.setContext(info->ContextRecord);

  dumpStackTrace(sw);

  return 1;
}

void CrashHandler::install() {
  SetUnhandledExceptionFilter(exception_filter);
}

#elif defined(_MSC_VER)

static LONG WINAPI exception_filter(LPEXCEPTION_POINTERS info) {
  StackWalkerMSVC sw;
  sw.setExtraSymbolsSearchPaths(SYMBOLS_SEARCH_PATHS);
  sw.setContext(info->ContextRecord);

  dumpStackTrace(sw);

  return 1;
}

void CrashHandler::install() {
  SetUnhandledExceptionFilter(exception_filter);
}
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
