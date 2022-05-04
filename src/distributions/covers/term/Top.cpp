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
#include "CoverOperator.h"
#include "Xes.h"


string Top::strEqual(
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  // Here lower and upper are identical.
  string side, otherSide;
  unsigned char value;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    otherSide = (symmFlag ? "Either opponent" : "East");
    value = Top::lower();
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    otherSide = (symmFlag ? "Either opponent" : "West");
    value = oppsTops - Top::lower();
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
    if (oppsTops == 2)
      ss << "Each opponent has one top";
    else
      ss << side << " has exactly one top";
  }
  else if (value+1 == oppsTops)
    ss << otherSide << " has exactly one top";
  else if (value == 2)
    ss << side << " has exactly two tops";
  else if (value+2 == oppsTops)
    ss << otherSide << " has exactly two tops";
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
    vLower = Top::lower();
    vUpper = (Top::getOperator() == COVER_GREATER_EQUAL ? 
      oppsTops : 
      Top::upper());
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    vLower = (Top::getOperator() == COVER_GREATER_EQUAL ? 
      0 : 
      oppsTops - Top::upper());
    vUpper = oppsTops - Top::lower();
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
    {
      if (oppsTops == 2)
        ss << side << " has one or both tops";
      else if (oppsTops == 3)
        ss << side << " has two or all three tops";
      else
        ss << side << " lacks at most one top";
    }
    else if (vLower+2 == oppsTops)
      ss << side << " lacks at most two tops";
    else
      ss << side << " lacks at most " << +(oppsTops - vLower) << " tops";
  }
  else if (vLower + vUpper == oppsTops)
  {
    ss << "Each opponent has " << +vLower << "-" << +vUpper << " tops";
  }
  else
  {
    ss << side << " has " << +vLower << "-" << +vUpper << " tops";
  }

  return ss.str();
}


string Top::strTop(
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  const CoverOperator oper = Top::getOperator();
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

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    otherSide = (symmFlag ? "Either opponent" : "East");
    value = Top::lower();
    length = distLength;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    otherSide = (symmFlag ? "Either opponent" : "West");
    value = oppsTops - Top::lower();
    length = oppsLength - distLength;
  }

  stringstream ss;

  if (length == 1)
  {
    if (value == 0)
      ss << side << " has a small singleton";
    else if (oppsTops == 1)
      ss << side << " has the singleton honor";
    else
      ss << side << " has one honor singleton";
  }
  else if (length == 2)
  {
    if (value == 0)
    {
      if (oppsLength == 4 && oppsTops == 2)
        ss << otherSide << " has two honors doubleton";
      else
        ss << side << " has a small doubleton";
    }
    else if (value == 1)
    {
      if (oppsTops == 1)
        ss << side << " has the doubleton honor";
      else
        ss << side << " has one honor doubleton";
    }
    else
      ss << side << " has two honors doubleton";
  }
  else if (length == 3)
  {
    if (value == 0)
      ss << side << " has a small tripleton";
    else if (value == 1)
    {
      if (oppsTops == 1)
        ss << side << " has the tripleton honor";
      else
        ss << side << " has one honor tripleton";
    }
    else if (value == 2)
    {
      if (oppsTops == 2)
        ss << side << " has the two honors tripleton";
      else
        ss << side << " has two honors tripleton";
    }
    else
      ss << side << " has three honors tripleton";
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
    value = Top::lower();
    maxLen = value + xes.westMax;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    xstr = xes.strEast;
    value = oppsTops - Top::upper();
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
    assert(false);

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


string Top::strTopBare(
  const unsigned char oppsTops,
  const Opponent simplestOpponent) const
{
  assert(Top::getOperator() != COVER_EQUAL);

  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    vLower = Top::lower();
    vUpper = (Top::getOperator() == COVER_GREATER_EQUAL ? 
      oppsTops : Top::upper());
  }
  else
  {
    vLower = (Top::getOperator() == COVER_GREATER_EQUAL ? 0 : 
      oppsTops - Top::upper());
    vUpper = oppsTops - Top::lower();
  }

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << "at most one top";
    else if (vUpper == 2)
      ss << "at most two tops";
    else
      ss << "at most " << +vUpper << " tops";
  }
  else if (vUpper == oppsTops)
  {
    if (vLower+1 == oppsTops)
    {
      if (oppsTops == 2)
        ss << "one or both tops";
      else if (oppsTops == 3)
        ss << "two or all three tops";
      else
        ss << +vLower << "-" << +vUpper << " tops";
    }
    else
      ss << +vLower << "-" << +vUpper << " tops";
  }
  else if (vLower + vUpper == oppsTops)
    ss << +vLower << "-" << +vUpper << " tops";

  return ss.str();
}


string Top::strEqualWithLength(
  const Length& length,
  const unsigned char oppsLength,
  const unsigned char oppsTops,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  assert(Top::getOperator() == COVER_EQUAL);

  if (length.getOperator() == COVER_EQUAL)
  {
    return Top::strExactLengthEqual(
      length.lower(), oppsLength, oppsTops, simplestOpponent, symmFlag);
  }
  else
  {
    Xes xes;
    const unsigned char effUpper = 
      (length.getOperator() == COVER_GREATER_EQUAL ?  
        oppsLength : 
        length.upper());

    xes.set(length.lower(), effUpper, Top::lower(), oppsLength, oppsTops);

    return Top::strLengthRangeEqual(
      oppsTops, xes, simplestOpponent, symmFlag);
  }
}

