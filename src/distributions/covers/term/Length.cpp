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


bool Length::notVoid() const
{
  // This is a special case in Product.
  return (lower == 1 && oper == COVER_GREATER_EQUAL);
}


string Length::strEqual(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  // Here lower and upper are identical.
  string side;
  unsigned char value;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    value = lower;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
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
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  string side;
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    vLower = lower;
    vUpper = (oper == COVER_GREATER_EQUAL ? oppsLength : upper);
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    vLower = (oper == COVER_GREATER_EQUAL ? 0 : oppsLength - upper);
    vUpper = oppsLength - lower;
  }

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << side << " has at most a singleton";
    else if (vUpper == 2)
      ss << side << " has at most a doubleton";
    else
      ss << side << " has at most " << +vUpper << " cards";
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    ss << "Neither opponent is void";
  }
  else if (lower + upper == oppsLength)
  {
    ss << "Each opponent has " << +lower << "-" << +upper << " cards";
  }
  else
  {
    ss << "The suit splits between " <<
      +lower << "=" << +(oppsLength - lower) << " and " <<
      +upper << "=" << +(oppsLength - upper);
  }

  return ss.str();
}


string Length::strLength(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  if (oper == COVER_EQUAL)
  {
    return Length::strEqual(oppsLength, simplestOpponent, symmFlag);
  }
  else if (oper == COVER_INSIDE_RANGE ||
           oper == COVER_LESS_EQUAL ||
           oper == COVER_GREATER_EQUAL)
  {
    return Length::strInside(oppsLength, simplestOpponent, symmFlag);
  }
  else
  {
    assert(false);
    return "";
  }
}

