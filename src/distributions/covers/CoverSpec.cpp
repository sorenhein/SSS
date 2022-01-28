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
  mode = {COVER_MODE_NONE, COVER_MODE_NONE};
  invertFlag = {false, false};
  symmFlag = false;
}


string CoverSpec::strLengthEqual(const unsigned specNumber) const
{
  stringstream ss;
  if (westLength[specNumber].value1 == 0)
  {
    ss << "West " <<
      (invertFlag[specNumber] ? "is not" : "is") << 
      " void";
  }
  else if (westLength[specNumber].value1 == oppsLength)
  {
    ss << "East " <<
      (invertFlag[specNumber] ? "is not" : "is") << 
      " void";
  }
  else if (westLength[specNumber].value1 == 1)
  {
    ss << "West " <<
      (invertFlag[specNumber] ? "does not have" : "has") << 
      " a singleton";
  }
  else if (westLength[specNumber].value1 == oppsLength-1)
  {
    ss << "East " <<
      (invertFlag[specNumber] ? "does not have" : "has") << 
      " a singleton";
  }
  else if (westLength[specNumber].value1 == 2)
  {
    if (oppsLength > 4)
    {
      ss << "West " <<
        (invertFlag[specNumber] ? "does not have" : "has") << 
        " a doubleton";
    }
    else
    {
      ss << "The suit " << 
        (invertFlag[specNumber] ? "does not split" : "splits") << " 2=2";
    }
  }
  else
  {
    ss << "The suit " <<
      (invertFlag[specNumber] ? "does not split" : "splits") << " " <<
      +westLength[specNumber].value1 << "=" <<
      +(oppsLength - westLength[specNumber].value1);
  }
  return ss.str();
}


string CoverSpec::strLengthInside(const unsigned specNumber) const
{
  stringstream ss;
  if (westLength[specNumber].value1 == 0)
  {
    if (invertFlag[specNumber])
    {
      if (westLength[specNumber].value2 == westLength[specNumber].value1)
        ss << "West is not void";
      else
        ss << "West has at least " << 
          +(westLength[specNumber].value2+1) << " cards";
    }
    else if (westLength[specNumber].value2 == 1)
      ss << "West has at most a singleton";
    else if (westLength[specNumber].value2 == 2)
      ss << "West has at most a doubleton";
    else
      ss << "West has at most " << 
        +westLength[specNumber].value2 << " cards";
  }
  else if (westLength[specNumber].value2 == oppsLength)
  {
    if (invertFlag[specNumber])
      ss << "West has at most " << 
        +(westLength[specNumber].value1-1) << " cards";
    else
      ss << "West has at least " << 
        +westLength[specNumber].value1 << " cards";
  }
  else if (westLength[specNumber].value1 == 1 && 
      westLength[specNumber].value2 == oppsLength-1)
  {
    ss << (invertFlag[specNumber] ? "Either" : "Neither") << 
      " opponent is void";
  }
  else if (westLength[specNumber].value1 + 
      westLength[specNumber].value2 == oppsLength)
  {
    if (westLength[specNumber].value1 + 1 == 
        westLength[specNumber].value2)
    {
      ss << "The suit " <<
        (invertFlag[specNumber] ? "does not split" : "splits") << " " <<
        to_string(westLength[specNumber].value1) + "-" + 
        to_string(westLength[specNumber].value2) + " either way";
    }
    else
    {
      if (invertFlag[specNumber])
        return "The suit splits less evenly than " +
          to_string(westLength[specNumber].value1) + "-" +
          to_string(westLength[specNumber].value2) + " either way";
      else
        return "The suit splits " +
          to_string(westLength[specNumber].value1) + "-" +
          to_string(westLength[specNumber].value2) + 
            " or better either way";
    }
  }
  else
  {
    if (invertFlag[specNumber])
      ss << "West has fewer than " << 
        +westLength[specNumber].value1 <<
        " or more than " << 
        +westLength[specNumber].value2 << " cards";
    else
      ss << "The suit splits between " <<
        +westLength[specNumber].value1 << "=" << 
        +(oppsLength - westLength[specNumber].value1) << " and " << 
        +westLength[specNumber].value2 << "=" << 
        +(oppsLength - westLength[specNumber].value2);
  }
  return ss.str();
}


