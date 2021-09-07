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


void Distribution::setSurvivorsVoid()
{
  // West void.
  distSurvivorsWestVoid.clear();
  distSurvivorsWestVoid.push_back({0, 0});
  distSurvivorsWestVoid.reducedSize = 1;
  assert(distributions[0].west.len == 0);

  // East void.
  assert(distributions.size() > 0);

  const unsigned char dlast = 
    static_cast<unsigned char>(distributions.size() - 1);
  distSurvivorsEastVoid.clear();
  distSurvivorsEastVoid.push_back({dlast, 0});
  distSurvivorsEastVoid.reducedSize = 1;
  assert(distributions[dlast].east.len == 0);
}


void Distribution::setSurvivorsGeneral()
{
  // Make the survivors in the absence of rank collapses.
  // Could mirror around the middle to save a bit of time,
  // but it's marginal.

  const unsigned char dlast = 
    static_cast<unsigned char>(distributions.size() - 1);
  distSurvivors.resize(rankSize);
  for (unsigned char d = 1; d < dlast; d++)
  {
    const DistInfo& dist = distributions[d];
    for (unsigned w = 0; w < rankSize; w++)
    {
      if (dist.west.counts[w] == 0)
        continue;

      for (unsigned e = 0; e < rankSize; e++)
      {
        if (dist.east.counts[e] == 0)
           continue;

        distSurvivors.data[w][e].push_back(
          {d, distSurvivors.data[w][e].reducedSize});
        distSurvivors.data[w][e].reducedSize++;
      }
    }
  }
}


void Distribution::precalcSurvivorsCollapse1(
  vector<vector<SideInfo>>& distCollapses1)
{
  const unsigned dlast = distributions.size() - 1;
  distCollapses1.resize(rankSize);
  for (unsigned c1 = 1; c1 < rankSize; c1++)
    distCollapses1[c1].resize(dlast);

  for (unsigned d = 1; d < dlast; d++)
  {
    const DistInfo& dist = distributions[d];
    for (unsigned c1 = 1; c1 < rankSize; c1++)
    {
      distCollapses1[c1][d] = dist.west;
      distCollapses1[c1][d].collapse1(c1);
    }
  }
}


void Distribution::precalcSurvivorsCollapse2(
  const vector<vector<SideInfo>>& distCollapses1,
  vector<vector<vector<SideInfo>>>& distCollapses2)
{
  const unsigned dlast = distributions.size() - 1;
  distCollapses2.resize(rankSize);
  for (unsigned c1 = 1; c1 < rankSize; c1++)
  {
    distCollapses2[c1].resize(rankSize);
    for (unsigned c2 = 1; c2 < rankSize; c2++)
      distCollapses2[c1][c2].resize(dlast);
  }

  for (unsigned c1 = 1; c1 < rankSize; c1++)
  {
    if (c1+1 < rankSize)
    {
      // Special case where two adjacent ranks are collapsed.
      for (unsigned d = 1; d < dlast; d++)
      {
        distCollapses2[c1][c1+1][d] = distCollapses1[c1][d];
        distCollapses2[c1][c1+1][d].collapse2(c1+1);
        distCollapses2[c1+1][c1][d] = distCollapses2[c1][c1+1][d];
      }
    }

    for (unsigned c2 = c1+2; c2 < rankSize; c2++)
    {
      // General case where the collapses are independent.
      for (unsigned d = 1; d < dlast; d++)
      {
        distCollapses2[c1][c2][d] = distCollapses1[c1][d];
        distCollapses2[c1][c2][d].collapse1(c2);
        distCollapses2[c2][c1][d] = distCollapses2[c1][c2][d];
      }
    }
  }
}


