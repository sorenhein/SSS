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
  CoverSpec::reset();
}


void CoverSpec::reset()
{
  setsWest[0].reset();
  setsWest[1].reset();
}


bool CoverSpec::includes(
  const unsigned specNumber,
  const unsigned char wlen,
  const unsigned char wtop) const
{
  // Could inline this in next method
  return setsWest[specNumber].includes(wlen, wtop, oppsLength, oppsTops1);
}


bool CoverSpec::includes(
  const unsigned char wlen,
  const unsigned char wtop) const
{
  return 
    CoverSpec::includes(0, wlen, wtop) ||
    CoverSpec::includes(1, wlen, wtop);
}


void CoverSpec::getIndices(
  unsigned char& length,
  unsigned char& tops1) const
{
  length = oppsLength;
  tops1 = oppsTops1;
}


string CoverSpec::strTop1Fixed(const unsigned specNumber) const
{
  stringstream ss;
  const string side = (setsWest[specNumber].symmFlag ? "Either opponent" : "West");

  const unsigned char xesWestMax = setsWest[specNumber].length.value2 - 
    setsWest[specNumber].top1.value1;
  const unsigned char xesWestMin = setsWest[specNumber].length.value1 - 
    setsWest[specNumber].top1.value1;

  const unsigned char xesEastMax = 
    (oppsLength - setsWest[specNumber].length.value1) -
    (oppsTops1 - setsWest[specNumber].top1.value1);
  const unsigned char xesEastMin = 
    (oppsLength - setsWest[specNumber].length.value2) -
    (oppsTops1 - setsWest[specNumber].top1.value1);

  const string strWest = string(xesWestMin, 'x') + 
    "(" + string(xesWestMax - xesWestMin, 'x') + ")";
  const string strEast = string(xesEastMin, 'x') + 
    "(" + string(xesEastMax - xesEastMin, 'x') + ")";

  if (setsWest[specNumber].top1.value1 == 0)
  {
    assert(! setsWest[specNumber].symmFlag);
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
  else if (setsWest[specNumber].top1.value1 == oppsTops1)
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
      cout << "WW" << setsWest[specNumber].strLength(oppsLength) << 
        ", and " << setsWest[specNumber].strTop1(oppsTops1) << endl;
      assert(false);
    }
  }
  else if (setsWest[specNumber].top1.value1 == 1)
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
        assert(! setsWest[specNumber].symmFlag);
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
  else if (setsWest[specNumber].top1.value1 + 1 == oppsTops1)
  {
    assert(! setsWest[specNumber].symmFlag);
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
  if (setsWest[specNumber].mode == COVER_LENGTHS_ONLY)
    return setsWest[specNumber].strLength(oppsLength);
  else if (setsWest[specNumber].mode == COVER_TOPS_ONLY)
  {
    return setsWest[specNumber].strTop1(oppsTops1);
  }
  else if (setsWest[specNumber].mode == COVER_LENGTHS_AND_TOPS)
  {
    if (setsWest[specNumber].length.oper == COVER_EQUAL)
    {
      if (setsWest[specNumber].top1.oper == COVER_EQUAL)
        return setsWest[specNumber].strBothEqual(
          oppsLength, oppsTops1);
      else
      {
        return "ZZ " + setsWest[specNumber].strLength(oppsLength) + ", and " + 
          setsWest[specNumber].strTop1(oppsTops1);
      }
    }
    else
    {
      if (setsWest[specNumber].top1.oper == COVER_EQUAL)
        return CoverSpec::strTop1Fixed(specNumber);
      else
      {
      }
    }
    // At the moment only 1=5/2=4 with 1-2 West tops
    return "XX " + setsWest[specNumber].strLength(oppsLength) + ", and " + 
      setsWest[specNumber].strTop1(oppsTops1);
  }
  else
    return "";
}


string CoverSpec::str() const
{
  if (setsWest[1].mode == COVER_MODE_NONE)
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

