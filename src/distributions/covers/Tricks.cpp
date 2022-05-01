/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <limits>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Tricks.h"

#include "product/Product.h"
#include "product/Profile.h"

#include "../../strategies/result/Result.h"
#include "../../strategies/result/ResConvert.h"

extern ResConvert resConvert;


void Tricks::clear()
{
  tricks.clear();
  signature.clear();
}


void Tricks::resize(const unsigned len)
{
  tricks.resize(len);
  signature.resize(resConvert.profileSize(len));

  // See table below.
  lastForward = (len-1) / 2;
  reverseSum = len + len - (len/2) - 1;
}


const unsigned char& Tricks::element(const unsigned extIndex) const
{
  /*
     Store the front half (including perhaps the middle) one way.
     Store the back half backward.

     N   lastF Forward (grouped)  Backward (grouped)  revSum Cost 
      3: 1     0-1                2                   4      1
      4: 1     0-1                3-2                 5      1
      5: 2     0-2                4-3                 7      1
      6: 2     0-2                5-3                 8      0
      7: 3     0-3                6-4                 10     0
      8: 3     0-3                7-4                 11     0
      9: 4     0-4                8-5                 13     0
     10: 4     0-4                9-5                 14     0
     11: 5     0-4   5            10-6                16     0
     12: 5     0-4   5            11-7    6           17     1
     13: 6     0-4   5-6          12-8    7           19     1
     14: 6     0-4   5-6          13-9    8-7         20     1
     15: 7     0-4   5-7          14-10   9-8         22     1
     16: 7     0-4   5-7          15-11   10-8        23     0
     17: 8     0-4   5-8          16-12   11-9        25     0
     18: 8     0-4   5-8          17-13   12-9        26     0
     19: 9     0-4   5-9          18-14   13-10       28     0

     So the average cost is 0.4 extra entries.
  */
  
  if (extIndex <= lastForward)
    return tricks[extIndex];
  else
    return tricks[reverseSum - extIndex];
}


unsigned char& Tricks::element(const unsigned extIndex)
{
  if (extIndex <= lastForward)
    return tricks[extIndex];
  else
    return tricks[reverseSum - extIndex];
}


void Tricks::set(
  const list<Result>& results,
  unsigned char& tricksMin)
{
  Tricks::resize(results.size());
  tricksMin = numeric_limits<unsigned char>::max();
  unsigned extIndex = 0;

  for (auto& res: results)
  {
    const unsigned char rt = res.getTricks();
    Tricks::element(extIndex) = rt;
    if (rt < tricksMin)
      tricksMin = rt;

    extIndex++;
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= tricksMin;
  
  resConvert.scrutinizeVector(tricks, signature);
}


void Tricks::weigh(
  const vector<unsigned char>& cases,
  unsigned& weight) const
{
  assert(cases.size() == tricks.size());

  weight = 0;
  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    weight += cases[extIndex] * Tricks::element(extIndex);
}


void Tricks::prepare(
  const Product& product,
  const bool symmFlag,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  unsigned& weight,
  unsigned char& numDist)
{
  const unsigned len = distProfiles.size();
  assert(len == cases.size());
  Tricks::resize(len);

  weight = 0;
  numDist = 0;

  if (symmFlag)
  {
    for (unsigned extIndex = 0; extIndex < len; extIndex++)
    {
      if (product.includes(distProfiles[extIndex]) ||
          product.includes(distProfiles[len-1-extIndex]))
      {
        Tricks::element(extIndex) = 1;
        // weight += static_cast<unsigned>(cases[extIndex]);
        numDist++;
      }
    }
  }
  else
  {
    for (unsigned extIndex = 0; extIndex < len; extIndex++)
    {
      if (product.includes(distProfiles[extIndex]))
      {
        Tricks::element(extIndex) = 1;
        // weight += static_cast<unsigned>(cases[extIndex]);
        numDist++;
      }
    }
  }

  resConvert.scrutinizeVector(tricks, signature);

  Tricks::weigh(cases, weight);
}


bool Tricks::symmetrize(
  const vector<unsigned char>& cases,
  unsigned& weight,
  unsigned char& numDist)
{
  // Will invalidate Tricks if not symmetrizable!
  // We only symmetrize if there is no overlap with the mirror.
  // We cannot be sure to double weight and numDist, as the middle
  // element in an odd-length tricks will not be repeated.

  // Loop over the high end of the internal trick numbers.
  unsigned lo, hi;
  for (lo = 0, hi = lastForward+1; hi < tricks.size(); lo++, hi++)
  {
    if (tricks[lo] == tricks[hi])
    {
      // Don't symmetrize if there is overlap.
      if (tricks[lo])
        return false;
    }
    else
    {
      // Overwrite one of them, but no need to test which one.
      tricks[lo] = 1;
      tricks[hi] = 1;
      numDist++;
    }
  }

  resConvert.scrutinizeVector(tricks, signature);

  Tricks::weigh(cases, weight);
  return true;
}


bool Tricks::possible(
  const Tricks& explained,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned& weightAdded) const
{
  assert(tricks.size() == explained.size());
  assert(tricks.size() == residuals.size());
  assert(tricks.size() == cases.size());
  assert(tricks.size() == additions.size());

  // The cover is an addition if it is not already set.
  // This can be written a bit more efficiently, but the idea is to
  // make it vectorizable if we go down that path.
  for (unsigned intIndex = 0; intIndex < tricks.size(); intIndex++)
    additions.tricks[intIndex] = 
      tricks[intIndex] & ~explained.tricks[intIndex];

  resConvert.scrutinizeVector(additions.tricks, additions.signature);
  /*
  vector<unsigned> addSignature;
  addSignature.resize(signature.size());
  for (unsigned i = 0; i < signature.size(); i++)
    addSignature[i] = signature[i] & ~ explained.signature[i];
    */

  if (additions <= residuals)
  {
    additions.weigh(cases, weightAdded);
    return (weightAdded > 0);
  }
  else
    return false;
}


Tricks& Tricks::operator += (const Tricks& tricks2)
{
  // No checking that we don't go out of the positive range (0..3).
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] += tricks2.tricks[i];

  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] += tricks2.signature[i];
  
  return * this;
}


