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


void CoverSet::setLength(
 const unsigned char len,
 const unsigned char oppsLength)
{
  length.set(oppsLength, len, len);
}


void CoverSet::setLength(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char oppsLength)
{
  length.set(oppsLength, len1, len2);
}


void CoverSet::setTop1(
 const unsigned char tops,
 const unsigned char oppsSize)
{
  top1.set(oppsSize, tops, tops);
}


void CoverSet::setTop1(
  const unsigned char tops1,
  const unsigned char tops2,
  const unsigned char oppsSize)
{
  top1.set(oppsSize, tops1, tops2);
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


string CoverSet::strLengthEqual(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  // Here lower and upper are one and the same.

  string side;
  unsigned char value;

  if (simplestOpponent == OPP_WEST)
  {
    side = "West";
    value = length.lower;
  }
  else
  {
    side = "East";
    value = oppsLength - length.lower;
  }

  stringstream ss;

  if (value == 0)
    ss << side << " is void";
  else if (value == 1)
    ss << side << " has a singleton";
  else if (value == 2)
  {
    if (oppsLength == 4)
      ss << "The suit splits 2=2";
    else
      ss << side << " has a doubleton";
  }
  else
    ss << "The suit splits " << +length.lower << "=" << 
      +(oppsLength - length.lower);

  return ss.str();

}



string CoverSet::strLengthInside(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  string side;
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    side = "West";
    vLower = length.lower;
    vUpper = length.upper;
  }
  else
  {
    side = "East";
    vLower = oppsLength - length.upper;
    vUpper = oppsLength - length.lower;
  }

  stringstream ss;

  if (vLower == 0)
  {
    if (vUpper == 1)
      ss << side << " has at most a singleton";
    else if (vUpper == 2)
      ss << side << " has at most a doubleton";
    else
      ss << side << " has at most " << +vUpper << " cards";
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    ss << "Neither opponent is void";
  }
  else if (length.lower + length.upper == oppsLength && 
      length.lower + 1 == length.upper)
  {
    ss << "The suit splits " << +length.lower << "-" << +length.upper <<
      " either way";
  }
  else
  {
    ss << "The suit splits between " <<
      +length.lower << "=" << +(oppsLength - length.lower) << " and " <<
      +length.upper << "=" << +(oppsLength - length.upper);
  }

  return ss.str();
}


string CoverSet::strLength(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  if (length.oper == COVER_EQUAL)
    return CoverSet::strLengthEqual(oppsLength, simplestOpponent);
  else if (length.oper == COVER_INSIDE_RANGE ||
      length.oper == COVER_GREATER_EQUAL ||
      length.oper == COVER_LESS_EQUAL)
    return CoverSet::strLengthInside(oppsLength, simplestOpponent);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSet::strTop1Equal(
  const unsigned char oppsTops1,
  const Opponent simplestOpponent) const
{
assert(! symmFlag);

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
    value = top1.lower;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    otherSide = (symmFlag ? "Either opponent" : "West");
    value = oppsTops1 - top1.lower;
  }

  stringstream ss;

  if (value == 0 || value == oppsTops1)
  {
    const string longSide = (value == 0 ? otherSide : side);

    if (oppsTops1 == 1)
      ss << longSide << " has the top";
    else if (oppsTops1 == 2)
      ss << longSide << " has both tops";
    else
      ss << longSide << " has all tops";
  }
  else if (value == 1)
  {
    ss << side << " has exactly one top";
  }
  else if (value+1 == oppsTops1)
  {
    ss << otherSide << " has exactly one top";
  }
  else if (value == 2)
  {
    ss << side << " has exactly two tops";
  }
  else if (value+2 == oppsTops1)
  {
    ss << otherSide << " has exactly two tops";
  }
  else
    ss << side << " has exactly " +value << " tops";

  return ss.str();
}


string CoverSet::strTop1Inside(
  const unsigned char oppsTops1,
  const Opponent simplestOpponent) const
{
// assert(! symmFlag);

  string side;
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? "Either opponent" : "West");
    vLower = top1.lower;
    vUpper = top1.upper;
  }
  else
  {
    side = (symmFlag ? "Either opponent" : "East");
    vLower = oppsTops1 - top1.upper;
    vUpper = oppsTops1 - top1.lower;
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
  else if (vUpper == oppsTops1)
  {
    if (vLower+1 == oppsTops1)
      ss << side << " lacks at most one top";
    else if (vLower+2 == oppsTops1)
      ss << side << " lacks at most two tops";
    else
      ss << side << " lacks at most " << +(oppsTops1 - vLower) << " tops";
  }
  else
  {
    ss << side << " has between " << +vLower << " and " <<
      +vUpper << " tops";
  }

  return ss.str();


  /*
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
  */
}


