/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverSet.h"
#include "Product.h"
#include "ProductProfile.h"


void CoverSet::reset()
{
  mode = COVER_MODE_NONE;
  symmFlag = false;
  length.reset();
  top1.reset();
}


void CoverSet::setSymm(const bool symmFlagIn)
{
  symmFlag = symmFlagIn;
}


void CoverSet::setMode(const CoverMode modeIn)
{
  mode = modeIn;
}


CoverMode CoverSet::getMode() const
{
  return mode;
}


void CoverSet::setLength(const unsigned char len)
{
  length.set(len, COVER_EQUAL);
}


void CoverSet::setLength(
  const unsigned char len1,
  const unsigned char len2)
{
  length.set(len1, len2, COVER_INSIDE_RANGE);
}


void CoverSet::setTop1(const unsigned char tops)
{
  top1.set(tops, COVER_EQUAL);
}


void CoverSet::setTop1(
  const unsigned char tops1,
  const unsigned char tops2)
{
  top1.set(tops1, tops2, COVER_INSIDE_RANGE);
}


bool CoverSet::includesLength(
  const unsigned char wlen,
  const unsigned char oppsLength) const
{
  if (symmFlag)
    return length.includes(wlen) || length.includes(oppsLength - wlen);
  else
    return length.includes(wlen);
}


bool CoverSet::includesTop1(
  const unsigned char wtop,
  const unsigned char oppsTops1) const
{
  if (symmFlag)
    return top1.includes(wtop) || length.includes(oppsTops1 - wtop);
  else
    return top1.includes(wtop);
}


bool CoverSet::includesLengthAndTop1(
  const unsigned char wlen,
  const unsigned char wtop,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  if (symmFlag)
  {
    return 
      (length.includes(wlen) && top1.includes(wtop)) ||
      (length.includes(oppsLength - wlen) && 
        top1.includes(oppsTops1 - wtop));
  }
  else
    return length.includes(wlen) && top1.includes(wtop);
}


bool CoverSet::includes(
  const ProductProfile& distProfile,
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  if (mode == COVER_MODE_NONE)
    return false;
  else if (mode == COVER_LENGTHS_ONLY)
    return CoverSet::includesLength(distProfile.length, oppsLength);
  else if (mode == COVER_TOPS_ONLY)
    return CoverSet::includesTop1(distProfile.tops[0], oppsTops1);
  else if (mode == COVER_LENGTHS_AND_TOPS)
    return CoverSet::includesLengthAndTop1(
      distProfile.length, distProfile.tops[0], oppsLength, oppsTops1);
  else
  {
    assert(false);
    return false;
  }
}


string CoverSet::strLengthEqual(const unsigned char oppsLength) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wlen = length.lower;

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



string CoverSet::strLengthInside(const unsigned char oppsLength) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wlen1 = length.lower;
  const unsigned char wlen2 = length.upper;

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


