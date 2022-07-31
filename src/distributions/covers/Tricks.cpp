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
#include "CoverCategory.h"
#include "ConvertData.h"

#include "product/FactoredProduct.h"
#include "product/Profile.h"

#include "../../strategies/result/Result.h"

extern TrickConvert trickConvert;
extern vector<vector<unsigned>> binomial;


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
  unsigned char& tricksMin)
{
  Tricks::resize(results.size());

  tricksMin = numeric_limits<unsigned char>::max();
  for (auto& res: results)
    tricksMin = min(tricksMin, res.getTricks());

  ConvertData convertData;

  // The forward half including the middle element if any.
  auto riter = results.begin();
  for (unsigned extIndex = 0; extIndex <= lastForward; 
      extIndex++, riter++)
  {
    convertData.increment(riter->getTricks() - tricksMin, signature);
  }
  convertData.finish(signature);

  // The backward half excluding the middle element.
  riter = prev(results.end());
  for (size_t extIndex = length-1; extIndex > lastForward; 
      extIndex--, riter--)
  {
    convertData.increment(riter->getTricks() - tricksMin, signature);
  }
  convertData.finish(signature);

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

  ConvertData convertData;

  unsigned char value;
  unsigned char numDist = 0;

  // The forward half including the middle element if any.
  for (unsigned extIndex = 0; extIndex <= lastForward; extIndex++)
  {
    value = Tricks::productValue(factoredProduct, symmetrizeFlag, 
      distProfiles, extIndex);
    numDist += value;

    convertData.increment(value, signature);
  }
  convertData.finish(signature);

  // The backward half excluding the middle element.
  for (size_t extIndex = length-1; extIndex > lastForward; extIndex--)
  {
    value = Tricks::productValue(factoredProduct, symmetrizeFlag, 
      distProfiles, extIndex);
    numDist += value;

    convertData.increment(value, signature);
  }
  convertData.finish(signature);

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


unsigned Tricks::factor()
{
  // It may be that a Tricks has only values of, say, 0 and 2 but
  // no 1, 3, ...  This can be represented as a factor times a
  // reduced vector.

  unsigned fac = numeric_limits<unsigned>::max();
  for (unsigned extIndex = 0; extIndex < length; extIndex++)
  {
    const unsigned char t = Tricks::lookup(extIndex);
    if (t > 0 && t < fac)
      fac = t;
  }

  if (fac == 1)
    return 1;

  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] /= fac;

  weight /= fac;

  return fac;
}


