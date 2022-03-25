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


CoverSpec::CoverSpec()
{
  CoverSpec::reset();
}


void CoverSpec::reset()
{
  setsWest.clear();
  setsWest.emplace_back(CoverSet());
}


void CoverSpec::setID(
  const unsigned char length,
  const unsigned char tops1)
{
  sumProfile.length = length;
  sumProfile.tops.resize(2);
  sumProfile.tops[1] = tops1;
}


void CoverSpec::setIndex(const unsigned indexIn)
{
  index = indexIn;
}


void CoverSpec::getID(
  unsigned char& length,
  unsigned char& tops1) const
{
  length = sumProfile.length;
  tops1 = sumProfile.tops[1];
}


unsigned CoverSpec::getIndex() const
{
  return index;
}


CoverSet& CoverSpec::addOrExtend(const CoverControl ctrl)
{
  if (ctrl == COVER_EXTEND)
    setsWest.emplace_back(CoverSet());

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
  CoverSpec::westLength(sumProfile.length - len, ctrl);
}


void CoverSpec::westLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSet& cset = CoverSpec::addOrExtend(ctrl);

  Profile lowerProfile, upperProfile;
  lowerProfile.tops.resize(2);
  upperProfile.tops.resize(2);
  lowerProfile.length = len1;
  upperProfile.length = len2;
  lowerProfile.tops[1] = 0;
  upperProfile.tops[1] = sumProfile.tops[1];

  cset.set(sumProfile, lowerProfile, upperProfile);
}


void CoverSpec::eastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec::westLengthRange(
    sumProfile.length - len2, 
    sumProfile.length - len1, 
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
  CoverSpec::westTop1(sumProfile.tops[1] - tops, ctrl);
}


void CoverSpec::westTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  CoverSet& cset = CoverSpec::addOrExtend(ctrl);

  Profile lowerProfile, upperProfile;
  lowerProfile.tops.resize(2);
  upperProfile.tops.resize(2);
  lowerProfile.length = 0;
  upperProfile.length = sumProfile.length;
  lowerProfile.tops[1] = tops1;
  upperProfile.tops[1] = tops2;

  cset.set(sumProfile, lowerProfile, upperProfile);
}


void CoverSpec::eastTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  CoverSpec::westTop1Range(
    sumProfile.tops[1] - tops2, 
    sumProfile.tops[1] - tops1, 
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
  CoverSet& cset = CoverSpec::addOrExtend(ctrl);

  Profile lowerProfile, upperProfile;
  lowerProfile.tops.resize(2);
  upperProfile.tops.resize(2);
  lowerProfile.length = len1;
  upperProfile.length = len2;
  lowerProfile.tops[1] = tops1;
  upperProfile.tops[1] = tops2;

  cset.set(sumProfile, lowerProfile, upperProfile, symmFlag);
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
    sumProfile.length - len2,
    sumProfile.length - len1,
    sumProfile.tops[1] - tops2,
    sumProfile.tops[1] - tops1,
    symmFlag,
    ctrl);
}


bool CoverSpec::includes(const Profile& distProfile) const
{
  assert(distProfile.tops.size() == 1);
  assert(sumProfile.tops.size() == 2);

  Profile dist2;
  dist2.tops.resize(2);
  dist2.length = distProfile.length;
  dist2.tops[1] = distProfile.tops[0];

  for (auto& set: setsWest)
  {
    if (set.includes(dist2, sumProfile))
      return true;
  }

  return false;
}


string CoverSpec::str() const
{
  string s = setsWest.front().str(sumProfile);
  
  for (auto iter = next(setsWest.begin()); iter != setsWest.end(); iter++)
    s += "; or\n  " + iter->str(sumProfile);

  return s;
}

