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
  symmFlags = {false, false};
}


void CoverSpec::getIndices(
  unsigned char& length,
  unsigned char& tops1) const
{
  length = oppsLength;
  tops1 = oppsTops1;
}


string CoverSpec::strLengthEqual(
  const unsigned char wlen,
  const bool symmFlag) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (wlen == 0)
    ss << side << " is void";
  else if (wlen == oppsLength)
  {
    assert(! symmFlag);
    ss << "East is void";
  }
  else if (wlen == 1)
    ss << side << " has a singleton";
  else if (wlen == oppsLength-1)
  {
    assert(! symmFlag);
    ss << "East has a singleton";
  }
  else if (wlen == 2)
  {
    if (oppsLength > 4)
      ss << side << " has a doubleton";
    else
      ss << "The suit splits 2=2";
  }
  else
    ss << "The suit splits " << +wlen << "=" << +(oppsLength - wlen);

  return ss.str();
}


string CoverSpec::strLengthInside(
  const unsigned char wlen1,
  const unsigned char wlen2,
  const bool symmFlag) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (wlen1 == 0)
  {
    if (wlen2 == 1)
      ss << side << " has at most a singleton";
    else if (wlen2 == 2)
      ss << side << " has at most a doubleton";
    else
      ss << side << " has at most " << +wlen2 << " cards";
  }
  else if (wlen2 == oppsLength)
  {
    ss << side << " has at least " << +wlen1 << " cards";
  }
  else if (wlen1 == 1 && wlen2 == oppsLength-1)
  {
    ss << "Neither opponent is void";
  }
  else if (wlen1 + wlen2 == oppsLength)
  {
    if (wlen1 + 1 == wlen2)
    {
      ss << "The suit splits " << +wlen1 << "-" << +wlen2 << " either way";
    }
    else
    {
      ss << "The suit splits " << +wlen1 << "-" << +wlen2 <<
        " or better either way";
    }
  }
  else
  {
    ss << "The suit splits between " <<
      +wlen1 << "=" << +(oppsLength - wlen1) << " and " << 
      +wlen2 << "=" << +(oppsLength - wlen2);
  }
  return ss.str();
}


string CoverSpec::strTop1Equal(
  const unsigned char wtop,
  const bool symmFlag) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (wtop == 0)
  {
    assert(! symmFlag);
    if (oppsTops1 == 1)
      ss << "East has the top";
    else
      ss << "East has the tops";
  }
  else if (wtop == oppsTops1)
  {
    if (oppsTops1 == 1)
      ss << side << " has the top";
    else
      ss << side << " has the tops";
  }
  else if (wtop == 1)
  {
    if (oppsTops1 == 1)
      ss << side << " has the top";
    else
      ss << side << " has exactly one top";
  }
  else if (wtop == oppsTops1-1)
  {
    assert(! symmFlag);
    ss << "East has exactly one top";
  }
  else if (wtop == 2)
  {
    ss << side << " has " <<
      (oppsTops1 == 2 ? "both" : "exactly two") << " tops";
  }
  else
    ss << side << " has exactly " << wtop << " tops";

  return ss.str();
}


string CoverSpec::strTop1Inside(
  const unsigned char wtop1,
  const unsigned char wtop2,
  const bool symmFlag) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (wtop1 == 0)
  {
    if (wtop2 == oppsTops1-1)
    {
      assert(! symmFlag);
      ss << "East has at least one top";
    }
    else
      ss << side << " has at most " << +wtop2 << " tops";
  }
  else if (wtop2 == oppsTops1)
  {
    if (wtop1 == 1)
      ss << side << " has at least one top";
    else
      ss << side << " has at least " << +wtop1 << " tops";
  }
  else
  {
      ss << side << 
        " has between " << +wtop1 << " and " << +wtop2 << " tops";
  }

  return ss.str();
}


