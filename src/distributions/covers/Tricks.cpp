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
  weight = 0;
  length = 0;

  signature.clear();
}


void Tricks::resize(const unsigned len)
{
  length = len;

  tricks.resize(len);
  signature.resize(resConvert.profileSize(len));

  // See table below.
  lastForward = (len-1) / 2;
  reverseSum = len + len - (len/2) - 1;
}


const unsigned char Tricks::sigElem(const unsigned extIndex) const
{
  // TODO Perhaps store tricks in the regular way and do all the
  // reversing in ResConvert?
  if (extIndex <= lastForward)
    return resConvert.lookup(signature, lastForward, extIndex);
  else
    return resConvert.lookup(signature, lastForward, reverseSum - extIndex);
}


const unsigned char& Tricks::element(const unsigned extIndex) const
{
  /*
     Store the front half (including perhaps the middle) one way.
     Store the back half backward.

      N  lastF Forward     Backward       revSum Cost siglen
      3  1     0-1         2              4      1    2
      4  1     0-1         3-2            5      1    2
      5  2     0-2         4-3            7      1    2
      6  2     0-2         5-3            8      0    2
      7  3     0-3         6-4            10     0    2
      8  3     0-3         7-4            11     0    2
      9  4     0-4         8-5            13     0    2
     10  4     0-4         9-5            14     0    2
     11  5     0-4   5     10-6           16     0    4
     12  5     0-4   5     11-7    6      17     1    4
     13  6     0-4   5-6   12-8    7      19     1    4
     14  6     0-4   5-6   13-9    8-7    20     1    4
     15  7     0-4   5-7   14-10   9-8    22     1    4
     16  7     0-4   5-7   15-11   10-8   23     0    4
     17  8     0-4   5-8   16-12   11-9   25     0    4
     18  8     0-4   5-8   17-13   12-9   26     0    4
     19  9     0-4   5-9   18-14   13-10  28     0    4
     20  9     0-4   5-9   19-15   14-10  29     0    4

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
  const vector<unsigned char>& cases,
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
  
  Tricks::weigh(cases);

  resConvert.scrutinizeVector(tricks, lastForward, signature);
}


void Tricks::weigh(const vector<unsigned char>& cases)
{
  assert(cases.size() == length);

  weight = 0;
  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    weight += cases[extIndex] * Tricks::element(extIndex);
}


bool Tricks::prepare(
  const Product& product,
  const bool symmFlag,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  const unsigned len = distProfiles.size();
  assert(len == cases.size());
  Tricks::resize(len);

  weight = 0;
  unsigned char numDist = 0;

  if (symmFlag)
  {
    for (unsigned extIndex = 0; extIndex < length; extIndex++)
    {
      if (product.includes(distProfiles[extIndex]) ||
          product.includes(distProfiles[len-1-extIndex]))
      {
        Tricks::element(extIndex) = 1;
        numDist++;
      }
    }
  }
  else
  {
    for (unsigned extIndex = 0; extIndex < length; extIndex++)
    {
      if (product.includes(distProfiles[extIndex]))
      {
        Tricks::element(extIndex) = 1;
        numDist++;
      }
    }
  }

  if (numDist == 0 || numDist == tricks.size())
    return false;

  resConvert.scrutinizeVector(tricks, lastForward, signature);

  Tricks::weigh(cases);
  return true;
}


bool Tricks::symmetrize()
{
  // Will invalidate Tricks if not symmetrizable!
  // We only symmetrize if there is no overlap with the mirror.
  // In particular, the middle element if any is zero.
  // There's no strong reason for this, but if we consider such tricks
  // symmetrizable, we have to add up the weight explicitly here,
  // for which we need cases: Tricks::weigh(cases);
  if (length & 1)
    assert(tricks[lastForward] == 0);

  // Loop over the high end of the internal trick numbers.
  unsigned lo, hi;
  unsigned numDist = 0;

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
      numDist += 2;
    }
  }

  // Don't allow a fully set vector.
  if (numDist == tricks.size())
    return false;

  const unsigned offset = signature.size() / 2;
  bool fullHouseFlag = true;

  for (unsigned i = 0; i < offset; i++)
  {
    // Later on this is the condition to return false.
    // As the middle element is not set, we will not get a stray '1'
    // in the last group on the high end.
    if (signature[i] & signature[i + offset])
      return false;

    const unsigned merge = signature[i] | signature[i + offset];
    if (! resConvert.fullHouse(merge))
      fullHouseFlag = false;

    signature[i] = merge;
    signature[i + offset] = merge;
  }

  if (fullHouseFlag)
    return false;

  // As there was no overlap, we can just double the weight.
  weight += weight;
  return true;
}


bool Tricks::possible(
  const Tricks& explained,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions) const
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

  for (unsigned i = 0; i < signature.size(); i++)
    additions.signature[i] = signature[i] & ~ explained.signature[i];

  if (additions <= residuals)
  {
    additions.weigh(cases);
    return (additions.weight > 0);
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

  weight += tricks2.weight;
  
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

  weight -= tricks2.weight;

  return * this;
}


Tricks& Tricks::orNormal(
 const Tricks& tricks2,
 const vector<unsigned char>& cases)
{
  assert(tricks.size() == tricks2.tricks.size());

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] |= tricks2.tricks[i];

  // This only works for binary vectors.
  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] |= tricks2.signature[i];

  Tricks::weigh(cases);
  
  return * this;
}


void Tricks::orSymm(
  const Tricks& tricks2,
  const vector<unsigned char>& cases)
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

  Tricks::weigh(cases);

  auto sig2 = signature;
  auto sig3 = signature;

  resConvert.scrutinizeVector(tricks, lastForward, signature);

  const unsigned offset = signature.size() / 2;
  for (unsigned i = 0; i < offset; i++)
  {
    const unsigned orVal = 
      tricks2.signature[i] | tricks2.signature[i + offset];

    sig2[i] |= orVal;

    if ((length & 1) && i+1 == offset)
    {
      // There is a middle element that should not be reproduced
      // on the high side.
      sig2[i + offset] |= resConvert.limit(lastForward, orVal);
// cout << "i " << i << ": orVal " << orVal << ", limit " <<
      // resConvert.limit(lastForward, orVal) << endl;

    }
    else
      sig2[i + offset] |= orVal;
  }

  for (unsigned i = 0; i < signature.size(); i++)
  {
    if (signature[i] != sig2[i])
    {
      cout << " n t2s sig sg2 sg3\n";
      for (unsigned j = 0; j < signature.size(); j++)
      {
        cout << setw(2) << j <<
          setw(4) << tricks2.signature[j] <<
          setw(4) << signature[i] <<
          setw(4) << sig2[j] <<
          setw(4) << sig3[j] << endl;
      }

      cout << " n t2\n";
      for (unsigned j = 0; j < tricks2.size(); j++)
        cout << setw(2) << j <<
          setw(4) << +tricks2.tricks[j] << endl;
      
      cout << "lastForward " << lastForward << endl;
      cout << "i " << i << endl;

    }
    assert(signature[i] == sig2[i]);
  }
}


bool Tricks::operator == (const Tricks& tricks2) const
{
  // TODO Could use weight first once we have it
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    if (tricks2.signature[i] != signature[i])
      return false;
  
  return true;
}


bool Tricks::operator <= (const Tricks& tricks2) const
{
  // TODO Could use weight first once we have it
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    if (! resConvert.greaterEqual(tricks2.signature[i], signature[i]))
      return false;
  
  return true;
}


unsigned Tricks::getWeight() const
{
  return weight;
}


unsigned Tricks::size() const
{
  return length;
  // return tricks.size();
}


string Tricks::strList() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    ss << 
      setw(2) << extIndex << 
      setw(4) << +Tricks::sigElem(extIndex) << "\n";

  return ss.str();
}


string Tricks::strShort() const
{
  string s;

  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    s += (Tricks::sigElem(extIndex) ? "1" : "-");

  return s + "  ";
}


string Tricks::strSpaced() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    ss << setw(2) << +Tricks::sigElem(extIndex);

  return ss.str() + "\n";
}

