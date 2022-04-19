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
  symmetricCache.clear();
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
      // Generally erase the later one.  But if the earlier one is
      // symmetric and the later one isn't, keep the later one.
      if (itMatch->symmetric() && ! itForward->symmetric())
        store.erase(itMatch);
      else
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
      // The earlier one cannot be symmetric.
      store.erase(itMatch);
      return;
    }
  }
}


void CoverStore::add(
  ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& productPair,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  // Make a Cover with symmFlag == false.
  coverScratch.reset();
  coverScratch.set(productMemory, sumProfile, productPair, false);

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
  
  // Discard some symmmetric versions.
  if (! coverScratch.symmetrizable(sumProfile))
    return;

  if (! coverScratch.symmetrize(cases))
    return;

  symmetricCache.push_back(coverScratch);
}


void CoverStore::admixSymmetric()
{
  /*
  cout << "Non-symmetrics are:\n";
  for (auto& c: store)
    cout << c.strLine();

  cout << "\nSymmetrics are:\n";
  for (auto& c: symmetricCache)
    cout << c.strLine();
    */

  for (auto& cover: symmetricCache)
  {
    auto result = store.insert(cover);
    assert(result.first != store.end());

    // These eliminations are too tough to do without search.
    CoverStore::eliminate(result.first);
  }

  symmetricCache.clear();

  /*
  cout << "Non-symmetrics are now:\n";
  for (auto& c: store)
    cout << c.strLine();
    */

}


const Cover& CoverStore::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.

  auto it = store.find(cover);
if (it == store.end())
{
  cout << cover.strLine();
  cout << "Covers are:\n";
  for (auto& c: store)
    cout << c.strLine();

  assert(it != store.end());
}

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


string CoverStore::str() const
{
  if (store.empty())
    return "";

  stringstream ss;
  ss << store.begin()->strHeader();

  for (auto& cover: store)
    ss << cover.strLine();

  return ss.str() + "\n";
}

