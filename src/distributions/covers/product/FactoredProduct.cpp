/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include "FactoredProduct.h"
#include "Product.h"

#include "../CoverCategory.h"

#include "../../../utils/Compare.h"
#include "../../../utils/table.h"

using namespace std;


void FactoredProduct::set(
  Product const * ptr,
  const unsigned char shift)
{
  canonicalPtr = ptr;
  canonicalShift = shift;
}


void FactoredProduct::set(
  const FactoredProduct& fp2,
  const unsigned char shift)
{
  FactoredProduct::set(fp2.canonicalPtr, shift);
}


unsigned long long FactoredProduct::code() const
{
  return canonicalPtr->code();
}


bool FactoredProduct::includes(const Profile& distProfile) const
{
  return canonicalPtr->includes(distProfile, canonicalShift);
}


bool FactoredProduct::symmetrizable(const Profile& sumProfile) const
{
  return canonicalPtr->symmetrizable(sumProfile, canonicalShift);
}


unsigned char FactoredProduct::getComplexity() const
{
  return canonicalPtr->getComplexity();
}


unsigned char FactoredProduct::effectiveDepth() const
{
  return canonicalPtr->effectiveDepth();
}


bool FactoredProduct::sameTops(const FactoredProduct& fp2) const
{
  // If they have the same tops, they have in particular the same shift.
  if (canonicalShift != fp2.canonicalShift)
    return false;
  else
    return canonicalPtr->sameTops(* fp2.canonicalPtr);
}


unsigned char FactoredProduct::minimumByTops(
  const Opponent voidSide,
  const Profile& sumProfile) const
{
  return canonicalPtr->minimumByTops(voidSide, sumProfile, canonicalShift);
}


bool FactoredProduct::discardSymmetric(const Profile& sumProfile) const
{
  return canonicalPtr->discardSymmetric(sumProfile, canonicalShift);
}


CoverComposition FactoredProduct::composition() const
{
  return canonicalPtr->composition();
}


CoverVerbal FactoredProduct::verbal() const
{
  return canonicalPtr->verbal();
}


bool FactoredProduct::lengthConsistent(
  const unsigned char specificLength) const
{
  return canonicalPtr->lengthConsistent(specificLength);
}


CompareType FactoredProduct::presentOrder(const FactoredProduct& fp2) const
{
  return canonicalPtr->presentOrder(* fp2.canonicalPtr);
}


string FactoredProduct::strHeader(const unsigned length) const
{
  return canonicalPtr->strHeader(length);
}


string FactoredProduct::strLine() const
{
  return canonicalPtr->strLine();
}


string FactoredProduct::strVerbal(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const CoverVerbal verbal,
  const bool symmFlag) const
{
  return canonicalPtr->strVerbal(
    sumProfile, canonicalShift, symmFlag, ranksNames, verbal);
}

