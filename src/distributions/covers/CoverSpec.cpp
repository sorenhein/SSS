/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverSpec.h"


CoverSpec::CoverSpec()
{
}


string CoverSpec::strLengthEqual() const
{
  stringstream ss;
  if (westLength.value1 == 0)
  {
    ss << "West " <<
      (invertFlag ? "is not" : "is") << 
      " void";
  }
  else if (westLength.value1 == oppsLength)
  {
    ss << "East " <<
      (invertFlag ? "is not" : "is") << 
      " void";
  }
  else if (westLength.value1 == 1)
  {
    ss << "West " <<
      (invertFlag ? "does not have" : "has") << 
      " a singleton";
  }
  else if (westLength.value1 == oppsLength-1)
  {
    ss << "East " <<
      (invertFlag ? "does not have" : "has") << 
      " a singleton";
  }
  else if (westLength.value1 == 2)
  {
    if (oppsLength > 4)
    {
      ss << "West " <<
        (invertFlag ? "does not have" : "has") << " a doubleton";
    }
    else
    {
      ss << "The suit " << (invertFlag ? "does not split" : "splits") <<
        " 2-2";
    }
  }
  else
  {
    ss << "The suit " <<
      (invertFlag ? "does not split" : "splits") << " " <<
      +westLength.value1 << "=" <<
      +(oppsLength - westLength.value1);
  }
  return ss.str();
}


string CoverSpec::strLengthInside() const
{
  stringstream ss;

  if (westLength.value1 == 0)
  {
    ss << "West has " << 
      (invertFlag ? "more than" : "at most") << " " << 
      +westLength.value2 << " cards";
  }
  else if (westLength.value2 == oppsLength)
  {
    ss << "West has " << 
      (invertFlag ? "at most" : "more than") << " " <<
      +westLength.value2 << " cards";
  }
  else if (westLength.value1 == 1 && westLength.value2 == oppsLength-1)
  {
    ss << (invertFlag ? "Either" : "Neither") << " opponent is void";
  }
  else if (westLength.value1 + westLength.value2 == oppsLength)
  {
    if (westLength.value1 + 1 == westLength.value2)
    {
      ss << "The suit " <<
        (invertFlag ? "does not split" : "splits") << " " <<
        to_string(westLength.value1) + "-" + 
        to_string(westLength.value2) + " either way";
    }
    else
    {
      if (invertFlag)
        return "The suit splits less evenly than " +
          to_string(westLength.value1) + "-" +
          to_string(westLength.value2) + " either way";
      else
        return "The suit splits " +
          to_string(westLength.value1) + "-" +
          to_string(westLength.value2) + " or better either way";
    }
  }
  else
  {
    if (invertFlag)
      ss << "West has fewer than " << +westLength.value1 <<
        " or more than " << +westLength.value2 << " cards";
    else
      ss << "The suit splits between " <<
        +westLength.value1 << "=" << 
        +(oppsLength-westLength.value1) << " and " << 
        +westLength.value2 << "=" << 
        +(oppsLength-westLength.value2);
  }
  return ss.str();
}


string CoverSpec::strTop1Equal() const
{
  stringstream ss;
  if (westTop1.value1 == 0)
  {
    if (oppsTops1 == 1)
      ss << (invertFlag ? "West" : "East") << " has the top";
    else
    {
      if (invertFlag)
        ss << "West has at least one top";
      else
        ss << "East has the tops";
    }
  }
  else if (westTop1.value1 == oppsTops1)
  {
    if (oppsTops1 == 1)
      ss << (invertFlag ? "East" : "West") << " has the top";
    else
    {
      if (invertFlag)
        ss << "East has at least one top";
      else
        ss << "West has the tops";
    }
  }
  else if (westTop1.value1 == 1)
  {
    if (oppsTops1 == 1)
      ss << (invertFlag ? "East" : "West") << " has the top";
    else
    {
      ss << "West " << (invertFlag ? "does not have" : "has") << " " <<
        "exactly one top";
    }
  }
  else if (westTop1.value1 == oppsTops1-1)
  {
    ss << "East " << (invertFlag ? "does not have" : "has") << " " <<
      "exactly one top";
  }
  else if (westTop1.value1 == 2)
  {
    ss << "West " << (invertFlag ? "does not have" : "has") << " " <<
      "exactly two tops";
  }
  else
  {
    ss << "West " << (invertFlag ? "does not have" : "has") << " " <<
      "exactly " << +westTop1.value1 << " tops";
  }
  return ss.str();
}


string CoverSpec::strLength() const
{
  if (westLength.oper == COVER_EQUAL)
  {
    return CoverSpec::strLengthEqual();
  }
  else if (westLength.oper == COVER_INSIDE_RANGE)
  {
    return CoverSpec::strLengthInside();
  }
  else
    return westLength.str("cards");
}


// TODO Goes away longer-term
string CoverSpec::strTop1() const
{
  if (westTop1.oper == COVER_EQUAL)
    return CoverSpec::strTop1Equal();
  else
    return westTop1.str("tops");
}


string CoverSpec::str() const
{
  if (mode == COVER_LENGTHS_ONLY)
    return CoverSpec::strLength();
  else if (mode == COVER_TOPS_ONLY)
    return CoverSpec::strTop1();
  else if (mode == COVER_LENGTHS_OR_TOPS)
    return CoverSpec::strLength() + ", or " + CoverSpec::strTop1();
  else if (mode == COVER_LENGTHS_AND_TOPS)
    return CoverSpec::strLength() + ", and " + CoverSpec::strTop1();
  else
    return "";
}

