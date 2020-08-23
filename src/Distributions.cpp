#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>
#include <algorithm>
#include <cassert>

#include "Distributions.h"
#include "const.h"

#define CHUNK_SIZE 50

const string genericNames = "HhGgIiJj";


Distributions::Distributions()
{
  Distributions::reset();
  Distributions::setBinomial();
}


Distributions::~Distributions()
{
}


void Distributions::reset()
{
  distributions.clear();
}


void Distributions::setBinomial()
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


void Distributions::mirror(
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

  Distribution * oldDist;
  Distribution * newDist;

  for (unsigned d = dtop; dtop-- > 0; )
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


void Distributions::set(
  const unsigned cards,
  const vector<RankInfo>& oppsRank)
{
  list<Distribution> stack(CHUNK_SIZE); // Unfinished expansions
  list<Distribution>::iterator stackIter;

  distributions.resize(CHUNK_SIZE);
  unsigned distIndex = 0; // Next one to write

  // TODO: Store this in Ranks.cpp and just pass it in?
  unsigned len = 0;
  for (auto oppr: oppsRank)
    len += oppr.count;

  const unsigned rankSize = oppsRank.size();
  unsigned rankNext; // Next one to write

  // Only do the first half and then mirror the other lengths
  // (optimization).
  const unsigned lenMid = ((len & 1) ? (len-1)/2 : len/2);
  Distribution * newDist;

  for (unsigned lenWest = 0; lenWest <= lenMid; lenWest++)
  {
    stack.emplace_back(Distribution());
    stackIter = prev(stack.end());
    newDist = &*stackIter;

    newDist->west.resize(cards);
    newDist->east.resize(cards);
    for (unsigned r = 0; r < rankSize; r++)
      newDist->west[r] = 0;
    newDist->lenWest = 0;
    newDist->cases = 1;

    rankNext = 0;

    while (true)
    {
      while (rankNext < rankSize && newDist->west[rankNext] == 0)
        rankNext++;
      assert(rankNext < rankSize);

      const unsigned gap = len - newDist->lenWest;
      const unsigned available = newDist->west[rankNext];

      for (unsigned r = 0; r <= min(gap, available); r++)
      {
        if (r == gap)
        {
          // End of "recursion".
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE);

          distributions[distIndex] = * newDist;
          * newDist = distributions[distIndex++];
        }
        else
        {
          // Continue the "recursion".  They will end up in reverse
          // rank order.
          auto stackInserted = stack.emplace(next(stackIter));
          * stackInserted = * newDist;
          * newDist = * stackInserted;
        }

        newDist->west[rankNext] = r;
        newDist->lenWest += r;
        newDist->cases *= binomial[available][r];

        if (r == gap)
        {
          // Fill out the East side.
          newDist->lenEast = len - newDist->lenWest;
          for (unsigned rr = 0; rr <= rankNext; rr++)
            newDist->east[rr] = oppsRank[rr].count - newDist->west[rr];
        }
      }
      stackIter = stack.erase(stackIter);
    }
  }

  assert(distIndex > 0);

  Distributions::mirror(len, lenMid, distIndex);
}


string Distributions::rank2str(
  const vector<unsigned>& ranks,
  const vector<string>& names) const
{
  string s = "";
  for (unsigned r = ranks.size(); --r > 0; )
  {
    if (ranks[r] > 0)
      s += names[r];
  }

  return (s == "" ? "-" : s);
}


string Distributions::str() const
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
  names[0] = "x";

  unsigned i = 0;
  for (unsigned r = maxRank; r > 0; r--)
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
      setw(8) << Distributions::rank2str(distributions[d].west, names) <<
      setw(8) << Distributions::rank2str(distributions[d].east, names) <<
      setw(8) << distributions[d].cases << "\n";
  }
  return ss.str();
}

