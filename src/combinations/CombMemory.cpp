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
const vector<unsigned> COMB_UNIQUE_COUNT =
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

const unsigned COMB_CHUNK_SIZE = 16;


mutex mtxCombMemory;


CombMemory::CombMemory()
{
  CombMemory::reset();
}


void CombMemory::reset()
{
  maxCards = 0;
  fullFlag = false;
  combEntries.clear();
  uniques.clear();
  counters.clear();
}


void CombMemory::resize(
  const unsigned maxCardsIn,
  const bool fullFlagIn)
{
  // If fullFlag is set, all the space for unique combinations is
  // allocated at once.  If not, the space grows as needed.
  // The latter is useful when we are only solving a single combination.

  maxCards = maxCardsIn;
  fullFlag = fullFlagIn;

  combEntries.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);
  counters.resize(maxCardsIn+1, 0);

  // There are three combinations with 1 card: It may be with
  // North, South or the opponents.
  unsigned numCombinations = 1;

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
        assert(cards < COMB_UNIQUE_COUNT.size());
        uniques[cards].resize(COMB_UNIQUE_COUNT[cards]);
      }
    }
    else
      uniques[cards].resize(COMB_CHUNK_SIZE);

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
  // When we do all combinations, we know the number in advance and
  // we do not change the memory locations.  But unlike in 
  // DistMemory we don't store pointers, so we only need to protect
  // the index generation here as well.

  assert(cards < combEntries.size()); 
  assert(holding < combEntries[cards].size());

  mtxCombMemory.lock();
  const unsigned uniqueIndex = counters[cards]++;

  // Grow if dynamic size is used up.
  if (! fullFlag && uniqueIndex >= uniques[cards].size())
    uniques[cards].resize(uniques[cards].size() + COMB_CHUNK_SIZE);

  mtxCombMemory.unlock();

  vector<Combination>& uniqs = uniques[cards];
  assert(uniqueIndex < uniqs.size());

  combEntries[cards][holding].setIndex(uniqueIndex);

  return uniqs[uniqueIndex];
}


Combination& CombMemory::getComb(
  const unsigned cards,
  const unsigned holding)
{
  assert(cards < combEntries.size());
  assert(holding < combEntries[cards].size());

  const unsigned index = combEntries[cards][holding].getIndex();
  assert(index < uniques[cards].size());

  return uniques[cards][index];
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


string CombMemory::strDynamic() const
{
  if (fullFlag)
    return "";

  stringstream ss;
  ss << "Combinations used\n";
  ss << right << setw(6) << "Cards" << setw(8) << "Used" << "\n";

  unsigned sum = 0;

  for (unsigned c = 0; c < counters.size(); c++)
  {
    if (counters[c])
    {
      ss << setw(6) << c << setw(8) << counters[c] << "\n";
      sum += counters[c];
    }
  }

  ss << string(14, '-') << "\n";
  ss << setw(14) << sum << "\n\n";

  return ss.str();
}

