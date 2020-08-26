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

cout << "distIndex " << distIndex << ", dtop " << dtop << endl;
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
cout << Distribution::strDist(distIndex);
}


void Distribution::set(
  const unsigned cards,
  const vector<RankInfo>& oppsRank)
{
  list<DistInfo> stack; // Unfinished expansions
  list<DistInfo>::iterator stackIter;

  distributions.resize(CHUNK_SIZE);
  unsigned distIndex = 0; // Next one to write

  // TODO: Store this in Ranks.cpp and just pass it in?
  unsigned len = 0;
  for (auto oppr: oppsRank)
    len += oppr.count;

cout << "len " << len << endl;

  const unsigned rankSize = oppsRank.size();
  unsigned rankNext; // Next one to write

  // Only do the first half and then mirror the other lengths
  // (optimization).
  const unsigned lenMid = ((len & 1) ? (len-1)/2 : len/2);
  DistInfo * newDist;

  for (unsigned lenWest = 0; lenWest <= lenMid; lenWest++)
  {
    stack.emplace_back(DistInfo());
    stackIter = prev(stack.end());
    newDist = &*stackIter;

    newDist->west.resize(cards);
    newDist->east.resize(cards);
    for (unsigned r = 0; r < rankSize; r++)
      newDist->west[r] = 0;
    newDist->lenWest = 0;
    newDist->used = 0;
    newDist->rankNext = 0;
    newDist->cases = 1;

cout << "Starting on west length " << lenWest << endl;
cout << "Stack:\n";
cout << Distribution::strStack(stack);
cout << "Distributions:\n";
cout << Distribution::strDist(distIndex);

    while (! stack.empty())
    {
      newDist = &*stackIter;
      rankNext = newDist->rankNext;
      while (rankNext < rankSize && oppsRank[rankNext].count == 0)
        rankNext++;
      assert(rankNext < rankSize);

      newDist->used += oppsRank[rankNext].count;

      const unsigned gap = lenWest - newDist->lenWest;
      const unsigned available = oppsRank[rankNext].count;
cout << "Current rank " << rankNext << ", gap " << gap << ", " <<
  "available " << available << endl;

      for (unsigned r = 0; r <= min(gap, available); r++)
      {
        if (r == gap)
        {
          // End of "recursion".
          if (distIndex == distributions.size())
            distributions.resize(distributions.size() + CHUNK_SIZE);

          distributions[distIndex] = * newDist;
          distributions[distIndex].west[rankNext] = r;
          distributions[distIndex].lenWest += r;
          distributions[distIndex].cases *= binomial[available][r];

          distributions[distIndex].lenEast = len - distributions[distIndex].lenWest;
          for (unsigned rr = 0; rr < rankSize; rr++)
            distributions[distIndex].east[rr] = oppsRank[rr].count - distributions[distIndex].west[rr];

          distIndex++;
        }
        else if (r + len >= gap + newDist->used)
        {
          // Can still reach our goal of lenWest cards.
          // Continue the "recursion".  They will end up in reverse
          // rank order.
          auto stackInserted = stack.emplace(next(stackIter));
          * stackInserted = * newDist;

          stackInserted->west[rankNext] = r;
          stackInserted->lenWest += r;
          stackInserted->rankNext = rankNext+1;
          stackInserted->cases *= binomial[available][r];
        }
        else
        {
cout << "lenWest goal not achievable from here: r " << r << ", used " <<
  newDist->used << "\n";
        }

if (r == gap)
  cout << "Ending recursion, r = " << r <<"\n";
else if (r + len >= gap + newDist->used)
  cout << "Continuing recursion, r = " << r << "\n";
else
  cout << "Ending recursion without a result\n";

cout << "Stack:\n";
cout << Distribution::strStack(stack);
cout << "Distributions:\n";
cout << Distribution::strDist(distIndex);

      }
      stackIter = stack.erase(stackIter);
cout << "Erased from stack, rankNext was " << rankNext << "\n";
cout << "Stack:\n";
cout << Distribution::strStack(stack);
cout << "Distributions:\n";
cout << Distribution::strDist(distIndex);
      rankNext++;
    }
cout << "Finished west length " << lenWest << endl;
cout << "Stack:\n";
cout << Distribution::strStack(stack);
cout << "Distributions:\n";
cout << Distribution::strDist(distIndex);
  }

  assert(distIndex > 0);
cout << "Done with recursion" << endl;

  Distribution::mirror(len, lenMid, distIndex);
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