Tricks& Tricks::operator -= (const Tricks& tricks2)
{
  // No checking of <= first.
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= tricks2.tricks[i];
  
  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] -= tricks2.signature[i];

  return * this;
}


Tricks& Tricks::operator |= (const Tricks& tricks2)
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] |= tricks2.tricks[i];

  // This only works for binary vectors.
  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] |= tricks2.signature[i];
  
  return * this;
}


void Tricks::orSymm(const Tricks& tricks2)
{
  const unsigned len = tricks.size();
  assert(len == tricks2.tricks.size());

  unsigned lo, hi;
  for (lo = 0, hi = lastForward+1; hi < tricks.size(); lo++, hi++)
  {
    const unsigned char orVal = tricks2.tricks[lo] | tricks2.tricks[hi];
    tricks[lo] |= orVal;
    tricks[hi] |= orVal;
  }

  if (lo == lastForward)
  {
    // There is a middle element.
    tricks[lo] |= tricks2.tricks[lo];
  }

  resConvert.scrutinizeVector(tricks, signature);
}


bool Tricks::operator == (const Tricks& tricks2) const
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i] != tricks2.tricks[i])
      return false;
  
  return true;
}


bool Tricks::lessEqual(const Tricks& tricks2) const
{
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    if (! resConvert.greaterEqual(tricks2.signature[i], signature[i]))
      return false;
  
  return true;
}


bool Tricks::operator <= (const Tricks& tricks2) const
{
  bool b = Tricks::lessEqual(tricks2);

  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i] > tricks2.tricks[i])
    {
      if (b)
      {
      assert(! b);
      }
      return false;
    }
  
  assert(b);
  return true;
}


unsigned Tricks::size() const
{
  return tricks.size();
}


string Tricks::strList() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    ss << 
      setw(2) << extIndex << 
      setw(4) << +Tricks::element(extIndex) << "\n";

  return ss.str();
}


string Tricks::strShort() const
{
  string s;

  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    s += (Tricks::element(extIndex) ? "1" : "-");

  return s + "  ";
}


string Tricks::strSpaced() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < tricks.size(); extIndex++)
    ss << setw(2) << +Tricks::element(extIndex);

  return ss.str() + "\n";
}

