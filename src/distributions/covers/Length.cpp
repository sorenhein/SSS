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


string Length::strEqual(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  // Here lower and upper are identical.
  string side;
  unsigned char value;

  if (simplestOpponent == OPP_WEST)
  {
    side = "West";
    value = lower;
  }
  else
  {
    side = "East";
    value = oppsLength - lower;
  }

  stringstream ss;

  if (value == 0)
    ss << side << " is void";
  else if (value == 1)
    ss << side << " has a singleton";
  else if (value == 2)
  {
    if (oppsLength == 4)
      ss << "The suit splits 2=2";
    else
      ss << side << " has a doubleton";
  }
  else
    ss << "The suit splits " << +lower << "=" << +(oppsLength - lower);

  return ss.str();
}



string Length::strInside(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  stringstream ss;
  // const string side = (symmFlag ? "Either opponent" : "West");
  const string side = "West";

  if (lower == 0)
  {
    if (upper == 1)
    {
assert(simplestOpponent == OPP_WEST);
      ss << side << " has at most a singleton";
    }
    else if (upper == 2)
    {
assert(simplestOpponent == OPP_WEST);
      ss << side << " has at most a doubleton";
    }
    else
    {
assert(simplestOpponent == OPP_WEST);
      ss << side << " has at most " << +upper << " cards";
    }
  }
  else if (upper == oppsLength)
  {
// assert(simplestOpponent == OPP_WEST);
    // ss << side << " has at least " << +lower << " cards";

    if (simplestOpponent == OPP_WEST)
      ss << "West has at least " << +lower << " cards";
    else
      ss << "East has at most " << +(oppsLength - lower) << " cards";
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


string Length::str(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  if (oper == COVER_EQUAL)
  {
    return Length::strEqual(oppsLength, simplestOpponent);
  }
  else if (oper == COVER_INSIDE_RANGE)
  {
    return Length::strInside(oppsLength, simplestOpponent);
  }
  else if (oper == COVER_LESS_EQUAL)
  {
    // TODO For now
    return Length::strInside(oppsLength, simplestOpponent);
  }
  else if (oper == COVER_GREATER_EQUAL)
  {
    // TODO For now
    return Length::strInside(oppsLength, simplestOpponent);
  }
  else
  {
    assert(false);
    return "";
  }
}

