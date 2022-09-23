/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cassert>

#include "Top.h"
#include "Length.h"
#include "CoverOperator.h"
#include "Xes.h"
#include "TopData.h"

#include "../../../ranks/RankNames.h"

#include "../../../utils/table.h"

const vector<string> topCount =
{
  "none",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen"
};

const vector<string> topOrdinal =
{
  "void",
  "singleton",
  "doubleton",
  "tripleton",
  "fourth",
  "fifth",
  "sixth",
  "seventh",
  "eighth",
  "ninth",
  "tenth",
  "eleventh",
  "twelfth",
  "thirteenth"
};


Opponent Top::simplestOpponent(const unsigned char maximum) const
{
  if (! Term::used())
    return OPP_EITHER;

  // Choose the fuller side, as this tends to be more intuitive.
  const unsigned char lsum = Top::lower() + Top::upper();

  if (lsum > maximum)
    return OPP_WEST;
  else if (lsum < maximum)
    return OPP_EAST;
  else
    return OPP_EITHER;
}


string Top::strExactLengthEqual(
  const unsigned char distLength,
  const unsigned char oppsLength,
  const TopData& oppsTopData,
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
    value = oppsTopData.value - Top::lower();
    length = oppsLength - distLength;
  }

  assert(oppsTopData.rankNamesPtr);
  const string strFull = 
    oppsTopData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_FULL);

  stringstream ss;

  if (length == 1)
  {
    if (value == 0)
      ss << side << " has a singleton without the " << strFull;
    else if (oppsTopData.value == 1)
      ss << side << " has the singleton " << strFull;
    else
      ss << side << " has one of " << strFull << " singleton";
  }
  else if (length == 2)
  {
    if (value == 0)
    {
      if (oppsLength == 4 && oppsTopData.value == 2)
        ss << otherSide << " has two of " << strFull << " doubleton";
      else if (oppsTopData.value == 1)
        ss << side << " has a doubleton without the " << strFull;
      else
        ss << side << " has a doubleton without either of " << strFull;
    }
    else if (value == 1)
    {
      if (oppsTopData.value == 1)
        ss << side << " has the " << strFull << " doubleton";
      else
        ss << side << " has one of " << strFull << " doubleton";
    }
    else
      ss << side << " has " << strFull << " doubleton";
  }
  else if (length == 3)
  {
    if (value == 0)
    {
      if (oppsTopData.value == 1)
        ss << side << " has a tripleton without the " << strFull;
      else
        ss << side << " has a tripleton without any of " << strFull;
    }
    else if (value == 1)
    {
      if (oppsTopData.value == 1)
        ss << side << " has the tripleton " << strFull;
      else
        ss << side << " has one of " << strFull << " tripleton";
    }
    else if (value == 2)
    {
      if (oppsTopData.value == 2)
        ss << side << " has " << strFull << " tripleton";
      else
        ss << side << " has two of " << strFull << " tripleton";
    }
    else
      ss << side << " has " << strFull;
  }
  else if (length == 4)
  {
    ss << side << " has " << +value << " of " << strFull << " fourth";
  }
  else
  {
    cout << "length " << +length << endl;
    assert(false);
  }

  return ss.str();
}


string Top::strLengthRangeEqual(
  const TopData& oppsTopData,
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
    value = oppsTopData.value - Top::upper();
    maxLen = value + xes.eastMax;
  }

  const string hstr = (value == 0 ? "" : string(value, 'H'));

  assert(oppsTopData.rankNamesPtr);
  const string strFull = 
    oppsTopData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_FULL);

  stringstream ss;

  string slen = topOrdinal[maxLen];

  if (value == oppsTopData.value)
  {
    string strT;
    if (value == 1)
      strT = "the " + strFull;
    else if (value == 2)
      strT = "both of " + strFull;
    else
      strT = "all of " + strFull;

    ss << side << " has " << strT << " at most " << slen;
  }
  else if (value > 0)
  {
    string strT;
    if (value == 1)
      strT = "one of " + strFull;
    else if (value == 2)
      strT = "two of " + strFull;
    else if (value == 3)
      strT = "three of " + strFull;
    else
      assert(false);

    ss << side << " has " << strT << " at most " << slen;
  }
  else
    ss << side << " has " << hstr << xstr;

  return ss.str();
}


string Top::strTopBare(
  const TopData& oppsTopData,
  const Opponent simplestOpponent) const
{
  assert(Top::getOperator() != COVER_EQUAL);

  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    vLower = Top::lower();
    vUpper = (Top::getOperator() == COVER_GREATER_EQUAL ? 
      oppsTopData.value : Top::upper());
  }
  else
  {
    vLower = (Top::getOperator() == COVER_GREATER_EQUAL ? 0 : 
      oppsTopData.value - Top::upper());
    vUpper = oppsTopData.value - Top::lower();
  }

  assert(oppsTopData.rankNamesPtr);
  const string strFull = 
    oppsTopData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_FULL);

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << "at most one of " << strFull;
    else if (vUpper == 2)
      ss << "at most two of " << strFull;
    else
      ss << "at most " << +vUpper << " of " << strFull;
  }
  else if (vUpper == oppsTopData.value)
  {
    if (vLower+1 == oppsTopData.value)
    {
      if (oppsTopData.value == 2)
        ss << "one or both of " << strFull;
      else if (oppsTopData.value == 3)
        ss << "two or all three of " << strFull;
      else
        ss << +vLower << "-" << +vUpper << " of " << strFull;
    }
    else
      ss << +vLower << "-" << +vUpper << " of " << strFull;
  }
  else if (vLower + vUpper == oppsTopData.value)
    ss << +vLower << "-" << +vUpper << " of " << strFull;

  return ss.str();
}


string Top::strTopBareEqual(
  const TopData& oppsTopData,
  const Opponent simplestOpponent) const
{
  assert(Top::getOperator() == COVER_EQUAL);

  const unsigned char value = (simplestOpponent != OPP_EAST ?
    Top::lower() : oppsTopData.value - Top::upper());

  assert(oppsTopData.rankNamesPtr);
  // const string str = 
    // oppsTopData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);

  if (value == 0)
    return "";
  else if (value == oppsTopData.value)
    // return str;
    return oppsTopData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);
  else
    return oppsTopData.rankNamesPtr->strComponent(RANKNAME_ABSOLUTE_SHORT).
      substr(0, value);

  /*
  stringstream ss;
  ss << "(" << topCount[value] << " of " << str << ")";
  return ss.str();
  */
}


string Top::strEqualWithLength(
  const Length& length,
  const unsigned char oppsLength,
  const TopData& oppsTopData,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  assert(Top::getOperator() == COVER_EQUAL);

  if (length.getOperator() == COVER_EQUAL)
  {
    return Top::strExactLengthEqual(
      length.lower(), oppsLength, oppsTopData, simplestOpponent, symmFlag);
  }
  else
  {
    Xes xes;
    const unsigned char effUpper = 
      (length.getOperator() == COVER_GREATER_EQUAL ?  
        oppsLength : 
        length.upper());

    xes.set(length.lower(), effUpper, Top::lower(), oppsLength, 
      oppsTopData.value);

    return Top::strLengthRangeEqual(
      oppsTopData, xes, simplestOpponent, symmFlag);
  }
}