string CoverSpec::strTop1Equal(const unsigned specNumber) const
{
  stringstream ss;
  if (westTop1[specNumber].value1 == 0)
  {
    if (oppsTops1 == 1)
      ss << (invertFlag[specNumber] ? "West" : "East") << " has the top";
    else
    {
      if (invertFlag[specNumber])
        ss << "West has at least one top";
      else
        ss << "East has the tops";
    }
  }
  else if (westTop1[specNumber].value1 == oppsTops1)
  {
    if (oppsTops1 == 1)
      ss << (invertFlag[specNumber] ? "East" : "West") << " has the top";
    else
    {
      if (invertFlag[specNumber])
        ss << "East has at least one top";
      else
        ss << "West has the tops";
    }
  }
  else if (westTop1[specNumber].value1 == 1)
  {
    if (oppsTops1 == 1)
      ss << (invertFlag[specNumber] ? "East" : "West") << " has the top";
    else
    {
      ss << "West " << 
        (invertFlag[specNumber] ? "does not have" : "has") << " " <<
        "exactly one top";
    }
  }
  else if (westTop1[specNumber].value1 == oppsTops1-1)
  {
    ss << "East " << (invertFlag[specNumber] ? "does not have" : "has") <<
      " " << "exactly one top";
  }
  else if (westTop1[specNumber].value1 == 2)
  {
    ss << "West " << (invertFlag[specNumber] ? "does not have" : "has") <<
      " " << (oppsTops1 == 2 ? "both" : "exactly two") << " tops";
  }
  else
  {
    ss << "West " << (invertFlag[specNumber] ? "does not have" : "has") <<
      " " << "exactly " << +westTop1[specNumber].value1 << " tops";
  }
  return ss.str();
}


string CoverSpec::strTop1Inside(const unsigned specNumber) const
{
  stringstream ss;
  if (westTop1[specNumber].value1 == 0)
  {
    if (westTop1[specNumber].value2 == oppsTops1-1)
    {
      ss << "East has " <<
        (invertFlag[specNumber] ? "less than " : "at least") << 
          " one top";
    }
    else
    {
      ss << "West has " << 
        (invertFlag[specNumber] ? "less than" : "at least") << " " << 
        +westTop1[specNumber].value2 << " tops";
    }
  }
  else if (westTop1[specNumber].value2 == oppsTops1)
  {
    if (westTop1[specNumber].value1 == 1)
    {
      ss << "West has " << 
        (invertFlag[specNumber] ? " no tops" : "at least one top");
    }
    else
    {
      ss << "West has " << 
        (invertFlag[specNumber] ? "less than" : "at least") << " " <<
        +westTop1[specNumber].value1 << " tops";
    }
  }
  else
  {
    if (invertFlag[specNumber])
      ss << "West has fewer than " << +westTop1[specNumber].value1 <<
        " or more than " << +westTop1[specNumber].value2 << " tops";
    else
      ss << "West has between " <<
        +westTop1[specNumber].value1 << " and " <<
        +westTop1[specNumber].value2 << " tops";
  }
  return ss.str();
}


