#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Combinations.h"
#include "Combination.h"
#include "Distributions.h"
#include "Distribution.h"
#include "Ranks.h"
#include "Plays.h"
#include "struct.h"


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


const Tvectors& Combination::strategize(
  const CombEntry& centry,
  const Combinations& combinations,
  const Distributions& distributions,
  Ranks& ranks,
  Plays& plays)
{
  // Look up a pointer to the EW distribution of this combination.
  distPtr = distributions.ptr(ranks.size(), centry.canonicalHolding2);

  // Make the plays.
  unsigned term;
  plays.reset();
  const CombinationType ctype = ranks.setPlays(plays, term);
  
  // If it's a trivial situation, make the strategies.
  if (ctype == COMB_TRIVIAL)
  {
    // Fill out a single constant strategy with the right value and size.
    strats.setTrivial(term, distPtr->size());
    return strats;
  }

cout << "A " << centry.canonicalHolding3 << endl;
  // Complete the plays such that their ends point to combinations.
  plays.setCombPtrs(combinations);
cout << "B " << centry.canonicalHolding3 << endl;
cout << "Plays\n" << plays.str() << "\n";
cout << "Distribution\n" << distPtr->str() << "\n";

  plays.strategize(distPtr, strats);
cout << "C " << centry.canonicalHolding3 << endl;

  // Make a note of the type of strategy? (COMB_TRIVIAL etc.)

  return strats;
}


const Tvectors& Combination::strategies() const
{
  return strats;
}

