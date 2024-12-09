/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/ParallelTools.h>

#ifdef TLP_NO_THREADS
// the header below is needed
// to have access to std::ignore
#include <tuple>

#else

#ifdef _OPENMP

#include <omp.h>

#ifdef __APPLE__

#include <cstdlib>

bool tlp::OpenMPLock::_canUseLock = true;
tlp::OpenMPLock::OpenMPLock() : _lock(nullptr) {
  if (_canUseLock) {
    _lock = new omp_lock_t;
    omp_init_lock(_lock);
  }
}
tlp::OpenMPLock::~OpenMPLock() {
  if (_canUseLock) {
    omp_destroy_lock(_lock);
  }
  delete _lock;
}
void tlp::OpenMPLock::lock() {
  if (_canUseLock) {
    omp_set_lock(_lock);
  }
}
void tlp::OpenMPLock::unlock() {
  if (_canUseLock) {
    omp_unset_lock(_lock);
  }
}
void tlp::OpenMPLock::exitHandler() {
  _canUseLock = false;
}
void tlp::OpenMPLock::registerExitHandler() {
  atexit(tlp::OpenMPLock::exitHandler);
}

#endif // __APPLE__

struct OpenMPDefaultOptions {
  OpenMPDefaultOptions() {
    int num_threads = omp_get_num_procs();
    auto *num_threads_ptr = getenv("OMP_NUM_THREADS");
    if (num_threads_ptr) {
      num_threads = atoi(num_threads_ptr);
    }
    omp_set_num_threads(num_threads);
  }
};

static OpenMPDefaultOptions openMpDefaultOptions;

#else

#include <condition_variable>
#include <talipot/hash.h>
#include <talipot/IdManager.h>

#endif

#endif

namespace tlp {

#ifndef TLP_NO_THREADS
#ifdef _OPENMP
uint tlp::ThreadManager::numberOfProcs(omp_get_num_procs());
uint tlp::ThreadManager::maxNumberOfThreads(omp_get_num_procs());
#else
uint ThreadManager::numberOfProcs(std::thread::hardware_concurrency());
uint ThreadManager::maxNumberOfThreads(std::thread::hardware_concurrency());
#endif
#else
uint ThreadManager::numberOfProcs(1);
uint ThreadManager::maxNumberOfThreads(1);
#endif

uint ThreadManager::getNumberOfProcs() {
  return numberOfProcs;
}

uint ThreadManager::getNumberOfThreads() {
  return maxNumberOfThreads;
}

void ThreadManager::setNumberOfThreads(uint nbThreads) {
#ifndef TLP_NO_THREADS
  maxNumberOfThreads = std::min(nbThreads, uint(TLP_MAX_NB_THREADS));
#ifdef _OPENMP
  omp_set_num_threads(maxNumberOfThreads);
#endif
#else
  std::ignore = nbThreads;
#endif
}

#if !defined(TLP_NO_THREADS) && !defined(_OPENMP)

// the manager of the thread associated number
// which must be in the range between 0 and maxNumberOfThreads
// 0 is reserved to the main thread
static IdContainer<uint> tNumManager;
// a mutex to ensure serialisation when allocating the thread number
static std::mutex tNumMtx;
// the global map used to register the thread number
static flat_hash_map<std::thread::id, uint> tNumMap;

void ThreadManager::allocateThreadNumber() {
  // exclusive access to tNumManager
  tNumMtx.lock();
  // 0 is reserved for main thread
  auto num = tNumManager.add() + 1;
  auto tNum = std::this_thread::get_id();
  tNumMap[tNum] = num;
  tNumMtx.unlock();
}

void ThreadManager::freeThreadNumber() {
  // exclusive access to tNumManager
  tNumMtx.lock();
  auto tNum = std::this_thread::get_id();
  uint num = tNumMap[tNum];
  tNumMap.erase(tNum);
  assert(num > 0);
  tNumManager.free(num - 1);
  tNumMtx.unlock();
}

#endif

uint ThreadManager::getThreadNumber() {
#ifndef TLP_NO_THREADS
#ifdef _OPENMP
  return omp_get_thread_num();
#else
  tNumMtx.lock();

  if (const auto it = tNumMap.find(std::this_thread::get_id()); it != tNumMap.end()) {
    tNumMtx.unlock();
    return it->second;
  }
  tNumMtx.unlock();
#endif
#endif
  return 0;
}
}
