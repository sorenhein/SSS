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
#include "Length.h"


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


string Top::strTop(
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



string Top::strExactLengthEqual(
  const unsigned char distLength,
  const unsigned char oppsLength,
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  // Here lower and upper are identical.
  string side, otherSide;
  unsigned char length, value;

  // TODO When combined with Length, I suppose this might look like:
  // Either opponent has a singleton, and either opponent has the honor.
  // But it's the same opponent.  See whether this becomes a problem.

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    otherSide = (symmFlag ? "Either opponent" : "East");
    value = lower;
    length = distLength;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    otherSide = (symmFlag ? "Either opponent" : "West");
    value = oppsTops - lower;
    length = oppsLength - distLength;
  }

  stringstream ss;

  if (length == 1)
  {
    if (value == 0)
      ss << side << " has a small singleton";
    else
    {
      ss << side << " has " << (oppsTops == 1 ? "the" : "a") << " " <<
        "singleton honor";
    }
  }
  else if (length == 2)
  {
    if (value == 0)
    {
      if (oppsLength == 4 && oppsTops == 2)
        ss << otherSide << " has doubleton honors (HH)";
      else
        ss << side << " has a small doubleton";
    }
    else if (value == 1)
    {
      ss << side << " has " << (oppsTops == 1 ? "the" : "a") << " " <<
        "doubleton honor (Hx)";
    }
    else
      ss << side << " has doubleton honors (HH)";
  }
  else if (length == 3)
  {
    if (value == 0)
      ss << side << " has a small tripleton";
    else if (value == 1)
    {
      ss << side << " has " << (oppsTops == 1 ? "the" : "a") << " " <<
        "tripleton honor (Hxx)";
    }
    else if (value == 2)
    {
      ss << side << " has " << (oppsTops == 1 ? "the" : "two") << " " <<
        "tripleton honors (HHx)";
    }
    else
    {
      ss << side << " has " << "tripleton honors (HHH)";
    }
  }
  else
  {
    cout << "length " << +length << endl;
    assert(false);
  }

  return ss.str();
}


string Top::strLengthRangeEqual(
  const unsigned char oppsTops,
  const Xes& xes,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  string side, xstr;
  unsigned char value, maxLen;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    xstr = xes.strWest;
    value = lower;
    maxLen = value + xes.westMax;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    xstr = xes.strEast;
    value = oppsTops - upper;
    maxLen = value + xes.eastMax;
  }

  const string hstr = (value == 0 ? "" : string(value, 'H'));

  stringstream ss;

  string slen;
  if (maxLen == 1)
    slen = "singleton";
  else if (maxLen == 2)
    slen = "doubleton";
  else if (maxLen == 3)
    slen = "tripleton";
  else if (maxLen == 4)
    slen = "fourth";
  else if (maxLen == 5)
    slen = "fifth";
  else
  {
cout << "maxLen " << +maxLen << endl;
    assert(false);
  }

  if (value == oppsTops)
  {
    string strT;
    if (value == 1)
      strT = "the top";
    else if (value == 2)
      strT = "both tops";
    else
      strT = "all tops";

    ss << side << " has " << strT << " at most " << slen;
  }
  else if (value > 0)
  {
    string strT;
    if (value == 1)
      strT = "one top";
    else if (value == 2)
      strT = "two tops";
    else if (value == 3)
      strT = "three tops";
    else
      assert(false);

    ss << side << " has " << strT << " at most " << slen;
  }
  else
    ss << side << " has " << hstr << xstr;

  return ss.str();
}


string Top::strWithLength(
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  assert(oper == COVER_EQUAL);

  if (length.getOperator() == COVER_EQUAL)
  {
    return Top::strExactLengthEqual(
      length.lower, oppsLength, oppsTops, simplestOpponent, symmFlag);
  }
  else
  {
    Xes xes;
    xes.set(length.lower, length.upper, lower, oppsLength, oppsTops);

    return Top::strLengthRangeEqual(
      oppsTops, xes, simplestOpponent, symmFlag);
  }
}

