/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Explain.h"

#include <cassert>


using namespace std;


void Explain::setTricks(
  const unsigned char tricksMinIn,
  const unsigned weightSymm,
  const unsigned weightAsymm)
{
  tricksMinInt = tricksMinIn;

  if (weightSymm == 0)
  {
    if (weightAsymm == 0)
      explain = EXPLAIN_TRIVIAL;
    else
      explain = EXPLAIN_ANTI_SYMMETRIC;
  }
  else if (weightAsymm == 0)
    explain = EXPLAIN_SYMMETRIC;
  else
    explain = EXPLAIN_GENERAL;
}


void Explain::setTops(const unsigned numStrategyTopsIn)
{
  numStrategyTops = numStrategyTopsIn;
}


void Explain::behaveSymmetrically()
{
  assert(Explain::symmetricComponent());
  behave = EXPLAIN_SYMMETRIC;
}


void Explain::behaveAntiSymmetrically()
{
  assert(Explain::asymmetricComponent());
  behave = EXPLAIN_ANTI_SYMMETRIC;
}


void Explain::behaveGenerally()
{
  assert(explain != EXPLAIN_TRIVIAL);
  behave = EXPLAIN_GENERAL;
}


bool Explain::skip(
  const unsigned char effectiveDepth,
  const bool symmetricCoverFlag) const
{
  if (effectiveDepth > numStrategyTops)
    return true;

  if (behave == EXPLAIN_SYMMETRIC && ! symmetricCoverFlag)
    return true;

  if (behave == EXPLAIN_ANTI_SYMMETRIC && symmetricCoverFlag)
    return true;

  return false;
}


bool Explain::symmetricComponent() const
{
  return (explain == EXPLAIN_SYMMETRIC || explain == EXPLAIN_GENERAL);
}


bool Explain::asymmetricComponent() const
{
  return (explain == EXPLAIN_ANTI_SYMMETRIC || explain == EXPLAIN_GENERAL);
}


unsigned char Explain::tricksMin() const
{
  return tricksMinInt;
}

