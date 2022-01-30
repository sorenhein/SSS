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
  for (auto& set: setsWest)
    set.reset();
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


void CoverSpec::westLength(
  const unsigned char len,
  const unsigned specNumber)
{
  setsWest[specNumber].setMode(COVER_LENGTHS_ONLY);
  setsWest[specNumber].setLength(len);

  // setsWest[specNumber].mode = COVER_LENGTHS_ONLY;
  // setsWest[specNumber].length.set(len, COVER_EQUAL);
}


void CoverSpec::eastLength(
  const unsigned char len,
  const unsigned specNumber)
{
  CoverSpec::westLength(oppsLength - len, specNumber);
}


void CoverSpec::westLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber)
{
  // setsWest[specNumber].mode = COVER_LENGTHS_ONLY;
  // setsWest[specNumber].length.set(len1, len2, COVER_INSIDE_RANGE);

  setsWest[specNumber].setMode(COVER_LENGTHS_ONLY);
  setsWest[specNumber].setLength(len1, len2);
}


void CoverSpec::eastLengthRange(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned specNumber)
{
  CoverSpec::westLengthRange(
    oppsLength - len2, 
    oppsLength - len1, 
    specNumber);
}


void CoverSpec::westTop1(
  const unsigned char tops,
  const unsigned specNumber)
{
  // setsWest[specNumber].mode = COVER_TOPS_ONLY;
  // setsWest[specNumber].top1.set(tops, COVER_EQUAL);

  setsWest[specNumber].setMode(COVER_TOPS_ONLY);
  setsWest[specNumber].setTop1(tops);
}


void CoverSpec::eastTop1(
  const unsigned char tops,
  const unsigned specNumber)
{
  CoverSpec::westTop1(oppsTops1 - tops, specNumber);
}


void CoverSpec::westTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const unsigned specNumber)
{
  // setsWest[specNumber].mode = COVER_TOPS_ONLY;
  // setsWest[specNumber].top1.set(tops1, tops2, COVER_INSIDE_RANGE);

  setsWest[specNumber].setMode(COVER_TOPS_ONLY);
  setsWest[specNumber].setTop1(tops1, tops2);
}


void CoverSpec::eastTop1Range(
  const unsigned char tops1,
  const unsigned char tops2,
  const unsigned specNumber)
{
  CoverSpec::westTop1Range(
    oppsTops1 - tops2, 
    oppsTops1 - tops1, 
    specNumber);
}


void CoverSpec::westGeneral(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char tops1,
  const unsigned char tops2,
  const bool symmFlag,
  const unsigned specNumber)
{
  if (len1 == len2)
    CoverSpec::westLength(len1, specNumber);
  else
    CoverSpec::westLengthRange(len1, len2, specNumber);

  if (tops1 == tops2)
    CoverSpec::westTop1(tops1, specNumber);
  else
    CoverSpec::westTop1Range(tops1, tops2, specNumber);

  /*
  setsWest[specNumber].mode = COVER_LENGTHS_AND_TOPS;

  if (len1 == len2)
    setsWest[specNumber].length.set(len1, COVER_EQUAL);
  else
    setsWest[specNumber].length.set(len1, len2, COVER_INSIDE_RANGE);

  if (tops1 == tops2)
    setsWest[specNumber].top1.set(tops1, COVER_EQUAL);
  else
    setsWest[specNumber].top1.set(tops1, tops2, COVER_INSIDE_RANGE);
    */

  setsWest[specNumber].setMode(COVER_LENGTHS_AND_TOPS);
  setsWest[specNumber].setSymm(symmFlag);
}


void CoverSpec::eastGeneral(
  const unsigned char len1,
  const unsigned char len2,
  const unsigned char tops1,
  const unsigned char tops2,
  const bool symmFlag,
  const unsigned specNumber)
{
  CoverSpec::westGeneral(
    oppsLength - len2,
    oppsLength - len1,
    oppsTops1 - tops2,
    oppsTops1 - tops1,
    symmFlag,
    specNumber);
}


bool CoverSpec::includes(
  const unsigned char wlen,
  const unsigned char wtop) const
{
  for (auto& set: setsWest)
  {
    if (set.includes(wlen, wtop, oppsLength, oppsTops1))
      return true;
  }

  return false;
}


string CoverSpec::strRaw() const
{
  stringstream ss;

  ss << "index " << index << ", " <<
    "ID " << +oppsLength << "-" << +oppsTops1 << "\n";
  ss << setsWest[0].strRaw();
  ss << setsWest[1].strRaw();

  return ss.str();
}


string CoverSpec::str() const
{
  if (setsWest[1].getMode() == COVER_MODE_NONE)
    return setsWest[0].str(oppsLength, oppsTops1);
  else
  {
    // At the moment
    // 57 YYThe suit splits 2-2, or East has the tops
    // 15 YYThe suit splits 3=3, or West has the tops
    // 24 YYWest has at least 2 cards, or East has the tops
    // 12 YYWest has at most 3 cards, or West has the tops

    return "YY " + setsWest[0].str(oppsLength, oppsTops1) + "; or " +
      setsWest[1].str(oppsLength, oppsTops1);
  }
}

