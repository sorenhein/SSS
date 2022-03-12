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
  topSize = 0;
  topCount = 0;
}


void CoverSetNew::resize(const unsigned compSize)
{
  tops.resize(compSize);
}


void CoverSetNew::set(
  const unsigned char lenActual,
  const unsigned char lenLow,
  const unsigned char lenHigh,
  const vector<unsigned char>& topsActual,
  const vector<unsigned char>& topsLow,
  const vector<unsigned char>& topsHigh)
{
  symmFlag = true;

  length.setNew(lenActual, lenLow, lenHigh);
  complexity = length.getComplexity();
  range = length.getRange();

  if (lenLow + lenHigh != lenActual)
    symmFlag = false;

  const unsigned topLowSize = topsLow.size();
  assert(topsHigh.size() == topLowSize);
  assert(tops.size() >= topLowSize);

  // Always skip the first one.
  for (unsigned char i = 1; i < topLowSize; i++)
  {
    tops[i].setNew(topsActual[i], topsLow[i], topsHigh[i]);

    // Note the first, i.e. lowest one.
    if (tops[i].used())
    {
      if (topSize == 0)
        topSize = i;

      topCount++;
    }

    complexity += tops[i].getComplexity();
    range += tops[i].getRange();

    if (topsLow[i] + topsHigh[i] != topsActual[i])
      symmFlag = false;
  }

  // If there is only a single distribution possible, this counts
  // as a complexity of 2 (don't make it absurdly attractive).
  if (range == 0 && length.used() && 
    topCount+1 == static_cast<unsigned char>(topLowSize))
  {
// cout << "XX " << CoverSetNew::strLine(lenActual, topsActual) << "\n";
    complexity = 2;
  }
}


bool CoverSetNew::includes(
  const unsigned char lengthIn,
  const vector<unsigned>& topsIn) const
{
  if (length.used() && ! length.includes(lengthIn))
    return false;

if (topsIn.size() != tops.size())
{
cout << "tops.size " << tops.size() << ", in " << topsIn.size() << endl;
if (topsIn.size() > 20)
  assert(false);

cout << "lengthIn " << +lengthIn << endl;
for (unsigned i = 0; i < tops.size(); i++)
  cout << i << ": " << tops[i].strRaw() << endl;
for (unsigned i = 0; i < topsIn.size(); i++)
  cout << i << ": " << topsIn[i] << endl;

  assert(topsIn.size() == tops.size());
}
  for (unsigned i = 0; i < topsIn.size(); i++)
  {
    if (tops[i].used() && 
        ! tops[i].includes(static_cast<unsigned char>(topsIn[i])))
      return false;
  }
  return true;
}


unsigned char CoverSetNew::getComplexity() const
{
  return complexity;
}


unsigned char CoverSetNew::getTopSize() const
{
  if (topSize == 0 || tops.empty())
    return 0;
  else
    return static_cast<unsigned char>(tops.size()) - topSize;
}


unsigned char CoverSetNew::getRangeSum() const
{
  unsigned char sum = length.getRange();
  for (auto& top: tops)
    sum += top.getRange();

  assert(sum == range);
  return sum;
}


bool CoverSetNew::explainable() const
{
  if (topCount == 0)
    return true;
  else if (CoverSetNew::getTopSize() == 1 && topCount == 1)
    return true;
  else
    return false;
}


string CoverSetNew::strHeader() const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in CoverNew.
  stringstream ss;

  ss << setw(8) << "Length";
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << ("Top #" + to_string(i));
  ss << setw(8) << "symm";

  return ss.str();
}


string CoverSetNew::strLine(
  const unsigned char lenActual,
  const vector<unsigned char>& topsActual) const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in CoverNew.
  stringstream ss;

  ss << setw(8) << length.strShort(lenActual);

  assert(tops.size() == topsActual.size());
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << tops[i].strShort(topsActual[i]);

  ss << setw(8) << (symmFlag ? "yes" : "-");

  return ss.str();
}


string CoverSetNew::strLine() const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in CoverNew.
  stringstream ss;

  ss << setw(8) << length.strShort();

  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << tops[i].strShort();

  ss << setw(8) << (symmFlag ? "yes" : "-");

  return ss.str();
}


string CoverSetNew::strVerbal(
  const unsigned char maxLength,
  const vector<unsigned char>& topTotals) const
{
  if (topCount == 0)
    return length.strLength(maxLength);
  else if (! length.used())
    return tops.back().strTop1(topTotals.back());
  else
    return length.strLengthTop1(tops.back(), maxLength, topTotals.back());
}

