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

#include "TableauCache.h"

mutex mtxTableauCache;


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
  mtxTableauCache.lock();

  entries.emplace_back(CacheEntry());
  CacheEntry& entry = entries.back();

  mtxTableauCache.unlock();

  entry.excessTricks = excessTricks;
  entry.tableau = tableau;
  entry.count = 1;
}


bool TableauCache::lookup(
  const Tricks& excessTricks,
  CoverTableau& solution)
{
  for (auto& entry: entries)
  {
    if (excessTricks == entry.excessTricks)
    {
      // This is not very time-consuming in practice compared to the
      // rest of the covers, but it could of course be made faster by
      // sorting on trick weight and/or hashing on the trick
      // signatures.
      solution = entry.tableau;

      mtxTableauCache.lock();
      entry.count++;
      mtxTableauCache.unlock();

      return true;
    }
  }
  return false;
}


size_t TableauCache::size() const
{
  return entries.size();
}


void TableauCache::getCounts(
  size_t& numTableaux,
  size_t& numUses) const
{
  numTableaux = entries.size();

  numUses = 0;
  for (auto& entry: entries)
    numUses += entry.count;
}


string TableauCache::str(
  const Profile& sumProfile,
  const RanksNames& ranksNames) const
{
  stringstream ss;

  for (auto& entry: entries)
    ss << entry.str(sumProfile, ranksNames);

  return ss.str();
}

