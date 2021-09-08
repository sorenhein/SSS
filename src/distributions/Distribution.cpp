/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <utility>
#include <cassert>

#include "Distribution.h"

#include "../plays/Play.h"

#include "../const.h"


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


mutex mtxDist;
static bool init_flag = false;

vector<vector<unsigned>> binomial;
vector<vector<char>> names;


Distribution::Distribution()
{
  Distribution::reset();

  mtxDist.lock();
  if (! init_flag)
  {
    Distribution::setBinomial();
    Distribution::setNames();
    init_flag = true;
  }
  mtxDist.unlock();
}


void Distribution::reset()
{
  distributions.clear();
  distCanonical = nullptr;
}


void Distribution::setBinomial()
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


void Distribution::setNames()
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


void Distribution::shrink(
  const unsigned maxFullRank,
  const unsigned maxReducedRank)
{
  rankSize = maxReducedRank + 1;
  full2reduced.resize(maxFullRank+1);
  reduced2full.resize(rankSize);
  opponents.counts.resize(rankSize);
}


void Distribution::setRanks(
  const unsigned cardsIn,
  const unsigned holding2)
{
  cards = cardsIn;
  // We go with a minimal representation of East-West in terms of ranks,
  // so the rank numbers will be smaller.  We keep a correspondence
  // back and forth with the outside world.
  full2reduced.resize(cards+1);
  reduced2full.resize(cards);
  opponents.counts.resize(cards);
  
  bool prev_is_NS = ((holding2 & 1) == PAIR_NS);

  // The full rank is the rank used in Combinations.
  // East-West might have ranks 1, 3 and 5, for example.
  unsigned nextFullRank = 1;
  unsigned maxFullRank = 1;

  // The reduced rank is used internally in Distribution and only
  // considers the East-West ranks which might be 0, 1 and 2.
  unsigned nextReducedRank = 0;
  unsigned maxReducedRank = 0;

  unsigned h = holding2;
  opponents.len = 0;

  // We could use a more compact rank format, but we want to have the
  // same rank numbers as in Combination.
  for (unsigned i = 0; i < cards; i++)
  {
    const unsigned c = h & 1;
    if (c == PAIR_NS)
    {
      if (! prev_is_NS)
      {
        nextFullRank++;
        nextReducedRank++;
      }

      prev_is_NS = true;
    }
    else
    {
      if (prev_is_NS)
        nextFullRank++;

      full2reduced[nextFullRank] = nextReducedRank;
      reduced2full[nextReducedRank] = nextFullRank;
      opponents.counts[nextReducedRank]++;
      opponents.len++;

      maxFullRank = nextFullRank;
      maxReducedRank = nextReducedRank;

      prev_is_NS = false;
    }
    h >>= 1;
  }

  // Shrink to fit.
  Distribution::shrink(maxFullRank, maxReducedRank);
}


