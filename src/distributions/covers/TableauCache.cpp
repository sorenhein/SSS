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
  const Tricks& excessTricks,
  const CoverTableau& tableau)
{
  entries.emplace_back(CacheEntry());
  CacheEntry& entry = entries.back();

  entry.excessTricks = excessTricks;
  entry.tableau = tableau;
  entry.count = 1;
}


bool TableauCache::lookup(
  const Tricks& excessTricks,
  CoverTableau const *& tableauPtr)
{
  for (auto& entry: entries)
  {
    if (excessTricks == entry.excessTricks)
    {
      // TODO This could surely be sped up, e.g. storing them in
      // a set by weight.  I don't know if timing is a problem here.
      tableauPtr = &entry.tableau;
      entry.count++;
      return true;
    }
  }
  return false;
}


unsigned TableauCache::size() const
{
  return entries.size();
}


void TableauCache::getCounts(
  unsigned& numTableaux,
  unsigned& numUses) const
{
  numTableaux = entries.size();

  numUses = 0;
  for (auto& entry: entries)
    numUses += entry.count;
}


string TableauCache::str() const
{
  stringstream ss;

  for (auto& entry: entries)
    ss << entry.str();

  return ss.str();
}

