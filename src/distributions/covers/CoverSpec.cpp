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
#include "Cover.h"
#include "Profile.h"
#include "ProfilePair.h"


CoverSpec::CoverSpec()
{
  CoverSpec::reset();
}


void CoverSpec::reset()
{
  setsWest.clear();
  setsWest.emplace_back(Cover());
}


void CoverSpec::setID(
  const unsigned char numTops,
  const unsigned char length,
  const unsigned char tops1)
{
  sumProfile.setSingle(numTops, length, tops1);
}


void CoverSpec::setIndex(const unsigned indexIn)
{
  index = indexIn;
}


void CoverSpec::getID(
  unsigned char& length,
  unsigned char& tops1) const
{
  const unsigned char highestTop =
    static_cast<unsigned char>(sumProfile.size()-1);

  length = sumProfile.getLength();
  tops1 = sumProfile.count(highestTop);
}


unsigned CoverSpec::getIndex() const
{
  return index;
}


Cover& CoverSpec::addOrExtend(const CoverControl ctrl)
{
  if (ctrl == COVER_EXTEND)
    setsWest.emplace_back(Cover());

  return setsWest.back();
}


void CoverSpec::westLength(
  ProductMemory& productMemory,
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec::westLengthRange(productMemory, len, len, ctrl);
}


void CoverSpec::eastLength(
  ProductMemory& productMemory,
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec::westLength(productMemory, sumProfile.getLength() - len, ctrl);
}


void CoverSpec::westLengthRange(
  ProductMemory& productMemory,
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  Cover& cset = CoverSpec::addOrExtend(ctrl);

  const unsigned char highestTop =
    static_cast<unsigned char>(sumProfile.size()-1);

  ProfilePair profilePair(sumProfile);
  profilePair.setLength(len1, len2);
  profilePair.addTop(highestTop, 0, sumProfile.count(highestTop));

  cset.set(productMemory, sumProfile, profilePair, false);
}


void CoverSpec::eastLengthRange(
  ProductMemory& productMemory,
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec::westLengthRange(
    productMemory,
    sumProfile.getLength() - len2, 
    sumProfile.getLength() - len1, 
    ctrl);
}


void CoverSpec::westTop1(
  ProductMemory& productMemory,
  const unsigned char tops,
  const CoverControl ctrl)
{
  CoverSpec::westTop1Range(productMemory, tops, tops, ctrl);
}


void CoverSpec::eastTop1(
  ProductMemory& productMemory,
  const unsigned char tops,
  const CoverControl ctrl)
{
  const unsigned char highestTop =
    static_cast<unsigned char>(sumProfile.size()-1);

  CoverSpec::westTop1(
    productMemory, 
    sumProfile.count(highestTop) - tops, 
    ctrl);
}


void CoverSpec::westTop1Range(
  ProductMemory& productMemory,
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  Cover& cset = CoverSpec::addOrExtend(ctrl);

  const unsigned char highestTop =
    static_cast<unsigned char>(sumProfile.size()-1);

  ProfilePair profilePair(sumProfile);
  profilePair.setLength(0, sumProfile.getLength());
  profilePair.addTop(highestTop, tops1, tops2);

  cset.set(productMemory, sumProfile, profilePair, false);
}


void CoverSpec::eastTop1Range(
  ProductMemory& productMemory,
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  const unsigned char highestTop =
    static_cast<unsigned char>(sumProfile.size()-1);

  CoverSpec::westTop1Range(
    productMemory,
    sumProfile.count(highestTop) - tops2, 
    sumProfile.count(highestTop) - tops1, 
    ctrl);
}


void CoverSpec::westGeneral(
  ProductMemory& productMemory,
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char tops1,
  const unsigned char tops2,
  const bool symmFlag,
  const CoverControl ctrl)
{
  Cover& cset = CoverSpec::addOrExtend(ctrl);

  const unsigned char highestTop =
    static_cast<unsigned char>(sumProfile.size()-1);

  ProfilePair profilePair(sumProfile);
  profilePair.setLength(len1, len2);
  profilePair.addTop(highestTop, tops1, tops2);

  cset.set(productMemory, sumProfile, profilePair, symmFlag);
}


void CoverSpec::eastGeneral(
  ProductMemory& productMemory,
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char tops1,
  const unsigned char tops2,
  const bool symmFlag,
  const CoverControl ctrl)
{
  const unsigned char highestTop =
    static_cast<unsigned char>(sumProfile.size()-1);

  CoverSpec::westGeneral(
    productMemory,
    sumProfile.getLength() - len2,
    sumProfile.getLength() - len1,
    sumProfile.count(highestTop) - tops2,
    sumProfile.count(highestTop) - tops1,
    symmFlag,
    ctrl);
}

