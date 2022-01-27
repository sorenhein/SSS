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
    if (invertFlag)
    {
      if (westLength.value2 == westLength.value1)
        ss << "West is not void";
      else
        ss << "West has at least " << +(westLength.value2+1) << " cards";
    }
    else if (westLength.value2 == 1)
      ss << "West has at most a singleton";
    else if (westLength.value2 == 2)
      ss << "West has at most a doubleton";
    else
      ss << "West has at most " << +westLength.value2 << " cards";
  }
  else if (westLength.value2 == oppsLength)
  {
    if (invertFlag)
      ss << "West has at most " << +(westLength.value1-1) << " cards";
    else
      ss << "West has at least " << +westLength.value1 << " cards";
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
      (oppsTops1 == 2 ? "both" : "exactly two") << " tops";
  }
  else
  {
    ss << "West " << (invertFlag ? "does not have" : "has") << " " <<
      "exactly " << +westTop1.value1 << " tops";
  }
  return ss.str();
}


string CoverSpec::strTop1Inside() const
{
  stringstream ss;
  if (westTop1.value1 == 0)
  {
    ss << "West has " << 
      (invertFlag ? "more than" : "at most") << " " << 
      +westLength.value2 << " tops";
  }
  else if (westTop1.value2 == oppsTops1)
  {
    ss << "West has " << 
      (invertFlag ? "at most" : "more than") << " " <<
      +westLength.value2 << " tops";
  }
  else
  {
    if (invertFlag)
      ss << "West has fewer than " << +westTop1.value1 <<
        " or more than " << +westTop1.value2 << " tops";
    else
      ss << "West has between " <<
        +westTop1.value1 << " and " <<
        +westTop1.value2 << " tops";
  }
  return ss.str();
}


string CoverSpec::strLength() const
{
  if (westLength.oper == COVER_EQUAL)
    return CoverSpec::strLengthEqual();
  else if (westLength.oper == COVER_INSIDE_RANGE)
    return CoverSpec::strLengthInside();
  else
  {
    assert(false);
    return "";
  }
}


string CoverSpec::strTop1() const
{
  if (westTop1.oper == COVER_EQUAL)
    return CoverSpec::strTop1Equal();
  else if (westTop1.oper == COVER_INSIDE_RANGE)
    return CoverSpec::strTop1Inside();
  else
  {
    assert(false);
    return "";
  }
}


