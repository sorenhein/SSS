/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <utility>
#include <cassert>

#include "DistCore.h"
#include "DistMap.h"

#include "covers/manual/Manual.h"

#include "covers/product/ProductMemory.h"

#include "../plays/Play.h"

#include "../const.h"

#include "../utils/Timer.h"
extern vector<Timer> timersStrat;

// The full size sequence is:
// http://oeis.org/A000792 (has something to do with partitions)
// Note that c(n) = 3 * c(n-3) for n >= 4.
// We make space for about half the worst case for larger n.

const vector<unsigned> CHUNK_SIZE =
{
   1, //  0
   2, //  1
   3, //  2
   4, //  3
   6, //  4
   9, //  5
  12, //  6
  18, //  7
  27, //  8
  36, //  9
  54, // 10
  54, // 11 (really 81)
  54, // 12 (really 108)
  81, // 13 (really 162)
 122, // 14 (really 243)
 162  // 15 (really 324)
};

/*
   Survivors are needed when we map the results of a subsequent trick
   up to the current one.  The subsequent trick will have fewer
   distributions in general.

   Reductions are needed when we have solved for a rank reduction and
   we want to calculate back and forth between a more complex and a
   simpler, reduced combination (for the same trick level).

   Some distributions are "canonical", and others are mapped to these
   canonical ones with the distCanonical pointer.  There are only
   explicit survivors and reductions for the canonical distributions.
   The others can be derived from the mapping between externally
   visible ranks and the internal ranks of a canonical distribution.
 */


mutex mtxDistCore;
static bool init_flag = false;

vector<vector<unsigned>> binomial;
vector<vector<char>> names;


DistCore::DistCore()
{
  DistCore::reset();

  mtxDistCore.lock();
  if (! init_flag)
  {
    DistCore::setBinomial();
    DistCore::setNames();
    init_flag = true;
  }
  mtxDistCore.unlock();
}


void DistCore::reset()
{
  distributions.clear();
}


void DistCore::setBinomial()
{
  binomial.resize(MAX_CARDS+1);
  for (unsigned n = 0; n <= MAX_CARDS; n++)
    binomial[n].resize(MAX_CARDS+1);

  binomial[0][0] = 1;
  for (unsigned n = 1; n < binomial.size(); n++)
  {
    binomial[n][0] = 1;
    binomial[n][n] = 1;
    for (unsigned k = 1; k < n; k++)
      binomial[n][k] = binomial[n-1][k] + binomial[n-1][k-1];
  }
}


void DistCore::setNames()
{
  const unsigned maxNumRanks = (MAX_CARDS+1) / 2;
  names.resize(maxNumRanks+1);

  for (unsigned numRanks = 1; numRanks < names.size(); numRanks++)
  {
    vector<char>& vec = names[numRanks];
    vec.resize(numRanks);
    vec[0] = 'x';
    for (unsigned rank = 1; rank < numRanks; rank++)
      vec[rank] = string(GENERIC_NAMES).at(numRanks-rank-1);
  }
}


void DistCore::mirror(
  const DistMap& distMap,
  unsigned& distIndex)
{
  // Go backwards from one past the first element to mirror.
  unsigned dtop;
  if (distMap.opponents.len & 1)
    dtop = distIndex;
  else
  {
    const unsigned char lenMid = distMap.opponents.len / 2;
    dtop = distIndex;
    while (dtop >= 1 && distributions[dtop-1].west.len == lenMid)
      dtop--;
    assert(dtop > 0 && distributions[dtop-1].west.len + 1 == lenMid);
  }

  DistInfo * oldDist;
  DistInfo * newDist;

  // Make/shrink room.
  distributions.resize(distIndex + dtop);

  for (unsigned d = dtop; d-- > 0; )
  {
    oldDist = &distributions[d];
    newDist = &distributions[distIndex++];

    newDist->west = oldDist->east;
    newDist->east = oldDist->west;
    newDist->cases = oldDist->cases;
  }
}


void DistCore::split(const DistMap& distMap)
{
  cards = distMap.cards;
  rankSize = distMap.rankSize;

  if (distMap.opponents.len == 0)
    return;

  list<StackInfo> stack; // Unfinished expansions

  distributions.resize(CHUNK_SIZE[cards]);
  unsigned distIndex = 0; // Position of next result to write

  // Only do the first half and then mirror the other lengths.
  for (unsigned char lenWest = 0; lenWest <= distMap.opponents.len / 2; 
      lenWest++)
  {
    stack.emplace_back(StackInfo(rankSize));

    while (! stack.empty())
    {
      auto stackIter = stack.begin();
      const unsigned rank = stackIter->rankNext; // Next to write
      const unsigned char gap = lenWest - stackIter->west.len;
      const unsigned char available = distMap.opponents.counts[rank];
      stackIter->seen += available;

      for (unsigned char count = 0; count <= min(gap, available); count++)
      {
        if (count == gap)
        {
          // Store the result.
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE[cards]);

          DistInfo& dist = distributions[distIndex];

          dist.west = stackIter->west;

          // if (rank < rankSize)
            dist.add(rank, rankSize, count, 
              stackIter->cases * binomial[available][count]);

          dist.east.diff(distMap.opponents, dist.west);

          distIndex++;
          break;
        }
        else if (count + distMap.opponents.len >= gap + stackIter->seen)
        {
          // Can still reach our goal of lenWest cards:
          // We have opponents.len - stackIter->seen cards still to come.
          // We have gap - count holes still to fill.
          stackIter = stack.insert(stackIter, * stackIter);

          // if (rank < rankSize)
            next(stackIter)->add(rank, rankSize, 
              count, binomial[available][count]);
        }
      }
      stack.pop_front();
    }
  }

  DistCore::mirror(distMap, distIndex);
}


