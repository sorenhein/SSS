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

#include "../../inputs/Control.h"

extern Control control;


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

// cout << "Got symm: " << symmetricFlag << "\n";
// cout << coverScratch.strLine() << endl;

  // Make its tricks and counts.
  if (! coverScratch.setByProduct(distProfiles, cases))
  {
// cout << "Couldn't set\n";
    return;
  }

  // Only enter covers that are not too deep.
  if (coverScratch.effectiveDepth() > control.verbalDepth())
  {
// cout << "Failed on depth" << endl;
// cout << "eff " << +coverScratch.effectiveDepth() << endl;
// cout << "ver " << +control.verbalDepth() << endl;
    return;
  }

  if (! coverScratch.explainable(sumProfile))
  {
    // TODO Not sure how much we're throwing away here for different
    // values of g?
    // cout << "NONEXPLAINABLE\n";
    // cout << coverScratch.strNumerical() << "\n";
    return;
  }

  if (symmetricFlag &&
      coverScratch.discardSymmetric(sumProfile))
  {
cout << "Discard\n";
cout << "sum profile " << sumProfile.strLine();
cout << "cover " << coverScratch.strLine() << "\n\n";
    return;
  }

  // Store it in "store".
  auto result = store.insert(coverScratch);
  assert(result.first != store.end());

  if (symmetricFlag)
  {
// cout << "Already symmetric\n";
// cout << "sum profile " << sumProfile.strLine();
// cout << "cover " << coverScratch.strLine() << "\n\n";
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
  {
// cout << "Not symmetrizable\n";
    return;
  }

  // TODO Is this needed too?
  if (! coverScratch.symmetrize())
  {
// cout << "Couldn't symmetrize\n";
    return;
  }

// cout << "Adding symmetric\n";
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
if (it == store.end())
{
  cout << "MISSING\n";
  cout << cover.strNumerical() << endl;
  cout << "FROM\n";
  cout << CoverStore::str() << endl;
}

  assert(it != store.end());
  return * it;
}


void CoverStore::heaviestPartial(
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  const Explain& explain,
  Partial& partial) const
{
  // This finds the best (highest-weight) cover consistent with explain.
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


void CoverStore::heaviestPartials(
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  const Explain& explain,
  const size_t numHeaviest,
  multiset<Partial>& partials) const
{
  // Like heaviestPartial(), but returns up to numHeaviest candidates.

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

    if (! row.possibleAdd(cover, tricks, cases, additions, weight))
    {
      // Only use fitting candidates.
      continue;
    }

    if (partials.size() >= numHeaviest)
    {
      auto plastIter = prev(partials.end());
      if (weight > plastIter->weight())
      {
        // Keep the size down to numHeaviest.
        partials.erase(plastIter);
      }
      else
      {
        // Discard too-light candidate.
        continue;
      }
    }

    // TODO This is quite inefficient.  I suppose partialCovers might
    // instead hold pointers to Partial's, and there would be a
    // list of the actual Partial's with one scratch element that
    // would get swapped in against the deleted element.

    Partial partial;
    partial.set(&cover, additions, weight);
    partials.emplace(partial);
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


bool CoverStore::duplicates() const
{
  // Not particularly efficient, but only for diagnostics.
  for (auto it1 = store.begin(); it1 != store.end(); it1++)
  {
    for (auto it2 = next(it1); it2 != store.end(); it2++)
    {
      auto& c1 = *it1;
      auto& c2 = *it2;
      if (c1.getWeight() == c2.getWeight() &&
          c1.getTricks() == c2.getTricks())
      {
        cout << "C1\n" << c1.strNumerical() << "\n";
        cout << "C2\n" << c2.strNumerical() << "\n";
        return true;
      }
    }
  }
  return false;
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

