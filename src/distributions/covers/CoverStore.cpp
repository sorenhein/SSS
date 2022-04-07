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

#include "ProductMemory.h"
#include "Profile.h"
#include "ProfilePair.h"


CoverStore::CoverStore()
{
  CoverStore::reset();
}


void CoverStore::reset()
{
  store.clear();
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
  // TODO May be able to use emplace?
  Cover cover;
  cover.set(productMemory, sumProfile, productPair, symmFlag);

  // Make its tricks and counts.
  cover.prepare(distProfiles, cases);
  if (cover.empty() || cover.full())
    return;

  // Store it in "store".
  auto result = store.insert(cover);
  assert(result.first != store.end());

  // The tricks should be unique.  First check towards the end.
  // There can be at most one duplicate.
  auto itForward = result.first;
  while (++itForward != store.end())
  {
    if (! result.first->sameWeight(* itForward))
      break;

    if (result.first->sameTricks(* itForward))
    {
      store.erase(itForward);
      return;
    }
  }

  auto itBackward = result.first;
  while (itBackward != store.begin())
  {
    itBackward--;
    if (! result.first->sameWeight(* itBackward))
      return;

    if (result.first->sameTricks(* itBackward))
    {
      store.erase(result.first);
      return;
    }
  }
}


const Cover& CoverStore::lookup(
  ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& productPair,
  const bool symmFlag) const
{
  Cover cover;
  cover.set(productMemory, sumProfile, productPair, symmFlag);

  auto it = store.find(cover);

  // This method assumes that the entry already exists.
  if (it == store.end())
    assert(false);

  return * it;
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

