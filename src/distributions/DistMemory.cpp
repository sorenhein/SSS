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

// The numbers of distributions are Fibonacci numbers.  
// If f(0) = 1, f(1) = 1, f(2) = 2, ...,  then dist(n) = f(2*n+1).
// There are a lot of possible interpretations on
// http://oeis.org/A001906 and http://oeis.org/A088305.

const vector<unsigned> DIST_UNIQUE_COUNT =
{
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
};

const unsigned DIST_CHUNK_SIZE = 16;


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
  counters.clear();
  cumulSplits.clear();
}


void DistMemory::resize(
  const unsigned maxCardsIn,
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

  counters.resize(maxCardsIn+1, 0);
  cumulSplits.resize(maxCardsIn+1, 0);

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


void DistMemory::add(
  const unsigned cards,
  const unsigned holding)
{
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

    const unsigned uniqueIndex = counters[cards]++;

    // Grow if dynamic size is used up.
    if (! fullFlag && uniqueIndex >= uniques[cards].size())
      uniques[cards].resize(uniques[cards].size() + DIST_CHUNK_SIZE);

    assert(uniqueIndex < uniqs.size());

// cout << "uniqueIndex " << uniqueIndex << ", size[" <<
  // cards << "] = " << uniqs.size() << endl;
    DistCore& distCore = uniqs[uniqueIndex];
// cout << "address " << &uniqs[uniqueIndex] << ", " << &distCore << endl;
    dist.setPtr(&distCore);

// cout << "Added (" << cards << ", " << holding << ")\n";

    mtxDistMemory.unlock();

// cout << "About to split" << endl;
    dist.split();
// cout << "About to lookup" << endl;
    dist.setLookups();
// cout << "After split and lookup" << endl;
// cout << uniqs[uniqueIndex].str() << endl;
// cout << "dist size " << dist.size() << endl;
    // distCore.splitAlternative(distEntry.distMap);
    // distCore.setLookups();

    mtxDistMemory.lock();
    cumulSplits[cards] += dist.size();
    mtxDistMemory.unlock();
  }
  else
  {
    mtxDistMemory.lock();

    dist.setPtr(distributions[distID.cards][distID.holding]);
    // distEntry.distCorePtr = 
      // distEntries[distID.cards][distID.holding].distCorePtr;

    cumulSplits[cards] += dist.size();

// cout << "Repeated (" << cards << ", " << holding << ") as (" <<
  // distID.cards << ", " << distID.holding << ")" << endl;

    mtxDistMemory.unlock();
  }

// cout << "Done adding\n" << endl;
}


const Distribution& DistMemory::get(
  const unsigned cards,
  const unsigned holding) const
{
  assert(holding < distributions[cards].size());
// cout << "DM get(" << cards << ", " << holding << "): " <<
  // uniqueIndex << "\n";
// cout << uniques[cards][uniqueIndex].str();
  // return * distEntries[cards][holding].distCorePtr;
  return distributions[cards][holding];
}


unsigned DistMemory::size(const unsigned cards) const
{
  assert(cards < distributions.size());
  return distributions[cards].size();
}


unsigned DistMemory::numUniques(const unsigned cards) const
{
  return counters[cards];
}


unsigned DistMemory::numSplits(const unsigned cards) const
{
  return cumulSplits[cards];
}


string DistMemory::strDynamic() const
{
  if (fullFlag)
    return "";

  stringstream ss;
  ss << "Number of distributions used\n";
  ss << right << setw(6) << "Cards" << setw(8) << "Used" << "\n";

  for (unsigned c = 0; c < counters.size(); c++)
  {
   if (counters[c])
     ss << setw(6) << c  << setw(8) << counters[c] << "\n";
  }

  return ss.str();
}


string DistMemory::str(const unsigned cards) const
{
  unsigned cmin, cmax;
  if (cards == 0)
  {
    cmin = 1;
    cmax = maxCards;
  }
  else
  {
    cmin = cards;
    cmax = cards;
  }

  stringstream ss;
  ss <<
    setw(5) << "Cards" <<
    setw(9) << "Count" <<
    setw(9) << "Dists" <<
    setw(9) << "Avg." <<
    setw(9) << "Uniques" <<
    "\n";

  for (unsigned c = cmin; c <= cmax; c++)
  {
    assert(c < cumulSplits.size() && c < distributions.size());
    if (cumulSplits[c] == 0)
      continue;

    ss <<
      setw(5) << c <<
      setw(9) << distributions[c].size() <<
      setw(9) << cumulSplits[c] <<
      setw(9) << fixed << setprecision(2) <<
        static_cast<double>(cumulSplits[c]) / distributions[c].size() <<
      setw(9) << counters[c] <<
      "\n";
  }

  return ss.str() + "\n";
}