void Distribution::mirror(unsigned& distIndex)
{
  // Go backwards from one past the first element to mirror.
  unsigned dtop;
  if (opponents.len & 1)
    dtop = distIndex;
  else
  {
    const unsigned lenMid = opponents.len / 2;
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


void Distribution::split()
{
  if (opponents.len == 0)
    return;

  list<StackInfo> stack; // Unfinished expansions

  distributions.resize(CHUNK_SIZE[cards]);
  unsigned distIndex = 0; // Position of next result to write

  // Only do the first half and then mirror the other lengths.
  for (unsigned lenWest = 0; lenWest <= opponents.len / 2; lenWest++)
  {
    stack.emplace_back(StackInfo(rankSize));

    while (! stack.empty())
    {
      auto stackIter = stack.begin();
      const unsigned rank = stackIter->rankNext; // Next to write
      const unsigned gap = lenWest - stackIter->west.len;
      const unsigned available = opponents.counts[rank];
      stackIter->seen += available;

      for (unsigned count = 0; count <= min(gap, available); count++)
      {
        if (count == gap)
        {
          // Store the result.
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE[cards]);

          DistInfo& dist = distributions[distIndex];

          dist.west = stackIter->west;
          dist.add(rank, count, binomial[available][count]);
          dist.east.diff(opponents, dist.west);

          distIndex++;
          break;
        }
        else if (count + opponents.len >= gap + stackIter->seen)
        {
          // Can still reach our goal of lenWest cards:
          // We have opponents.len - stackIter->seen cards still to come.
          // We have gap - count holes still to fill.
          stackIter = stack.insert(stackIter, * stackIter);
          next(stackIter)->add(rank, count, binomial[available][count]);
        }
      }
      stack.pop_front();
    }
  }

  Distribution::mirror(distIndex);
}


void Distribution::splitAlternative()
{
  // I thought it might be faster to have two vectors whose sizes
  // don't change (much), rather than a list.  But it turns out to
  // be about the same.  I've left the code in.

  if (opponents.len == 0)
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
  for (unsigned lenWest = 0; lenWest <= opponents.len / 2; lenWest++)
  {
    (*stackRead)[0] = StackInfo(rankSize);
    indexRead = 0;
    countRead = 1;

    while (indexRead < countRead)
    {
      StackInfo& stackElem = (*stackRead)[indexRead];
      const unsigned rank = stackElem.rankNext; // Next to write
      const unsigned gap = lenWest - stackElem.west.len;
      const unsigned available = (gap == 0 ? 0 : opponents.counts[rank]);
      stackElem.seen += available;

      unsigned upperPlus1;
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
          if (gap > 0)
             dist.add(rank, gap, binomial[available][gap]);
          dist.east.diff(opponents, dist.west);

          distIndex++;
        }
        else
        {
          // Must keep it in stackWrite.
          if (indexWrite == stackWrite->size())
            stackWrite->resize(stackWrite->size() + CHUNK_SIZE[cards]);

          (*stackWrite)[indexWrite] = (*stackRead)[indexRead];
          if (gap > 0)
            (*stackWrite)[indexWrite].add(rank, gap, 
              binomial[available][gap]);

          indexWrite++;
        }
        upperPlus1 = gap;
      }
      else
       upperPlus1 = available + 1;

      unsigned clow;
      if (gap + stackElem.seen >= opponents.len)
        clow = gap + stackElem.seen - opponents.len;
      else
        clow = 0;

      for (unsigned count = upperPlus1; count-- > clow; )
      {
        // Put to stackWrite
        if (indexWrite == stackWrite->size())
          stackWrite->resize(stackWrite->size() + CHUNK_SIZE[cards]);
          
        (*stackWrite)[indexWrite] = (*stackRead)[indexRead];
        (*stackWrite)[indexWrite].add(rank, count, 
          binomial[available][count]);
        indexWrite++;
      }

      indexRead++;
      if (indexRead < countRead)
        continue;
      if (indexWrite > 0)
      {
        vector<StackInfo> * tmp = stackRead;
        stackRead = stackWrite;
        stackWrite = tmp;

        countRead = indexWrite;
        indexRead = 0;
        indexWrite = 0;
      }
    }
  }

  Distribution::mirror(distIndex);
}


void Distribution::setPtr(Distribution const * distCanonicalIn)
{
  distCanonical = distCanonicalIn;
}


Distribution const * Distribution::getPtr() const
{
  return distCanonical;
}


unsigned Distribution::size() const
{
  if (opponents.len == 0)
    return 1;
  else if (distCanonical)
    return distCanonical->size();
  else
    return distributions.size();
}


DistID Distribution::getID() const
{
  // The canonical holding arises when EW have the lowest card and
  // when each NS rank consists of exactly 1 card.  So effectively
  // the canonical holding is a binary representation of a composition
  // of len cards into the particular rank profile.  Therefore the
  // number of such representations is the number of compositions.

  // The number of bits needed to represent the canonical holding,
  // i.e. the number of cards that both sides need, is.
  DistID res;
  res.cards = opponents.len + rankSize - 1;

  // This assumes that PAIR_EW is 1.
  assert(PAIR_EW == 1);

  res.holding = 0;
  for (unsigned r = rankSize; r-- > 0; )
  {
    // Make room for bits.
    // If we need e.g. 3 bits, they are going to be "011".
    const unsigned bits = opponents.counts[r] + 1;
    res.holding = (res.holding << bits) | ((1u << (bits-1)) - 1u);
  }

  return res;
}