string CoverSpec::strBothEqual() const
{
  stringstream ss;
  if (westLength.value1 == 1)
  {
    assert(! invertFlag);

    if (westTop1.value1 == 0)
      ss << "West has a small singleton";
    else
      ss << "West has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else if (westLength.value1+1 == oppsLength)
  {
    assert(! invertFlag);

    if (westTop1.value1 == oppsTops1)
      ss << "East has a small singleton";
    else
      ss << "East has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else if (westLength.value1 == 2)
  {
    assert(! invertFlag);

    if (westTop1.value1 == 0)
    {
      if (oppsLength == 4 && oppsTops1 == 2)
        ss << "East has doubleton honors (HH)";
      else
        ss << "West has a small doubleton";
    }
    else if (westTop1.value1 == 1)
      ss << "West has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << "West has doubleton honors (HH)";
  }
  else if (westLength.value1+2 == oppsLength)
  {
    assert(! invertFlag);

    if (westTop1.value1 == oppsTops1)
      ss << "East has a small doubleton";
    else if (westTop1.value1+1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << "East has doubleton honors (HH)";
  }
  else if (westLength.value1 == 3)
  {
    assert(! invertFlag);

    if (westTop1.value1 == 0)
      ss << "West has a small tripleton";
    else if (westTop1.value1 == 1)
      ss << "West has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (westTop1.value1 == 2)
      ss << "West has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << "West has tripleton honors (HHH)";
  }
  else if (westLength.value1+3 == oppsLength)
  {
    assert(! invertFlag);

    if (westTop1.value1 == oppsTops1)
      ss << "East has a small tripleton";
    else if (westTop1.value1+1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (westTop1.value1+2 == oppsTops1)
      ss << "East has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << "East has tripleton honors (HHH)";
  }
  else
    assert(false);

  return ss.str();
}


string CoverSpec::strTop1Fixed() const
{
  stringstream ss;

  const unsigned char xesWestMax = westLength.value2 - westTop1.value1;
  const unsigned char xesWestMin = westLength.value1 - westTop1.value1;

  const unsigned char xesEastMax = (oppsLength - westLength.value1) -
    (oppsTops1 - westTop1.value1);
  const unsigned char xesEastMin = (oppsLength - westLength.value2) -
    (oppsTops1 - westTop1.value1);

  const string strWest = string(xesWestMin, 'x') + 
    "(" + string(xesWestMax - xesWestMin, 'x') + ")";
  const string strEast = string(xesEastMin, 'x') + 
    "(" + string(xesEastMax - xesEastMin, 'x') + ")";

  assert(! invertFlag);

  if (westTop1.value1 == 0)
  {
    if (oppsTops1 == 1)
    {
      if (xesEastMax == 1)
        ss << "East has the top at most doubleton";
      else
        ss << "East has H" << strEast;
    }
    else if (oppsTops1 == 2)
    {
      if (xesEastMax == 1)
        ss << "East has both tops at most tripleton";
      else
        ss << "East has HH" << strEast;
    }
    else
      assert(false);
  }
  else if (westTop1.value1 == oppsTops1)
  {
    if (oppsTops1 == 1)
    {
      if (xesWestMax == 1)
        ss << "West has the top at most doubleton";
      else
        ss << "West has H" << strWest;
    }
    else if (oppsTops1 == 2)
    {
      if (xesWestMax == 1)
        ss << "West has both tops at most tripleton";
      else
        ss << "West has HH" << strWest;
    }
    else if (oppsTops1 == 3)
    {
      ss << "West has HHH" << strWest;
    }
    else
    {
      cout << "xesWestMax " << +xesWestMax << 
        ", xesEastMax " << +xesEastMax << "\n";
      cout << "WW" << CoverSpec::strLength() << 
        ", and " <<CoverSpec::strTop1() << endl;
      assert(false);
    }
  }
  else if (westTop1.value1 == 1)
  {
    if (oppsTops1 == 2)
    {
      // Look at it from the shorter side
      if (xesWestMax <= xesEastMax)
      {
        if (xesWestMax == 1)
          ss << "West has one top at most doubleton";
        else
          ss << "West has H" << strWest;
      }
      else
      {
        if (xesEastMax == 1)
          ss << "East has one top at most doubleton";
        else
          ss << "East has H" << strEast;
      }
    }
    else
    {
      if (xesWestMax == 1)
        ss << "West has one top at most doubleton";
      else
        ss << "West has H" << strWest;
    }
  }
  else if (westTop1.value1+1 == oppsTops1)
  {
    if (xesEastMax == 1)
      ss << "East has one top at most doubleton";
    else
      ss << "East has H" << strEast;
  }
  else
    assert(false);

  return ss.str();
}


string CoverSpec::str() const
{
  if (mode == COVER_LENGTHS_ONLY)
    return CoverSpec::strLength();
  else if (mode == COVER_TOPS_ONLY)
    return CoverSpec::strTop1();
  else if (mode == COVER_LENGTHS_AND_TOPS)
  {
    if (westLength.oper == COVER_EQUAL)
    {
      if (westTop1.oper == COVER_EQUAL)
        return CoverSpec::strBothEqual();
      else
        assert(false);
    }
    else
    {
      if (westTop1.oper == COVER_EQUAL)
        return CoverSpec::strTop1Fixed();
      else
      {
      }
    }
    // At the moment only 1=5/2=4 with 1-2 West tops
    return "XX " + CoverSpec::strLength() + ", and " + CoverSpec::strTop1();
  }
  else if (mode == COVER_LENGTHS_OR_TOPS)
  {
    // At the moment
    // 57 YYThe suit splits 2-2, or East has the tops
    // 15 YYThe suit splits 3=3, or West has the tops
    // 24 YYWest has at least 2 cards, or East has the tops
    // 12 YYWest has at most 3 cards, or West has the tops

    return "YY " + CoverSpec::strLength() + ", or " + CoverSpec::strTop1();
  }
  else
    return "";
}

