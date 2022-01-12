/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
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


CombMemory::CombMemory()
{
  CombMemory::reset();
}


void CombMemory::reset()
{
  maxCards = 0;
  combEntries.clear();
  uniques.clear();
  counter = 0;
}


void CombMemory::resize(const unsigned maxCardsIn)
{
  maxCards = maxCardsIn;

  combEntries.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);

  // There are three combinations with 1 card: It may be with
  // North, South or the opponents.
  unsigned numCombinations = 1;

  // for (unsigned cards = 1; cards <= maxCards; cards++)
  for (unsigned cards = 0; cards < combEntries.size(); cards++)
  {
    combEntries[cards].resize(numCombinations);

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

    numCombinations *= 3;
  }
}


Combination& CombMemory::add(
  const unsigned cards,
  const unsigned holding)
{
  const unsigned uniqueIndex = counter++; // Atomic
  vector<Combination>& uniqs = uniques[cards];
  assert(uniqueIndex < uniqs.size());

  assert(cards < combEntries.size());
  assert(holding < combEntries[cards].size());
  combEntries[cards][holding].setIndex(uniqueIndex);

  return uniqs[uniqueIndex];
}


Combination& CombMemory::get(
  const unsigned cards,
  const unsigned holding)
{
  assert(cards < combEntries.size());
  assert(holding < combEntries[cards].size());

  const unsigned index = combEntries[cards][holding].getIndex();
  assert(index < uniques[cards].size());

  return uniques[cards][index];
}