void Distribution::collapseSurvivors(
  const vector<SideInfo>& distCollapses,
  const SurvivorList& survivorsUnreduced,
  SurvivorList& survivorsReduced)
{
  // Start by copying the uncollapsed list (if this is the first collapse)
  // or the once-collapsed list (if this is the second collapse).
  survivorsReduced = survivorsUnreduced;

  // If there is no prospect of collapsing anything, continue.
  if (survivorsReduced.sizeFull() <= 1)
    return;
  
  auto iter = next(survivorsReduced.distNumbers.begin());
  unsigned char rankLastReduced = 0;
  unsigned char rankCurrentReduced;

  while (iter != survivorsReduced.distNumbers.end())
  {
    const unsigned char dno = iter->fullNo;

    // Look back until we run out of distributions with the same
    // length.  The distributions are not necessarily in perfect
    // order for a rank collapse, but they are in length order for sure.
    auto iterPrev = prev(iter);
    while (true)
    {
      const unsigned dnoPrev = iterPrev->fullNo;
      const SideCompare compare = distCollapses[dno].compare(
        distCollapses[dnoPrev]);

      if (compare == SC_DIFFERENT_LENGTH ||
          (compare == SC_DIFFERENT_VALUES && 
            iterPrev == survivorsReduced.distNumbers.begin()))
      {
        // There is no identical previous rank collapse.
        rankLastReduced++;
        rankCurrentReduced = rankLastReduced;
        break;
      }
      else if (compare == SC_SAME)
      {
        rankCurrentReduced = iterPrev->reducedNo;
        break;
      }
      else
        iterPrev--;
    }

    iter->reducedNo = rankCurrentReduced;
    iter++;
  }

  survivorsReduced.reducedSize = rankLastReduced + 1;
}
  
  
void Distribution::setSurvivors()
{
  // The internal reduced ranks are 0-based.
  // The external full ranks are 1-based, and "0" means "void".
  // The survivors matrix is expressed in reduced ranks.
  // Therefore we need the voids separately.
  Distribution::setSurvivorsVoid();
  Distribution::setSurvivorsGeneral();

  // Pre-calculate the collapses of each distribution.
  vector<vector<SideInfo>> distCollapses1;
  Distribution::precalcSurvivorsCollapse1(distCollapses1);

  vector<vector<vector<SideInfo>>> distCollapses2;
  Distribution::precalcSurvivorsCollapse2(distCollapses1, distCollapses2);

  // We collapse downward, so rank 1 always survives and rank 2
  // may collapse onto rank 1.
  distSurvivorsCollapse1.resize(rankSize);
  for (unsigned c1 = 1; c1 < rankSize; c1++)
    distSurvivorsCollapse1[c1].resize(rankSize);

  distSurvivorsCollapse2.resize(rankSize);
  for (unsigned c1 = 1; c1 < rankSize; c1++)
  {
    distSurvivorsCollapse2[c1].resize(rankSize);
    for (unsigned c2 = 1; c2 < rankSize; c2++)
      distSurvivorsCollapse2[c1][c2].resize(rankSize);
  }
  // Make the rank collapses.
  for (unsigned w = 0; w < rankSize; w++)
  {
    for (unsigned e = 0; e < rankSize; e++)
    {
      for (unsigned c1 = 1; c1 < rankSize; c1++)
      {
        collapseSurvivors(distCollapses1[c1],
          distSurvivors.data[w][e],
          distSurvivorsCollapse1[c1].data[w][e]);

        for (unsigned c2 = c1+1; c2 < rankSize; c2++)
        {
          collapseSurvivors(distCollapses2[c1][c2],
            distSurvivorsCollapse1[c1].data[w][e],
            distSurvivorsCollapse2[c1][c2].data[w][e]);

          // TODO Could perhaps order c1 and c2 when calling,
          // such that we only need half the matrix.
          distSurvivorsCollapse2[c2][c1].data[w][e] = 
            distSurvivorsCollapse2[c1][c2].data[w][e];
        }
      }
    }
  }
}


const SurvivorList& Distribution::survivorsUncollapsed(
  const unsigned westRank,
  const unsigned eastRank) const
{
  // This method uses full (externally visible) ranks.
// cout << "west " << westRank << " east " << eastRank << " ref " <<
  // full2reduced.size() << endl;
  assert(westRank != 0 || eastRank != 0);
  assert(westRank < full2reduced.size());
  assert(eastRank < full2reduced.size());

  if (westRank == 0)
    return Distribution::survivorsWestVoid();
  else if (eastRank == 0)
    return Distribution::survivorsEastVoid();
  else
    return Distribution::survivorsReduced(
      full2reduced[westRank],
      full2reduced[eastRank]);
}


const SurvivorList& Distribution::survivorsCollapse1(
  const unsigned westRank,
  const unsigned eastRank,
  const unsigned collapse1) const
{
  // This method uses full (externally visible) ranks.
// cout << "Collapse1 west " << westRank << " east " << eastRank << 
  // " collapse1 " << collapse1 <<
  // " ref " << full2reduced.size() << endl;
  assert(westRank != 0 || eastRank != 0);
  assert(westRank < full2reduced.size());
  assert(eastRank < full2reduced.size());

  if (westRank == 0)
  {
// cout << "West void" << endl;
    return Distribution::survivorsWestVoid();
  }
  else if (eastRank == 0)
  {
// cout << "East void" << endl;
    return Distribution::survivorsEastVoid();
  }
  // else if (collapse1 <= 1 || collapse1 >= full2reduced.size())
  else if (collapse1 <= 1)
  {
// cout << "Not really collapsed" << endl;
    return Distribution::survivorsReduced(
      full2reduced[westRank],
      full2reduced[eastRank]);
  }
  else
  {
// cout << "Regular collapse" << endl;
    return Distribution::survivorsReducedCollapse1(
      full2reduced[westRank],
      full2reduced[eastRank],
      full2reduced[collapse1]);
  }
}


