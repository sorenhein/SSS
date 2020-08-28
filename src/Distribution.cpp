#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>
#include <algorithm>
#include <cassert>

#include "Distribution.h"
#include "const.h"

#define CHUNK_SIZE 50

const string genericNames = "HhGgIiJj";


Distribution::Distribution()
{
  Distribution::reset();
  Distribution::setBinomial();
}


Distribution::~Distribution()
{
}


void Distribution::reset()
{
  distributions.clear();
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


void Distribution::shrink(
  const unsigned maxFullRank,
  const unsigned maxReducedRank)
{
  if (opponents.len > 0)
  {
    rankSize = maxReducedRank + 1;
    full2reduced.resize(maxFullRank+1);
    reduced2full.resize(rankSize);
    opponents.counts.resize(rankSize);
  }
  else
  {
    rankSize = 0;
    full2reduced.clear();
    reduced2full.clear();
    opponents.counts.clear();
  }
}


void Distribution::setRanks(
  const unsigned cards,
  const unsigned holding2)
{
  // We go with a minimal representation of East-West in terms of ranks,
  // so the rank numbers will be smaller.  We keep a correspondence
  // back and forth with the outside world.
  full2reduced.resize(cards);
  reduced2full.resize(cards);
  opponents.counts.resize(cards);
  
  bool prev_is_NS = ((holding2 & 1) == CONVERT_NS);

  // The full rank is the rank used in Combinations.
  // East-West might have ranks 1, 3 and 5, for example.
  unsigned nextFullRank = 0;
  unsigned maxFullRank = 0;

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
    if (c == CONVERT_NS)
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

  for (unsigned d = dtop; d-- > 0; )
  {
    if (distIndex == distributions.size())
      distributions.resize(distributions.size() + CHUNK_SIZE);

    oldDist = &distributions[d];
    newDist = &distributions[distIndex++];

    newDist->west = oldDist->east;
    newDist->east = oldDist->west;
    newDist->cases = oldDist->cases;
  }

  distributions.resize(distIndex); // Shrink to fit
}


unsigned Distribution::set(
  const unsigned cards,
  const unsigned holding2)
{
  Distribution::setRanks(cards, holding2);
  if (opponents.len == 0)
    return 1;

  list<StackInfo> stack; // Unfinished expansions

  distributions.resize(CHUNK_SIZE);
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
            distributions.resize(distributions.size() + CHUNK_SIZE);

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
  return distIndex;
}


string Distribution::str() const
{
  if (distributions.empty())
    return "No distributions\n";

  vector<char> names(rankSize);
  names[0] = 'x';
  for (unsigned rank = 1; rank < rankSize; rank++)
    names[rank] = genericNames.at(rankSize-rank-1);

  stringstream ss;
  for (unsigned d = 0; d < distributions.size(); d++)
  {
    ss << 
      setw(8) << distributions[d].west.str(names) <<
      setw(8) << distributions[d].east.str(names) <<
      setw(8) << distributions[d].cases << "\n";
  }
  return ss.str();
}

