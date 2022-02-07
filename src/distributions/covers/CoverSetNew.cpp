/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverSetNew.h"


CoverSetNew::CoverSetNew()
{
  CoverSetNew::reset();
}


void CoverSetNew::reset()
{
  symmFlag = false;
  complexity = 0;
  length.reset();
  tops.clear();
}


void CoverSetNew::resize(const unsigned compSize)
{
  tops.resize(compSize);
}


void CoverSetNew::set(
  const unsigned char lenActual,
  const unsigned char lenLow,
  const unsigned char lenHigh,
  vector<unsigned char>& topsLow,
  vector<unsigned char>& topsHigh)
{
  symmFlag = true;

  length.setNew(lenActual, lenLow, lenHigh);
  complexity = length.getComplexity();
  if (lenLow + lenHigh != lenActual)
    symmFlag = false;

  const unsigned topSize = topsLow.size();
  assert(topsHigh.size() == topSize);

  for (unsigned i = 0; i < topSize; i++)
  {
    tops[i].setNew(lenActual, topsLow[i], topsHigh[i]);
    complexity += tops[i].getComplexity();
    if (topsLow[i] + topsHigh[i] != lenActual)
      symmFlag = false;
  }
}


bool CoverSetNew::includes(
  const unsigned char lengthIn,
  const vector<unsigned char>& topsIn)
{
  if (length.used() && ! length.includes(lengthIn))
    return false;

  assert(tops.size() == topsIn.size());
  for (unsigned i = 0; i < tops.size(); i++)
  {
    if (tops[i].used() && ! tops[i].includes(topsIn[i]))
      return false;
  }
  return true;
}


string CoverSetNew::strHeader() const
{
  stringstream ss;

  ss << setw(8) << "Length";
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << ("Top #" + to_string(i));
  ss << setw(8) << "Cmplx" <<
    setw(8) << "symm" << "\n";

  return ss.str();
}


string CoverSetNew::strLine(const unsigned char lenActual) const
{
  stringstream ss;

  ss << setw(8) << length.strShort(lenActual);
  for (auto& top: tops)
    ss << setw(8) << top.strShort(lenActual);
  ss << setw(8) << +complexity;
  ss << setw(8) << (symmFlag ? "yes" : "-") << "\n";

  return ss.str();
}

