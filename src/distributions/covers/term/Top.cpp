/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cassert>

#include "Top.h"
#include "Length.h"
#include "CoverOperator.h"
#include "Xes.h"
#include "TopData.h"

#include "../../../ranks/RankNames.h"

#include "../../../utils/table.h"


Opponent Top::simplestOpponent(const unsigned char maximum) const
{
  if (! Term::used())
    return OPP_EITHER;

  // Choose the fuller side, as this tends to be more intuitive.
  const unsigned char lsum = Top::lower() + Top::upper();

  if (lsum > maximum)
    return OPP_WEST;
  else if (lsum < maximum)
    return OPP_EAST;
  else
    return OPP_EITHER;
}


string Top::strTopBare(
  const TopData& oppsTopData,
  const Opponent simplestOpponent) const
{
  assert(Top::getOperator() != COVER_EQUAL);

  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    vLower = Top::lower();
    vUpper = (Top::getOperator() == COVER_GREATER_EQUAL ? 
      oppsTopData.value : Top::upper());
  }
  else
  {
    vLower = (Top::getOperator() == COVER_GREATER_EQUAL ? 0 : 
      oppsTopData.value - Top::upper());
    vUpper = oppsTopData.value - Top::lower();
  }

  assert(oppsTopData.rankNamesPtr);
  const string strFull = 
    oppsTopData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_FULL);

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << "at most one of " << strFull;
    else if (vUpper == 2)
      ss << "at most two of " << strFull;
    else
      ss << "at most " << +vUpper << " of " << strFull;
  }
  else if (vUpper == oppsTopData.value)
  {
    if (vLower+1 == oppsTopData.value)
    {
      if (oppsTopData.value == 2)
        ss << "one or both of " << strFull;
      else if (oppsTopData.value == 3)
        ss << "two or all three of " << strFull;
      else
        ss << +vLower << "-" << +vUpper << " of " << strFull;
    }
    else
      ss << +vLower << "-" << +vUpper << " of " << strFull;
  }
  else if (vLower + vUpper == oppsTopData.value)
    ss << +vLower << "-" << +vUpper << " of " << strFull;

  return ss.str();
}

