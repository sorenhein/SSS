/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cassert>

#include "DistMemory.h"

// The numbers of splits are Fibonacci numbers.  
// If f(0) = 1, f(1) = 1, f(2) = 2, ...,  then dist(n) = f(2*n+1).
// There are a lot of possible interpretations on
// http://oeis.org/A001906 and http://oeis.org/A088305.

/*
  1,      //  0
  3,      //  1
  8,      //  2
  21,     //  3
  55,     //  4
  144,    //  5
  377,    //  6
  987,    //  7
  2584,   //  8
  6765,   //  9
  17711,  // 10
  46368,  // 11
  121383, // 12
  317811, // 13
  832040, // 14
  2178409 // 15
*/

// The numbers of unique distributions are also Fibonacci numbers.

const vector<unsigned> DIST_UNIQUE_COUNT =
{
  1,      //  0
  1,      //  1
  1,      //  2
  2,      //  3
  3,      //  4
  5,      //  5
  8,      //  6
  13,     //  7
  21,     //  8
  34,     //  9
  55,     // 10
  89,     // 11
  144,    // 12
  233,    // 13
  377,    // 14
  610     // 15
};



mutex mtxDistMemory;


DistMemory::DistMemory()
{
  DistMemory::reset();
}


void DistMemory::reset()
{
  maxCards = 0;
  fullFlag = false;
  distributions.clear();
  uniques.clear();
  usedCounts.clear();
  coverMemory.reset();
}


void DistMemory::resize(
  const unsigned char maxCardsIn,
  const bool fullFlagIn)
{
  // If fullFlag is set, all the space for distributions is
  // allocated at once.  If not, the space grows as needed.
  // The latter is useful when we are only solving a single combination.

  maxCards = maxCardsIn;
  fullFlag = fullFlagIn;

  distributions.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);

  // We count the total number of card splits for East and West.
  //
  // counters is the number of unique card splits, and they should
  // agree with the table in the comment at the top.
  //
  // cumulDists is the total number of distributions, so if one
  // distribution includes 8 card splits for East and West, then
  // that counts as 8.

  usedCounts.resize(maxCardsIn+1, 0);

  // A distribution is a set of card splits for a given rank
  // vector covering both East and West.
  // There are 2 E-W distributions with 1 card: Either they have it
  // or they don't.
  unsigned numDistributions = 1;

  for (unsigned cards = 0; cards < distributions.size(); cards++)
  {
    distributions[cards].resize(numDistributions);
    numDistributions <<= 1;

    // If we are running a single combination, resizeSingle must
    // be called first.
    if (fullFlag)
      uniques[cards].resize(DIST_UNIQUE_COUNT[cards]);
  }

  coverMemory.prepare(maxCardsIn);
}


void DistMemory::resizeSingle(const vector<set<unsigned>>& dependenciesCan)
{
  // Make just enough room for the canonical distributions we need.
  fullFlag = false;

  for (unsigned cards = 0; cards < dependenciesCan.size(); cards++)
  {
    const unsigned num = dependenciesCan[cards].size();
    if (num > 0)
      uniques[cards].resize(num);
  }
}


void DistMemory::resizeStats(ExplStats& explStats) const
{
  coverMemory.resizeStats(explStats);
}


Distribution& DistMemory::addFullMT(
  const unsigned char cards,
  const unsigned holding)
{
  // When we do all distributions, we know the number in advance and
  // we do not change the memory locations.  Therefore we can set
  // pointers directly.

  assert(cards < distributions.size());
  assert(holding < distributions[cards].size());

  Distribution& dist = distributions[cards][holding];
  dist.setRanks(cards, holding);
  const DistID distID = dist.getID();

  assert(cards < uniques.size());
  vector<DistCore>& uniqs = uniques[cards];

  if (distID.cards == cards && distID.holding == holding)
  {
    mtxDistMemory.lock();
    const unsigned uniqueIndex = usedCounts[cards]++;
    mtxDistMemory.unlock();

    assert(uniqueIndex < uniqs.size());
    DistCore& distCore = uniqs[uniqueIndex];

    dist.setPtr(&distCore);
    dist.split();
    dist.setLookups();
    dist.prepareCovers(coverMemory);
  }
  else
    dist.setPtr(distributions[distID.cards][distID.holding]);

  return dist;

}


