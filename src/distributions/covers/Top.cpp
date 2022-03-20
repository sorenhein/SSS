/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Top.h"


string Top::strEqual(
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  // Here lower and upper are identical.
  string side, otherSide;
  unsigned char value;

  // TODO When combined with Length, I suppose this might look like:
  // Either opponent has a singleton, and either opponent has the honor.
  // But it's the same opponent.  See whether this becomes a problem.

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    otherSide = (symmFlag ? "Either opponent" : "East");
    value = lower;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    otherSide = (symmFlag ? "Either opponent" : "West");
    value = oppsTops - lower;
  }

  stringstream ss;

  if (value == 0 || value == oppsTops)
  {
    const string longSide = (value == 0 ? otherSide : side);

    if (oppsTops == 1)
      ss << longSide << " has the top";
    else if (oppsTops == 2)
      ss << longSide << " has both tops";
    else
      ss << longSide << " has all tops";
  }
  else if (value == 1)
  {
    ss << side << " has exactly one top";
  }
  else if (value+1 == oppsTops)
  {
    ss << otherSide << " has exactly one top";
  }
  else if (value == 2)
  {
    ss << side << " has exactly two tops";
  }
  else if (value+2 == oppsTops)
  {
    ss << otherSide << " has exactly two tops";
  }
  else
    ss << side << " has exactly " +value << " tops";

  return ss.str();
}


string Top::strInside(
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  string side;
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    vLower = lower;
    vUpper = upper;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    vLower = oppsTops - upper;
    vUpper = oppsTops - lower;
  }

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << side << " has at most one top";
    else if (vUpper == 2)
      ss << side << " has at most two tops";
    else
      ss << side << " has at most " << +vUpper << " tops";
  }
  else if (vUpper == oppsTops)
  {
    if (vLower+1 == oppsTops)
      ss << side << " lacks at most one top";
    else if (vLower+2 == oppsTops)
      ss << side << " lacks at most two tops";
    else
      ss << side << " lacks at most " << +(oppsTops - vLower) << " tops";
  }
  else
  {
    ss << side << " has between " << +vLower << " and " <<
      +vUpper << " tops";
  }

  return ss.str();
}


string Top::str(
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  if (oper == COVER_EQUAL)
  {
    return Top::strEqual(oppsTops, simplestOpponent, symmFlag);
  }
  else if (oper == COVER_INSIDE_RANGE ||
           oper == COVER_LESS_EQUAL ||
           oper == COVER_GREATER_EQUAL)
  {
    return Top::strInside(oppsTops, simplestOpponent, symmFlag);
  }
  else
  {
    assert(false);
    return "";
  }
}