const SurvivorList& Distribution::survivorsCollapse2(
  const unsigned westRank,
  const unsigned eastRank,
  const unsigned collapse1,
  const unsigned collapse2) const
{
  // This method uses full (externally visible) ranks.
// cout << "Collapse2 west " << westRank << " east " << eastRank << 
  // " collapses " << collapse1 <<", " << collapse2 <<
  // " ref " <<
  // full2reduced.size() << endl;
  assert(westRank != 0 || eastRank != 0);
  assert(westRank < full2reduced.size());
  assert(eastRank < full2reduced.size());

  if (westRank == 0)
    return Distribution::survivorsWestVoid();
  else if (eastRank == 0)
    return Distribution::survivorsEastVoid();
  // else if (collapse1 <= 1 || collapse1 >= rankSize)
  else if (collapse1 <= 1)
  {
// TODO Do these discards ever happen?  Do we have to test for them?
// Can we avoid them in Ranks.cpp?
// cout << "Discarding collapse1\n";
    // if (collapse2 <= 2 || collapse2 >= full2reduced.size())
    if (collapse2 <= 1 || collapse1 == collapse2)
    {
// cout << "Discarding collapse2\n";
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
  // else if (collapse2 <= 2 || collapse2 >= full2reduced.size())
  else if (collapse2 <= 1)
  {
// cout << "Discarding collapse2\n";
      return Distribution::survivorsReducedCollapse1(
        full2reduced[westRank],
        full2reduced[eastRank],
        full2reduced[collapse1]);
  }
  else
  {
// cout << "Discarding nothing\n";
    return Distribution::survivorsReducedCollapse2(
      full2reduced[westRank],
      full2reduced[eastRank],
      full2reduced[collapse1],
      full2reduced[collapse2]);
  }
}


const SurvivorList& Distribution::survivorsWestVoid() const
{
  if (distCanonical == nullptr)
    // This distribution is canonical.
    return distSurvivorsWestVoid;
  else
    return distCanonical->survivorsWestVoid();
}


const SurvivorList& Distribution::survivorsEastVoid() const
{
  if (distCanonical == nullptr)
    // This distribution is canonical.
    return distSurvivorsEastVoid;
  else
    return distCanonical->survivorsEastVoid();
}


const SurvivorList& Distribution::survivorsReduced(
  const unsigned westRank,
  const unsigned eastRank) const
{
  // Voids are not possible here.
  assert(westRank < rankSize);
  assert(eastRank < rankSize);

// cout << "  reduced " << westRank << " " << eastRank << 
  // (distCanonical == nullptr ? " canonical" : " not canonical") << endl;

  if (distCanonical == nullptr)
    return distSurvivors.data[westRank][eastRank];
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

// cout << "  collapse1 reduced " << westRank << " " << eastRank << 
  // " collapse1 " << collapse1 <<
  // (distCanonical == nullptr ? " canonical" : " not canonical") << endl;

  if (distCanonical == nullptr)
    return distSurvivorsCollapse1[collapse1].data[westRank][eastRank];
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

// cout << "  collapse2 reduced " << westRank << " " << eastRank << 
  // (distCanonical == nullptr ? " canonical" : " not canonical") << endl;

  if (distCanonical == nullptr)
    return distSurvivorsCollapse2[collapse1][collapse2].data[westRank][eastRank];
  else
    return distCanonical->survivorsReducedCollapse2(westRank, eastRank, 
      collapse1, collapse2);
}


const SurvivorList& Distribution::survivors(const Play& play) const
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

  if (westRank == 0 || eastRank == 0)
    return Distribution::survivorsUncollapsed(westRank, eastRank);
  else if (play.leadCollapse && play.pardCollapse)
  {
    return Distribution::survivorsCollapse2(
      westRank, eastRank, play.pard()+1, play.lead()+1);
  }
  else if (play.leadCollapse)
  {
    return Distribution::survivorsCollapse1(
      westRank, eastRank, play.lead()+1);
  }
  else if (play.pardCollapse)
  {
    return Distribution::survivorsCollapse1(
      westRank, eastRank, play.pard()+1);
  }
  else
    return Distribution::survivorsUncollapsed(westRank, eastRank);
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

