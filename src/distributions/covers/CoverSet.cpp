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


/*
void CoverSet::setLength(
 const unsigned char len,
 const unsigned char oppsLength)
{
  length.set(oppsLength, len, len);
}
*/


void CoverSet::setLength(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char oppsLength)
{
  length.set(oppsLength, len1, len2);
}


/*
void CoverSet::setTop1(
 const unsigned char tops,
 const unsigned char oppsSize)
{
  top1.set(oppsSize, tops, tops);
}
*/


void CoverSet::setTop1(
  const unsigned char tops1,
  const unsigned char tops2,
  const unsigned char oppsSize)
{
  top1.set(oppsSize, tops1, tops2);
}


bool CoverSet::includesLength(
  const ProductProfile& distProfile,
  const ProductProfile& sumProfile) const
{
  if (symmFlag)
    return length.includes(distProfile.length) || length.includes(sumProfile.length - distProfile.length);
  else
    return length.includes(distProfile.length);
}


bool CoverSet::includesTop1(
  const ProductProfile& distProfile,
  const ProductProfile& sumProfile) const
{
  if (symmFlag)
    return top1.includes(distProfile.tops[0]) || length.includes(sumProfile.tops[0] - distProfile.tops[0]);
  else
    return top1.includes(distProfile.tops[0]);
}


bool CoverSet::includesLengthAndTop1(
  const ProductProfile& distProfile,
  const ProductProfile& sumProfile) const
{
  if (symmFlag)
  {
    return 
      (length.includes(distProfile.length) && top1.includes(distProfile.tops[0])) ||
      (length.includes(sumProfile.length - distProfile.length) && 
        top1.includes(sumProfile.tops[0] - distProfile.tops[0]));
  }
  else
    return length.includes(distProfile.length) && top1.includes(distProfile.tops[0]);
}


bool CoverSet::includes(
  const ProductProfile& distProfile,
  const ProductProfile& sumProfile) const
{
  if (mode == COVER_MODE_NONE)
    return false;
  else if (mode == COVER_LENGTHS_ONLY)
    return CoverSet::includesLength(
      distProfile, sumProfile);
  else if (mode == COVER_TOPS_ONLY)
    return CoverSet::includesTop1(
      distProfile, sumProfile);
  else if (mode == COVER_LENGTHS_AND_TOPS)
    return CoverSet::includesLengthAndTop1(
      distProfile, sumProfile);
  else
  {
    assert(false);
    return false;
  }
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

  Opponent simplestOpponent;
  if (sumProfile.flip(lowerProfile, upperProfile))
  {
    simplestOpponent = OPP_EAST;
  }
  else
  {
    simplestOpponent = OPP_WEST;
  }

  if (mode == COVER_LENGTHS_ONLY)
  {
    return length.strLength(oppsLength, simplestOpponent, symmFlag);
  }
  else if (mode == COVER_TOPS_ONLY)
  {
    return top1.strTop(oppsTops1, simplestOpponent, symmFlag);
  }
  else if (mode == COVER_LENGTHS_AND_TOPS)
  {
    if (length.oper == COVER_EQUAL)
    {
      if (top1.oper == COVER_EQUAL)
      {
        return top1.strWithLength(
          length,
          oppsLength,
          oppsTops1,
          simplestOpponent,
          symmFlag);
      }
      else
      {
        return 
          length.strLength(oppsLength, simplestOpponent, symmFlag) +
          ", and " +
          top1.strTop(oppsTops1, simplestOpponent, symmFlag);
      }
    }
    else
    {
      if (top1.oper == COVER_EQUAL)
      {
        return top1.strWithLength(
          length,
          oppsLength,
          oppsTops1,
          simplestOpponent,
          symmFlag);
      }
      else
      {
        return 
          length.strLength(oppsLength, simplestOpponent, symmFlag) +
          ", and " +
          top1.strTop(oppsTops1, simplestOpponent, symmFlag);
      }
    }
  }

  assert(false);
  return "";
}

