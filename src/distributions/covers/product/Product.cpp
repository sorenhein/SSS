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

#include "../CoverCategory.h"

#include "../term/CoverOperator.h"
#include "../term/TopData.h"

#include "../../../utils/Compare.h"
#include "../../../utils/table.h"


enum ExplainEqual: unsigned
{
  EQUAL_FROM_TOP  = 0,
  EQUAL_ANY = 1,
  EQUAL_NONE = 2
};


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


void Product::resize(const size_t topCount)
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

  const size_t topLowSize = lowerProfile.size();
  assert(upperProfile.size() == topLowSize);
  assert(sumProfile.size() == topLowSize);
  assert(tops.size() <= topLowSize);

  const unsigned char canonicalShift = 
    static_cast<unsigned char>(topLowSize - tops.size());

  // Always skip the first one.
  // for (unsigned char i = 1; i < topLowSize; i++)
  for (unsigned char i = 1; i < tops.size(); i++)
  {
    tops[i].set(
      sumProfile[i + canonicalShift], 
      lowerProfile[i + canonicalShift], 
      upperProfile[i + canonicalShift]);

    // Note the first, i.e. lowest one.
    if (tops[i].used())
    {
      // TODO This just seems like a bug.  I'm not so sure about
      // topSize in this class in general
      if (topSize == 0)
        topSize = i;

      activeCount++;
    }

    complexity += tops[i].complexity();
  }

  // Aesthetic adjustment when the only active entries are equals.
  if (length.used() && 
      complexity > 3 &&
      lowerProfile.onlyEquals(upperProfile, sumProfile))
  {
    complexity = 3;
  }
}



bool Product::includes(
  const Profile& distProfile,
  const unsigned char canonicalShift) const
{
  if (length.used() && ! length.includes(distProfile.length()))
    return false;

  assert(tops.size() + canonicalShift == distProfile.size());

  for (unsigned char i = 0; i < tops.size(); i++)
  {
    if (tops[i].used() && ! tops[i].includes(
        distProfile[i + canonicalShift]))
      return false;
  }

  return true;
}


