/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_ROWSTORE_H
#define SSS_ROWSTORE_H

#include <set>
// #include <list>
// #include <string>

#include "CoverRow.h"

using namespace std;


class RowStore
{
  private:

    set<CoverRow> store;

    // To avoid creating a temporary one every time.
    CoverRow rowScratch;


  public:

    RowStore();

    void reset();

    void addDirectly(
      list<Cover const *>& coverPtrs,
      const vector<unsigned char>& cases);

    const CoverRow& add(
      const Cover& cover,
      const Tricks& additions,
      const unsigned rawWeightAdder,
      Tricks& residuals);

    // void sort();

    set<CoverRow>::const_iterator begin() const { return store.begin(); };

    set<CoverRow>::const_iterator end() const { return store.end(); };
};

#endif