string CoverSet::strLength(const unsigned char oppsLength) const
{
  if (length.oper == COVER_EQUAL)
    return CoverSet::strLengthEqual(oppsLength);
  else if (length.oper == COVER_INSIDE_RANGE)
    return CoverSet::strLengthInside(oppsLength);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSet::strTop1Equal(const unsigned char oppsTops1) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wtop = top1.lower;

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


string CoverSet::strTop1Inside(const unsigned char oppsTops1) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");
  const unsigned char wtop1 = top1.lower;
  const unsigned char wtop2 = top1.upper;

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


string CoverSet::strTop1(const unsigned char oppsTops1) const
{
  if (top1.oper == COVER_EQUAL)
    return CoverSet::strTop1Equal(oppsTops1);
  else if (top1.oper == COVER_INSIDE_RANGE)
    return CoverSet::strTop1Inside(oppsTops1);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSet::strBothEqual0(
  const string& side) const
{
  if (length.lower == 0)
    return side + " is void";
  else
    return "East is void";
}


string CoverSet::strBothEqual1(
  const unsigned char oppsTops1,
  const string& side) const
{
  stringstream ss;

  if (length.lower == 1)
  {
    if (top1.lower == 0)
      ss << side << " has a small singleton";
    else
      ss <<  side << " has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }
  else
  {
    assert(! symmFlag);
    if (top1.lower == oppsTops1)
      ss << "East has a small singleton";
    else
      ss << "East has " << (oppsTops1 == 1 ? "the" : "a") << " " <<
        "singleton honor";
  }

  return ss.str();
}


string CoverSet::strBothEqual2(
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const string& side) const
{
  stringstream ss;

  if (length.lower == 2)
  {
    if (top1.lower == 0)
    {
      if (oppsLength == 4 && oppsTops1 == 2)
      {
        assert(! symmFlag);
        ss << "East has doubleton honors (HH)";
      }
      else
        ss << side << " has a small doubleton";
    }
    else if (top1.lower == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << side << " has doubleton honors (HH)";
  }
  else
  {
    assert(! symmFlag);
    if (top1.lower == oppsTops1)
      ss << "East has a small doubleton";
    else if (top1.lower + 1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor doubleton (Hx)";
    else
      ss << "East has doubleton honors (HH)";
  }

  return ss.str();
}


string CoverSet::strBothEqual3(
  [[maybe_unused]] const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const string& side) const
{
  stringstream ss;

  if (length.lower == 3)
  {
    if (top1.lower == 0)
      ss << side << " has a small tripleton";
    else if (top1.lower == 1)
      ss << side << " has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (top1.lower == 2)
      ss << side << " has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << side << " has tripleton honors (HHH)";
  }
  else
  {
    assert(! symmFlag);
    if (top1.lower == oppsTops1)
      ss << "East has a small tripleton";
    else if (top1.lower + 1 == oppsTops1)
      ss << "East has " << (oppsTops1 == 1 ? "the" : "an") << " " <<
        "honor tripleton (Hxx)";
    else if (top1.lower + 2 == oppsTops1)
      ss << "East has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "honors tripleton (HHx)";
    else
      ss << "East has tripleton honors (HHH)";
  }

  return ss.str();
}


string CoverSet::strBothEqual(
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  const string side = (symmFlag ? "Either opponent" : "West");

  if (length.lower == 0 || length.lower == oppsLength)
    return CoverSet::strBothEqual0(side);
  else if (length.lower == 1 || length.lower + 1 == oppsLength)
    return CoverSet::strBothEqual1(oppsTops1, side);
  else if (length.lower == 2 || length.lower + 2 == oppsLength)
    return CoverSet::strBothEqual2(oppsLength, oppsTops1, side);
  else if (length.lower == 3 || length.lower + 3 == oppsLength)
    return CoverSet::strBothEqual3(oppsLength, oppsTops1, side);
  else
  {
    assert(false);
    return "";
  }
}


void CoverSet::strXes(
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  CoverXes& coverXes) const
{
  coverXes.westMax = length.upper - top1.lower;
  coverXes.westMin = length.lower - top1.lower;

  coverXes.eastMax =
    (oppsLength - length.lower) - (oppsTops1 - top1.lower);
  coverXes.eastMin =
    (oppsLength - length.upper) - (oppsTops1 - top1.lower);

  coverXes.strWest = string(coverXes.westMin, 'x') +
    "(" + string(coverXes.westMax - coverXes.westMin, 'x') + ")";
  coverXes.strEast = string(coverXes.eastMin, 'x') +
    "(" + string(coverXes.eastMax - coverXes.eastMin, 'x') + ")";
}


string CoverSet::strTop1Fixed0(
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const string& side,
  const CoverXes& coverXes) const
{
  stringstream ss;

  if (top1.lower == 0)
  {
    assert(! symmFlag);
    if (oppsTops1 == 1)
    {
      if (coverXes.eastMax == 1)
        ss << "East has the top at most doubleton";
      else
        ss << "East has H" << coverXes.strEast;
    }
    else if (oppsTops1 == 2)
    {
      if (coverXes.eastMax == 1)
        ss << "East has both tops at most tripleton";
      else
        ss << "East has HH" << coverXes.strEast;
    }
    else
    {
      cout << CoverSet::strRaw() << endl;
      assert(false);
    }
  }
  else
  {
    if (oppsTops1 == 1)
    {
      if (coverXes.westMax == 1)
        ss << side << " has the top at most doubleton";
      else
        ss << side << " has H" << coverXes.strWest;
    }
    else if (oppsTops1 == 2)
    {
      if (coverXes.westMax == 1)
        ss << side << " has both tops at most tripleton";
      else
        ss << side << " has HH" << coverXes.strWest;
    }
    else if (oppsTops1 == 3)
      ss << side << " has HHH" << coverXes.strWest;
    else
    {
      cout << coverXes.str();
      cout << CoverSet::strLength(oppsLength) << ", and " <<
        CoverSet::strTop1(oppsTops1) << endl;
      assert(false);
    }
  }
  
  return ss.str();
}


string CoverSet::strTop1Fixed1(
  const unsigned char oppsTops1,
  const string& side,
  const CoverXes& coverXes) const
{
  stringstream ss;

  if (top1.lower == 1)
  {
    if (oppsTops1 == 2)
    {
      // Look at it from the shorter side
      if (coverXes.westMax <= coverXes.eastMax)
      {
        if (coverXes.westMax == 1)
          ss << side << " has one top at most doubleton";
        else
          ss << side << " has H" << coverXes.strWest;
      }
      else
      {
        assert(! symmFlag);
        if (coverXes.eastMax == 1)
          ss << "East has one top at most doubleton";
        else
          ss << "East has H" << coverXes.strEast;
      }
    }
    else
    {
      if (coverXes.westMax == 1)
        ss << side << " has one top at most doubleton";
      else
        ss << side << " has H" << coverXes.strWest;
    }
  }
  else
  {
    assert(! symmFlag);
    if (coverXes.eastMax == 1)
      ss << "East has one top at most doubleton";
    else
      ss << "East has H" << coverXes.strEast;
  }

  return ss.str();
}


string CoverSet::strTop1Fixed(
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  CoverXes coverXes;
  CoverSet::strXes(oppsLength, oppsTops1, coverXes);

  if (top1.lower == 0 ||top1.lower == oppsTops1)
    return CoverSet::strTop1Fixed0(oppsLength, oppsTops1, side, coverXes);
  else if (top1.lower == 1 ||top1.lower + 1 == oppsTops1)
    return CoverSet::strTop1Fixed1(oppsTops1, side, coverXes);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSet::strRaw() const
{
  stringstream ss;

  ss << "mode ";
  if (mode == COVER_MODE_NONE)
    ss << "NONE";
  else if (mode == COVER_LENGTHS_ONLY)
    ss << "LENGTHS";
  else if (mode == COVER_TOPS_ONLY)
    ss << "TOPS";
  else
    ss << "UNKNOWN";
  ss << "\n";

  ss << "symm " << (symmFlag ? "yes" : "no") << "\n";
  ss << "length " << length.strRaw();
  ss << "top1   " << top1.strRaw();
  
  return ss.str();
}


string CoverSet::str(
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  if (mode == COVER_LENGTHS_ONLY)
    return CoverSet::strLength(oppsLength);
  else if (mode == COVER_TOPS_ONLY)
    return CoverSet::strTop1(oppsTops1);
  else if (mode == COVER_LENGTHS_AND_TOPS)
  {
    if (length.oper == COVER_EQUAL)
    {
      if (top1.oper == COVER_EQUAL)
        return CoverSet::strBothEqual(oppsLength, oppsTops1);
      else
        return CoverSet::strLength(oppsLength) + ", and " +
          CoverSet::strTop1(oppsTops1);
    }
    else
    {
      if (top1.oper == COVER_EQUAL)
        return CoverSet::strTop1Fixed(oppsLength, oppsTops1);
      else
        return CoverSet::strLength(oppsLength) + ", and " +
          CoverSet::strTop1(oppsTops1);
    }
  }

  assert(false);
  return "";
}