/* */
const SurvivorList& Distribution::survivorsUncollapsed(
  const unsigned westRankReduced,
  const unsigned eastRankReduced,
  const bool westVoidFlag,
  const bool eastVoidFlag) const
{
  // This method uses reduced (internally visible) ranks.

  if (westVoidFlag)
    return Distribution::survivorsWestVoid();
  else if (eastVoidFlag)
    return Distribution::survivorsEastVoid();
  else
  {
    // TODO Why can this happen?
    // Because it's called once with known void, once without voids!
    return Distribution::survivorsReduced(
      westRankReduced,
      eastRankReduced);
  }
}


const SurvivorList& Distribution::survivorsCollapse1(
  const unsigned westRankReduced,
  const unsigned eastRankReduced,
  const unsigned collapseReduced) const
{
  return Distribution::survivorsReducedCollapse1(
    westRankReduced, eastRankReduced, collapseReduced);
}


const SurvivorList& Distribution::survivorsCollapse2(
  const unsigned westRank,
  const unsigned eastRank,
  const unsigned collapse1,
  const unsigned collapse2) const
{
  // This method uses full (externally visible) ranks.
  assert(westRank != 0 || eastRank != 0);
  assert(westRank < full2reduced.size());
  assert(eastRank < full2reduced.size());

  if (westRank == 0)
  {
assert(false);
    return Distribution::survivorsWestVoid();
  }
  else if (eastRank == 0)
  {
assert(false);
    return Distribution::survivorsEastVoid();
  }
  else if (collapse1 <= 1)
  {
assert(false);
// TODO Do these discards ever happen?  Do we have to test for them?
// Can we avoid them in Ranks.cpp?
    if (collapse2 <= 1 || collapse1 == collapse2)
    {
      // Discarding collapse2
      return Distribution::survivorsReduced(
        full2reduced[westRank],
        full2reduced[eastRank]);
    }
    else
      return Distribution::survivorsReducedCollapse1(
        full2reduced[westRank],
        full2reduced[eastRank],
        full2reduced[collapse2]);
  }
  else if (collapse2 <= 1)
  {
assert(false);
    // Discarding collapse2
      return Distribution::survivorsReducedCollapse1(
        full2reduced[westRank],
        full2reduced[eastRank],
        full2reduced[collapse1]);
  }
  else
  {
    // Discarding nothing
    return Distribution::survivorsReducedCollapse2(
      full2reduced[westRank],
      full2reduced[eastRank],
      full2reduced[collapse1],
      full2reduced[collapse2]);
  }
}
/* */

void Distribution::setSurvivors()
{
  survivors.setGlobal(rankSize);
  survivors.setSurvivors(distributions);
}


const SurvivorList& Distribution::survivorsWestVoid() const
{
  if (distCanonical == nullptr)
    // This distribution is canonical.
    // return distSurvivorsWestVoid;
    return survivors.survivorsWestVoid();
  else
  {
// assert(distCanonical->distSurvivorsWestVoid ==
       // distCanonical->survivorsWestVoid());
assert(distCanonical->distCanonical == nullptr);

    return distCanonical->survivorsWestVoid();
  }
}


const SurvivorList& Distribution::survivorsEastVoid() const
{
  if (distCanonical == nullptr)
    // This distribution is canonical.
    // return distSurvivorsEastVoid;
    return survivors.survivorsEastVoid();
  else
  {
// This is not always true, e.g. 4/8: Why?
// assert(distCanonical->distSurvivorsEastVoid ==
       // distCanonical->survivorsEastVoid());

assert(distCanonical->distCanonical == nullptr);

    return distCanonical->survivorsEastVoid();
  }
}


const SurvivorList& Distribution::survivorsReduced(
  const unsigned westRank,
  const unsigned eastRank) const
{
  // Voids are not possible here.
  // assert(westRank < rankSize);
  // assert(eastRank < rankSize);

  if (distCanonical == nullptr)
    return survivors.survivorsReduced(westRank, eastRank);
  else
    return distCanonical->survivorsReduced(westRank, eastRank);
}


const SurvivorList& Distribution::survivorsReducedCollapse1(
  const unsigned westRank,
  const unsigned eastRank,
  const unsigned collapse1) const
{
  // Voids are not possible here.
  assert(westRank < rankSize);
  assert(eastRank < rankSize);
  assert(collapse1 >= 1 && collapse1 < rankSize);

  if (distCanonical == nullptr)
    return survivors.survivorsReducedCollapse1(
      westRank, eastRank, collapse1);
  else
    return distCanonical->survivorsReducedCollapse1(westRank, eastRank, 
      collapse1);
}


