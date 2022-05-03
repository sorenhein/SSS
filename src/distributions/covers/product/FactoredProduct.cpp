/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "FactoredProduct.h"
#include "Product.h"

using namespace std;


unsigned long long FactoredProduct::code() const
{
  // TODO What does it do in ProductStats to switch this over?
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


bool FactoredProduct::explainable() const
{
  return canonicalPtr->explainable();
}


Opponent FactoredProduct::simplestOpponent(const Profile& sumProfile) const
{
  return canonicalPtr->simplestOpponent(sumProfile, canonicalShift);
}


string FactoredProduct::strHeader() const
{
  return canonicalPtr->strHeader();
}


string FactoredProduct::strLine() const
{
  return canonicalPtr->strLine();
}


string FactoredProduct::strVerbal(
  const Profile& sumProfile,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  return canonicalPtr->strVerbal(sumProfile, simplestOpponent, symmFlag);
}

