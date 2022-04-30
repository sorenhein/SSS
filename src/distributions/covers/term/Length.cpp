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
  return (
    Length::lower() == 1 && Length::getOperator() == COVER_GREATER_EQUAL);
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
    value = Length::lower();
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    value = oppsLength - Length::lower();
  }

  stringstream ss;

  if (value == 0)
    ss << side << " is void";
  else if (value == 1)
    ss << side << " has a singleton";
  else if (value == 2)
    ss << side << " has a doubleton";
  else
    ss << "The suit splits " << 
      +Length::lower() << "=" << +(oppsLength - Length::lower());

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
    vLower = Length::lower();
    vUpper = (Length::getOperator() == COVER_GREATER_EQUAL ? 
      oppsLength : Length::upper());
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    vLower = (Length::getOperator() == COVER_GREATER_EQUAL ? 
      0 : oppsLength - Length::upper());
    vUpper = oppsLength - Length::lower();
  }

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << side << " has at most a singleton";
    else if (vUpper == 2)
      ss << side << " has at most a doubleton";
    else if (vUpper == 3)
      ss << side << " has at most a tripleton";
    else
      ss << side << " has at most " << +vUpper << " cards";
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
    ss << "Neither opponent is void";
  else if (vLower + vUpper == oppsLength)
    ss << "Each opponent has " << +vLower << "-" << +vUpper << " cards";
  else
    ss << side << " has " << +vLower << "-" << +vUpper << " cards";

  return ss.str();
}


string Length::strLengthBare(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    vLower = Length::lower();
    vUpper = (Length::getOperator() == COVER_GREATER_EQUAL ?  oppsLength : 
      Length::upper());
  }
  else
  {
    vLower = (
      Length::getOperator() == COVER_GREATER_EQUAL ? 
      0 : oppsLength - Length::upper());
    vUpper = oppsLength - Length::lower();
  }

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << "at most singleton";
    else if (vUpper == 2)
      ss << "at most doubleton";
    else if (vUpper == 3)
      ss << "at most tripleton";
    else
      ss << "with at most " << +vUpper << " cards";
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
    ss << "with no void on either side";
  else
    ss << "with " << +vLower << "-" << +vUpper << " cards";

  return ss.str();
}


string Length::strLength(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  const CoverOperator oper = Length::getOperator();
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

