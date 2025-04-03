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

#ifndef TALIPOT_BMD_LINK_H
#define TALIPOT_BMD_LINK_H

template <typename TYPE>
class BmdList;
template <typename TYPE>
class BmdListTest;

namespace tlp {
template <typename TYPE>
class BmdLink {

    friend class BmdList<TYPE>;
    friend class BmdListTest<TYPE>;

  public:
    TYPE getData() {
        return data;
    }
    BmdLink *prev() {
        return pre;
    }
    BmdLink *succ() {
        return suc;
    }
    // protected:
    TYPE data;
    BmdLink *pre;
    BmdLink *suc;
    BmdLink(TYPE a, BmdLink<TYPE> *pre, BmdLink<TYPE> *suc);
};
}
#include <talipot/cxx/BmdLink.cxx>

#endif // TALIPOT_BMD_LINK_H
