/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "Explain.h"

#include "CoverCategory.h"

#include <cassert>


using namespace std;


void Explain::setParameters(
  const unsigned numStrategyTopsIn,
  const unsigned char tricksMinIn)
{
  numStrategyTops = numStrategyTopsIn;
  tricksMinInt = tricksMinIn;
}


void Explain::setComposition(const ExplainComposition compositionIn)
{
  composition = compositionIn;
}


void Explain::behave(const ExplainSymmetry behaveIn)
{
  behaveInt = behaveIn;
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


bool Explain::skip(
  const unsigned char effectiveDepth,
  const ExplainSymmetry coverSymmetry,
  const ExplainComposition coverComposition) const
{
  if (Explain::skip(effectiveDepth, coverSymmetry))
    return true;

  if (composition == EXPLAIN_LENGTH_ONLY &&
      coverComposition != EXPLAIN_LENGTH_ONLY)
    return true;
  else if (composition == EXPLAIN_TOPS_ONLY &&
      coverComposition != EXPLAIN_TOPS_ONLY)
    return true;

  return false;
}


unsigned char Explain::tricksMin() const
{
  return tricksMinInt;
}

