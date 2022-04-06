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
  Cover cover;
  cover.set(productMemory, sumProfile, productPair, symmFlag);

  // Make its tricks and counts.
  cover.prepare(distProfiles, cases);

  // Store it in "store".
  store[cover] = 1;
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

  return it->first;
}


map<Cover, unsigned>::const_iterator CoverStore::begin() const
{
  return store.begin();
}


map<Cover, unsigned>::const_iterator CoverStore::end() const
{
  return store.end();
}