string CoverSpec::strLength(const unsigned specNumber) const
{
  if (westLength[specNumber].oper == COVER_EQUAL)
    return CoverSpec::strLengthEqual(
      westLength[specNumber].value1,
      symmFlags[specNumber]);
  else if (westLength[specNumber].oper == COVER_INSIDE_RANGE)
    return CoverSpec::strLengthInside(
      westLength[specNumber].value1, 
      westLength[specNumber].value2,
      symmFlags[specNumber]);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSpec::strTop1(const unsigned specNumber) const
{
  if (westTop1[specNumber].oper == COVER_EQUAL)
    return CoverSpec::strTop1Equal(
      westTop1[specNumber].value1,
      symmFlags[specNumber]);
  else if (westTop1[specNumber].oper == COVER_INSIDE_RANGE)
    return CoverSpec::strTop1Inside(
      westTop1[specNumber].value1, 
      westTop1[specNumber].value2,
      symmFlags[specNumber]);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSpec::strBothEqual(
  const unsigned char wlen,
  const unsigned char wtop,
  const bool symmFlag) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  if (wlen == 0)
  {
    ss << side << " is void";
  }
  else if (wlen == 1)
  {
    if (wtop == 0)
      ss << side << " has a small singleton";
    else
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else if (wlen+1 == oppsLength)
  {
    assert(! symmFlag);
    if (wtop == oppsTops1)
      ss << "East has a small singleton";
    else
      ss << "East has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else if (wlen == 2)
  {
    if (wtop == 0)
    {
      if (oppsLength == 4 && oppsTops1 == 2)
      {
        assert(! symmFlag);
        ss << "East has doubleton honors (HH)";
      }
      else
        ss << side << " has a small doubleton";
    }
    else if (wtop == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << side << " has doubleton honors (HH)";
  }
  else if (wlen+2 == oppsLength)
  {
    assert(! symmFlag);
    if (wtop == oppsTops1)
      ss << "East has a small doubleton";
    else if (wtop+1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << "East has doubleton honors (HH)";
  }
  else if (wlen == 3)
  {
    if (wtop == 0)
      ss << side << " has a small tripleton";
    else if (wtop == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (wtop == 2)
      ss << side << " has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << side << " has tripleton honors (HHH)";
  }
  else if (wlen+3 == oppsLength)
  {
    assert(! symmFlag);
    if (wtop == oppsTops1)
      ss << "East has a small tripleton";
    else if (wtop+1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (wtop+2 == oppsTops1)
      ss << "East has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << "East has tripleton honors (HHH)";
  }
  else
  {
cout << "WLEN " << +wlen << " WTOP " << +wtop << endl;
    assert(false);
  }

  return ss.str();
}


string CoverSpec::strTop1Fixed(const unsigned specNumber) const
{
  stringstream ss;
  const string side = (symmFlags[specNumber] ? "Either opponent" : "West");

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

  if (westTop1[specNumber].value1 == 0)
  {
    assert(! symmFlags[specNumber]);
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
        ss << side << " has the top at most doubleton";
      else
        ss << side << " has H" << strWest;
    }
    else if (oppsTops1 == 2)
    {
      if (xesWestMax == 1)
        ss << side << " has both tops at most tripleton";
      else
        ss << side << " has HH" << strWest;
    }
    else if (oppsTops1 == 3)
    {
      ss << side << " has HHH" << strWest;
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
          ss << side << " has one top at most doubleton";
        else
          ss << side << " has H" << strWest;
      }
      else
      {
        assert(! symmFlags[specNumber]);
        if (xesEastMax == 1)
          ss << "East has one top at most doubleton";
        else
          ss << "East has H" << strEast;
      }
    }
    else
    {
      if (xesWestMax == 1)
        ss << side << " has one top at most doubleton";
      else
        ss << side << " has H" << strWest;
    }
  }
  else if (westTop1[specNumber].value1+1 == oppsTops1)
  {
    assert(! symmFlags[specNumber]);
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
        return CoverSpec::strBothEqual(
          westLength[specNumber].value1,
          westTop1[specNumber].value1,
          symmFlags[specNumber]);
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