void Tricks::uniqueOver(
  const Tricks& compare,
  const vector<unsigned char>& cases)
{
  assert(length == compare.size());

  for (unsigned i = 0; i < signature.size(); i++)
    signature[i] &= ~ compare.signature[i];

  Tricks::weigh(cases);
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


bool Tricks::symmetricWith(const Tricks& tricks2) const
{
  // TODO I don't actually see why this should be the case.
  // But if it isn't, then maybe symmetric() is broken?
  assert(signature.size() % 2 == 0);

  const size_t offset = signature.size() / 2;

  for (size_t i = 0; i < signature.size(); i++)
  {
    const size_t corr = (i < offset ? i+offset : i-offset);

    if ((length & 1) && i+1 == offset)
    {
      const size_t limited1 = trickConvert.limit(lastForward, signature[i]);
      if (limited1 != tricks2.signature[corr])
        return false;
    }
    else if ((length & 1) && corr+1 == offset)
    {
      const size_t limited2 = trickConvert.limit(lastForward, 
        tricks2.signature[corr]);
      if (signature[i]!= limited2)
        return false;
    }
    else
    {
      if (signature[i] != tricks2.signature[corr])
        return false;
    }
  }
  return true;
}


bool Tricks::antiSymmetric() const
{
  const size_t offset = signature.size() / 2;
  for (size_t i = 0; i < offset; i++)
  {
    if ((length & 1) && i+1 == offset)
    {
      // There is a middle element that should not be reproduced
      // on the high side.
      const size_t limited = trickConvert.limit(lastForward, signature[i]);
      if (limited & signature[i + offset])
        return false;
    }
    else if (signature[i] & signature[i + offset])
      return false;
  }

  // So now we know that there is no bit-level overlap.
  // But we could still have 1 and 2 tricks, which AND to 0,
  // and which would share a symmetric component of 1.

  for (unsigned extIndex = 0; extIndex < length/2; extIndex++)
  {
    if (Tricks::lookup(extIndex) > 0 && 
        Tricks::lookup(length - extIndex - 1) > 0)
      return false;
  }

  return true;
}


CoverSymmetry Tricks::symmetry() const
{
  if (Tricks::symmetric())
    return EXPLAIN_SYMMETRIC;
  else if (Tricks::antiSymmetric())
    return EXPLAIN_ANTI_SYMMETRIC;
  else
    return EXPLAIN_GENERAL;
}


void Tricks::partition(
   Tricks& tricksSymmetric,
   Tricks& tricksAntisymmetric,
   const vector<unsigned char>& cases) const
{
  tricksSymmetric.resize(length);
  tricksAntisymmetric.resize(length);

  ConvertData dataSymm, dataAntisymm;

  // The forward half including the middle element if any.
  for (unsigned extIndex = 0; extIndex <= lastForward; extIndex++)
  {
    const unsigned char f = Tricks::lookup(extIndex);
    const unsigned char b = Tricks::lookup(length - extIndex - 1);
    const unsigned char mint = min(f, b);

    dataSymm.increment(mint, tricksSymmetric.signature);
    dataAntisymm.increment(f - mint, tricksAntisymmetric.signature);
  }
  dataSymm.finish(tricksSymmetric.signature);
  dataAntisymm.finish(tricksAntisymmetric.signature);

  // The backward half excluding the middle element.
  for (size_t extIndex = length-1; extIndex > lastForward; extIndex--)
  {
    const unsigned char f = Tricks::lookup(extIndex);
    const unsigned char b = Tricks::lookup(length - extIndex - 1);
    const unsigned char mint = min(f, b);

    dataSymm.increment(mint, tricksSymmetric.signature);
    dataAntisymm.increment(f - mint, tricksAntisymmetric.signature);
  }
  dataSymm.finish(tricksSymmetric.signature);
  dataAntisymm.finish(tricksAntisymmetric.signature);

  tricksSymmetric.weigh(cases);
  tricksAntisymmetric.weigh(cases);
}


unsigned Tricks::casesToLengthEW(const vector<unsigned char>& cases) const
{
  // Figure out the number of East-West cards.
  // TODO No doubt we could pass this in more efficiently.

  unsigned caseSum = 0;
  for (auto c: cases)
    caseSum += static_cast<unsigned>(c);

  unsigned lengthEW = 0;
  while (caseSum > 1)
  {
    lengthEW++;
    caseSum >>= 1;
  }

  return lengthEW;
}


void Tricks::lengthBoundary(
  size_t& cindex,
  const unsigned target,
  const vector<unsigned char>& cases) const
{
  // Starting from cindex, increase the index until we have seen
  // cases totaling exactly target.
  // TODO No doubt this too could be determined more easily from
  // a distribution.

  unsigned binomRunning = 0;

  while (true)
  {
    assert(cindex < cases.size());
    binomRunning += cases[cindex];
    cindex++;
    if (binomRunning >= target)
    {
      assert(binomRunning == target);
      return;
    }
  }
}


void Tricks::transfer(
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  const size_t cstart,
  const size_t cend,
  Tricks& tricksMin)
{
  // Transfer tricks[cstart, cend).  If they share a minimum > 0,
  // subtract this out.
  // TODO This is an inefficient hack.
  // At least the intermediate list could be a list<unsigned char>?

  list<Result> results;
  results.resize(length);

  unsigned char tmin = numeric_limits<unsigned char>::max();
  for (size_t extIndex = cstart; extIndex < cend; extIndex++)
  {
    const unsigned char t = tricks.lookup(extIndex);
    if (t < tmin)
      tmin = t;
  }

  unsigned char tmp;

  if (tmin > 0)
  {
    auto riter = results.begin();
    for (size_t extIndex = 0; extIndex < cstart; extIndex++, riter++) ;

    for (size_t extIndex = cstart; extIndex < cend; extIndex++, riter++)
      riter->setTricks(tmin);

    tricksMin.setByResults(results, cases, tmp);
  }

  // We can reuse the results list.
  auto riter = results.begin();
  for (size_t extIndex = 0; extIndex < cstart; extIndex++, riter++) ;

  for (size_t extIndex = cstart; extIndex < cend; extIndex++, riter++)
    riter->setTricks(tricks.lookup(extIndex) - tmin);

  Tricks::setByResults(results, cases, tmp);
}


void Tricks::partitionGeneral(
  vector<Tricks>& tricksByLength,
  vector<Tricks>& tricksMinByLength,
  const vector<unsigned char>& cases) const
{
  // Use Pascal numbers and cases to split tricks by length.
  const unsigned lengthEW = Tricks::casesToLengthEW(cases);

  tricksByLength.resize(lengthEW+1);
  for (auto& tbl: tricksByLength)
    tbl.resize(length);

  tricksMinByLength.resize(lengthEW+1);
  for (auto& tbl: tricksMinByLength)
    tbl.resize(length);

  size_t cindex = 0;

  for (unsigned lEW = 0; lEW <= lengthEW; lEW++)
  {
    const size_t cstart = cindex;
    Tricks::lengthBoundary(cindex, binomial[lengthEW][lEW], cases);

    tricksByLength[lEW].transfer(* this, cases,
      cstart, cindex, tricksMinByLength[lEW]);
  }
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


size_t Tricks::nonzero() const
{
  // Returns the number of non-zero elements (the number of
  // distributions that are set).
  size_t count = 0;
  for (unsigned extIndex = 0; extIndex < length; extIndex++)
  {
    if (Tricks::lookup(extIndex))
      count++;
  }

  return count;
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

