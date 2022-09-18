/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "VerbalCover.h"

#include "../term/Length.h"

#include "../../../ranks/RanksNames.h"

#include "../../../utils/table.h"


VerbalCover::VerbalCover()
{
  lengthFlag = false;
  lengthLower = 0;
  lengthUpper = 0;
  lengthOper = COVER_EQUAL;
  westFlag = false;
  eastFlag = false;
}


void VerbalCover::push_back(const Completion& completion)
{
  completions.push_back(completion);
}


void VerbalCover::setLength(const Length& length)
{
  lengthFlag = true;
  lengthLower = length.lower();
  lengthUpper = length.upper();
  lengthOper = length.getOperator();
}


Completion& VerbalCover::activateSide(const Opponent opponent)
{
  assert(opponent == OPP_WEST || opponent == OPP_EAST);
  if (opponent == OPP_WEST)
  {
    westFlag = true;
    return west;
  }
  else
  {
    eastFlag = true;
    return east;
  }
}


void VerbalCover::setSide(
  const Completion& completion,
  const Opponent opponent)
{
  assert(opponent == OPP_WEST || opponent == OPP_EAST);
  if (opponent == OPP_WEST)
  {
    westFlag = true;
    west = completion;
  }
  else
  {
    eastFlag = true;
    east = completion;
  }
}


void VerbalCover::stable_sort()
{
  std::stable_sort(completions.begin(), completions.end());
}


unsigned char VerbalCover::size() const
{
  return static_cast<unsigned char>(completions.size());
}


Opponent VerbalCover::simplestOpponent(const unsigned char oppsLength) const
{
  if (! lengthFlag)
    return OPP_EITHER;

  // Choose the shorter side, as this tends to be more intuitive.
  const unsigned char lsum = lengthLower + lengthUpper;

  if (lsum > oppsLength)
    return OPP_EAST;
  else if (lsum < oppsLength)
    return OPP_WEST;
  else
    return OPP_EITHER;
}


string VerbalCover::strLengthEqual(
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
    value = lengthLower;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    value = oppsLength - lengthLower;
  }

  stringstream ss;

  if (value == 0)
    ss << side << " is void";
  else if (value == 1)
    ss << side << " has a singleton";
  else if (value == 2)
    ss << side << " has a doubleton";
  else if (value == 3 && oppsLength > 7)
    ss << side << " has a tripleton";
  else
    ss << "The suit splits " <<
      +lengthLower << "=" << +(oppsLength - lengthLower);

  return ss.str();
}


string VerbalCover::strInside(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  string side;
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    vLower = lengthLower;
    vUpper = (lengthOper == COVER_GREATER_EQUAL ?
      oppsLength : lengthUpper);
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    vLower = (lengthOper == COVER_GREATER_EQUAL ?
      0 : oppsLength - lengthUpper);
    vUpper = oppsLength - lengthLower;
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


string VerbalCover::strLength(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  if (lengthOper == COVER_EQUAL)
  {
    return VerbalCover::strLengthEqual(
      oppsLength, simplestOpponent, symmFlag);
  }
  else if (lengthOper == COVER_INSIDE_RANGE ||
           lengthOper == COVER_LESS_EQUAL ||
           lengthOper == COVER_GREATER_EQUAL)
  {
    return VerbalCover::strInside(oppsLength, simplestOpponent, symmFlag);
  }
  else
  {
    assert(false);
    return "";
  }
}


string VerbalCover::strCompletions(const RanksNames& ranksNames) const
{
  string s;
  size_t i = 0;

  for (auto& completion: completions)
  {
    if (i > 0)
      s += (i+1 == completions.size() ? " or " : ", ");

    s += completion.str(ranksNames);
    i++;
  }
  return s;
}


string VerbalCover::str(const RanksNames& ranksNames) const
{
  return VerbalCover::strCompletions(ranksNames);
}


string VerbalCover::strGeneral(
  const unsigned char oppsLength,
  const bool symmFlag,
  const RanksNames& ranksNames) const
{
  string lstr = "", wstr = "", estr = "";
  if (lengthFlag)
  {
    Opponent simplestOpponent;
    if (symmFlag)
      simplestOpponent = OPP_WEST;
    else if (westFlag == eastFlag)
      simplestOpponent = VerbalCover::simplestOpponent(oppsLength);
    else if (westFlag)
      simplestOpponent = OPP_WEST;
    else
      simplestOpponent = OPP_EAST;

    lstr = VerbalCover::strLength(oppsLength, simplestOpponent, symmFlag);
  }

  if (westFlag)
    wstr = west.str(ranksNames);

  if (eastFlag)
    estr = east.str(ranksNames);

  if (lengthFlag)
  {
    if (westFlag)
    {
      if (eastFlag)
      {
        if (symmFlag)
        {
          if (wstr == estr)
            return lstr + ", and West and East each have " + wstr;
          else
            return lstr + ", and " + wstr + " and " + estr +
              " are split";
        }
        else if (wstr == estr)
          return lstr + ", West and East each have " + wstr;
        else
          return lstr + ", West has " + wstr + " and East has " + estr;
      }
      else
      {
        if (symmFlag)
          return lstr + " with " + wstr;
        else
          return lstr + " and West has " + wstr;
      }
    }
    else if (eastFlag)
    {
      if (symmFlag)
        return lstr + " without " + estr;
      else
        return lstr + " and East has " + estr;
    }
    else
      return lstr;
  }
  else if (westFlag)
  {
    if (eastFlag)
    {
      assert(wstr == estr);
      return "West and East each have " + wstr;
    }
    else
    {
      if (symmFlag)
        return "Either side has " + wstr;
      else
        return "West has " + wstr;
    }
  }
  else if (eastFlag)
  {
    if (symmFlag)
      return "Either side has " + estr;
    else
      return "East has " + estr;
  }
  else
    assert(false);

  return "";
}