string CoverSpec::strLength(const unsigned specNumber) const
{
  if (westLength[specNumber].oper == COVER_EQUAL)
    return CoverSpec::strLengthEqual(specNumber);
  else if (westLength[specNumber].oper == COVER_INSIDE_RANGE)
    return CoverSpec::strLengthInside(specNumber);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSpec::strTop1(const unsigned specNumber) const
{
  if (westTop1[specNumber].oper == COVER_EQUAL)
    return CoverSpec::strTop1Equal(specNumber);
  else if (westTop1[specNumber].oper == COVER_INSIDE_RANGE)
    return CoverSpec::strTop1Inside(specNumber);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSpec::strBothEqual(const unsigned specNumber) const
{
  stringstream ss;
  if (westLength[specNumber].value1 == 1)
  {
    assert(! invertFlag[specNumber]);

    if (westTop1[specNumber].value1 == 0)
      ss << "West has a small singleton";
    else
      ss << "West has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else if (westLength[specNumber].value1+1 == oppsLength)
  {
    assert(! invertFlag[specNumber]);

    if (westTop1[specNumber].value1 == oppsTops1)
      ss << "East has a small singleton";
    else
      ss << "East has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else if (westLength[specNumber].value1 == 2)
  {
    assert(! invertFlag[specNumber]);

    if (westTop1[specNumber].value1 == 0)
    {
      if (oppsLength == 4 && oppsTops1 == 2)
        ss << "East has doubleton honors (HH)";
      else
        ss << "West has a small doubleton";
    }
    else if (westTop1[specNumber].value1 == 1)
      ss << "West has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << "West has doubleton honors (HH)";
  }
  else if (westLength[specNumber].value1+2 == oppsLength)
  {
    assert(! invertFlag[specNumber]);

    if (westTop1[specNumber].value1 == oppsTops1)
      ss << "East has a small doubleton";
    else if (westTop1[specNumber].value1+1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << "East has doubleton honors (HH)";
  }
  else if (westLength[specNumber].value1 == 3)
  {
    assert(! invertFlag[specNumber]);

    if (westTop1[specNumber].value1 == 0)
      ss << "West has a small tripleton";
    else if (westTop1[specNumber].value1 == 1)
      ss << "West has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (westTop1[specNumber].value1 == 2)
      ss << "West has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << "West has tripleton honors (HHH)";
  }
  else if (westLength[specNumber].value1+3 == oppsLength)
  {
    assert(! invertFlag[specNumber]);

    if (westTop1[specNumber].value1 == oppsTops1)
      ss << "East has a small tripleton";
    else if (westTop1[specNumber].value1+1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (westTop1[specNumber].value1+2 == oppsTops1)
      ss << "East has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << "East has tripleton honors (HHH)";
  }
  else
    assert(false);

  return ss.str();
}


string CoverSpec::strTop1Fixed(const unsigned specNumber) const
{
  stringstream ss;

  const unsigned char xesWestMax = westLength[specNumber].value2 - 
    westTop1[specNumber].value1;
  const unsigned char xesWestMin = westLength[specNumber].value1 - 
    westTop1[specNumber].value1;

  const unsigned char xesEastMax = 
    (oppsLength - westLength[specNumber].value1) -
    (oppsTops1 - westTop1[specNumber].value1);
  const unsigned char xesEastMin = 
    (oppsLength - westLength[specNumber].value2) -
    (oppsTops1 - westTop1[specNumber].value1);

  const string strWest = string(xesWestMin, 'x') + 
    "(" + string(xesWestMax - xesWestMin, 'x') + ")";
  const string strEast = string(xesEastMin, 'x') + 
    "(" + string(xesEastMax - xesEastMin, 'x') + ")";

  assert(! invertFlag[specNumber]);

  if (westTop1[specNumber].value1 == 0)
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
  else if (westTop1[specNumber].value1 == oppsTops1)
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
      cout << "WW" << CoverSpec::strLength(specNumber) << 
        ", and " <<CoverSpec::strTop1(specNumber) << endl;
      assert(false);
    }
  }
  else if (westTop1[specNumber].value1 == 1)
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
  else if (westTop1[specNumber].value1+1 == oppsTops1)
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


string CoverSpec::strSet(const unsigned specNumber) const
{
  if (mode[specNumber] == COVER_LENGTHS_ONLY)
    return CoverSpec::strLength(specNumber);
  else if (mode[specNumber] == COVER_TOPS_ONLY)
    return CoverSpec::strTop1(specNumber);
  else if (mode[specNumber] == COVER_LENGTHS_AND_TOPS)
  {
    if (westLength[specNumber].oper == COVER_EQUAL)
    {
      if (westTop1[specNumber].oper == COVER_EQUAL)
        return CoverSpec::strBothEqual(specNumber);
      else
      {
        return "ZZ " + CoverSpec::strLength(specNumber) + ", and " + 
          CoverSpec::strTop1(specNumber);
      }
    }
    else
    {
      if (westTop1[specNumber].oper == COVER_EQUAL)
        return CoverSpec::strTop1Fixed(specNumber);
      else
      {
      }
    }
    // At the moment only 1=5/2=4 with 1-2 West tops
    return "XX " + CoverSpec::strLength(specNumber) + ", and " + 
      CoverSpec::strTop1(specNumber);
  }
  else
    return "";
}


string CoverSpec::str() const
{
  if (mode[1] == COVER_MODE_NONE)
    return CoverSpec::strSet(0);
  else
  {
    // At the moment
    // 57 YYThe suit splits 2-2, or East has the tops
    // 15 YYThe suit splits 3=3, or West has the tops
    // 24 YYWest has at least 2 cards, or East has the tops
    // 12 YYWest has at most 3 cards, or West has the tops
    return "YY " + CoverSpec::strSet(0) + "; or " + CoverSpec::strSet(1);
  }
}

