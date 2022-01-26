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
  return westLength.str("cards");
}


// TODO Goes away longer-term
string CoverSpec::strTop1() const
{
  return westTop1.str("tops");
}


string CoverSpec::str() const
{
  return "";
}

