/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Length.h"


string Length::strEqual(const unsigned char oppsLength) const
{
  // Here lower and upper are one and the same.

  stringstream ss;
  // const string side = (symmFlag ? "Either opponent" : "West");
  const string side = "West";

  if (lower == 0)
  {
    ss << side << " is void";
  }
  else if (lower == oppsLength)
  {
    // assert(! symmFlag);
    ss << "East is void";
  }
  else if (lower == 1)
  {
    ss << side << " has a singleton";
  }
  else if (lower+1 == oppsLength)
  {
    // assert(! symmFlag);
    ss << "East has a singleton";
  }
  else if (lower == 2)
  {
    if (oppsLength > 4)
      ss << side << " has a doubleton";
    else
      ss << "The suit splits 2=2";
  }
  else
  {
    ss << "The suit splits " << +lower << "=" << +(oppsLength - lower);
  }

  return ss.str();
}



string Length::strInside(const unsigned char oppsLength) const
{
  stringstream ss;
  // const string side = (symmFlag ? "Either opponent" : "West");
  const string side = "West";

  if (lower == 0)
  {
    if (upper == 1)
      ss << side << " has at most a singleton";
    else if (upper == 2)
      ss << side << " has at most a doubleton";
    else
      ss << side << " has at most " << +upper << " cards";
  }
  else if (upper == oppsLength)
  {
    ss << side << " has at least " << +lower << " cards";
  }
  else if (lower == 1 && upper == oppsLength-1)
  {
    ss << "Neither opponent is void";
  }
  else if (lower + upper == oppsLength)
  {
    if (lower + 1 == upper)
    {
      ss << "The suit splits " << +lower << "-" << +upper << 
        " either way";
    }
    else
    {
      ss << "The suit splits " << +lower << "-" << +upper <<
        " or better either way";
    }
  }
  else
  {
    ss << "The suit splits between " <<
      +lower << "=" << +(oppsLength - lower) << " and " <<
      +upper << "=" << +(oppsLength - upper);
  }

  return ss.str();
}


string Length::str(const unsigned char oppsLength) const
{
  if (oper == COVER_EQUAL)
  {
    return Length::strEqual(oppsLength);
  }
  else if (oper == COVER_INSIDE_RANGE)
  {
    return Length::strInside(oppsLength);
  }
  else
  {
    assert(false);
    return "";
  }
}

