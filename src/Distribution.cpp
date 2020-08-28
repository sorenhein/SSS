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


void Distribution::setRanks(
  const unsigned cards,
  const unsigned holding2,
  vector<unsigned>& oppsFullRank,
  vector<RankEntry>& oppsReducedRank,
  unsigned& len)
{
  oppsFullRank.resize(cards);
  full2reduced.resize(cards);

  oppsReducedRank.resize(cards); // Can be reduced to about half
  
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
  len = 0;

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

      oppsFullRank[nextFullRank]++;

      oppsReducedRank[nextReducedRank].rank = nextFullRank;
      oppsReducedRank[nextReducedRank].count++;

      maxFullRank = nextFullRank;
      maxReducedRank = nextReducedRank;
      len++;
      prev_is_NS = false;
    }
    h >>= 1;
  }

  // Shrink to fit.
  if (len > 0)
  {
    oppsFullRank.resize(maxFullRank+1);
    oppsReducedRank.resize(maxReducedRank+1);
  }
}


unsigned Distribution::set(
  const unsigned cards,
  const unsigned holding2)
{
  vector<unsigned> oppsFullRank;
  vector<RankEntry> oppsReducedRank;
  unsigned len;
  Distribution::setRanks(cards, holding2, oppsFullRank, oppsReducedRank, len);

  if (len == 0)
    return 1;

  list<DistInfo> stackFull; // Unfinished expansions
  list<DistInfo>::iterator stackFullIter;

  list<StackInfo> stackReduced; // Unfinished expansions
  list<StackInfo>::iterator stackReducedIter;

  distributions.resize(CHUNK_SIZE);
  unsigned distIndex = 0; // Next one to write

  const unsigned rankFullSize = oppsFullRank.size();
  unsigned rankFullNext; // Next one to write

  const unsigned rankReducedSize = oppsReducedRank.size();
  unsigned rankReducedNext; // Next one to write

  // Only do the first half and then mirror the other lengths
  // (optimization).
  // TODO Just len/2 ?
  const unsigned lenMid = ((len & 1) ? (len-1)/2 : len/2);

  for (unsigned lenWest = 0; lenWest <= lenMid; lenWest++)
  {
    assert(stackFull.empty());
    stackFull.emplace_back(DistInfo(rankFullSize));
    stackFullIter = stackFull.begin();

    assert(stackReduced.empty());
    stackReduced.emplace_back(StackInfo());
    stackReducedIter = stackReduced.begin();

    stackReducedIter->west.ranks.resize(rankReducedSize);
    stackReducedIter->west.len = 0;
    stackReducedIter->seen = 0;
    stackReducedIter->rankNext = 0;
    stackReducedIter->cases = 1;

    assert(stackFull.size() == stackReduced.size());

    while (! stackFull.empty())
    {
      stackFullIter = stackFull.begin();
      rankFullNext = stackFullIter->rankNext;
      while (rankFullNext < rankFullSize && oppsFullRank[rankFullNext] == 0)
        rankFullNext++;
      assert(rankFullNext < rankFullSize);
      
      stackReducedIter = stackReduced.begin();
      rankReducedNext = stackReducedIter->rankNext;
      assert(rankFullNext == oppsReducedRank[rankReducedNext].rank);

      stackFullIter->used += oppsFullRank[rankFullNext];
      stackReducedIter->seen += oppsReducedRank[rankReducedNext].count;
      assert(oppsFullRank[rankFullNext] == oppsReducedRank[rankReducedNext].count);
      assert(stackFullIter->used == stackReducedIter->seen);

      const unsigned gap = lenWest - stackFullIter->lenWest;
      const unsigned available = oppsFullRank[rankFullNext];

      assert(stackFullIter->lenWest == stackReducedIter->west.len);
      assert(oppsFullRank[rankFullNext] == oppsReducedRank[rankReducedNext].count);

      for (unsigned r = 0; r <= min(gap, available); r++)
      {
        if (r == gap)
        {
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE);

          for (unsigned rred = 0; rred < rankReducedNext; rred++)
          {
            assert(stackFullIter->west[ stackReducedIter->west.ranks[rred].rank ] == stackReducedIter->west.ranks[rred].count);

          }

          assert(stackFullIter->lenWest == stackReducedIter->west.len);
          assert(stackFullIter->cases == stackReducedIter->cases);

          distributions[distIndex] = * stackFullIter;
          distributions[distIndex].west[rankFullNext] = r;
          distributions[distIndex].lenWest += r;
          distributions[distIndex].cases *= binomial[available][r];

          distributions[distIndex].lenEast = len - distributions[distIndex].lenWest;
          for (unsigned rr = 0; rr < rankFullSize; rr++)
            distributions[distIndex].east[rr] = oppsFullRank[rr] - distributions[distIndex].west[rr];

          distIndex++;
          break;
        }
        else if (r + len >= gap + stackFullIter->used)
        {
          // Can still reach our goal of lenWest cards.
          // Continue the "recursion".  They will end up in reverse
          // rank order.

          stackFullIter = stackFull.insert(stackFullIter, * stackFullIter);
          auto stackFullInserted = next(stackFullIter);

          stackFullInserted->west[rankFullNext] = r;
          stackFullInserted->lenWest += r;
          stackFullInserted->rankNext = rankFullNext+1;
          stackFullInserted->cases *= binomial[available][r];

          stackReducedIter = stackReduced.insert(stackReducedIter, * stackReducedIter);
          auto stackReducedInserted = next(stackReducedIter);
          stackReducedInserted->west.ranks[rankReducedNext].rank =
            oppsReducedRank[rankReducedNext].rank;

          stackReducedInserted->west.ranks[rankReducedNext].count = r;
          stackReducedInserted->west.len += r;
          stackReducedInserted->rankNext = rankReducedNext+1;
          stackReducedInserted->cases *= binomial[available][r];
        }
      }
      stackFull.pop_front();
      stackReduced.pop_front();

      rankFullNext++;
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

