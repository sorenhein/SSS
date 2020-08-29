#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
#include <cassert>

#include "Distribution.h"
#include "const.h"


const string genericNames = "HhGgIiJj";

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


once_flag onceFlag;
vector<vector<unsigned>> binomial;
vector<vector<char>> names;


Distribution::Distribution()
{
  Distribution::reset();

  // https://stackoverflow.com/questions/8412630/
  // how-to-execute-a-piece-of-code-only-once
  if (static auto called = false; ! exchange(called, true))
  {
    Distribution::setBinomial();
    Distribution::setNames();
  }
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
      vec[rank] = genericNames.at(numRanks-rank-1);
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

  // Make/shrink room.
  distributions.resize(distIndex + dtop);

  for (unsigned d = dtop; d-- > 0; )
  {
assert(d < distributions.size());
assert(distIndex < distributions.size());
    oldDist = &distributions[d];
    newDist = &distributions[distIndex++];

    newDist->west = oldDist->east;
    newDist->east = oldDist->west;
    newDist->cases = oldDist->cases;
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
  return distIndex;
}


unsigned Distribution::setAlternative(
  const unsigned cards,
  const unsigned holding2)
{
  // I thought it might be faster to have two vectors whose sizes
  // don't change (much), rather than a list.  But it turns out to
  // be about the same.  I've left the code in.

  Distribution::setRanks(cards, holding2);
  if (opponents.len == 0)
    return 1;

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
  return distIndex;
}


string Distribution::str() const
{
  if (distributions.empty())
    return "No distributions\n";

  stringstream ss;
  for (unsigned d = 0; d < distributions.size(); d++)
  {
    ss << 
      setw(8) << distributions[d].west.str(names[rankSize]) <<
      setw(8) << distributions[d].east.str(names[rankSize]) <<
      setw(8) << distributions[d].cases << "\n";
  }
  return ss.str();
}

