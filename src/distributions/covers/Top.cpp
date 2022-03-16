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


string Top::strEqual(const unsigned char oppsTops) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (lower == 0)
  {
    assert(! symmFlag);
    if (oppsTops == 1)
      ss << "East has the top";
    else
      ss << "East has the tops";
  }
  else if (lower == oppsTops)
  {
    if (oppsTops == 1)
      ss << side << " has the top";
    else
      ss << side << " has the tops";
  }
  else if (lower == 1)
  {
    if (oppsTops == 1)
      ss << side << " has the top";
    else
      ss << side << " has exactly one top";
  }
  else if (lower == oppsTops-1)
  {
    assert(! symmFlag);
    ss << "East has exactly one top";
  }
  else if (lower == 2)
  {
    ss << side << " has " <<
      (oppsTops == 2 ? "both" : "exactly two") << " tops";
  }
  else
    ss << side << " has exactly " << lower << " tops";

  return ss.str();
}


string Top::strInside(const unsigned char oppsTops) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (lower == 0)
  {
    if (upper == oppsTops-1)
    {
      assert(! symmFlag);
      ss << "East has at least one top";
    }
    else
      ss << side << " has at most " << +upper << " tops";
  }
  else if (upper == oppsTops)
  {
    if (lower == 1)
      ss << side << " has at least one top";
    else
      ss << side << " has at least " << +lower << " tops";
  }
  else
  {
      ss << side <<
        " has between " << +lower << " and " << +upper << " tops";
  }

  return ss.str();
}


string Top::str(const unsigned char oppsTops) const
{
  if (oper == COVER_EQUAL)
  {
    return Top::strEqual(oppsTops);
  }
  else if (oper == COVER_INSIDE_RANGE)
  {
    return Top::strInside(oppsTops);
  }
  else
  {
    assert(false);
    return "";
  }
}

