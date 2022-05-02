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

// TMP
#include "../../utils/Timers.h"
extern vector<Timer> timersStrat;


void Tricks::clear()
{
  weight = 0;
  length = 0;

  signature.clear();
}


void Tricks::resize(const unsigned len)
{
  length = len;

  // tricks.resize(len);
  signature.resize(resConvert.profileSize(len));

  // See table below.
  lastForward = (len-1) / 2;
  reverseSum = len + len - (len/2) - 1;
}


const unsigned char Tricks::sigElem(const unsigned extIndex) const
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
  
  // TODO Perhaps store tricks in the regular way and do all the
  // reversing in ResConvert?
  if (extIndex <= lastForward)
    return resConvert.lookup(signature, lastForward, extIndex);
  else
    return resConvert.lookup(signature, lastForward, reverseSum - extIndex);
}


unsigned char& Tricks::element(
  vector<unsigned char>& tricks,
  const unsigned extIndex)
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
  // Seems fast.
  
  vector<unsigned char> tricks;
  tricks.resize(results.size());

  Tricks::resize(results.size());
  tricksMin = numeric_limits<unsigned char>::max();
  unsigned extIndex = 0;

  for (auto& res: results)
  {
    const unsigned char rt = res.getTricks();
    Tricks::element(tricks, extIndex) = rt;
    if (rt < tricksMin)
      tricksMin = rt;

    extIndex++;
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= tricksMin;
  
  resConvert.scrutinizeVector(tricks, lastForward, signature);

  Tricks::weigh(cases);
}


void Tricks::weigh(const vector<unsigned char>& cases)
{
  // Seems fast.

  assert(cases.size() == length);

  weight = 0;
  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    weight += cases[extIndex] * Tricks::sigElem(extIndex);
}


bool Tricks::prepare(
  const Product& product,
  const bool symmFlag,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
timersStrat[34].start();
  const unsigned len = distProfiles.size();
  assert(len == cases.size());
  Tricks::resize(len);

  // weight = 0;
  unsigned char numDist = 0;

  if (symmFlag)
  {
    unsigned counter = 0;
    unsigned accum = 0;
    unsigned char value;
    unsigned position = 0;

    for (unsigned extIndex = 0; extIndex <= lastForward; extIndex++)
    {
      value = (product.includes(distProfiles[extIndex]) ||
               product.includes(distProfiles[len-1-extIndex]) ? 1 : 0);
      numDist += value;

      resConvert.increment(counter, accum, value, position, 
        signature[position]);
    }

    resConvert.finish(counter, accum, position, signature[position]);

    for (unsigned extIndex = length-1; extIndex > lastForward; extIndex--)
    {
      value = (product.includes(distProfiles[extIndex]) ||
               product.includes(distProfiles[len-1-extIndex]) ? 1 : 0);
      numDist += value;

      resConvert.increment(counter, accum, value, position, 
        signature[position]);
    }

    resConvert.finish(counter, accum, position, signature[position]);
  }
  else
  {
    unsigned counter = 0;
    unsigned accum = 0;
    unsigned char value;
    unsigned position = 0;

    for (unsigned extIndex = 0; extIndex <= lastForward; extIndex++)
    {
      value = (product.includes(distProfiles[extIndex]) ? 1 : 0);
      numDist += value;

      resConvert.increment(counter, accum, value, position, 
        signature[position]);
    }

    resConvert.finish(counter, accum, position, signature[position]);

    for (unsigned extIndex = length-1; extIndex > lastForward; extIndex--)
    {
      value = (product.includes(distProfiles[extIndex]) ? 1 : 0);
      numDist += value;

      resConvert.increment(counter, accum, value, position, 
        signature[position]);
    }

    resConvert.finish(counter, accum, position, signature[position]);
  }

  if (numDist == 0 || numDist == length)
  {
timersStrat[34].stop();
    return false;
  }


  /*
  if (! symmFlag)
  {
    for (unsigned i = 0; i < signature.size(); i++)
    {
      if (signature[i] != sig2[i])
      {
        cout << "Tricks as stored\n";
        for (unsigned j = 0; j < tricks.size(); j++)
          cout << setw(2) << j <<
            setw(4) << +tricks[j] << "\n";
        cout << "\n";

        cout << Tricks::strShort() << "\n";

        cout << "sigs\n";
        for (unsigned j = 0; j < signature.size(); j++)
        {
          cout << setw(2) << j <<
            setw(4) << signature[j] <<
            setw(4) << sig2[j] << "\n";
        }
        cout << endl;

        assert(signature[i] == sig2[i]);
      }
    }
  }
  */

  Tricks::weigh(cases);
timersStrat[34].stop();
  return true;
}


bool Tricks::symmetrize()
{
  // Seems fast.

  // Will invalidate Tricks if not symmetrizable!
  // We only symmetrize if there is no overlap with the mirror.
  // In particular, the middle element if any is zero.
  // There's no strong reason for this, but if we consider such tricks
  // symmetrizable, we have to add up the weight explicitly here,
  // for which we need cases: Tricks::weigh(cases);
  if (length & 1)
    assert(Tricks::sigElem(lastForward) == 0);

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
  // Seems fast.

  assert(length == explained.size());
  assert(length == residuals.size());
  assert(length == cases.size());
  assert(length == additions.size());

  for (unsigned i = 0; i < signature.size(); i++)
    additions.signature[i] = signature[i] & ~ explained.signature[i];

  if (additions <= residuals)
  {
    additions.weigh(cases);
    return (additions.weight > 0);
  }
  else
  {
    return false;
  }
}


Tricks& Tricks::operator += (const Tricks& tricks2)
{
  // Seems fast.

  // No checking that we don't go out of the positive range (0..3).
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] += tricks2.signature[i];

  weight += tricks2.weight;
  
  return * this;
}


Tricks& Tricks::operator -= (const Tricks& tricks2)
{
  // Seems fast.

  // No checking of <= first.
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] -= tricks2.signature[i];

  weight -= tricks2.weight;

  return * this;
}


Tricks& Tricks::orNormal(
 const Tricks& tricks2,
 const vector<unsigned char>& cases)
{
  // Seems unused.

  assert(signature.size() == tricks2.signature.size());

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
  // Seems fast.

  assert(signature.size() == tricks2.signature.size());

  const unsigned offset = signature.size() / 2;
  for (unsigned i = 0; i < offset; i++)
  {
    const unsigned orVal = 
      tricks2.signature[i] | tricks2.signature[i + offset];

    signature[i] |= orVal;

    if ((length & 1) && i+1 == offset)
    {
      // There is a middle element that should not be reproduced
      // on the high side.
      signature[i + offset] |= resConvert.limit(lastForward, orVal);

    }
    else
      signature[i + offset] |= orVal;
  }

  Tricks::weigh(cases);
}


bool Tricks::operator == (const Tricks& tricks2) const
{
  // Seems fast.

  // TODO Could use weight first once we have it
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    if (tricks2.signature[i] != signature[i])
    {
      return false;
    }
  
  return true;
}


bool Tricks::operator <= (const Tricks& tricks2) const
{
  // Seems fast.

  // TODO Could use weight first once we have it
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    if (! resConvert.greaterEqual(tricks2.signature[i], signature[i]))
    {
      return false;
    }
  
  return true;
}


unsigned Tricks::getWeight() const
{
  return weight;
}


unsigned Tricks::size() const
{
  return length;
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