void DistCore::splitAlternative(const DistMap& distMap)
{
  // I thought it might be faster to have two vectors whose sizes
  // don't change (much), rather than a list.  But it turns out to
  // be about the same.  I've left the code in.

  cards = distMap.cards;
  rankSize = distMap.rankSize;
  if (distMap.opponents.len == 0)
    return;

  vector<StackInfo> stack1(CHUNK_SIZE[cards]); // Unfinished expansion
  vector<StackInfo> stack2(CHUNK_SIZE[cards]); // Unfinished expansion

  vector<StackInfo> * stackRead = &stack1;
  vector<StackInfo> * stackWrite = &stack2;

  unsigned indexRead = 0; // Next index to read
  unsigned indexWrite = 0; // Next index to write
  unsigned countRead = 0;

  distributions.resize(CHUNK_SIZE[cards]);
  unsigned distIndex = 0; // Position of next result to write

  // Only do the first half and then mirror the other lengths.
  for (unsigned char lenWest = 0; lenWest <= distMap.opponents.len / 2; 
      lenWest++)
  {
    (*stackRead)[0] = StackInfo(rankSize);
    indexRead = 0;
    countRead = 1;

    while (indexRead < countRead)
    {
      StackInfo& stackElem = (*stackRead)[indexRead];
      const unsigned rank = stackElem.rankNext; // Next to write
      const unsigned char gap = lenWest - stackElem.west.len;
      const unsigned char available = 
        (gap == 0 ? 0 : distMap.opponents.counts[rank]);
      stackElem.seen += available;

      unsigned char upperPlus1;
      if (gap <= available)
      {
        // Can complete a distribution.
        if (indexRead == 0)
        {
          // Can write it to distributions.
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE[cards]);

          DistInfo& dist = distributions[distIndex];

          dist.west = stackElem.west;

          dist.add(rank, rankSize, gap, 
            stackElem.cases * binomial[available][gap]);

          dist.east.diff(distMap.opponents, dist.west);

          distIndex++;
        }
        else
        {
          // Must keep it in stackWrite.
          if (indexWrite == stackWrite->size())
            stackWrite->resize(stackWrite->size() + CHUNK_SIZE[cards]);

          (*stackWrite)[indexWrite] = (*stackRead)[indexRead];

          // TODO Could pass in rankSize to add() and deal with it there.
          // if (rank < rankSize)
            (*stackWrite)[indexWrite].add(rank, rankSize,
             gap, binomial[available][gap]);

          indexWrite++;
        }
        upperPlus1 = gap;
      }
      else
       upperPlus1 = available + 1;

      unsigned char clow;
      if (gap + stackElem.seen >= distMap.opponents.len)
        clow = gap + stackElem.seen - distMap.opponents.len;
      else
        clow = 0;

      for (unsigned char count = upperPlus1; count-- > clow; )
      {
        // Put to stackWrite
        if (indexWrite == stackWrite->size())
          stackWrite->resize(stackWrite->size() + CHUNK_SIZE[cards]);
          
        (*stackWrite)[indexWrite] = (*stackRead)[indexRead];

        // if (rank < rankSize)
          (*stackWrite)[indexWrite].add(rank, rankSize, count, 
            binomial[available][count]);

        indexWrite++;
      }

      indexRead++;
      if (indexRead < countRead)
        continue;
      if (indexWrite > 0)
      {
        // TODO Use swap?
        vector<StackInfo> * tmp = stackRead;
        stackRead = stackWrite;
        stackWrite = tmp;

        countRead = indexWrite;
        indexRead = 0;
        indexWrite = 0;
      }
    }
  }

  DistCore::mirror(distMap, distIndex);
}


unsigned DistCore::size() const
{
  if (distributions.empty())
    return 1;
  else
    return distributions.size();
}


void DistCore::setLookups()
{
  survivors.setGlobal(rankSize);

  if (distributions.size() > 0)
    survivors.setSurvivors(distributions);

  reductions.set(distributions, rankSize);
}


