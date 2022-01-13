/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Combinations.h"
#include "Combination.h"
#include "CombEntry.h"

#include "../distributions/Distributions.h"

#include "../inputs/Control.h"

#include "../plays/Plays.h"

#include "../ranks/Ranks.h"

#include "../utils/Timers.h"

extern Control control;
extern Timers timers;


Combination::Combination()
{
  Combination::reset();
}


void Combination::reset()
{
  maxRank = 0;
}


void Combination::setMaxRank(const unsigned char maxRankIn)
{
  maxRank = maxRankIn;
}


unsigned char Combination::getMaxRank() const
{
  return maxRank;
}


void Combination::setTrivial(
  const Result& entry,
  const unsigned char count,
  const bool debugFlag)
{
  strats.setTrivial(entry, count);

  if (debugFlag && control.outputBasicResults())
    cout << strats.str("Trivial result", 
      control.runRankComparisons()) << "\n";
}


const Strategies& Combination::strategize(
  const CombEntry& centry,
  const Combinations& combinations,
  const Distributions& distributions,
  Ranks& ranks,
  Plays& plays,
  bool debugFlag)
{
  if (control.outputHolding())
  {
    cout << "Cards" << setw(3) << ranks.size() << ": " <<
      centry.strHolding() << endl;
  }

  // Look up a pointer to the EW distribution of this combination.
  timers.start(TIMER_PLAYS);

  // Make the plays.
  Result trivialEntry;
  plays.clear();

  // distPtr = distributions.ptrNoncanonical(
    // ranks.size(), centry.getHolding2());
  const Distribution& dist = distributions.get(
    ranks.size(), centry.getHolding2());

  if (control.outputBasicResults() || debugFlag)
  {
    wcout << "\n" << ranks.wstrDiagram() << "\n";
    cout << ranks.strTable();
    cout << "Distributions\n" << dist.str() << "\n";
  }

  const CombinationType ctype = ranks.setPlays(plays, trivialEntry);

  // If it's a trivial situation, make the strategies.
  if (ctype == COMB_CONSTANT)
  {
    // Fill out a single constant strategy with the right value and size.
    Combination::setTrivial(trivialEntry, 
      static_cast<unsigned char>(dist.size()), true);

    return strats;
  }

  // Complete the plays such that their ends point to combinations.
  plays.setCombPtrs(combinations);
  timers.stop(TIMER_PLAYS);

  if (debugFlag)
    cout << "Plays\n" << plays.str() << endl;

  plays.clearStrategies();

  timers.start(TIMER_STRATEGIZE);
  strats = plays.strategize(dist, 
    (debugFlag ? static_cast<DebugPlay>(0x3f) : DEBUGPLAY_NONE));
  timers.stop(TIMER_STRATEGIZE);

  // Make a note of the type of strategy? (COMB_CONSTANT etc.)

  if (control.outputBasicResults())
    cout << strats.str("Result", 
      control.runRankComparisons()) << "\n";

  return strats;
}


const Strategies& Combination::strategizeVoid(
  const CombEntry& centry,
  const Combinations& combinations,
  const Distributions& distributions,
  Ranks& ranks,
  Plays& plays,
  bool debugFlag)
{
cout << "cholding2 is " << centry.getHolding2() << 
  ", size " << ranks.size() << endl;

  // Look up a pointer to the EW distribution of this combination.
  // distPtr = distributions.ptrNoncanonical(
    // ranks.size(), centry.getHolding2());
  const Distribution& dist = distributions.get(
    ranks.size(), centry.getHolding2());

  // Make the plays.
  Result trivialEntry;
  plays.clear();
if (centry.getHolding3() == 208)
{
  cout << "HERE0\n";
}
  const CombinationType ctype = ranks.setPlays(plays, trivialEntry);
  
  // If it's a trivial situation, make the strategies.
  if (ctype == COMB_CONSTANT)
  {
    // Fill out a single constant strategy with the right value and size.
    strats.setTrivial(trivialEntry, 
      static_cast<unsigned char>(dist.size()));
    return strats;
  }

// return strats;

cout << "A " << centry.getHolding3() << endl;
  // Complete the plays such that their ends point to combinations.
// if (ranks.size() == 8 && centry.canonicalHolding3 == 530)
// {
  // cout << "HERE\n";
// }
  plays.setCombPtrs(combinations);
// cout << "B " << centry.canonicalHolding3 << endl;
if (debugFlag)
{
  cout << "Plays\n" << plays.str() << endl;
  cout << "Distribution\n" << dist.str() << endl;
}

  // plays.strategizeVoid(distPtr, strats, debugFlag);

  // strats = plays.strategizeVoid(distPtr, static_cast<DebugPlay>(0x3f));
  strats = plays.strategize(dist, static_cast<DebugPlay>(0x3f));

// cout << "C " << centry.canonicalHolding3 << endl;

  // Make a note of the type of strategy? (COMB_CONSTANT etc.)

  return strats;
}


const Strategies& Combination::strategies() const
{
  return strats;
}

