/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_ROWSTORE_H
#define SSS_ROWSTORE_H

// #include <set>
#include <list>
// #include <string>

#include "CoverRow.h"

using namespace std;


class RowStore
{
  private:

    list<CoverRow> store;


  public:

    RowStore();

    void reset();

    CoverRow& add();

    void sort();

    list<CoverRow>::const_iterator begin() const { return store.begin(); };

    list<CoverRow>::const_iterator end() const { return store.end(); };
};

#endif
