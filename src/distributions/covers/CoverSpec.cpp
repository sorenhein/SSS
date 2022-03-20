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
  setsWest.clear();
  setsWest.emplace_back(CoverSet());
}


void CoverSpec::setID(
  const unsigned char length,
  const unsigned char tops1)
{
  oppsLength = length;
  oppsTops1 = tops1;
}


void CoverSpec::setIndex(const unsigned indexIn)
{
  index = indexIn;
}


void CoverSpec::getID(
  unsigned char& length,
  unsigned char& tops1) const
{
  length = oppsLength;
  tops1 = oppsTops1;
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
  CoverSet& cset = CoverSpec::addOrExtend(ctrl);
  cset.setMode(COVER_LENGTHS_ONLY);
  cset.setLength(len);
}


void CoverSpec::eastLength(
  const unsigned char len,
  const CoverControl ctrl)
{
  CoverSpec::westLength(oppsLength - len, ctrl);
}


void CoverSpec::westLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSet& cset = CoverSpec::addOrExtend(ctrl);
  cset.setMode(COVER_LENGTHS_ONLY);
  cset.setLength(len1, len2);
}


void CoverSpec::eastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const CoverControl ctrl)
{
  CoverSpec::westLengthRange(
    oppsLength - len2, 
    oppsLength - len1, 
    ctrl);
}


void CoverSpec::westTop1(
  const unsigned char tops,
  const CoverControl ctrl)
{
  CoverSet& cset = CoverSpec::addOrExtend(ctrl);
  cset.setMode(COVER_TOPS_ONLY);
  cset.setTop1(tops);
}


void CoverSpec::eastTop1(
  const unsigned char tops,
  const CoverControl ctrl)
{
  CoverSpec::westTop1(oppsTops1 - tops, ctrl);
}


void CoverSpec::westTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  CoverSet& cset = CoverSpec::addOrExtend(ctrl);
  cset.setMode(COVER_TOPS_ONLY);
  cset.setTop1(tops1, tops2);
}


void CoverSpec::eastTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const CoverControl ctrl)
{
  CoverSpec::westTop1Range(
    oppsTops1 - tops2, 
    oppsTops1 - tops1, 
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
  cset.setMode(COVER_LENGTHS_AND_TOPS);
  cset.setSymm(symmFlag);

  if (len1 == len2)
    cset.setLength(len1);
  else
    cset.setLength(len1, len2);

  if (tops1 == tops2)
    cset.setTop1(tops1);
  else
    cset.setTop1(tops1, tops2);
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
    oppsLength - len2,
    oppsLength - len1,
    oppsTops1 - tops2,
    oppsTops1 - tops1,
    symmFlag,
    ctrl);
}


bool CoverSpec::includes(const ProductProfile& distProfile) const
{
  for (auto& set: setsWest)
  {
    if (set.includes(distProfile, oppsLength, oppsTops1))
      return true;
  }

  return false;
}


string CoverSpec::strRaw() const
{
  stringstream ss;

  ss << "index " << index << ", " <<
    "ID " << +oppsLength << "-" << +oppsTops1 << "\n";

  for (auto& set: setsWest)
    ss << set.strRaw();

  return ss.str();
}


string CoverSpec::str() const
{
  string s = setsWest.front().str(oppsLength, oppsTops1);
  
  for (auto iter = next(setsWest.begin()); iter != setsWest.end(); iter++)
    s += "; or\n  " + iter->str(oppsLength, oppsTops1);

  return s;
}

