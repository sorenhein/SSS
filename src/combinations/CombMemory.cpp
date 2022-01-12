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

#include "CombMemory.h"

#include "../inputs/Control.h"

extern Control control;


// http://oeis.org/A051450
const vector<unsigned> UNIQUE_COUNT =
{
  1,      //  0
  2,      //  1
  5,      //  2
  12,     //  3
  30,     //  4
  76,     //  5
  195,    //  6
  504,    //  7
  1309,   //  8
  3410,   //  9
  8900,   // 10
  23256,  // 11
  60813,  // 12
  159094, // 13
  416325, // 14
  1089648 // 15
};


mutex mtxCombMemory;


CombMemory::CombMemory()
{
  CombMemory::reset();
}


void CombMemory::reset()
{
  maxCards = 0;
  combEntries.clear();
  uniques.clear();
  counters.clear();
}


void CombMemory::resize(
  const unsigned maxCardsIn,
  const bool fullFlag)
{
  // If fullFlag is set, all the space for unique combinations is
  // allocated at once.  If not, the space grows as needed.
  // The latter is useful when we are only solving a single combination.

  maxCards = maxCardsIn;

  combEntries.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);
  counters.resize(maxCardsIn+1, 0);

  // There are three combinations with 1 card: It may be with
  // North, South or the opponents.
  unsigned numCombinations = 1;

  // for (unsigned cards = 1; cards <= maxCards; cards++)
  for (unsigned cards = 0; cards < combEntries.size(); cards++)
  {
    combEntries[cards].resize(numCombinations);

    if (fullFlag)
    {
      if (control.runRankComparisons())
      {
        // One void plus half of the rest, as North always has the
        // highest card among the North-South.

        const unsigned numRanked = 1 + ((numCombinations-1) >> 1);
        uniques[cards].resize(numRanked);
      }
      else
      {
        assert(cards < UNIQUE_COUNT.size());
        uniques[cards].resize(UNIQUE_COUNT[cards]);
      }
    }
    else
    {
      // TODO Have some block size, grow later when needed.
      assert(false);
    }

    numCombinations *= 3;
  }
}


unsigned CombMemory::size(const unsigned cards) const
{
  return combEntries[cards].size();
}


Combination& CombMemory::add(
  const unsigned cards,
  const unsigned holding)
{
// cout << "ADDING " << cards << ", " << holding << endl;

  mtxCombMemory.lock();
  const unsigned uniqueIndex = counters[cards]++;
  mtxCombMemory.unlock();

  vector<Combination>& uniqs = uniques[cards];
/*
if (uniqueIndex >= uniqs.size())
{
  cout << "cards " << cards << endl;
  cout << "uniqueIndex " << uniqueIndex << endl;
  cout << "uniqs.size " << uniqs.size() << endl;
}
*/
  assert(uniqueIndex < uniqs.size());

  assert(cards < combEntries.size());
  assert(holding < combEntries[cards].size());
  combEntries[cards][holding].setIndex(uniqueIndex);

  return uniqs[uniqueIndex];
}


const Combination& CombMemory::getComb(
  const unsigned cards,
  const unsigned holding) const
{
  assert(cards < combEntries.size());
  assert(holding < combEntries[cards].size());

  const unsigned index = combEntries[cards][holding].getIndex();
  assert(index < uniques[cards].size());

  return uniques[cards][index];
}


CombEntry& CombMemory::getEntry(
  const unsigned cards,
  const unsigned holding)
{
  assert(cards < combEntries.size());
  assert(holding < combEntries[cards].size());

  return combEntries[cards][holding];
}


const CombEntry& CombMemory::getEntry( 
  const unsigned cards,
  const unsigned holding) const
{
  assert(cards < combEntries.size());
  assert(holding < combEntries[cards].size());

  return combEntries[cards][holding];
}

