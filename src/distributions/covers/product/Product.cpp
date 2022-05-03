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
#include "Profile.h"


Product::Product()
{
  Product::reset();
}


void Product::reset()
{
  length.reset();
  tops.clear();
  codeInt = 0;
  complexity = 0;
  topSize = 0;
  activeCount = 0;
}


void Product::resize(const unsigned topCount)
{
  tops.resize(topCount);
}


void Product::set(
  const Profile& sumProfile,
  const Profile& lowerProfile,
  const Profile& upperProfile,
  const unsigned long long code)
{
  codeInt = code;

  length.set(
    sumProfile.length(), 
    lowerProfile.length(), 
    upperProfile.length());

  complexity = length.complexity();

  const unsigned topLowSize = lowerProfile.size();
  assert(upperProfile.size() == topLowSize);
  assert(tops.size() >= topLowSize);

  // Always skip the first one.
  for (unsigned char i = 1; i < topLowSize; i++)
  {
    tops[i].set(
      sumProfile[i], 
      lowerProfile[i], 
      upperProfile[i]);

    // Note the first, i.e. lowest one.
    if (tops[i].used())
    {
      if (topSize == 0)
        topSize = i;

      activeCount++;
    }

    complexity += tops[i].complexity();
  }

  // If there is only a single distribution possible, this counts
  // as a complexity of 3 (don't make it absurdly attractive).
  // TODO Relies on the profile being "minimal", as otherwise the
  // empty terms make the product seem not full.  So maybe this
  // correction is not a good idea?
  if (length.used() && 
      complexity > 3 &&
      activeCount+1 == static_cast<unsigned char>(topLowSize) &&
      lowerProfile == upperProfile)
  {
    complexity = 3;
  }
}



bool Product::includes(const Profile& distProfile) const
{
  if (length.used() && ! length.includes(distProfile.length()))
    return false;

  assert(distProfile.size() == tops.size());

  for (unsigned char i = 0; i < distProfile.size(); i++)
  {
    if (tops[i].used() && ! tops[i].includes(distProfile[i]))
      return false;
  }

  return true;
}


bool Product::symmetrizable(const Profile& sumProfile) const
{
  unsigned consecutive = 0;
  if (length.used())
  {
    SymmTerm symmLength = length.symmetrizable(sumProfile.length());
    if (symmLength == TERM_SYMMETRIZABLE)
      return true;
    else if (symmLength == TERM_NOT_SYMMETRIZABLE)
      return false;
    else if (symmLength == TERM_OPEN_CONSECUTIVE)
      consecutive++;
  }

  // So now symmLength can be:
  // OPEN_CONSECUTIVE (1-2 of length 5), or
  // OPEN_CENTERED (exactly 2 of length 4).
  // If all set terms are centered, it is not really symmetrizable.
  // A single consecutive term is also not enough.

  for (unsigned char i = static_cast<unsigned char>(tops.size()); --i > 0; )
  {
    if (tops[i].used())
    {
      const SymmTerm sterm = tops[i].symmetrizable(sumProfile[i]);
      if (sterm == TERM_SYMMETRIZABLE)
        return true;
      else if (sterm == TERM_NOT_SYMMETRIZABLE)
        return false;
      else if (sterm == TERM_OPEN_CONSECUTIVE)
        consecutive++;
    }
  }

  return (consecutive > 1);
}


bool Product::canonical() const
{
  return (static_cast<unsigned>(activeCount + 1) == tops.size());
}


unsigned Product::getCanonicalShift() const
{
  return tops.size() - activeCount - 1;
}


unsigned char Product::getComplexity() const
{
  return complexity;
}


unsigned long long Product::code() const
{
  return codeInt;
}


unsigned char Product::size() const
{
  return static_cast<unsigned char>(tops.size());
}


unsigned char Product::effectiveDepth() const
{
  if (topSize == 0 || tops.empty())
    return 0;
  else
    return static_cast<unsigned char>(tops.size()) - topSize;
}


bool Product::explainable() const
{
  if (activeCount == 0)
    return true;
  else if (Product::effectiveDepth() == 1 && activeCount == 1)
    return true;
  else
    return false;
}


Opponent Product::simplestOpponent(const Profile& sumProfile) const
{
  // We want to express a Product in terms that make as much
  // intuitive sense as possible.  I think this tends to be in
  // terms of shortness.

  // With 6 cards, we generally want 1-4 to remain, but 2-5 to be 
  // considered as 1-4 from the other side.
  Opponent backstop = OPP_WEST;
  const Opponent lOpp = length.simplestOpponent(sumProfile.length());

  if (lOpp == OPP_WEST)
    return OPP_WEST;
  else if (lOpp == OPP_EAST)
  {
    // Special case: This is easier to say as "not void".
    if (length.notVoid())
      backstop = OPP_EAST;
    else
      return OPP_EAST;
  }
  
  const unsigned char s = static_cast<unsigned char>(tops.size());

  // Start from the highest top.
  for (unsigned char i = s; --i > 0; )
  {
    const Opponent lTop = tops[i].simplestOpponent(sumProfile[i]);
    if (lTop == OPP_WEST)
      return OPP_WEST;
    else if (lTop == OPP_EAST)
      return OPP_EAST;
  }

  return backstop;
}


string Product::strHeader() const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in Cover.
  stringstream ss;

  ss << setw(8) << "Length";
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << ("Top #" + to_string(i));

  return ss.str();
}


string Product::strLine() const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in Cover.
  stringstream ss;

  ss << setw(8) << length.strGeneral();

  for (auto& top: tops)
    ss << setw(8) << top.strGeneral();

  return ss.str();
}


string Product::strVerbal(
  const Profile& sumProfile,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  if (activeCount == 0)
  {
    return length.strLength(
      sumProfile.length(),
      simplestOpponent, 
      symmFlag);
  }

  const unsigned char highestTopCount =
    sumProfile[static_cast<unsigned char>(sumProfile.size()-1)];

  if (! length.used())
  {
    return tops.back().strTop(
      highestTopCount,
      simplestOpponent, 
      symmFlag);
  }

  auto& top = tops.back();

  if (top.getOperator() == COVER_EQUAL)
  {
    return top.strEqualWithLength(
      length,
      sumProfile.length(), 
      highestTopCount,
      simplestOpponent,
      symmFlag);
  }
  else if (length.getOperator() == COVER_EQUAL)
  {
    // No inversion, but "has a doubleton with one or both tops"
    return 
      length.strLength(
        sumProfile.length(), 
        simplestOpponent, 
        symmFlag) + 
      " with " +
      top.strTopBare(
        highestTopCount,
        simplestOpponent);
  }
  else
  {
    // Inversion, e.g. "has one top at most doubleton"
    return 
      top.strTop(
        highestTopCount,
        simplestOpponent, 
        symmFlag) +
      " " +
      length.strLengthBare(
        sumProfile.length(), 
        simplestOpponent);
  }
}

