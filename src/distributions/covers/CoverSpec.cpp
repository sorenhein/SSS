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
#include "Profile.h"
#include "ProfilePair.h"


CoverSpec::CoverSpec()
{
  CoverSpec::reset();
}


void CoverSpec::reset()
{
  setsWest.clear();
  setsWest.emplace_back(ProductPlus());
}


void CoverSpec::setID(
  const unsigned char length,
  const unsigned char tops1)
{
  sumProfile.setSingle(length, tops1);
}


void CoverSpec::setIndex(const unsigned indexIn)
{
  index = indexIn;
}


void CoverSpec::getID(
  unsigned char& length,
  unsigned char& tops1) const
{
  length = sumProfile.getLength();
  tops1 = sumProfile.count(1);
}


unsigned CoverSpec::getIndex() const
{
  return index;
}


ProductPlus& CoverSpec::addOrExtend(const CoverControl ctrl)
{
  if (ctrl == COVER_EXTEND)
    setsWest.emplace_back(ProductPlus());

  return setsWest.back();
}


void CoverSpec::westLength(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec::westLengthRange(len, len, ctrl);
}


void CoverSpec::eastLength(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec::westLength(sumProfile.getLength() - len, ctrl);
}


void CoverSpec::westLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  ProductPlus& cset = CoverSpec::addOrExtend(ctrl);

  ProfilePair profilePair(sumProfile);
  profilePair.setLength(len1, len2);
  profilePair.addTop(1, 0, sumProfile.count(1));

  cset.set(sumProfile, profilePair, false);
}


void CoverSpec::eastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec::westLengthRange(
    sumProfile.getLength() - len2, 
    sumProfile.getLength() - len1, 
    ctrl);
}


void CoverSpec::westTop1(
  const unsigned char tops,
  const CoverControl ctrl)
{
  CoverSpec::westTop1Range(tops, tops, ctrl);
}


void CoverSpec::eastTop1(
  const unsigned char tops,
  const CoverControl ctrl)
{
  CoverSpec::westTop1(sumProfile.count(1) - tops, ctrl);
}


void CoverSpec::westTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  ProductPlus& cset = CoverSpec::addOrExtend(ctrl);

  ProfilePair profilePair(sumProfile);
  profilePair.setLength(0, sumProfile.getLength());
  profilePair.addTop(1, tops1, tops2);

  cset.set(sumProfile, profilePair, false);
}


void CoverSpec::eastTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  CoverSpec::westTop1Range(
    sumProfile.count(1) - tops2, 
    sumProfile.count(1) - tops1, 
    ctrl);
}


void CoverSpec::westGeneral(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char tops1,
  const unsigned char tops2,
  const bool symmFlag,
  const CoverControl ctrl)
{
  ProductPlus& cset = CoverSpec::addOrExtend(ctrl);

  ProfilePair profilePair(sumProfile);
  profilePair.setLength(len1, len2);
  profilePair.addTop(1, tops1, tops2);

  cset.set(sumProfile, profilePair, symmFlag);
}


void CoverSpec::eastGeneral(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char tops1,
  const unsigned char tops2,
  const bool symmFlag,
  const CoverControl ctrl)
{
  CoverSpec::westGeneral(
    sumProfile.getLength() - len2,
    sumProfile.getLength() - len1,
    sumProfile.count(1) - tops2,
    sumProfile.count(1) - tops1,
    symmFlag,
    ctrl);
}


bool CoverSpec::includes(const Profile& distProfile) const
{
  assert(distProfile.size() == 2);
  assert(sumProfile.size() == 2);

  for (auto& set: setsWest)
  {
    if (set.product.includes(distProfile))
      return true;
    else if (! set.symmFlag)
      continue;
    else
    {
      Profile mirror = distProfile;
      mirror.mirrorAround(sumProfile);
      
      if (set.product.includes(mirror))
        return true;
    }
  }

  return false;
}


string CoverSpec::str() const
{
  string s = setsWest.front().strVerbal(sumProfile);
  
  for (auto iter = next(setsWest.begin()); iter != setsWest.end(); iter++)
    s += "; or\n  " + iter->strVerbal(sumProfile);

  return s;
}

