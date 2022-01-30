/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverSpec.h"


CoverSpec::CoverSpec()
{
  CoverSpec::reset();
}


void CoverSpec::reset()
{
  for (auto& set: setsWest)
    set.reset();
}


bool CoverSpec::includes(
  const unsigned char wlen,
  const unsigned char wtop) const
{
  for (auto& set: setsWest)
    if (set.includes(wlen, wtop, oppsLength, oppsTops1))
      return true;

  return false;
}


void CoverSpec::getIndices(
  unsigned char& length,
  unsigned char& tops1) const
{
  length = oppsLength;
  tops1 = oppsTops1;
}


string CoverSpec::str() const
{
  if (setsWest[1].mode == COVER_MODE_NONE)
    return setsWest[0].str(oppsLength, oppsTops1);
  else
  {
    // At the moment
    // 57 YYThe suit splits 2-2, or East has the tops
    // 15 YYThe suit splits 3=3, or West has the tops
    // 24 YYWest has at least 2 cards, or East has the tops
    // 12 YYWest has at most 3 cards, or West has the tops

    return "YY " + setsWest[0].str(oppsLength, oppsTops1) + "; or " +
      setsWest[1].str(oppsLength, oppsTops1);
  }
}

