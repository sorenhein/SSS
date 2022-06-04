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
#include "TrickConvert.h"

#include "product/FactoredProduct.h"
#include "product/Profile.h"

#include "../../strategies/result/Result.h"

extern TrickConvert trickConvert;


void Tricks::clear()
{
  length = 0;
  weight = 0;

  signature.clear();
}


void Tricks::resize(const size_t len)
{
  length = len;

  signature.resize(trickConvert.profileSize(len));

  // This is the last element in the forward half.
  lastForward = (len-1) / 2;
}


const unsigned char Tricks::lookup(const size_t extIndex) const
{
  // Store the front half (including perhaps the middle) one way.
  // Store the back half backward.
  
  const size_t effIndex = (extIndex <= lastForward ?
    extIndex : length + lastForward - extIndex);

  return trickConvert.lookup(signature, lastForward, effIndex);
}


void Tricks::weigh(const vector<unsigned char>& cases)
{
  assert(cases.size() == length);

  weight = 0;
  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    weight += cases[extIndex] * Tricks::lookup(extIndex);
}


void Tricks::setByResults(
  const list<Result>& results,
  const vector<unsigned char>& cases,
  unsigned char& tricksMin,
  bool& symmetricFlag)
{
  Tricks::resize(results.size());

  tricksMin = numeric_limits<unsigned char>::max();
  for (auto& res: results)
    tricksMin = min(tricksMin, res.getTricks());

  unsigned counter = 0;
  unsigned accum = 0;
  unsigned position = 0;

  // The forward half including the middle element if any.
  auto riter = results.begin();
  for (unsigned extIndex = 0; extIndex <= lastForward; 
      extIndex++, riter++)
  {
    trickConvert.increment(
      counter, 
      accum, 
      riter->getTricks() - tricksMin,
      position, 
      signature[position]);
  }
  trickConvert.finish(counter, accum, position, signature[position]);

  // The backward half excluding the middle element.
  riter = prev(results.end());
  for (size_t extIndex = length-1; extIndex > lastForward; 
      extIndex--, riter--)
  {
    trickConvert.increment(
      counter, 
      accum, 
      riter->getTricks() - tricksMin,
      position, 
      signature[position]);
  }
  trickConvert.finish(counter, accum, position, signature[position]);

  symmetricFlag = Tricks::symmetric();

  Tricks::weigh(cases);
}


unsigned char Tricks::productValue(
  const FactoredProduct& factoredProduct,
  const bool symmetrizeFlag,
  const vector<Profile>& distProfiles,
  const size_t extIndex) const
{
  // It is slightly wasteful to test symmFlag every time, but it
  // cuts down on the code below.
  if (symmetrizeFlag)
  {
    return (factoredProduct.includes(distProfiles[extIndex]) ||
      factoredProduct.includes(distProfiles[length-1-extIndex]) ? 1 : 0);
  }
  else
  {
    return (factoredProduct.includes(distProfiles[extIndex]) ? 1 : 0);
  }
}


