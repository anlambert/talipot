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

#ifndef TALIPOT_MEMORY_POOL_H
#define TALIPOT_MEMORY_POOL_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>

#include <talipot/ParallelTools.h>

static const size_t BUFFOBJ = 20;

namespace tlp {
/**
 * @class MemoryPool
 * \brief That class enables to easily create a memory pool for an a class
 *
 * It allocates chunk of BUFFOBJ size of continuous memory to allocate
 * instance of the class. After a delete the memory is not free, and
 * will be reused at the next new of the class.
 *
 * @warning it is not recommended to inherit from an object that inherit of that class
 *
 * The following  code only calls malloc one time even if NBTRY object are created
 * in that example, the speedup is about 23, without MemoryPool malloc is called NBTRY times
 * @code
 * class A : public MemoryPool<A> {
 * public:
 *     A(){}
 *    ~A(){}
 *    int data[1000];
 * };
 *
 * size_t NBTRY = 1000 * 1000;
 * for (size_t j=0; j < NBTRY; ++j) {
 *    A *a = new A();
 *    a->data[100] = j;
 *    r1 += a->data[100];
 *    delete a;
 * }
 * @endcode
 *
 */
template <typename TYPE>
class MemoryPool {
  public:
    MemoryPool() = default;

#ifndef NDEBUG
    void *operator new(size_t sizeofObj) {
#else
    void *operator new(size_t) {
#endif
        assert(sizeof(TYPE) == sizeofObj); // to prevent inheritance with different size of object
        TYPE *t;
        t = _memoryChunkManager.getObject();
        return t;
    }

    void operator delete(void *p) {
        _memoryChunkManager.releaseObject(p);
    }

  private:
    class MemoryChunkManager {
      public:
        ~MemoryChunkManager() {
            for (const auto &chunks : _allocatedChunks) {
                for (auto *chunk : chunks) {
                    free(chunk);
                }
            }
        }

        TYPE *getObject() {
            uint threadId = tlp::ThreadManager::getThreadNumber();
            TYPE *result = nullptr;

            if (_freeObject[threadId].empty()) {
                void *chunk = malloc(BUFFOBJ * sizeof(TYPE));
                TYPE *p = static_cast<TYPE *>(chunk);
                _allocatedChunks[threadId].push_back(chunk);

                for (size_t j = 0; j < BUFFOBJ - 1; ++j) {
                    _freeObject[threadId].push_back(static_cast<void *>(p));
                    p += 1;
                }

                result = p;
            } else {
                result = static_cast<TYPE *>(_freeObject[threadId].back());
                _freeObject[threadId].pop_back();
            }

            return result;
        }

        void releaseObject(void *p) {
            uint threadId = tlp::ThreadManager::getThreadNumber();
            _freeObject[threadId].push_back(p);
        }

      private:
        std::vector<void *> _allocatedChunks[TLP_MAX_NB_THREADS];
        std::vector<void *> _freeObject[TLP_MAX_NB_THREADS];
    };

    static MemoryChunkManager _memoryChunkManager;
};

template <typename TYPE>
typename MemoryPool<TYPE>::MemoryChunkManager MemoryPool<TYPE>::_memoryChunkManager;
}
#endif // TALIPOT_MEMORY_POOL_H
