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

#include "CoverStore.h"
#include "CoverRow.h"

#include "Tricks.h"
#include "Explain.h"

#include "heuristic/Partial.h"

#include "product/ProfilePair.h"

mutex mtxCoverStore;


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
      // symmetrized and the later one isn't, keep the later one.
      if (itMatch->symmetrized() && ! itForward->symmetrized())
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
  const ProfilePair& profilePair,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  lock_guard<mutex> lg(mtxCoverStore);

  // The productPair may already be symmetric with respect to
  // sumProfile.
  const bool symmetricFlag = profilePair.symmetricAgainst(sumProfile);

  // Make a Cover.  It may be symmetric.  It will not ever be symmetrized.
  coverScratch.reset();
  coverScratch.set(productMemory, sumProfile, profilePair, 
    symmetricFlag, false);

  // Make its tricks and counts.
  if (! coverScratch.setByProduct(distProfiles, cases))
    return;

  // Store it in "store".
  auto result = store.insert(coverScratch);
  assert(result.first != store.end());

  if (symmetricFlag)
  {
    // Nothing to symmetrize.
    return;
  }

  // In Covers we generate covers using ProfilePair.
  // Assuming that the eliminations (in particular in active()) are
  // done comprehensively, there is nothing left to eliminate here.
  // If we needed it:
  // CoverStore::eliminate(result.first);
  
  // Discard some symmmetric versions.
  if (! coverScratch.symmetrizable(sumProfile))
    return;

  if (! coverScratch.symmetrize())
    return;

  symmetricCache.push_back(coverScratch);
}


void CoverStore::admixSymmetric()
{
  // The symmetrics are mixed into the set in the right places.
  // This may lead to eliminations that were too hard to recognize
  // when we made the symmetrics.

  lock_guard<mutex> lg(mtxCoverStore);

  for (auto& cover: symmetricCache)
  {
    auto result = store.insert(cover);
    assert(result.first != store.end());

    CoverStore::eliminate(result.first);
  }

  symmetricCache.clear();
}


const Cover& CoverStore::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.

  auto it = store.find(cover);
  assert(it != store.end());
  return * it;
}


void CoverStore::heaviestPartial(
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  const Explain& explain,
  Partial& partial) const
{
  // This find the best (highest-weight) cover consistent with explain.
  // For example, it can find the length-only or tops-only winner.
  // Note that coverPtr == nullptr if nothing is found, which is
  // indeed possible (e.g. 8/4894, Strategy #1).

  Tricks additions;
  additions.resize(tricks.size());
  unsigned weight = 0;

  CoverRow row;
  row.resize(tricks.size());

  for (auto& cover: store)
  {
    if (explain.skip(
        cover.effectiveDepth(),
        cover.symmetry(),
        cover.composition()))
    {
      // Select consistent candidates.
      continue;
    }

    if (row.possibleAdd(cover, tricks, cases, additions, weight))
    {
      if (weight > partial.weight())
        partial.set(&cover, additions, weight);
    }
  }
}


set<Cover>::const_iterator CoverStore::begin() const
{
  return store.begin();
}


set<Cover>::const_iterator CoverStore::end() const
{
  return store.end();
}


size_t CoverStore::size() const
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
    ss << cover.strLine() << "\n";

  return ss.str() + "\n";
}