bool Tricks::setByProduct(
  const FactoredProduct& factoredProduct,
  const bool symmetrizeFlag,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  // This is the only Tricks method that takes appreciable time,
  // and this is probably due to product.includes().
  assert(distProfiles.size() == cases.size());
  Tricks::resize(cases.size());

  unsigned counter = 0;
  unsigned accum = 0;
  unsigned char value;
  unsigned position = 0;
  unsigned char numDist = 0;

  // The forward half including the middle element if any.
  for (unsigned extIndex = 0; extIndex <= lastForward; extIndex++)
  {
    value = Tricks::productValue(factoredProduct, symmetrizeFlag, 
      distProfiles, extIndex);
    numDist += value;

    trickConvert.increment(counter, accum, value, position, 
      signature[position]);
  }
  trickConvert.finish(counter, accum, position, signature[position]);

  // The backward half excluding the middle element.
  for (size_t extIndex = length-1; extIndex > lastForward; extIndex--)
  {
    value = Tricks::productValue(factoredProduct, symmetrizeFlag, 
      distProfiles, extIndex);
    numDist += value;

    trickConvert.increment(counter, accum, value, position, 
      signature[position]);
  }
  trickConvert.finish(counter, accum, position, signature[position]);

  if (numDist == 0 || numDist == length)
    return false;

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
    assert(Tricks::lookup(lastForward) == 0);

  const size_t offset = signature.size() / 2;
  bool fullHouseFlag = true;

  for (unsigned i = 0; i < offset; i++)
  {
    // No overlap permitted.
    if (signature[i] & signature[i + offset])
      return false;

    const unsigned merge = signature[i] | signature[i + offset];
    if (! trickConvert.fullHouse(merge))
      fullHouseFlag = false;

    // As the middle element is not set, we will not get a stray '1'
    // in the last group on the high end.
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
  // Effectively additions equals (this & ~ explained) which must be
  // <= residuals in order to be a valid addition to a CoverRow.
  assert(length == explained.size());
  assert(length == residuals.size());
  assert(length == cases.size());
  assert(length == additions.size());

  for (unsigned i = 0; i < signature.size(); i++)
  {
    additions.signature[i] = signature[i] & ~ explained.signature[i];

    if (! trickConvert.greaterEqual(
        residuals.signature[i], 
        additions.signature[i]))
    {
      return false;
    }
  }

  additions.weigh(cases);
  return (additions.weight > 0);
}


Tricks& Tricks::orNormal(
 const Tricks& tricks2,
 const vector<unsigned char>& cases)
{
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
  assert(signature.size() == tricks2.signature.size());

  const size_t offset = signature.size() / 2;
  for (size_t i = 0; i < offset; i++)
  {
    const unsigned orVal = 
      tricks2.signature[i] | tricks2.signature[i + offset];

    signature[i] |= orVal;

    if ((length & 1) && i+1 == offset)
    {
      // There is a middle element that should not be reproduced
      // on the high side.
      signature[i + offset] |= trickConvert.limit(lastForward, orVal);

    }
    else
      signature[i + offset] |= orVal;
  }

  Tricks::weigh(cases);
}


bool Tricks::symmetric() const
{
  const size_t offset = signature.size() / 2;
  for (size_t i = 0; i < offset; i++)
  {
    if ((length & 1) && i+1 == offset)
    {
      // There is a middle element that should not be reproduced
      // on the high side.
      const size_t limited = trickConvert.limit(lastForward, signature[i]);
      if (limited != signature[i + offset])
        return false;
    }
    else if (signature[i] != signature[i + offset])
      return false;
  }
  return true;
}


Tricks& Tricks::operator += (const Tricks& tricks2)
{
  // No checking that we don't go out of the positive range (0..3).
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] += tricks2.signature[i];

  weight += tricks2.weight;
  
  return * this;
}


Tricks& Tricks::operator -= (const Tricks& tricks2)
{
  // No checking of <= first.
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] -= tricks2.signature[i];

  weight -= tricks2.weight;

  return * this;
}


bool Tricks::operator == (const Tricks& tricks2) const
{
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
  {
    if (tricks2.signature[i] != signature[i])
      return false;
  }
  
  return true;
}


bool Tricks::operator <= (const Tricks& tricks2) const
{
  assert(signature.size() == tricks2.signature.size());

  for (unsigned i = 0; i < signature.size(); i++)
  {
    if (! trickConvert.greaterEqual(tricks2.signature[i], signature[i]))
      return false;
  }
  
  return true;
}


size_t Tricks::size() const
{
  return length;
}


unsigned Tricks::getWeight() const
{
  return weight;
}


string Tricks::strList() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    ss << 
      setw(2) << extIndex << 
      setw(4) << +Tricks::lookup(extIndex) << "\n";

  return ss.str();
}


string Tricks::strShort() const
{
  string s;

  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    s += (Tricks::lookup(extIndex) ? "1" : "-");

  return s + "  ";
}


string Tricks::strSpaced() const
{
  stringstream ss;

  for (unsigned extIndex = 0; extIndex < length; extIndex++)
    ss << setw(2) << +Tricks::lookup(extIndex);

  return ss.str() + "\n";
}