const SurvivorList& Distribution::survivorsReducedCollapse2(
  const unsigned westRank,
  const unsigned eastRank,
  const unsigned collapse1,
  const unsigned collapse2) const
{
  // Voids are not possible here.
  assert(westRank < rankSize);
  assert(eastRank < rankSize);
  assert(collapse1 >= 1 && collapse1 < rankSize);

  if (distCanonical == nullptr)
    return survivors.survivorsReducedCollapse2(westRank, eastRank,
      collapse1, collapse2); 
  else
    return distCanonical->survivorsReducedCollapse2(westRank, eastRank, 
      collapse1, collapse2);
}


const SurvivorList& Distribution::getSurvivors(const Play& play) const
{
  // This doesn't work.
  // I think the reason is: Let's say West is void.
  // D::survivorsUncollapsed
  // Will test and potentially use "our" full2reduced
  //   D::survivorsWestVoid
  //   If not canonical, will return 
  //
  // canonical survivors::survivorsUncollapsed
  // Will use "their" full2reduced
  //
  // Maybe the solution is first to map westRank and eastRank
  // to the reduced versions and then always to pass these.
  // Then Survivors doesn't need to know full2reduced either.
  //

  /*
  Distribution const * dptr = (distCanonical ? distCanonical : this);
cout << play.strLine();
if (distCanonical)
  cout << "Switch to canonical" << endl;
else
  cout << "Already canonical" << endl;

  return dptr->survivors.getSurvivors(play);
  */

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

  // assert(westRank < rankSize);
  // assert(eastRank < rankSize);
  assert(westRank < full2reduced.size());
  assert(eastRank < full2reduced.size());
  assert(westRank > 0 || eastRank > 0);
  const unsigned westRankReduced = full2reduced[westRank];
  const unsigned eastRankReduced = full2reduced[eastRank];

  // These are the corresponding EW ranks that may have to be collapsed.
  // They have to be 1 higher (alternating ranks).
  const unsigned collapseLeadReduced =
    (play.leadCollapse ? full2reduced[play.lead()+1] : 0);
  const unsigned collapsePardReduced =
    (play.pardCollapse ? full2reduced[play.pard()+1] : 0);

  /*
  Distribution const * dptr = (distCanonical ? distCanonical : this);
  return dptr->survivors.getSurvivors(play,
    westRankReduced, eastRankReduced, 
    collapseLeadReduced, collapsePardReduced);
    */

  bool westVoidFlag, eastVoidFlag;
  play.setVoidFlags(westVoidFlag, eastVoidFlag);

  /* */
  if (westRank == 0 || eastRank == 0)
  {
assert(westVoidFlag || eastVoidFlag);
    return Distribution::survivorsUncollapsed(
      westRankReduced, eastRankReduced,
      westVoidFlag, eastVoidFlag);
  }
  else if (play.leadCollapse && play.pardCollapse)
  {
    return Distribution::survivorsCollapse2(
      westRank, eastRank, play.pard()+1, play.lead()+1);
  }
  else if (play.leadCollapse)
  {
    return Distribution::survivorsCollapse1(
      westRankReduced, eastRankReduced, collapseLeadReduced);
  }
  else if (play.pardCollapse)
  {
    return Distribution::survivorsCollapse1(
      westRankReduced, eastRankReduced, collapsePardReduced);
  }
  else
    return Distribution::survivorsUncollapsed(
      westRankReduced, eastRankReduced,
      westVoidFlag, eastVoidFlag);
  /* */
}


string Distribution::strHeader() const
{
  stringstream ss;
  ss << 
    setw(4) << right << "Dist" <<
    setw(14) <<  "West" << 
    setw(14) <<  "East" << 
    setw(8) <<  "Cases" << "\n";
  return ss.str();
}


string Distribution::str() const
{
  if (distCanonical != nullptr)
    return "Distribution is not canonical.\n\n" +
      distCanonical->str();

  if (distributions.empty())
    return "No distributions\n";

  stringstream ss;
  ss << Distribution::strHeader();

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

