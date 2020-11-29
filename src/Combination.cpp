#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

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


void Combination::strategize(
  const CombEntry& centry,
  const Distributions& distributions,
  Ranks& ranks,
  Plays& plays)
{
  distPtr = distributions.ptr(ranks.size(), centry.canonicalHolding2);

  plays.reset();

  unsigned term;
  ranks.setPlays(plays, term);

  // Like UNUSED
  unsigned h = centry.canonicalHolding3;
  h++;
}

