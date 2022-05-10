/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "RowStore.h"

mutex mtxRowStore;


RowStore::RowStore()
{
  RowStore::reset();
}


void RowStore::reset()
{
  store.clear();
}


CoverRow& RowStore::add()
{
  lock_guard<mutex> lg(mtxRowStore);

  store.emplace_back(CoverRow());
  return store.back();

}


void RowStore::sort()
{
  store.sort();
}

