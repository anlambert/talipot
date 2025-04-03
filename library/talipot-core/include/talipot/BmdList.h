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

#ifndef TALIPOT_BMD_LIST_H
#define TALIPOT_BMD_LIST_H

#include <cassert>
#include <talipot/BmdLink.h>

namespace tlp {
template <typename TYPE>
class BmdList {
  public:
    typedef tlp::BmdLink<TYPE> BMDTYPE;

    BmdList();
    virtual ~BmdList();
    BMDTYPE *firstItem();
    BMDTYPE *lastItem();
    TYPE entry(BMDTYPE *it);
    int size();
    BMDTYPE *nextItem(BMDTYPE *p, BMDTYPE *predP);
    BMDTYPE *predItem(BMDTYPE *p, BMDTYPE *succP);
    BMDTYPE *cyclicPred(BMDTYPE *it, BMDTYPE *succIt);
    BMDTYPE *cyclicSucc(BMDTYPE *it, BMDTYPE *predIt);
    BMDTYPE *push(const TYPE &a);
    BMDTYPE *append(const TYPE &a);
    TYPE delItem(BMDTYPE *it);
    TYPE pop();
    TYPE popBack();
    void reverse();
    void conc(BmdList<TYPE> &l);
    void clear();
    void swap(BmdList<TYPE> &l);

  private:
    BMDTYPE *head;
    BMDTYPE *tail;
    int count;
};

#include <talipot/cxx/BmdList.cxx>

template <typename TYPE>
struct BmdListIt : public Iterator<TYPE> {

    BmdListIt(BmdList<TYPE> &bmdList) : bmdList(bmdList) {
        pos = bmdList.firstItem();
        pred = nullptr;
    }
    bool hasNext() override {
        return pos != nullptr;
    }
    TYPE next() override {
        TYPE val = pos->getData();
        tlp::BmdLink<TYPE> *tmp = pos;
        pos = bmdList.nextItem(pos, pred);
        pred = tmp;
        return val;
    }

  private:
    tlp::BmdLink<TYPE> *pos;
    tlp::BmdLink<TYPE> *pred;
    BmdList<TYPE> &bmdList;
};

template <typename TYPE>
struct BmdListRevIt : public Iterator<TYPE> {
    BmdListRevIt(BmdList<TYPE> &bmdList) : bmdList(bmdList) {
        pos = bmdList.lastItem();
        suc = nullptr;
    }
    bool hasNext() override {
        return pos != nullptr;
    }
    TYPE next() override {
        TYPE val = pos->getData();
        tlp::BmdLink<TYPE> *tmp = pos;
        pos = bmdList.predItem(pos, suc);
        suc = tmp;
        return val;
    }

  private:
    tlp::BmdLink<TYPE> *pos;
    tlp::BmdLink<TYPE> *suc;
    BmdList<TYPE> &bmdList;
};
}
#endif // TALIPOT_BMD_LIST_H
