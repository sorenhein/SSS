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

// TMP
#include "stats/Timer.h"
extern vector<Timer> timers1, timers2;


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
  distPtr = distributions.ptrNoncanonical(
    ranks.size(), centry.canonicalHolding2);

  // Make the plays.
  Result trivialEntry;
  plays.clear();

  DebugPlay debugFlagTmp = DEBUGPLAY_NONE;
  // if (centry.canonicalHolding3 == 14480 && ranks.size() == 10)
  if (centry.canonicalHolding3 == 60 && ranks.size() == 5)
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
    strats.setTrivial(trivialEntry, distPtr->size());
    return strats;
  }

  // Complete the plays such that their ends point to combinations.
  plays.setCombPtrs(combinations);

  if (debugFlag)
  {
    cout << "Plays\n" << plays.str() << endl;
    cout << "Distribution\n" << distPtr->str() << endl;
  }

  // TODO Probably don't need to pass in ranks to make memory
  // stay alive.

unsigned ps = plays.size();
if (ps >= 100)
  ps = 99;

assert(ps < timers1.size());
if (ranks.partnerVoid())
  timers1[ps].start();
  // plays.strategize(ranks, distPtr, strats, debugFlagTmp);
  // plays.strategize(distPtr, strats, debugFlagTmp);
  // TODO It gets copied here.  Keep a pointer instead?
  strats = plays.strategize(distPtr, debugFlagTmp);
if (ranks.partnerVoid())
  timers1[ps].stop();

if (ranks.partnerVoid())
{
plays.clearStrategies();

assert(ps < timers2.size());
timers2[ps].start();
// const Strategies& stmp = plays.strategizeAdvanced(distPtr, debugFlagTmp);
const Strategies& stmp = plays.strategizeVoid(distPtr, debugFlagTmp);
timers2[ps].stop();

  if (! (strats == stmp))
  {
    if (ranks.partnerVoid())
    {
      cout << "VOID " << strats.size() << " vs " << stmp.size() << endl;
      cout << ranks.strTable();
      wcout << ranks.wstrDiagram();
      cout << distPtr->str();
      cout << strats.str("Old", false);
      cout << stmp.str("New", false);
    }
    else
    {
      cout << "REGULAR " << strats.size() << " vs " << stmp.size() << endl;
      cout << ranks.strTable();
      wcout << ranks.wstrDiagram();
      cout << distPtr->str();
      cout << strats.str("Old", false);
      cout << stmp.str("New", false);
    }
  }
}

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
    strats.setTrivial(trivialEntry, distPtr->size());
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
  strats = plays.strategizeVoid(distPtr, static_cast<DebugPlay>(0x3f));
// cout << "C " << centry.canonicalHolding3 << endl;

  // Make a note of the type of strategy? (COMB_TRIVIAL etc.)

  return strats;
}


const Strategies& Combination::strategies() const
{
  return strats;
}

