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


void CoverSet::set(
  const ProductProfile& sumProfile,
  const ProductProfile& lowerProfile,
  const ProductProfile& upperProfile)
{
  length.set(sumProfile.length, lowerProfile.length, upperProfile.length);

  assert(lowerProfile.tops.size() == 1);
  assert(upperProfile.tops.size() == 1);

  top1.set(sumProfile.tops[0], lowerProfile.tops[0], upperProfile.tops[0]);
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
    return CoverSet::includesLength(distProfile, sumProfile);
  else if (mode == COVER_TOPS_ONLY)
    return CoverSet::includesTop1(distProfile, sumProfile);
  else if (mode == COVER_LENGTHS_AND_TOPS)
    return CoverSet::includesLengthAndTop1(distProfile, sumProfile);
  else
  {
    assert(false);
    return false;
  }
}


string CoverSet::str(const ProductProfile& sumProfile) const
{
  // Figure out simplestOpponent analogously to ProductProfile.

  ProductProfile lowerProfile, upperProfile;
  lowerProfile.tops.resize(1);
  upperProfile.tops.resize(1);

  if (length.used())
  {
    lowerProfile.length = length.lower;
    upperProfile.length = length.upper;
  }
  else
  {
    lowerProfile.length = 0;
    upperProfile.length = sumProfile.length;
  }

  if (top1.used())
  {
    lowerProfile.tops[0] = top1.lower;
    upperProfile.tops[0] = top1.upper;
  }
  else
  {
    lowerProfile.tops[0] = 0;
    upperProfile.tops[0] = sumProfile.tops[0];
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
    return length.strLength(sumProfile.length, simplestOpponent, symmFlag);
  }
  else if (mode == COVER_TOPS_ONLY)
  {
    return top1.strTop(sumProfile.tops[0], simplestOpponent, symmFlag);
  }
  else if (mode == COVER_LENGTHS_AND_TOPS)
  {
    if (length.oper == COVER_EQUAL)
    {
      if (top1.oper == COVER_EQUAL)
      {
        return top1.strWithLength(
          length,
          sumProfile.length,
          sumProfile.tops[0],
          simplestOpponent,
          symmFlag);
      }
      else
      {
        return 
          length.strLength(sumProfile.length, simplestOpponent, symmFlag) +
          ", and " +
          top1.strTop(sumProfile.tops[0], simplestOpponent, symmFlag);
      }
    }
    else
    {
      if (top1.oper == COVER_EQUAL)
      {
        return top1.strWithLength(
          length,
          sumProfile.length,
          sumProfile.tops[0],
          simplestOpponent,
          symmFlag);
      }
      else
      {
        return 
          length.strLength(sumProfile.length, simplestOpponent, symmFlag) +
          ", and " +
          top1.strTop(sumProfile.tops[0], simplestOpponent, symmFlag);
      }
    }
  }

  assert(false);
  return "";
}

