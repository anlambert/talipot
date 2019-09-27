/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */
#include <tulip/IdManager.h>
#include <climits>
#include <cassert>
using namespace std;

namespace tlp {

//-----------------------------------------------------------
bool IdManager::is_free(const unsigned int id) const {
  if (id < state.firstId)
    return true;

  if (id >= state.nextId)
    return true;

  if (state.freeIds.find(id) != state.freeIds.end())
    return true;

  return false;
}
//-----------------------------------------------------------
void IdManager::free(const unsigned int id) {
  if (id < state.firstId)
    return;

  if (id >= state.nextId)
    return;

  if (state.freeIds.find(id) != state.freeIds.end())
    return;

  if (state.firstId == state.nextId)
    return;

  if (id == state.firstId) {
    for (;;) {
      set<unsigned int>::iterator it = state.freeIds.find(++state.firstId);

      if (it == state.freeIds.end())
        break;

      state.freeIds.erase(it);
    }

    // At that point, it means that all ids have been freed.
    // So we can safely reset the first and next id to 0.
    if (state.firstId == state.nextId) {
      state.firstId = state.nextId = 0;
    }
  } else
    state.freeIds.insert(id);
}
//-----------------------------------------------------------
#ifndef TLP_NO_IDS_REUSE
unsigned int IdManager::getFreeId() {
  std::set<unsigned int>::iterator it = state.freeIds.begin();
  assert(it != state.freeIds.end());
  unsigned int tmp = *it;
  state.freeIds.erase(it);
  return tmp;
}
#endif
//-----------------------------------------------------------
void IdManager::getFreeId(unsigned int id) {
  assert(id > state.firstId);

  if (id >= state.nextId) {
    if (state.firstId == state.nextId)
      state.firstId = id;
    else {
      for (; state.nextId < id; ++state.nextId)
        state.freeIds.insert(state.nextId);
    }

    state.nextId = id + 1;
  } else {
    assert(state.freeIds.find(id) != state.freeIds.end());
    state.freeIds.erase(state.freeIds.find(id));
  }
}
} // namespace tlp

//-----------------------------------------------------------
ostream &tlp::operator<<(std::ostream &os, const tlp::IdManager &idM) {
  os << endl << "--------------------------------------" << endl;
  os << "Id Manager Information :" << endl;
  os << "Minimum index :" << idM.state.firstId << endl;
  os << "Maximum index :" << idM.state.nextId - 1 << endl;
  os << "Size          :" << idM.state.freeIds.size() << endl;
  os << "Fragmentation :"
     << double(idM.state.freeIds.size()) / (1 + idM.state.nextId - idM.state.firstId) << endl;
  return os;
}
