#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Combinations.h"
#include "Combination.h"
#include "Distributions.h"
#include "Distribution.h"
#include "CombEntry.h"

#include "plays/Plays.h"

#include "ranks/Ranks.h"

#include "stats/Timers.h"

extern Timers timers;


Combination::Combination()
{
  Combination::reset();
}


Combination::~Combination()
{
}


void Combination::reset()
{
}


const Strategies& Combination::strategize(
  const CombEntry& centry,
  const Combinations& combinations,
  const Distributions& distributions,
  Ranks& ranks,
  Plays& plays,
  bool debugFlag)
{
  cout << "Cards" << setw(3) << ranks.size() << ": " <<
    centry.canonicalHolding3 << " / " <<
    centry.canonicalHolding2 << endl;

  // Look up a pointer to the EW distribution of this combination.
  timers.start(TIMER_PLAYS);
  distPtr = distributions.ptrNoncanonical(
    ranks.size(), centry.canonicalHolding2);

  // Make the plays.
  Result trivialEntry;
  plays.clear();

  DebugPlay debugFlagTmp = DEBUGPLAY_NONE;
  // if (centry.canonicalHolding3 == 14480 && ranks.size() == 10)
  // if (centry.canonicalHolding3 == 59 && ranks.size() == 5)
  // if (centry.canonicalHolding3 == 132889 && ranks.size() == 12)
  // if (centry.canonicalHolding3 == 4025 && ranks.size() == 10)
  // if (centry.canonicalHolding3 == 4801 && ranks.size() == 8)
  // if (centry.canonicalHolding3 == 4757 && ranks.size() == 9)
  if (centry.canonicalHolding3 == 4801 && ranks.size() == 8)
  // if (centry.canonicalHolding3 == 1608 && ranks.size() == 8)
  // if (centry.canonicalHolding3 == 1598 && ranks.size() == 8)
  // if (centry.canonicalHolding3 == 1585 && ranks.size() == 8)
  // if (centry.canonicalHolding3 == 59 && ranks.size() == 5)
  {
    debugFlagTmp = static_cast<DebugPlay>(0x3f);
    cout << ranks.strTable();
    wcout << ranks.wstrDiagram();
    cout << distPtr->str();
  }

  const CombinationType ctype = ranks.setPlays(plays, trivialEntry);

  // If it's a trivial situation, make the strategies.
  if (ctype == COMB_TRIVIAL)
  {
    // Fill out a single constant strategy with the right value and size.
    strats.setTrivial(trivialEntry, 
      static_cast<unsigned char>(distPtr->size()));
    return strats;
  }

  // Complete the plays such that their ends point to combinations.
  plays.setCombPtrs(combinations);
  timers.stop(TIMER_PLAYS);

  if (debugFlag)
  {
    cout << "Plays\n" << plays.str() << endl;
    cout << "Distribution\n" << distPtr->str() << endl;
  }

  plays.clearStrategies();

  timers.start(TIMER_STRATEGIZE);
  strats = plays.strategize(distPtr, debugFlagTmp);
  timers.stop(TIMER_STRATEGIZE);

  // Make a note of the type of strategy? (COMB_TRIVIAL etc.)

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
cout << "cholding2 is " << centry.canonicalHolding2 << 
  ", size " << ranks.size() << endl;

  // Look up a pointer to the EW distribution of this combination.
  distPtr = distributions.ptrNoncanonical(
    ranks.size(), centry.canonicalHolding2);

  // Make the plays.
  Result trivialEntry;
  plays.clear();
if (centry.canonicalHolding3 == 208)
{
  cout << "HERE0\n";
}
  const CombinationType ctype = ranks.setPlays(plays, trivialEntry);
  
  // If it's a trivial situation, make the strategies.
  if (ctype == COMB_TRIVIAL)
  {
    // Fill out a single constant strategy with the right value and size.
    strats.setTrivial(trivialEntry, 
      static_cast<unsigned char>(distPtr->size()));
    return strats;
  }

// return strats;

cout << "A " << centry.canonicalHolding3 << endl;
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
  cout << "Distribution\n" << distPtr->str() << endl;
}

  // plays.strategizeVoid(distPtr, strats, debugFlag);

  // strats = plays.strategizeVoid(distPtr, static_cast<DebugPlay>(0x3f));
  strats = plays.strategize(distPtr, static_cast<DebugPlay>(0x3f));

// cout << "C " << centry.canonicalHolding3 << endl;

  // Make a note of the type of strategy? (COMB_TRIVIAL etc.)

  return strats;
}


const Strategies& Combination::strategies() const
{
  return strats;
}

