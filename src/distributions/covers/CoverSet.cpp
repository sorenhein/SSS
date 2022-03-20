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


string CoverSet::strLength(
  const unsigned char oppsLength,
  const Opponent simplestOpponent) const
{
  return length.strLength(oppsLength, simplestOpponent, false);
}


string CoverSet::strTop1(
  const unsigned char oppsTops1,
  const Opponent simplestOpponent) const
{
  return top1.strTop(oppsTops1, simplestOpponent, false);
}


string CoverSet::strBothEqual(
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const Opponent simplestOpponent) const
{
  return top1.strWithLength(
    length,
    oppsLength,
    oppsTops1,
    simplestOpponent,
    symmFlag);
}


string CoverSet::strTop1Fixed(
  const unsigned char oppsLength,
  const unsigned char oppsTops1,
  const Opponent simplestOpponent) const
{
  return top1.strWithLength(
    length,
    oppsLength,
    oppsTops1,
    simplestOpponent,
    symmFlag);
}


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
        return CoverSet::strBothEqual(oppsLength, oppsTops1, simplestOpponent);
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

