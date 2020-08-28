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


unsigned Distribution::set(
  const unsigned cards,
  const unsigned holding2)
{
  Distribution::setRanks(cards, holding2);
  if (opponents.len == 0)
    return 1;

  list<StackInfo> stack; // Unfinished expansions
  list<StackInfo>::iterator stackIter;

  distributions.resize(CHUNK_SIZE);
  unsigned distIndex = 0; // Next one to write

  // Only do the first half and then mirror the other lengths.
  const unsigned lenMid = opponents.len / 2;

  for (unsigned lenWest = 0; lenWest <= lenMid; lenWest++)
  {
    assert(stack.empty());
    stack.emplace_back(StackInfo(rankSize));
    stackIter = stack.begin();

    while (! stack.empty())
    {
      stackIter = stack.begin();
      const unsigned rank = stackIter->rankNext; // Next to write

      stackIter->seen += opponents.counts[rank];

      const unsigned gap = lenWest - stackIter->west.len;
      const unsigned available = opponents.counts[rank];

      for (unsigned r = 0; r <= min(gap, available); r++)
      {
        if (r == gap)
        {
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE);

          DistInfo& dist = distributions[distIndex];

          dist.west = stackIter->west;
          dist.add(rank, r, binomial[available][r]);
          dist.east.diff(opponents, dist.west);

          distIndex++;
          break;
        }
        else if (r + opponents.len >= gap + stackIter->seen)
        {
          // Can still reach our goal of lenWest cards.
          // Continue the "recursion".  They will end up in reverse
          // rank order.

          stackIter = stack.insert(stackIter, * stackIter);
          auto stackInserted = next(stackIter);

          stackInserted->add(rank, r, binomial[available][r]);
        }
      }
      stack.pop_front();
    }
  }

  assert(distIndex > 0);

  Distribution::mirror(opponents.len, lenMid, distIndex);
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
  const vector<unsigned>& counts,
  const vector<string>& names) const
{
  string s = "";
  for (unsigned r = counts.size(); --r > 0; )
  {
    if (counts[r]> 0)
      s += string(counts[r], names[r].at(0));
  }

  return (s == "" ? "-" : s);
}


string Distribution::str() const
{
  if (distributions.empty())
    return "No distributions\n";

  unsigned numRanks = 0;
  unsigned maxRank = 0;
  for (unsigned r = 0; r < distributions[0].west.counts.size(); r++)
  {
    if (distributions[0].west.counts[r] == 0 && 
        distributions[0].east.counts[r] == 0)
      continue;

    numRanks++;
    maxRank = r;
  }

  vector<string> names(maxRank+1);
  unsigned minRank;
  if (distributions[0].west.counts[0] == 0 && 
      distributions[0].east.counts[0] == 0)
    minRank = 1;
  else
    minRank = 0;
  names[minRank] = "x";

  unsigned i = 0;
  for (unsigned r = maxRank; r > minRank; r--)
  {
    if (distributions[0].west.counts[r] > 0 || 
        distributions[0].east.counts[r] > 0)
    {
      names[r] = genericNames.substr(i, 1);
      i++;
    }
  }

  stringstream ss;
  for (unsigned d = 0; d < distributions.size(); d++)
  {
    ss << 
      setw(8) << Distribution::rank2str(distributions[d].west.counts, names) <<
      setw(8) << Distribution::rank2str(distributions[d].east.counts, names) <<
      setw(8) << distributions[d].cases << "\n";
  }
  return ss.str();
}