void DistMemory::addCanonicalMT(
  const unsigned char cards,
  const unsigned holding)
{
  // As resizeSingle has been called, there is space for the uniques.
  // There is not much point in multi-threading this, but it should work.

  assert(cards < distributions.size());
  assert(holding < distributions[cards].size());

  Distribution& dist = distributions[cards][holding];
  dist.setRanks(cards, holding);

  assert(cards < uniques.size());
  vector<DistCore>& uniqs = uniques[cards];

  mtxDistMemory.lock();
  const unsigned uniqueIndex = usedCounts[cards]++;
  mtxDistMemory.unlock();

  assert(uniqueIndex < uniqs.size());
  DistCore& distCore = uniqs[uniqueIndex];

  dist.setPtr(&distCore);
  dist.split();
  dist.setLookups();
  dist.prepareCovers(coverMemory);
}


void DistMemory::addNoncanonicalMT(
  const unsigned char cards,
  const unsigned holding)
{
  // Just have to refer to the right unique ones.

  assert(cards < distributions.size());
  assert(holding < distributions[cards].size());

  Distribution& dist = distributions[cards][holding];
  dist.setRanks(cards, holding);
  const DistID distID = dist.getID();

  dist.setPtr(distributions[distID.cards][distID.holding]);
}


const Distribution& DistMemory::get(
  const unsigned char cards,
  const unsigned holding) const
{
  assert(holding < distributions[cards].size());
  return distributions[cards][holding];
}


Distribution& DistMemory::get(
  const unsigned char cards,
  const unsigned holding)
{
  assert(holding < distributions[cards].size());
  return distributions[cards][holding];
}


unsigned DistMemory::size(const unsigned char cards) const
{
  assert(cards < distributions.size());
  return distributions[cards].size();
}


unsigned DistMemory::used(const unsigned char cards) const
{
  assert(cards < usedCounts.size());
  return usedCounts[cards];
}


string DistMemory::strDynamic() const
{
  if (fullFlag)
    return "";

  stringstream ss;
  ss << "Distributions used\n";
  ss << right << setw(6) << "Cards" << setw(8) << "Used" << "\n";

  unsigned sum = 0;

  for (unsigned c = 0; c < usedCounts.size(); c++)
  {
    if (usedCounts[c])
    {
      ss << setw(6) << c  << setw(8) << usedCounts[c] << "\n";
      sum += usedCounts[c];
    }
  }

  ss << string(14, '-') << "\n";
  ss << setw(14) << sum << "\n\n";

  return ss.str();
}


string DistMemory::strCovers(const unsigned char cards) const
{
  if (usedCounts[cards] == 0)
    return "";

  stringstream ss;
  unsigned numTableaux, numUses;

  for (auto& distCore: uniques[cards])
  {
    distCore.getCoverCounts(numTableaux, numUses);
    if (numTableaux == 0)
      continue;

    ss << distCore.str() << "\n";
    ss << distCore.strCovers() << "\n";
  }

  return ss.str();
}


string DistMemory::strCoverCountsHeader() const
{
  stringstream ss;
  ss <<
    setw(6) << "Cards" <<
    setw(10) << "Tableaux" <<
    setw(10) << "Uses" << 
    setw(10) << "Avg" << 
    "\n";
  return ss.str();
}


string DistMemory::strCoverCounts(
  const unsigned char cmin,
  const unsigned char cmax) const
{
  stringstream ss;
  unsigned sumTableaux = 0;
  unsigned sumUses = 0;

  for (unsigned char cards = cmin; cards <= cmax; cards++)
  {
   if (usedCounts[cards] == 0)
     continue;

    unsigned cumTableaux = 0;
    unsigned cumUses = 0;
    unsigned numTableaux, numUses;

    for (auto& distCore: uniques[cards])
    {
      distCore.getCoverCounts(numTableaux, numUses);
      cumTableaux += numTableaux;
      cumUses += numUses;
    }

    if (cumTableaux == 0)
      continue;

    ss <<
      setw(6) << +cards <<
      setw(10) << cumTableaux <<
      setw(10) << cumUses << 
      setw(10) << fixed << setprecision(1) <<
        static_cast<float>(cumUses) / 
        static_cast<float>(cumTableaux) << "\n";
    
    sumTableaux += cumTableaux;
    sumUses += cumUses;
  }

  ss << string(36, '-') << "\n";

  ss <<
    setw(6) << "Sum" <<
    setw(10) << sumTableaux <<
    setw(10) << sumUses << 
    setw(10) << fixed << setprecision(1) <<
      static_cast<float>(sumUses) / 
      static_cast<float>(sumTableaux) << "\n\n";

  return ss.str();
}

