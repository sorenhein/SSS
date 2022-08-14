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
#include "CoverCategory.h"

mutex mtxRowStore;


RowStore::RowStore()
{
  RowStore::reset();
}


void RowStore::reset()
{
  store.clear();
}


void RowStore::addDirectly(
  list<Cover const *>& coverPtrs,
  const vector<unsigned char>& cases)
{
  lock_guard<mutex> lg(mtxRowStore);

  // Make a CoverRow.
  rowScratch.reset();
  rowScratch.resize(cases.size());
  rowScratch.fillDirectly(coverPtrs, cases);

  // Store it.
  auto result = store.insert(rowScratch);
  assert(result.first != store.end());
}


const CoverRow& RowStore::add(
  const Cover& cover,
  const Tricks& additions,
  const unsigned rawWeightAdder,
  Tricks& residuals)
{
  lock_guard<mutex> lg(mtxRowStore);

  // Make a CoverRow.
  rowScratch.reset();
  rowScratch.resize(residuals.size());
  rowScratch.add(cover, additions, rawWeightAdder, residuals, 
    VERBAL_GENERAL);

  // Store it.
  auto result = store.insert(rowScratch);
  assert(result.first != store.end());

  return * result.first;
}