bool Product::symmetrizable(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  assert(tops.size() + canonicalShift == sumProfile.size());

  // We need at least one term that is set and symmetrizable on its own.

  bool uncenteredFlag = false;

  if (length.used())
  {
    SymmTerm symmLength = length.symmetrizable(sumProfile.length());
    if (symmLength == TERM_SYMMETRIZABLE_LOW)
      return true;
    else if (activeCount == 0)
    {
      // A single length can be described as well without symmetry.
      return false;
    }
    else if (symmLength == TERM_SYMMETRIZABLE_HIGH)
    {
      // The first symmetrize term should be a low one.
      return false;
    }
    else if (symmLength == TERM_OPEN_UNCENTERED_LOW)
    {
      // Needs something more -- not symmetrizable in itself.
      uncenteredFlag = true;
    }
    else if (symmLength == TERM_OPEN_UNCENTERED_HIGH)
    {
      // Will never be symmetrizable low.
      return false;
    }
  }

  for (unsigned char i = static_cast<unsigned char>(tops.size()); --i > 0; )
  {
    if (tops[i].used())
    {
      const SymmTerm sterm = tops[i].symmetrizable(
        sumProfile[i + canonicalShift]);

      if (sterm == TERM_SYMMETRIZABLE_LOW)
      {
        // Compatible with whatever came before.
        return true;
      }
      else if (sterm == TERM_SYMMETRIZABLE_HIGH)
      {
        if (uncenteredFlag)
          return true;
        else
          return false;
      }
      else if (sterm == TERM_OPEN_UNCENTERED_LOW)
      {
        if (uncenteredFlag)
          return true;
        else
          uncenteredFlag = true;
      }
      else if (sterm == TERM_OPEN_UNCENTERED_HIGH)
      {
        if (uncenteredFlag)
          return true;
        else
          return false;
      }
    }
  }

  return false;
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


bool Product::sameTops(const Product& product2) const
{
  if (tops.size() != product2.tops.size())
    return false;

  if (topSize != product2.topSize || activeCount != product2.activeCount)
    return false;

  // TODO topSize is probably messed up, but should be <= topSize?
  for (size_t t = 0; t < tops.size(); t++)
  {
    if (tops[t].used() != product2.tops[t].used())
      return false;

    if (! tops[t].used())
      continue;

    if (! (tops[t] == product2.tops[t]))
      return false;
  }

  return true;
}


unsigned char Product::minimumByTops(
  const Opponent voidSide,
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  // Determine the minimum number of cards that voidSide must have,
  // based only on the tops and ignoring the actual length.  This is
  // useful for determining whether a void is a valid extension of
  // a cover.
  
assert(tops.size() + canonicalShift == sumProfile.size());

  unsigned char min = 0;
  if (voidSide == OPP_WEST)
  {
    for (auto& top: tops)
    {
      if (! top.used())
        continue;

      min += top.lower();
    }
  }
  else
  {
    unsigned char no = 0;
    for (auto& top: tops)
    {
      if (! top.used())
      {
        no++;
        continue;
      }

      const unsigned char tu = top.upper();
      if (tu != 0xf)
      {
        // There is an actual upper bound.
        const unsigned char sno = no + canonicalShift;
        const unsigned char u = sumProfile[sno] - tu;
        min += u;
      }
      
      no++;
    }
  }
  
  return min;
}


CoverComposition Product::composition() const
{
  const bool lengthFlag = length.used();

  if (topSize == 0 || tops.empty())
  {
    if (lengthFlag)
      return EXPLAIN_LENGTH_ONLY;
    else
      return EXPLAIN_COMPOSITION_UNSET;
  }
  else if (lengthFlag)
    return EXPLAIN_MIXED_TERMS;
  else
    return EXPLAIN_TOPS_ONLY;
}


bool Product::lengthConsistent(const unsigned char specificLength) const
{
  return (length.used() &&
      length.getOperator() == COVER_EQUAL &&
      length.lower() == specificLength);
}


ExplainEqual Product::mostlyEqual() const
{
  // EQUAL_FROM_TOP: All the tops that are set are the highest ones.
  // EQUAL_ANY: They are not.
  // EQUAL_NONE: There are too many (> 1) tops that do not have a 
  // single value, but a range (>=, <=, a-b).
  // It turns out (in this context, so >= 2 active tops) that
  // we only really need tops that are set exactly.
  // Length may be set in any way, or not set.
  
  bool firstFlag = false;
  size_t firstSet = 0;

  bool lastFlag = false;
  size_t lastUnused = 0;

  for (size_t topNo = 0; topNo < tops.size(); topNo++)
  {
    const Top& top = tops[topNo];

    if (! top.used())
    {
      lastFlag = true;
      lastUnused = topNo;
      continue;
    }

    if (! firstFlag)
    {
      firstFlag = true;
      firstSet = topNo;
    }

    if (top.getOperator() != COVER_EQUAL)
      return EQUAL_NONE;
  }

  assert(firstFlag);
  if (! lastFlag || lastUnused < firstSet)
    return EQUAL_FROM_TOP;
  else
    return EQUAL_ANY;
}


bool Product::explainable() const
{
  // This is a relatively simple version that says whether there is 
  // a human-readable output.
  if (activeCount == 0)
    return true;
  else if (activeCount == 1)
    return true;
  else
    return false;
}


bool Product::explainableNew() const
{
  // This is a fuller version that determines whether a cover should
  // be kept in the central list at all.
  // TODO Once we have string outputs of all these, we can get rid
  // of the simpler explainable().
  if (activeCount == 0)
    return true;
  else if (activeCount == 1)
  {
    // Eliminate top range.
    // TODO I think this is right, but Manual needs them...
    /*
    for (auto& top: tops)
    {
      if (top.used() && top.getOperator() == COVER_INSIDE_RANGE)
        return false;
    }
    */

    return true;
  }
  else
    return (activeCount <= 5 && Product::mostlyEqual() != EQUAL_NONE);
}


Opponent Product::simplestOpponent(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
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
  assert(static_cast<unsigned>(s + canonicalShift) == sumProfile.size());

  // Start from the highest top.
  for (unsigned char i = s; --i > 0; )
  {
    const Opponent lTop = 
      tops[i].simplestOpponent(sumProfile[i + canonicalShift]);

    if (lTop == OPP_WEST)
      return OPP_WEST;
    else if (lTop == OPP_EAST)
      return OPP_EAST;
  }

  return backstop;
}


CompareType Product::presentOrder(const Product& product2) const
{
  const unsigned char ac1 = activeCount + 
    (length.used() ? 1 : 0);
  const unsigned char ac2 = product2.activeCount + 
    (product2.length.used() ? 1 : 0);

  if (ac1 < ac2)
    return WIN_FIRST;
  else if (ac1 > ac2)
    return WIN_SECOND;

  CompareType clen = length.presentOrder(product2.length);
  // See comment in Term::presentOrder.
  if (clen == WIN_FIRST || clen == WIN_SECOND)
    return clen;

  if (complexity < product2.complexity)
    return WIN_FIRST;
  else if (complexity > product2.complexity)
    return WIN_SECOND;

  unsigned char i, j;
  for (i = static_cast<unsigned char>(tops.size()),
       j = static_cast<unsigned char>(product2.tops.size()); 
       (--i > 0) && (--j > 0); )
  {
    // So up to now everything is identical.  Either nothing at all
    // is set, or at least one term is set, and these are identical.
    const CompareType ctop = tops[i].presentOrder(product2.tops[j]);
    if (ctop == WIN_FIRST || ctop == WIN_SECOND)
      return ctop;
  }

  // To have something.
  return WIN_FIRST;
}


string Product::strHeader(const unsigned lengthIn) const
{
  // Does not end on a linebreak, as it may be concatenated with
  // more in Cover.
  stringstream ss;

  const size_t tno = (lengthIn == 0 ? tops.size() : lengthIn);

  ss << setw(8) << "Length";
  for (unsigned i = 0; i < tno; i++)
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
  [[maybe_unused]] const RanksNames& ranksNames,
  const Opponent simplestOpponent,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  if (activeCount == 0)
  {
    return length.strLength(
      sumProfile.length(),
      simplestOpponent, 
      symmFlag);
  }

  TopData topData;
  unsigned char topNo;
  for (topNo = static_cast<unsigned char>(tops.size()); --topNo > 0; )
  {
    if (tops[topNo].used())
    {
      sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);
      break;
    }
  }
  assert(topData.used());

  if (! length.used())
  {
    return tops[topNo].strTop(
      topData,
      simplestOpponent, 
      symmFlag);
  }

  assert(topNo > 0 && topNo < tops.size());
  auto& top = tops[topNo];

  if (top.getOperator() == COVER_EQUAL)
  {
    return top.strEqualWithLength(
      length,
      sumProfile.length(), 
      topData,
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
        topData,
        simplestOpponent);
  }
  else
  {
    // Inversion, e.g. "has one top at most doubleton"
    return 
      top.strTop(
        topData,
        simplestOpponent, 
        symmFlag) +
      " " +
      length.strLengthBare(
        sumProfile.length(), 
        simplestOpponent);
  }
}