const SurvivorList& DistCore::getSurvivors(
  const DistMap& distMap,
  const Play& play) const
{
  unsigned westRank, eastRank;
  if (play.side == SIDE_NORTH)
  {
    westRank = play.rho();
    eastRank = play.lho();
  }
  else
  {
    westRank = play.lho();
    eastRank = play.rho();
  }

  assert(westRank < distMap.full2reduced.size());
  assert(eastRank < distMap.full2reduced.size());
  assert(westRank > 0 || eastRank > 0);

  struct SurvivorControl sc;

  sc.westRank = distMap.full2reduced[westRank];
  sc.eastRank = distMap.full2reduced[eastRank];
  assert(sc.westRank < rankSize);
  assert(sc.eastRank < rankSize);

  play.setVoidFlags(sc.westVoidFlag, sc.eastVoidFlag);

  // These are the corresponding EW ranks that may have to be collapsed.
  // They have to be 1 higher (alternating ranks).

  sc.collapseLead = (play.leadCollapse ? 
    distMap.full2reduced[play.lead()+1] : 0);

  sc.collapsePard = (play.pardCollapse ? 
    distMap.full2reduced[play.pard()+1] : 0);

  assert(sc.collapseLead < rankSize);
  assert(sc.collapsePard < rankSize);

  sc.collapseLeadFlag = play.leadCollapse;
  sc.collapsePardFlag = play.pardCollapse;

  return survivors.getSurvivors(sc);
}


const Reduction& DistCore::getReduction(
  const DistMap& distMap,
  const unsigned char rankNS) const
{
  assert(rankNS != 1 && 
    (rankNS <= distMap.full2reduced.size() || rankNS == UCHAR_NOT_SET));

  // All EW ranks < the NS rank are grouped together.
  // A reducer of 0 ("NS take no rank tricks") is actually like
  // a maximal reducer.

  const unsigned rankReducedEW = 
    ((rankNS == 0 || rankNS == UCHAR_NOT_SET) ? 
      rankSize-1 : distMap.full2reduced[rankNS-1]);

  return reductions.get(rankReducedEW);
}


void DistCore::getCoverData(
  vector<Profile>& distProfiles,
  vector<unsigned char>& cases,
  Profile& sumProfile) const
{
  const unsigned len = distributions.size();
  assert(len > 0);

  distProfiles.resize(len);
  cases.resize(len);

  sumProfile.setSum(
    distributions[0].west.counts, 
    distributions[0].east.counts);

  for (unsigned i = 0; i < len; i++)
  {
    // A SideInfo actually has the same data elements as a Profile,
    // but they are used differently, so we don't merge them here.
    const DistInfo& dist = distributions[i];

    distProfiles[i].set(dist.west.counts);

    cases[i] = static_cast<unsigned char>(dist.cases);
  }
}


void DistCore::prepareManualCovers(
  ProductMemory& productMemory,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  const Profile& sumProfile)
{
  // Currently unused -- left so that it's possible to turn on.
  // See Manual.cpp

  // Get the manual covers.
  list<list<ManualData>> manualData;
  Manual manual(sumProfile, manualData);

  // Set the manual rows in covers.
  for (auto& manualList: manualData)
  {
    list<Cover const *> coverPtrs;
    for (auto& man: manualList)
    {
      Cover cover;
      cover.setExisting(
        productMemory,
        sumProfile,
        man.profilePair,
        man.symmFlag);

      cover.setByProduct(distProfiles, cases);

      // Need to look up the cover that has memory permanence.
      const Cover& clookup = covers.lookup(cover);
      coverPtrs.push_back(&clookup);
    }

    CoverRow& row = covers.addRow();
    row.resize(distProfiles.size());
    row.fillDirectly(coverPtrs, cases);
  }

  covers.sortRows();
}


void DistCore::prepareCovers(ProductMemory& productMemory)
{
  if (distributions.size() == 0)
    return;

  vector<Profile> distProfiles;
  vector<unsigned char> cases;
  Profile sumProfile;
  DistCore::getCoverData(distProfiles, cases, sumProfile);

  // Prepare each cover.
  timersStrat[22].start();
  covers.prepare(productMemory, distProfiles, cases, sumProfile);
  timersStrat[22].stop();

  // Set the manual covers.
  timersStrat[21].start();
  DistCore::prepareManualCovers(
    productMemory,
    distProfiles,
    cases,
    sumProfile);
  timersStrat[21].stop();
}


Covers& DistCore::getCovers()
{
  return covers;
}


void DistCore::getCoverCounts(
  unsigned& numTableaux,
  unsigned& numUses) const
{
  covers.getCoverCounts(numTableaux, numUses);
}


string DistCore::strHeader() const
{
  stringstream ss;
  ss << 
    setw(4) << right << "Dist" <<
    setw(14) <<  "West" << 
    setw(14) <<  "East" << 
    setw(8) <<  "Cases" << "\n";
  return ss.str();
}


string DistCore::str() const
{
  if (distributions.empty())
    return "No distributions\n";

  stringstream ss;
  ss << DistCore::strHeader();

  for (unsigned d = 0; d < distributions.size(); d++)
  {
    ss << 
      setw(4) << d <<
      setw(14) << distributions[d].west.str(names[rankSize]) <<
      setw(14) << distributions[d].east.str(names[rankSize]) <<
      setw(8) << distributions[d].cases << "\n";
  }
  return ss.str();
}


string DistCore::strCovers() const
{
  return covers.strCache();
}

