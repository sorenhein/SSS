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
    newDist->west.len = oldDist->east.len;
    newDist->east.len = oldDist->west.len;
    newDist->cases = oldDist->cases;
  }

  distributions.resize(distIndex); // Shrink to fit
}


void Distribution::setRanks(
  const unsigned cards,
  const unsigned holding2,
  vector<RankEntry>& oppsReducedRank,
  unsigned& len)
{
  // We go with a minimal representation of East-West in terms of ranks,
  // so the rank numbers will be smaller.  We keep a correspondence
  // back and forth with the outside world.
  full2reduced.resize(cards);
  reduced2full.resize(cards);

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

      oppsReducedRank[nextReducedRank].rank = nextFullRank;
      oppsReducedRank[nextReducedRank].count++;

      full2reduced[nextFullRank] = nextReducedRank;
      reduced2full[nextReducedRank] = nextFullRank;

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
    oppsReducedRank.resize(maxReducedRank+1);
    full2reduced.resize(maxFullRank+1);
    reduced2full.resize(maxReducedRank+1);
  }
}


unsigned Distribution::set(
  const unsigned cards,
  const unsigned holding2)
{
  vector<RankEntry> oppsReducedRank;
  unsigned len;
  Distribution::setRanks(cards, holding2, oppsReducedRank, len);

  if (len == 0)
    return 1;

  list<StackInfo> stackReduced; // Unfinished expansions
  list<StackInfo>::iterator stackReducedIter;

  distributions.resize(CHUNK_SIZE);
  unsigned distIndex = 0; // Next one to write

  const unsigned rankReducedSize = oppsReducedRank.size();
  unsigned rankReducedNext; // Next one to write

  // Only do the first half and then mirror the other lengths
  // (optimization).
  // TODO Just len/2 ?
  const unsigned lenMid = ((len & 1) ? (len-1)/2 : len/2);

  for (unsigned lenWest = 0; lenWest <= lenMid; lenWest++)
  {
    assert(stackReduced.empty());
    stackReduced.emplace_back(StackInfo(rankReducedSize));
    stackReducedIter = stackReduced.begin();

    stackReducedIter->west.ranks.resize(rankReducedSize);
    stackReducedIter->west.len = 0;
    stackReducedIter->seen = 0;
    stackReducedIter->rankNext = 0;
    stackReducedIter->cases = 1;

    while (! stackReduced.empty())
    {
      stackReducedIter = stackReduced.begin();
      rankReducedNext = stackReducedIter->rankNext;

      stackReducedIter->seen += oppsReducedRank[rankReducedNext].count;

      const unsigned gap = lenWest - stackReducedIter->west.len;
      const unsigned available = oppsReducedRank[rankReducedNext].count;

      for (unsigned r = 0; r <= min(gap, available); r++)
      {
        if (r == gap)
        {
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE);

          distributions[distIndex].west.ranks.resize(rankReducedSize);
          distributions[distIndex].east.ranks.resize(rankReducedSize);

          distributions[distIndex].west = stackReducedIter->west;
          distributions[distIndex].west.ranks[rankReducedNext].rank = 
            oppsReducedRank[rankReducedNext].rank;
          distributions[distIndex].west.ranks[rankReducedNext].count = r;
          distributions[distIndex].west.len =
            stackReducedIter->west.len + r;
          distributions[distIndex].cases =
            stackReducedIter->cases * binomial[available][r];
            
          distributions[distIndex].east.len = len - distributions[distIndex].west.len;
          for (unsigned rr = 0; rr < rankReducedSize; rr++)
          {
            distributions[distIndex].east.ranks[rr].rank = 
              distributions[distIndex].west.ranks[rr].rank;

            distributions[distIndex].east.ranks[rr].count = 
              oppsReducedRank[rr].count - distributions[distIndex].west.ranks[rr].count;
          }

          distIndex++;
          break;
        }
        else if (r + len >= gap + stackReducedIter->seen)
        {
          // Can still reach our goal of lenWest cards.
          // Continue the "recursion".  They will end up in reverse
          // rank order.

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
      stackReduced.pop_front();
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
  const vector<RankEntry>& ranks,
  const vector<string>& names) const
{
  string s = "";
  for (unsigned r = ranks.size(); --r > 0; )
  {
    if (ranks[r].count > 0)
      s += string(ranks[r].count, names[r].at(0));
  }

  return (s == "" ? "-" : s);
}


string Distribution::str() const
{
  if (distributions.empty())
    return "No distributions\n";

  unsigned numRanks = 0;
  unsigned maxRank = 0;
  for (unsigned r = 0; r < distributions[0].west.ranks.size(); r++)
  {
    if (distributions[0].west.ranks[r].count == 0 && 
        distributions[0].east.ranks[r].count == 0)
      continue;

    numRanks++;
    maxRank = r;
  }

  vector<string> names(maxRank+1);
  unsigned minRank;
  if (distributions[0].west.ranks[0].count == 0 && 
      distributions[0].east.ranks[0].count == 0)
    minRank = 1;
  else
    minRank = 0;
  names[minRank] = "x";

  unsigned i = 0;
  for (unsigned r = maxRank; r > minRank; r--)
  {
    if (distributions[0].west.ranks[r].count > 0 || 
        distributions[0].east.ranks[r].count > 0)
    {
      names[r] = genericNames.substr(i, 1);
      i++;
    }
  }

  stringstream ss;
  for (unsigned d = 0; d < distributions.size(); d++)
  {
    ss << 
      setw(8) << Distribution::rank2str(distributions[d].west.ranks, names) <<
      setw(8) << Distribution::rank2str(distributions[d].east.ranks, names) <<
      setw(8) << distributions[d].cases << "\n";
  }
  return ss.str();
}

