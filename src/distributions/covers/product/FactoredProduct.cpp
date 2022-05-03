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
  return noncanonicalPtr->code();
}


bool FactoredProduct::includes(const Profile& distProfile) const
{
  const bool b = noncanonicalPtr->includes(distProfile);
  // assert(b == canonicalPtr->includes(distProfile, canonicalShift));
  return b;
}


bool FactoredProduct::symmetrizable(const Profile& sumProfile) const
{
  const bool b = noncanonicalPtr->symmetrizable(sumProfile);
  // assert(b == canonicalPtr->symmetrizable(sumProfile, canonicalShift));
  return b;
}


unsigned char FactoredProduct::getComplexity() const
{
  const unsigned char c = noncanonicalPtr->getComplexity();
  // assert(c == canonicalPtr->getComplexity());
  return c;
}


unsigned char FactoredProduct::effectiveDepth() const
{
  const unsigned char e = noncanonicalPtr->effectiveDepth();
  // assert(e == canonicalPtr->effectiveDepth());
  return e;
}


bool FactoredProduct::explainable() const
{
  const bool b = noncanonicalPtr->explainable();
  // assert(b == canonicalPtr->explainable());
  return b;
}


Opponent FactoredProduct::simplestOpponent(const Profile& sumProfile) const
{
  const Opponent o = noncanonicalPtr->simplestOpponent(sumProfile);
  // assert(o == canonicalPtr->simplestOpponent(sumProfile, canonicalShift));
  return o;
}


string FactoredProduct::strHeader() const
{
  return noncanonicalPtr->strHeader();
}


string FactoredProduct::strLine() const
{
  return noncanonicalPtr->strLine();
}


string FactoredProduct::strVerbal(
  const Profile& sumProfile,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  const string s =
    noncanonicalPtr->strVerbal(sumProfile, simplestOpponent, symmFlag);
  // assert(s == canonicalPtr->strVerbal(sumProfile, simplestOpponent,
    // symmFlag));
  return s;
}

