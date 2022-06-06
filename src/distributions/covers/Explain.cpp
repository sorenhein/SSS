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
  const unsigned weightAntisymm)
{
  tricksMinInt = tricksMinIn;

  if (weightSymm == 0)
  {
    if (weightAntisymm == 0)
      explain = EXPLAIN_TRIVIAL;
    else
      explain = EXPLAIN_ANTI_SYMMETRIC;
  }
  else if (weightAntisymm == 0)
    explain = EXPLAIN_SYMMETRIC;
  else
    explain = EXPLAIN_GENERAL;
}


void Explain::setTops(const unsigned numStrategyTopsIn)
{
  numStrategyTops = numStrategyTopsIn;
}


void Explain::behave(const ExplainSymmetry behaveIn)
{
  if (behaveIn == EXPLAIN_SYMMETRIC)
  {
    assert(Explain::symmetricComponent());
    behaveInt = EXPLAIN_SYMMETRIC;
  }
  else if (behaveIn == EXPLAIN_ANTI_SYMMETRIC)
  {
    assert(Explain::asymmetricComponent());
    behaveInt = EXPLAIN_ANTI_SYMMETRIC;
  }
  else if (behaveIn != EXPLAIN_TRIVIAL)
    behaveInt = EXPLAIN_GENERAL;
  else
    assert(false);
}


bool Explain::skip(
  const unsigned char effectiveDepth,
  const ExplainSymmetry coverSymmetry) const
{
  if (effectiveDepth > numStrategyTops)
    return true;

  if (behaveInt == EXPLAIN_SYMMETRIC && 
      (coverSymmetry != EXPLAIN_SYMMETRIC))
    return true;

  if (behaveInt == EXPLAIN_ANTI_SYMMETRIC && 
      (coverSymmetry != EXPLAIN_ANTI_SYMMETRIC))
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

