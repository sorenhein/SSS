/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverStore.h"


CoverStore::CoverStore()
{
  CoverStore::reset();
}


void CoverStore::reset()
{
  store.clear();
}


void CoverStore::eliminate(set<Cover>::iterator& itMatch)
{
  // The tricks should be unique.  First check towards the end.
  // There can be at most one duplicate.
  auto itForward = itMatch;
  while (++itForward != store.end())
  {
    if (! itMatch->sameWeight(* itForward))
      break;

    if (itMatch->sameTricks(* itForward))
    {
      store.erase(itForward);
      return;
    }
  }

  // If there is an earlier match, the current one is erased.
  auto itBackward = itMatch;
  while (itBackward != store.begin())
  {
    itBackward--;
    if (! itMatch->sameWeight(* itBackward))
      return;

    if (itMatch->sameTricks(* itBackward))
    {
      store.erase(itMatch);
      return;
    }
  }
}


void CoverStore::add(
  ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& productPair,
  const bool symmFlag,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  // Make a Cover.
  coverScratch.reset();
  coverScratch.set(productMemory, sumProfile, productPair, symmFlag);

  // Make its tricks and counts.
  coverScratch.prepare(distProfiles, cases);
  if (coverScratch.empty() || coverScratch.full())
    return;

  // Store it in "store".
  auto result = store.insert(coverScratch);
  assert(result.first != store.end());

  // In Covers we generate covers using ProfilePair.
  // Assuming that the eliminations (in particular in active()) are
  // done comprehensively, there is nothing left to eliminate here.

  // CoverStore::eliminate(result.first);
}


const Cover& CoverStore::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.

  auto it = store.find(cover);
  assert(it != store.end());

  return * it;
}


const Cover& CoverStore::lookup(
  const ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& productPair,
  const bool symmFlag)
{
  // This is const except for coverScratch being modified.

  coverScratch.setExisting(
    productMemory, sumProfile, productPair, symmFlag);

  return CoverStore::lookup(coverScratch);
}


set<Cover>::const_iterator CoverStore::begin() const
{
  return store.begin();
}


set<Cover>::const_iterator CoverStore::end() const
{
  return store.end();
}


unsigned CoverStore::size() const
{
  return store.size();
}

