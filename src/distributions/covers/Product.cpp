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
#include "ProfilePair.h"


Product::Product()
{
  Product::reset();
}


void Product::reset()
{
  complexity = 0;
  length.reset();
  tops.clear();
  topSize = 0;
  topCount = 0;
}


void Product::resize(const unsigned compSize)
{
  tops.resize(compSize);
}


void Product::set(
  const Profile& sumProfile,
  const Profile& lowerProfile,
  const Profile& upperProfile)
{
  length.set(
    sumProfile.getLength(), 
    lowerProfile.getLength(), 
    upperProfile.getLength());

  complexity = length.complexity();
  range = length.range();

  const unsigned topLowSize = lowerProfile.size();
  assert(upperProfile.size() == topLowSize);
  assert(tops.size() >= topLowSize);

  // Always skip the first one.
  for (unsigned char i = 1; i < topLowSize; i++)
  {
    tops[i].set(
      sumProfile.count(i), 
      lowerProfile.count(i), 
      upperProfile.count(i));

    // Note the first, i.e. lowest one.
    if (tops[i].used())
    {
      if (topSize == 0)
        topSize = i;

      topCount++;
    }

    complexity += tops[i].complexity();
    range += tops[i].range();
  }

  // If there is only a single distribution possible, this counts
  // as a complexity of 2 (don't make it absurdly attractive).
  if (range == 0 && length.used() && 
    topCount+1 == static_cast<unsigned char>(topLowSize))
  {
    complexity = 2;
  }
}


void Product::set(
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  Product::set(
    sumProfile, 
    profilePair.lowerProfile, 
    profilePair.upperProfile);
}


bool Product::includes(const Profile& distProfile) const
{
  if (length.used() && ! length.includes(distProfile.getLength()))
    return false;

  assert(distProfile.size() == tops.size());

  for (unsigned char i = 0; i < distProfile.size(); i++)
  {
    if (tops[i].used() && 
        ! tops[i].includes(distProfile.count(i)))
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
  unsigned char sum = length.range();
  for (auto& top: tops)
    sum += top.range();

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


Opponent Product::simplestOpponent(const Profile& sumProfile) const
{
  // We want to express a Product in terms that make as much
  // intuitive sense as possible.  I think this tends to be in
  // terms of shortness.

  // With 6 cards, we generally want 1-4 to remain, but 2-5 to be 
  // considered as 1-4 from the other side.
  Opponent backstop = OPP_WEST;
  const Opponent lOpp = length.simplestOpponent(sumProfile.getLength());

  if (lOpp == OPP_WEST)
    return OPP_WEST;
  else if (lOpp == OPP_EAST)
  {
    // Special case: This is easier to say as "not void".
    if (length.notVoid())
    {
      backstop = OPP_EAST;
    }
    else
      return OPP_EAST;
  }
  
  const unsigned char s = static_cast<unsigned char>(tops.size());

  // Start from the highest top.
  for (unsigned char i = s; --i > 0; )
  {
    const Opponent lTop = tops[i].simplestOpponent(sumProfile.count(i));
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
  // more in CoverNew.
  stringstream ss;

  ss << setw(8) << "Length";
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << ("Top #" + to_string(i));
  ss << setw(8) << "symm";

  return ss.str();
}


string Product::strLine(const Profile& sumProfile) const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in CoverNew.
  stringstream ss;

  ss << setw(8) << length.strGeneral();

  assert(tops.size() == sumProfile.size());
  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << tops[i].strGeneral();

  return ss.str();
}


string Product::strLine() const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in CoverNew.
  stringstream ss;

  ss << setw(8) << length.strGeneral();

  for (unsigned i = 0; i < tops.size(); i++)
    ss << setw(8) << tops[i].strGeneral();

  return ss.str();
}


string Product::strVerbal(
  const Profile& sumProfile,
  const Opponent simplestOpponent,
  const bool symmFlag) const
{
  if (topCount == 0)
  {
    return length.strLength(
      sumProfile.getLength(),
      simplestOpponent, 
      symmFlag);
  }

  if (! length.used())
  {
    return tops.back().strTop(
      sumProfile.count(static_cast<unsigned char>(sumProfile.size()-1)),
      simplestOpponent, 
      symmFlag);
  }

  auto& top = tops.back();

  if (top.getOperator() == COVER_EQUAL)
  {
    return top.strWithLength(
      length,
      sumProfile.getLength(), 
      sumProfile.count(static_cast<unsigned char>(sumProfile.size()-1)),
      simplestOpponent,
      symmFlag);
  }
  else
  {
    return 
      length.strLength(
        sumProfile.getLength(), 
        simplestOpponent, 
        symmFlag) + 
      ", and " + 
      top.strTop(
        sumProfile.count(static_cast<unsigned char>(sumProfile.size()-1)),
        simplestOpponent, 
        symmFlag);
  }
}

