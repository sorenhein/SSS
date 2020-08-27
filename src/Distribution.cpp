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


void Distribution::mirror(
  const unsigned len, 
  const unsigned lenMid,
  unsigned& distIndex)
{
  // Go backwards from one past the first element to mirror.
  unsigned dtop;
  if (len & 1)
    dtop = distIndex;
  else
  {
    dtop = distIndex;
    while (dtop >= 1 && distributions[dtop-1].lenWest == lenMid)
      dtop--;
    assert(dtop > 0 && distributions[dtop-1].lenWest + 1 == lenMid);
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
    newDist->lenWest = oldDist->lenEast;
    newDist->lenEast = oldDist->lenWest;
    newDist->cases = oldDist->cases;
  }

  distributions.resize(distIndex); // Shrink to fit
}


unsigned Distribution::set(
  const unsigned cards,
  const unsigned holding2)
{
  // Turn holding into oppsRank
  vector<RankInfo> oppsRank;
  oppsRank.resize(cards+1);
  
  bool prev_is_NS = true;
  unsigned nextRank = 0;
  unsigned h = holding2;

  // Quick and dirty.  Move to more compact vector format.
  for (unsigned i = 0; i < cards; i++)
  {
    const unsigned c = h & 1;
    if (c == CONVERT_NS)
      prev_is_NS = true;
    else
    {
      if (! prev_is_NS)
        nextRank++;

      oppsRank[nextRank].count++;
      prev_is_NS = false;
    }
    
    h >>= 1;
  }

  return Distribution::set(cards, oppsRank);
}


unsigned Distribution::set(
  const unsigned cards,
  const vector<RankInfo>& oppsRank)
{
UNUSED(cards);
  // TODO: Store this in Ranks.cpp and just pass it in?
  unsigned len = 0;
  for (auto oppr: oppsRank)
    len += oppr.count;

  if (len == 0)
    return 1;

  list<DistInfo> stack; // Unfinished expansions
  list<DistInfo>::iterator stackIter;

  distributions.resize(CHUNK_SIZE);
  unsigned distIndex = 0; // Next one to write

  const unsigned rankSize = oppsRank.size();
  unsigned rankNext; // Next one to write

  // Only do the first half and then mirror the other lengths
  // (optimization).
  const unsigned lenMid = ((len & 1) ? (len-1)/2 : len/2);

  for (unsigned lenWest = 0; lenWest <= lenMid; lenWest++)
  {
    assert(stack.empty());
    stack.emplace_back(DistInfo(rankSize));
    stackIter = stack.begin();

    while (! stack.empty())
    {
      stackIter = stack.begin();
      rankNext = stackIter->rankNext;
      while (rankNext < rankSize && oppsRank[rankNext].count == 0)
        rankNext++;
      assert(rankNext < rankSize);

      // newDist = &*stackIter;
      stackIter->used += oppsRank[rankNext].count;

      const unsigned gap = lenWest - stackIter->lenWest;
      const unsigned available = oppsRank[rankNext].count;

      for (unsigned r = 0; r <= min(gap, available); r++)
      {
        if (r == gap)
        {
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE);

          distributions[distIndex] = * stackIter;
          distributions[distIndex].west[rankNext] = r;
          distributions[distIndex].lenWest += r;
          distributions[distIndex].cases *= binomial[available][r];

          distributions[distIndex].lenEast = len - distributions[distIndex].lenWest;
          for (unsigned rr = 0; rr < rankSize; rr++)
            distributions[distIndex].east[rr] = oppsRank[rr].count - distributions[distIndex].west[rr];

          distIndex++;
          break;
        }
        else if (r + len >= gap + stackIter->used)
        {
          // Can still reach our goal of lenWest cards.
          // Continue the "recursion".  They will end up in reverse
          // rank order.

          stackIter = stack.insert(stackIter, * stackIter);
          auto stackInserted = next(stackIter);

          stackInserted->west[rankNext] = r;
          stackInserted->lenWest += r;
          stackInserted->rankNext = rankNext+1;
          stackInserted->cases *= binomial[available][r];
        }
        else
        {
        }
      }
      stack.pop_front();

      rankNext++;
    }
  }

  assert(distIndex > 0);

  Distribution::mirror(len, lenMid, distIndex);
  return distIndex;
}


string Distribution::strStack(const list<DistInfo>& stack) const
{
  int i = 0;
  string s = "";
  for (auto& st: stack)
  {
    s += to_string(i) + "   " + st.str();
    i++;
  }
  return s + "\n";
}


string Distribution::strDist(const unsigned distIndex) const
{
  string s = "";
  for (unsigned i = 0; i < distIndex; i++)
    s += to_string(i) + "   " + distributions[i].str();
  return s + "\n";
}


string Distribution::rank2str(
  const vector<unsigned>& ranks,
  const vector<string>& names) const
{
  string s = "";
  for (unsigned r = ranks.size(); --r > 0; )
  {
    if (ranks[r] > 0)
      s += string(ranks[r], names[r].at(0));
  }

  return (s == "" ? "-" : s);
}


string Distribution::str() const
{
  if (distributions.empty())
    return "No distributions\n";

  unsigned numRanks = 0;
  unsigned maxRank = 0;
  for (unsigned r = 0; r < distributions[0].west.size(); r++)
  {
    if (distributions[0].west[r] == 0 && distributions[0].east[r] == 0)
      continue;

    numRanks++;
    maxRank = r;
  }

  vector<string> names(maxRank+1);
  unsigned minRank;
  if (distributions[0].west[0] == 0 && distributions[0].east[0] == 0)
    minRank = 1;
  else
    minRank = 0;
  names[minRank] = "x";

  unsigned i = 0;
  for (unsigned r = maxRank; r > minRank; r--)
  {
    if (distributions[0].west[r] > 0 || distributions[0].east[r] > 0)
    {
      names[r] = genericNames.substr(i, 1);
      i++;
    }
  }

  stringstream ss;
  for (unsigned d = 0; d < distributions.size(); d++)
  {
    ss << 
      setw(8) << Distribution::rank2str(distributions[d].west, names) <<
      setw(8) << Distribution::rank2str(distributions[d].east, names) <<
      setw(8) << distributions[d].cases << "\n";
  }
  return ss.str();
}