string CoverSet::strTop1(
  const unsigned char oppsTops1,
  const Opponent simplestOpponent) const
{
  if (top1.oper == COVER_EQUAL)
    return CoverSet::strTop1Equal(oppsTops1, simplestOpponent);
  else if (top1.oper == COVER_INSIDE_RANGE ||
      top1.oper == COVER_GREATER_EQUAL ||
      top1.oper == COVER_LESS_EQUAL)
    return CoverSet::strTop1Inside(oppsTops1, simplestOpponent);
  else
  {
    assert(false);
    return "";
  }
}


string CoverSet::strBothEqual0(
  const string& side) const
{
assert(false);
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
        "doubleton honor (Hx)";
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
        "doubleton honor (Hx)";
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
        "tripleton honor (Hxx)";
    else if (top1.lower == 2)
      ss << side << " has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "tripleton honors (HHx)";
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
        "tripleton honor (Hxx)";
    else if (top1.lower + 2 == oppsTops1)
      ss << "East has " << (oppsTops1 == 2 ? "the" : "two") << " " <<
        "tripleton honors (HHx)";
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
  const CoverXes& coverXes,
  const Opponent simplestOpponent) const
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
      cout << CoverSet::str(oppsLength, oppsTops1) << endl;
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
      cout << CoverSet::strLength(oppsLength, simplestOpponent) << ", and " <<
        CoverSet::strTop1(oppsTops1, simplestOpponent) << endl;
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
  const unsigned char oppsTops1,
  const Opponent simplestOpponent) const
{
  stringstream ss;
  const string side = (symmFlag ? "Either opponent" : "West");

  CoverXes coverXes;
  CoverSet::strXes(oppsLength, oppsTops1, coverXes);

  if (top1.lower == 0 ||top1.lower == oppsTops1)
    return CoverSet::strTop1Fixed0(oppsLength, oppsTops1, side, coverXes, simplestOpponent);
  else if (top1.lower == 1 ||top1.lower + 1 == oppsTops1)
    return CoverSet::strTop1Fixed1(oppsTops1, side, coverXes);
  else
  {
    assert(false);
    return "";
  }
}


/*
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
  ss << "length " << length.strGeneral();
  ss << "top1   " << top1.strGeneral();
  
  return ss.str();
}
*/


string CoverSet::str(
  const unsigned char oppsLength,
  const unsigned char oppsTops1) const
{
  // Figure out simplestOpponent analogously to ProductProfile.

  ProductProfile sumProfile, lowerProfile, upperProfile;
  sumProfile.tops.resize(1);
  lowerProfile.tops.resize(1);
  upperProfile.tops.resize(1);

  sumProfile.length = oppsLength;
  sumProfile.tops[0] = oppsTops1;

  if (length.used())
  {
    lowerProfile.length = length.lower;
    upperProfile.length = length.upper;
  }
  else
  {
    lowerProfile.length = 0;
    upperProfile.length = oppsLength;
  }

  if (top1.used())
  {
    lowerProfile.tops[0] = top1.lower;
    upperProfile.tops[0] = top1.upper;
  }
  else
  {
    lowerProfile.tops[0] = 0;
    upperProfile.tops[0] = oppsTops1;
  }

// cout << "Sum   " << sumProfile.str() << "\n";
// cout << "Lower " << lowerProfile.str() << "\n";
// cout << "Upper " << upperProfile.str() << endl;

  Opponent simplestOpponent;
  if (sumProfile.flip(lowerProfile, upperProfile))
  {
// cout << "FLIP\n";
    simplestOpponent = OPP_EAST;
  }
  else
  {
// cout << "DON'T\n";
    simplestOpponent = OPP_WEST;
  }

  if (mode == COVER_LENGTHS_ONLY)
  {
    return CoverSet::strLength(oppsLength, simplestOpponent);
  }
  else if (mode == COVER_TOPS_ONLY)
    return CoverSet::strTop1(oppsTops1, simplestOpponent);
  else if (mode == COVER_LENGTHS_AND_TOPS)
  {
    if (length.oper == COVER_EQUAL)
    {
      if (top1.oper == COVER_EQUAL)
        return CoverSet::strBothEqual(oppsLength, oppsTops1);
      else
        return CoverSet::strLength(oppsLength, simplestOpponent) + ", and " +
          CoverSet::strTop1(oppsTops1, simplestOpponent);
    }
    else
    {
      if (top1.oper == COVER_EQUAL)
        return CoverSet::strTop1Fixed(oppsLength, oppsTops1, simplestOpponent);
      else
        return CoverSet::strLength(oppsLength, simplestOpponent) + ", and " +
          CoverSet::strTop1(oppsTops1, simplestOpponent);
    }
  }

  assert(false);
  return "";
}

