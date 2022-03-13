/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "TableauCache.h"


TableauCache::TableauCache()
{
  TableauCache::reset();
}


void TableauCache::reset()
{
  entries.clear();
}


void TableauCache::store(
  const vector<unsigned char>& excessTricksIn,
  const CoverTableau& tableau)
{
  entries.emplace_back(CacheEntry());
  CacheEntry& entry = entries.back();

  entry.excessTricks = excessTricksIn;
  entry.tableau = tableau;
}


bool TableauCache::lookup(
  const vector<unsigned char>& excessTricksIn,
  CoverTableau const *& tableauPtr) const
{
  for (auto& entry: entries)
  {
    bool diffFlag = false;
    assert(excessTricksIn.size() == entry.excessTricks.size());

    auto iter1 = excessTricksIn.begin();
    auto iter2 = entry.excessTricks.begin();
    while (iter1 != excessTricksIn.end())
    {
      if (* iter1 != * iter2)
      {
        diffFlag = true;
        break;
      }
      iter1++;
      iter2++;
    }

    if (! diffFlag)
    {
      tableauPtr = &entry.tableau;
      return true;
    }
  }
  return false;
}


unsigned TableauCache::size() const
{
  return entries.size();
}


string TableauCache::str() const
{
  stringstream ss;

  for (auto& entry: entries)
    ss << entry.str();

  return ss.str();
}

