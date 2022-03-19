/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Product.h"


Product::Product()
{
  Product::reset();
}


void Product::reset()
{
  symmFlag = false;
  complexity = 0;
  length.reset();
  tops.clear();
  topSize = 0;
  topCount = 0;

  // cout << "LENGTH SIZE " << sizeof(length) << endl;
  // assert(false);
}


void Product::resize(const unsigned compSize)
{
  tops.resize(compSize);
}


void Product::set(
  const ProductProfile& sumProfile,
  const ProductProfile& lowerProfile,
  const ProductProfile& upperProfile)
{
  symmFlag = true;

  length.setNew(sumProfile.length, lowerProfile.length, upperProfile.length);
  complexity = length.getComplexity();
  range = length.getRange();

  if (lowerProfile.length + upperProfile.length != sumProfile.length)
    symmFlag = false;

  const unsigned topLowSize = lowerProfile.tops.size();
  assert(upperProfile.tops.size() == topLowSize);
  assert(tops.size() >= topLowSize);

  // Always skip the first one.
  for (unsigned char i = 1; i < topLowSize; i++)
  {
    tops[i].setNew(
      sumProfile.tops[i], 
      lowerProfile.tops[i], 
      upperProfile.tops[i]);

    // Note the first, i.e. lowest one.
    if (tops[i].used())
    {
      if (topSize == 0)
        topSize = i;

      topCount++;
    }

    complexity += tops[i].getComplexity();
    range += tops[i].getRange();

    if (lowerProfile.tops[i] + upperProfile.tops[i] != sumProfile.tops[i])
      symmFlag = false;
  }

  // If there is only a single distribution possible, this counts
  // as a complexity of 2 (don't make it absurdly attractive).
  if (range == 0 && length.used() && 
    topCount+1 == static_cast<unsigned char>(topLowSize))
  {
// cout << "XX " << Product::strLine(lenActual, topsActual) << "\n";
    complexity = 2;
  }
}


bool Product::includes(const ProductProfile& distProfile) const
{
  if (length.used() && ! length.includes(distProfile.length))
    return false;

if (distProfile.tops.size() != tops.size())
{
cout << 
  "tops.size " << tops.size() << 
  ", in " << distProfile.tops.size() << endl;
if (distProfile.tops.size() > 20)
  assert(false);

cout << "lengthIn " << +distProfile.length << endl;
for (unsigned i = 0; i < tops.size(); i++)
  cout << i << ": " << tops[i].strRaw() << endl;
for (unsigned i = 0; i < distProfile.tops.size(); i++)
  cout << i << ": " << distProfile.tops[i] << endl;

  assert(distProfile.tops.size() == tops.size());
}
  for (unsigned i = 0; i < distProfile.tops.size(); i++)
  {
    if (tops[i].used() && 
        ! tops[i].includes(static_cast<unsigned char>(distProfile.tops[i])))
      return false;
  }
  return true;
}


unsigned char Product::getComplexity() const
{
  return complexity;
}


unsigned char Product::getTopSize() const
{
  if (topSize == 0 || tops.empty())
    return 0;
  else
    return static_cast<unsigned char>(tops.size()) - topSize;
}


unsigned char Product::getRangeSum() const
{
  unsigned char sum = length.getRange();
  for (auto& top: tops)
    sum += top.getRange();

  assert(sum == range);
  return sum;
}


bool Product::explainable() const
{
  if (topCount == 0)
    return true;
  else if (Product::getTopSize() == 1 && topCount == 1)
    return true;
  else
    return false;
}


string Product::strHeader() const
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


string Product::strLine(const ProductProfile& sumProfile) const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in CoverNew.
  stringstream ss;

  ss << setw(8) << length.strShort(sumProfile.length);

  assert(tops.size() == sumProfile.tops.size());
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << tops[i].strShort(sumProfile.tops[i]);

  ss << setw(8) << (symmFlag ? "yes" : "-");

  return ss.str();
}


string Product::strLine() const
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


string Product::strVerbal(const ProductProfile& sumProfile) const
{
  if (topCount == 0)
    return length.str(sumProfile.length);
  else if (! length.used())
    return tops.back().str(sumProfile.tops.back());
  else if (length.oper == COVER_EQUAL)
  {
    auto& top = tops.back();
    if (top.oper == COVER_EQUAL)
      return top.strBothEqual(
        length, 
        sumProfile.length, 
        sumProfile.tops.back());
    else
      return length.str(sumProfile.length) + ", and " +
        top.str(sumProfile.tops.back());
  }
  else
  {
    auto& top = tops.back();
    if (top.oper == COVER_EQUAL)
      return top.strTop1Fixed(length, 
        sumProfile.length, sumProfile.tops.back());
    else
      return length.str(sumProfile.length) + ", and " +
        top.str(sumProfile.tops.back());
  }
}

