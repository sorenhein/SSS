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


const unsigned DIST_CHUNK_SIZE = 10;


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

    if (fullFlag)
      uniques[cards].resize(DIST_UNIQUE_COUNT[cards]);
    else
      uniques[cards].resize(DIST_CHUNK_SIZE);
  }
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
  }
  else
    dist.setPtr(distributions[distID.cards][distID.holding]);

  return dist;

}


Distribution& DistMemory::addIncr(
  const unsigned char cards,
  const unsigned holding)
{
  // It is too much trouble to make this multi-threaded, as the 
  // memory locations in uniques may shift around until we are done
  // filling out the entries.  

  assert(cards < distributions.size());
  assert(holding < distributions[cards].size());

  Distribution& dist = distributions[cards][holding];
  dist.setRanks(cards, holding);
  const DistID distID = dist.getID();

  assert(cards < uniques.size());
  vector<DistCore>& uniqs = uniques[cards];

  if (distID.cards == cards && distID.holding == holding)
  {
    const unsigned uniqueIndex = usedCounts[cards]++;

    // Grow if dynamic size is used up.
    if (! fullFlag && uniqueIndex >= uniques[cards].size())
      uniques[cards].resize(uniques[cards].size() + DIST_CHUNK_SIZE);

    assert(uniqueIndex < uniqs.size());

    // We can't yet be sure of the final memory locaiton in uniques.
    dist.setIndex(uniqueIndex);
  }
  else
    dist.setIndex(distributions[distID.cards][distID.holding]);

  return dist;
}


void DistMemory::finishIncrMT(
  const unsigned char cards,
  const unsigned holding)
{
  Distribution& dist = distributions[cards][holding];
  const unsigned index = dist.getIndex();
  dist.setPtr(&uniques[cards][index]);

  // Only now can we run these methods.
  dist.setRanks(cards, holding);
  const DistID distID = dist.getID();
  if (distID.cards == cards && distID.holding == holding)
  {
    dist.split();
    dist.setLookups();
  }
}


const Distribution& DistMemory::get(
  const unsigned char cards,
  const unsigned holding) const
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
  ss << "Number of distributions used\n";
  ss << right << setw(6) << "Cards" << setw(8) << "Used" << "\n";

  for (unsigned c = 0; c < usedCounts.size(); c++)
  {
   if (usedCounts[c])
     ss << setw(6) << c  << setw(8) << usedCounts[c] << "\n";
  }

  return ss.str();
}

