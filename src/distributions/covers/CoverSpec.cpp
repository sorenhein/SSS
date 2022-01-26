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
}


// TODO Goes away longer-term
string CoverSpec::strLength() const
{
  if (westLength.oper == COVER_EQUAL &&
      westLength.value1 == 0)
  {
    if (invertFlag)
      return "West is not void";
    else
      return "West is void";
  }
  else if (westLength.oper == COVER_EQUAL &&
      westLength.value1 == oppsLength)
  {
    if (invertFlag)
      return "East is not void";
    else
      return "East is void";
  }
  else
    return westLength.str("cards");
}


// TODO Goes away longer-term
string CoverSpec::strTop1() const
{
  return westTop1.str("tops");
}


string CoverSpec::str() const
{
  if (mode == COVER_LENGTHS_ONLY)
    return CoverSpec::strLength();
  else if (mode == COVER_TOPS_ONLY)
    return CoverSpec::strTop1();
  else if (mode == COVER_LENGTHS_OR_TOPS)
    return CoverSpec::strLength() + ", or " + CoverSpec::strTop1();
  else if (mode == COVER_LENGTHS_AND_TOPS)
    return CoverSpec::strLength() + ", and " + CoverSpec::strTop1();
  else
    return "";
}

