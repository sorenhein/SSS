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
#include "../term/Xes.h"

#include "../../../ranks/RanksNames.h"
#include "../../../ranks/RankNames.h"

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

  bool nonEqualFlag = false;

  // Always skip the first one.
  for (unsigned char i = 1; i < tops.size(); i++)
  {
    const auto& lower = lowerProfile[i + canonicalShift];
    const auto& upper = upperProfile[i + canonicalShift];

    tops[i].set(sumProfile[i + canonicalShift], lower, upper);

    if (lower != upper)
      nonEqualFlag = true;

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

  if (! nonEqualFlag && activeCount > 1)
  {
    // Don't count multiple equal tops (that are unmarred by
    // unequal ones) as 2 each, but as 2,3,4,5,...

    complexity = length.complexity() + activeCount + 1;

    // Add 1 if the tops are not the consecutive highest ones.
    if (tops.size() > topSize + static_cast<size_t>(activeCount))
      complexity++;
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


bool Product::discardSymmetric(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  // Some singular covers end up duplicated in such a way that it
  // complicates the string output.  For example, with HHT8 one symmetric
  // cover may be length 2, exactly 1 H and one 1 T.  But another
  // symmetric cover mab be length 2, exactly 1 H.  The second card is
  // unspecified and may be 8 or T, but the symmetry makes the covers
  // the same.  In general this can happen when there are exactly two
  // top slots left open (the bottom one which is always unset, and
  // one other), and there the lengths are balanced.
  if (! length.used() ||
      length.getOperator() != COVER_EQUAL ||
      2 * length.lower() != sumProfile.length())
    return false;
  else if (canonicalShift >= 2 || activeCount == 0)
    return false;
  else if (activeCount + 2 != 
      static_cast<unsigned char>(tops.size() + canonicalShift))
    return false;
  else
    return true;
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


CoverVerbal Product::verbal() const
{
  // This only chooses among those options that apply to a Product,
  // so no singular options and not VERBAL_HEURISTIC as this applies
  // to a CoverRow with two Covers, not to a Product.
  
  if (activeCount == 0)
  {
    assert(length.used());
    return VERBAL_LENGTH_ONLY;
  }

  const ExplainEqual ee = Product::mostlyEqual();
  if (ee == EQUAL_FROM_TOP)
    return VERBAL_HIGH_TOPS_EQUAL;
  else if (ee == EQUAL_ANY)
    return VERBAL_ANY_TOPS_EQUAL;

  if (activeCount >= 2)
    return VERBAL_GENERAL;
  else if (! length.used())
    return VERBAL_TOPS_ONLY;
  else
    return VERBAL_LENGTH_AND_ONE_TOP;
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


Opponent Product::simplestOpponent(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  if (length.used() && length.getOperator() == COVER_EQUAL)
    return Product::simplestSingular(sumProfile, canonicalShift);

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


Opponent Product::simplestSingular(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  assert(length.used());
  assert(length.getOperator() == COVER_EQUAL);
  const auto lWest = length.lower();
  const auto lSum = sumProfile.length();
  
  // Pick a side that is 2+ cards shorter than the other.
  if (lWest + 1 < lSum - lWest)
    return OPP_WEST;
  else if (lSum - lWest + 1 < lWest)
    return OPP_EAST;

  const unsigned char s = static_cast<unsigned char>(tops.size());
  assert(static_cast<unsigned>(s + canonicalShift) == sumProfile.size());

  // Start from the highest top.
  for (unsigned char i = s; --i > 0; )
  {
    // TODO if tops[i].used() ??
    const Opponent lTop = 
      tops[i].simplestOpponent(sumProfile[i + canonicalShift]);

    if (lTop == OPP_WEST)
      return OPP_WEST;
    else if (lTop == OPP_EAST)
      return OPP_EAST;
  }

  if (lWest <= lSum - lWest)
    return OPP_WEST;
  else
    return OPP_EAST;
}


bool Product::simplerThan(const Product& p2) const
{
  const unsigned char s = static_cast<unsigned char>(tops.size());
  assert(p2.tops.size() == s);

  assert(length.used());
  assert(length.getOperator() == COVER_EQUAL);
  assert(p2.length.used());
  assert(p2.length.getOperator() == COVER_EQUAL);

  if (length.lower() < p2.length.lower())
    return true;
  else if (p2.length.lower() < length.lower())
    return false;

/*
  // Pick a side that is 2+ cards shorter than the other.
  // But a singleton always wins.
  if (length.lower() <= 1 && p2.length.lower() > 1)
    return true;
  else if (p2.length.lower() <= 1 && length.lower() > 1)
    return false;
  else if (length.lower() + 1 < p2.length.lower())
    return true;
  else if (p2.length.lower() + 1 < length.lower())
    return false;
    */


  // Start from the highest top.  Include the 0 top.
  for (unsigned char topNo = s; topNo-- > 0; )
  {
    const auto& top1 = tops[topNo];
    const auto& top2 = p2.tops[topNo];
    assert(top1.used() && top2.used());

    if (top1.lower() > top2.lower())
      return true;
    else if (top2.lower() > top1.lower())
      return false;
  }

  // Backstop.
  return (length.lower() <= p2.length.lower());
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


void Product::characterize(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const Opponent simplestOpponent,
  const unsigned char canonicalShift,
  unsigned char& actualTops,
  unsigned char& otherActualTops,
  unsigned char& actualLength,
  unsigned char& otherActualLength,
  unsigned char& hidden) const
{
  // TODO
  // Don't really need ranksNames.  Instead look up in sumProfile
  // directly.
  unsigned char westActualTops = 0;
  unsigned char eastActualTops = 0;
  hidden = 0;
  
  TopData topData;
  unsigned char topNo;

  // Skip the lowest entry (unused).
  for (topNo = static_cast<unsigned char>(tops.size()); --topNo > 0; )
  {
    const auto& top = tops[topNo];
    sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);

    if (top.used())
    {
      assert(top.getOperator() == COVER_EQUAL);
      westActualTops += top.lower();
      eastActualTops += topData.value - top.lower();
    }
  }

  // Add any that are hidden by the canonical shift.
  for (unsigned char r = 0; r <= canonicalShift; r++)
  {
    sumProfile.getTopData(r, ranksNames, topData);
    hidden += topData.value;
  }

  if (simplestOpponent == OPP_EAST)
  {
    actualTops = eastActualTops;
    otherActualTops = westActualTops;
    actualLength = sumProfile.length() - length.lower();
    otherActualLength = length.lower();
  }
  else
  {
    actualTops = westActualTops;
    otherActualTops = eastActualTops;
    actualLength = length.lower();
    otherActualLength = sumProfile.length() - length.lower();
  }
}


void Product::getWestLengths(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const Opponent simplestOpponent,
  const unsigned char canonicalShift,
  unsigned char& xesMin,
  unsigned char& xesMax,
  unsigned char& xesHidden,
  unsigned char& xesAvailable,
  unsigned char& topsExact,
  unsigned char& topsAvailable) const
{
  // Calculate the number of free West cards for the current Product and
  // for the whole sumProfile.  For example, if only the lowest rank
  // (never set) is free, then we calculate the number of x's that West
  // may hold as well as the available maximum number of x's.
  // If only some tops above the lowest one are set, it's the same idea,
  // but with the number of cards outside of those ranks.
  unsigned char distLengthLower;
  unsigned char distLengthUpper;
  topsExact = 0;
  const unsigned char oppsLength = sumProfile.length();
  unsigned char oppsTops = 0;
  topsAvailable = 0;

  unsigned char topsExactEast = 0;

  if (length.used())
  {
    // length could be of the >= kind.
    distLengthLower = length.lower();
    distLengthUpper = min(oppsLength, length.upper());
  }
  else
  {
    distLengthLower = 0;
    distLengthUpper = oppsLength;
  }

  TopData topData;
  unsigned char topNo;

  // Skip the lowest entry (unused).
  for (topNo = static_cast<unsigned char>(tops.size()); --topNo > 0; )
  {
    const auto& top = tops[topNo];
    sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);
    oppsTops += topData.value;

    if (! top.used())
    {
      topsAvailable += topData.value;
      continue;
    }
    else
      topsAvailable += top.lower();

    assert(top.getOperator() == COVER_EQUAL);

    topsExact += top.lower();
    topsExactEast += topData.value - top.lower();
  }

  xesHidden = 0;
  // Add any that are hidden by the canonical shift.
  for (unsigned char r = 0; r <= canonicalShift; r++)
  {
    sumProfile.getTopData(canonicalShift - r, ranksNames, topData);
    xesHidden += topData.value;
  }


/*
cout << "Product " << Product::strLine() << endl;
cout << "Xes: sumProfile " << sumProfile.strLine() << endl;
cout << "distLength " << +distLengthLower << " to " <<
  +distLengthUpper << endl;
cout << "topsExact " << +topsExact << endl;
cout << "oppsLength " << +oppsLength << endl;
cout << "oppsTops " << +oppsTops << endl;
*/

  Xes xes;
  xes.set(distLengthLower, distLengthUpper, topsExact, 
    oppsLength, oppsTops);
  xes.getRange(simplestOpponent, xesMin, xesMax);

  xesAvailable = oppsLength - oppsTops;

  if (simplestOpponent == OPP_EAST)
    topsExact = topsExactEast;

// cout << "xes " << +xesMin << " to " << +xesMax << " avail " <<
  // +xesAvailable << endl;
  if (! length.used())
    assert(xesMin == 0);
}


string Product::strExactTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const Opponent simplestOpponent,
  const unsigned char canonicalShift,
  const  bool skipUnusedFlag) const
{
  TopData topData;
  unsigned char topNo;
  string result;

  for (topNo = static_cast<unsigned char>(tops.size()); --topNo > 0; )
  {
    // This leaves out the lowest top (number 0).
    const auto& top = tops[topNo];

    if (! skipUnusedFlag && ! top.used())
    {
      // Add the whole rank string.
      sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);
      result += topData.strTops(topData.value);
    }
    else if (top.used())
    {
      // Add some (or all) of the rank string.
      assert(top.getOperator() == COVER_EQUAL);
      sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);
      result += topData.strTops(
        (simplestOpponent == OPP_EAST ? topData.value - top.lower() :
          top.lower()));
    }
  }

  return result;
}


string Product::strAvailableTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift) const
{
  TopData topData;
  unsigned char topNo;
  string result;

  for (topNo = static_cast<unsigned char>(tops.size()); topNo-- > 0; )
  {
    if (! tops[topNo].used())
      continue;

    const auto& top = tops[topNo];
    assert(top.getOperator() == COVER_EQUAL);

    sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);

    result += topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);
  }
  return result;
}


string Product::strVerbalLengthOnly(
  const Profile& sumProfile,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount == 0);

  const Opponent simplestOpponent =
    Product::simplestOpponent(sumProfile, canonicalShift);

  return 
    length.strLength(
      sumProfile.length(), 
      simplestOpponent, 
      symmFlag);
}


string Product::strVerbalOneTopOnly(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount == 1);

  const Opponent simplestOpponent =
    Product::simplestOpponent(sumProfile, canonicalShift);

  TopData topData;
  unsigned char topNo;

  for (topNo = static_cast<unsigned char>(tops.size()); topNo-- > 0; )
  {
    if (! tops[topNo].used())
      continue;

    sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);

    return tops[topNo].strTop(
      topData,
      simplestOpponent, 
      symmFlag);
  }

  assert(false);
  return "";
}


string Product::strVerbalLengthAndOneTop(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount == 1);
  assert(length.used());

  const Opponent simplestOpponent =
    Product::simplestOpponent(sumProfile, canonicalShift);

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


string Product::strVerbalEqualTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const CoverVerbal verbal,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount > 0);

  const Opponent simplestOpponent =
    Product::simplestOpponent(sumProfile, canonicalShift);

  string result;
  if (symmFlag)
    result = "Either opponent";
  else if (simplestOpponent == OPP_WEST)
    result = "West";
  else
    result = "East";

  result += " has ";

  const string exact = Product::strExactTops(sumProfile, ranksNames, 
    simplestOpponent, canonicalShift, true);
  const string avail = Product::strAvailableTops(sumProfile, ranksNames, 
      canonicalShift);

  if (exact == "")
    result += "none";
  else
    result += exact;

  result += " out of " + avail;

  unsigned char xesMin, xesMax, xesAvailable, xesHidden,
    topsExact, topsAvailable;
  Product::getWestLengths(
    sumProfile, 
    ranksNames, 
    simplestOpponent,
    canonicalShift,
    xesMin,
    xesMax,
    xesHidden,
    xesAvailable,
    topsExact,
    topsAvailable);

  string other;
  if (verbal == VERBAL_HIGH_TOPS_EQUAL)
    other = " lower cards";
  else if (verbal == VERBAL_ANY_TOPS_EQUAL)
    other = " cards of other ranks";
  else
    assert(false);

  if (xesMin == xesMax)
    result += " and " + to_string(xesMin) + other;
  else
  {
    result += " and " + to_string(xesMin) + "-" +
      to_string(xesMax) + other;
  }

  return result;
}


void Product::fillUsedTops(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  Product& productWest,
  Product& productEast,
  unsigned char& westTops,
  unsigned char& eastTops) const
{
  const unsigned char slength = sumProfile.length();
  const unsigned char wlength = length.lower();
  productWest.length = length;
  productEast.length.set(slength, slength - wlength, slength - wlength);

  westTops = 0;
  eastTops = 0;

  for (unsigned char topNo = 0; topNo < tops.size(); topNo++)
  {
    auto& top = tops[topNo];
    if (! top.used())
      continue;

    const unsigned char tlength = sumProfile[topNo + canonicalShift];
    productWest.tops[topNo] = top;
    productEast.tops[topNo].set(
      tlength,
      tlength - top.lower(),
      tlength - top.lower());

    westTops += top.lower();
    eastTops += tlength - top.lower();
  }
}


void Product::fillUnusedTops(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const Opponent fillOpponent,
  Product& productWest,
  Product& productEast) const
{
  // The side specified by fillOpponent gets all unused tops except
  // the lowest one, which remains unset.
  for (unsigned char topNo = 1; topNo < tops.size(); topNo++)
  {
    auto& top = tops[topNo];
    if (top.used())
      continue;

    const unsigned char tlength = sumProfile[topNo + canonicalShift];

    if (fillOpponent == OPP_WEST)
    {
      productWest.tops[topNo].set(tlength, tlength, tlength);
      productEast.tops[topNo].set(tlength, 0, 0);
    }
    else
    {
      productWest.tops[topNo].set(tlength, 0, 0);
      productEast.tops[topNo].set(tlength, tlength, tlength);
    }
  }
}


unsigned char Product::countHidden(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  // Add any that are hidden by the canonical shift.
  // TODO Could even be a method in sumProfile.
  unsigned char count = 0;

  for (unsigned char hiddenNo = 0; hiddenNo <= canonicalShift; hiddenNo++)
    count += sumProfile[hiddenNo];

  return count;
}


void Product::fillSideBottoms(
  const Opponent fillOpponent,
  const unsigned char hidden,
  Product& productWest,
  Product& productEast) const
{
  // The side specified by fillOpponent gets all unused bottoms.
  // This may span several ranks, depending on canonicalShift.
  if (fillOpponent == OPP_WEST)
  {
    productWest.tops[0].set(hidden, hidden, hidden);
    productEast.tops[0].set(hidden, 0, 0);
  }
  else
  {
    productWest.tops[0].set(hidden, 0, 0);
    productEast.tops[0].set(hidden, hidden, hidden);
  }
}


void Product::separateSingular(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  Product& productWest,
  Product& productEast) const
{
  // Fill the West and East products out completely with the single
  // option that exists.
  productWest.resize(tops.size());
  productEast.resize(tops.size());
  
  assert(length.used());
  assert(length.getOperator() == COVER_EQUAL);

  unsigned char westTops, eastTops;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast,
    westTops, eastTops);

// cout << "filled used tops West " << productWest.strLine() << endl;
// cout << "filled used tops East " << productEast.strLine() << endl;

  const unsigned char slength = sumProfile.length();
  const unsigned char wlength = length.lower();

  const unsigned char hidden = 
    Product::countHidden(sumProfile, canonicalShift);

// cout << "hidden " << +hidden << endl;

  if (westTops == wlength)
  {
    // The unused tops are 0 for West, maximum for East.
    // East also gets any low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_EAST,
      productWest, productEast);

// cout << "Case 1 unused West " << productWest.strLine() << endl;
// cout << "Case 1 unused East " << productEast.strLine() << endl;

    Product::fillSideBottoms(OPP_EAST, hidden, productWest, productEast);
  }
  else if (eastTops == slength - wlength)
  {
    // The unused tops are 0 for East, maximum for West.
    // West also gets any low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_WEST,
      productWest, productEast);

// cout << "Case 2 unused West " << productWest.strLine() << endl;
// cout << "Case 2 unused East " << productEast.strLine() << endl;

    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
  }
  else if (westTops + hidden == wlength)
  {
    // East gets any unused tops.  West gets all the low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_EAST,
      productWest, productEast);

// cout << "Case 3 unused West " << productWest.strLine() << endl;
// cout << "Case 3 unused East " << productEast.strLine() << endl;

    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
  }
  else if (eastTops + hidden == slength - wlength)
  {
    // West gets any unused tops.  East gets all the low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_WEST,
      productWest, productEast);

// cout << "Case 4 unused West " << productWest.strLine() << endl;
// cout << "Case 4 unused East " << productEast.strLine() << endl;

    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
    Product::fillSideBottoms(OPP_EAST, hidden, productWest, productEast);
  }
  else if (canonicalShift == 0)
  {
    // There should be no tops left to fill out, as all are used.
    // Add the right number of low cards to each side.
    assert(static_cast<unsigned char>(activeCount+1) == tops.size());

    const unsigned char westXes = wlength - westTops;
    const unsigned char allXes = sumProfile[0];
    productWest.tops[0].set(allXes, westXes, westXes);
    productEast.tops[0].set(allXes, allXes - westXes, allXes - westXes);
  }
  else
    assert(false);
}


string Product::strExactTop(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const unsigned char topNo) const
{
  const auto& top = tops[topNo];
  assert(top.used());

  if (top.lower() == 0)
    return "";

  TopData topData;
// cout << "Filling " << +topNo << ": " << +top.lower() << endl;
  sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);
  return topData.strTops(top.lower());
}


string Product::strExact(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const string& anchor,
  const unsigned char canonicalShift) const
{
  string start = anchor + " has ";

  if (! length.used())
    start += "exactly ";
  else if (length.lower() == 1)
    start += "the singleton ";
  else if (length.lower() == 2)
    start += "the doubleton ";
  else
    start += "exactly ";

  TopData topData;
  string result = "";
  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    --topNo > 0; )
  {
    // This excludes the lowest top (number 0).
    result += Product::strExactTop(sumProfile, ranksNames, 
      canonicalShift, topNo);
  }

  if (canonicalShift == 0)
  {
    // Same principle.
    result += Product::strExactTop(sumProfile, ranksNames, canonicalShift, 0);
  }
  else if (tops[0].lower() > 0)
  {
    // All the low cards.
    for (unsigned char hiddenNo = canonicalShift+1; hiddenNo-- > 0; )
    {
// cout << "FillingY " << +hiddenNo << ": " << +sumProfile[hiddenNo] << endl;
      sumProfile.getTopData(hiddenNo, ranksNames, topData);
      result += topData.strTops(topData.value);
    }
  }

  if (result.size() != length.lower())
  {
    cout << "result " << result << endl;
    cout << "this product " << Product::strLine() << endl;
  assert(result.size() == length.lower());
  }

  return start + result;
}


string Product::strVerbalSingular(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const CoverVerbal verbal,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  Opponent simplestOpponent;
  if (verbal == VERBAL_SINGULAR_EITHER)
    simplestOpponent = OPP_EITHER;
  else if (verbal == VERBAL_SINGULAR_WEST)
    simplestOpponent = OPP_WEST;
  else
    simplestOpponent = OPP_EAST;

  if (activeCount == 0)
  {
    // This can, happen, e.g. 9/18975, JT96/7 missing AKQ8.
    // One cover applies to d == 3 or 4, so Hx/HH or HH/Hx.
    // This gets classified as a VERBAL_SINGULAR_EITHER as it is
    // symmetric and covers two distributions.
    // TODO Can we detect and skip this too in CoverStore?
    // symmetric, singular, yet no length set
    return length.strLength(
      sumProfile.length(), 
      simplestOpponent, 
      symmFlag);
  }

  Product productWest, productEast;
  Product::separateSingular(sumProfile, canonicalShift, 
    productWest, productEast);

// cout << "\nsum profile " << sumProfile.strLine() << endl;
// cout << "initial product West " << Product::strLine() << endl;
// cout << "final product West " << productWest.strLine() << endl;
// cout << "final product East " << productEast.strLine() << endl;

  string resultNew = "";
  if (productWest.simplerThan(productEast))
  {
// cout << "West is simpler" << endl;
    resultNew = productWest.strExact(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "West"), canonicalShift);

// cout << "rnew " << resultNew << endl;
    // assert(productWest.length.lower() == resultNew.size());
  }
  else
  {
// cout << "East is simpler" << endl;
    resultNew = productEast.strExact(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "East"), canonicalShift);

// cout << "rnew " << resultNew << endl;
    // assert(productEast.length.lower() == resultNew.size());
  }


  return resultNew;

  if (length.used())
    assert(length.getOperator() == COVER_EQUAL);

  if (length.getOperator() != COVER_EQUAL)
    cout << "UNEXPECTED: " << Product::strLine() << endl;

  // string start = "{len " + to_string(sumProfile.length()) + "} ";
  string start = "";

  if (symmFlag)
    start += "Either opponent";
  else if (simplestOpponent == OPP_WEST)
    start += "West";
  else
    start += "East";

  start += " has ";

  if (! length.used())
    start += "exactly ";
  else if (simplestOpponent == OPP_EAST)
  {
    if (sumProfile.length() - length.lower() == 1)
      start += "the singleton ";
    else if (sumProfile.length() - length.lower() == 2)
      start += "the doubleton ";
    else
      start += "exactly ";
  }
  else
  {
    if (length.lower() == 1)
      start += "the singleton ";
    else if (length.lower() == 2)
      start += "the doubleton ";
    else
      start += "exactly ";
  }

  // TODO This part is quite hideous and should go somewhere else,
  // possibly in Top.cpp

  unsigned char actualTops, otherActualTops;
  unsigned char actualLength, otherActualLength;
  unsigned char hidden;
  Product::characterize(
    sumProfile, 
    ranksNames, 
    simplestOpponent,
    canonicalShift,
    actualTops,
    otherActualTops,
    actualLength,
    otherActualLength,
    hidden);

  string result;

  if (actualTops == actualLength)
  {
// cout <<"Case 1" << endl;
    // Just fill out the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);
  }
  else if (otherActualTops == otherActualLength)
  {
// cout <<"Case 2" << endl;
    // Fill out the used and all the unused equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, false);

    // Add all low cards.
    for (unsigned char r = 0; r <= canonicalShift; r++)
    {
      TopData topData;
      sumProfile.getTopData(canonicalShift - r, ranksNames, topData);
      result +=
        topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);
    }
  }
  else if (actualTops + hidden == actualLength)
  {
// cout <<"Case 3" << endl;
    // Start with the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);

    // Add all low cards.
    for (unsigned char r = 0; r <= canonicalShift; r++)
    {
      TopData topData;
      sumProfile.getTopData(canonicalShift - r, ranksNames, topData);
      result +=
        topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);
    }
  }
  else if (canonicalShift == 0)
  {
// cout <<"Case 4" << endl;
    // Start with the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);

    // Add the right number of low cards.
    TopData topData;
    sumProfile.getTopData(canonicalShift, ranksNames, topData);

    // TODO Use 'x' not the actual name?  Fix RankNames?
    // In 11/136397 #4 we have Hh and we have A and 4 available.
    // This code picks 4 which is OK, but perhaps A is easier
    // to comprehend.
    const string str = 
      topData.rankNamesPtr->strComponent(RANKNAME_ABSOLUTE_SHORT);

    result += str.substr(0, actualLength - actualTops);
  }
  else
    assert(false);

if (actualLength != result.size())
{
  cout << "\nProduct      " << Product::strLine() << endl;
  cout << "sum profile " << sumProfile.strLine() << endl;
  cout << "simplestOpponent " << simplestOpponent << "\n";
  cout << "canonicalShift   " << +canonicalShift << "\n";
  cout << "actualTops       " << +actualTops << "\n";
  cout << "otherActualTops  " << +otherActualTops << "\n";
  cout << "actualLength     " << +actualLength << "\n";
  cout << "otherActualLen   " << +otherActualLength << "\n";
  cout << "hidden           " << +hidden << "\n";
  cout << "length.lower     " << +length.lower() << "\n";
  cout << "sum length       " << +sumProfile.length() << "\n";
  cout << "result           " << result << "\n";
}

  assert(actualLength == result.size());

  return start + result;
}


string Product::strVerbal(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const CoverVerbal verbal,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(verbal != VERBAL_GENERAL && verbal != VERBAL_HEURISTIC);

  if (verbal == VERBAL_LENGTH_ONLY)
  {
    return Product::strVerbalLengthOnly(
      sumProfile, 
      symmFlag, 
      canonicalShift);
  }
  else if (verbal == VERBAL_TOPS_ONLY)
  {
    return Product::strVerbalOneTopOnly(
      sumProfile, 
      ranksNames, 
      symmFlag, 
      canonicalShift);
  }
  else if (verbal == VERBAL_LENGTH_AND_ONE_TOP)
  {
    return Product::strVerbalLengthAndOneTop(
      sumProfile, 
      ranksNames, 
      symmFlag, 
      canonicalShift);
  }
  else if (verbal == VERBAL_HIGH_TOPS_EQUAL ||
      verbal == VERBAL_ANY_TOPS_EQUAL)
  {
    return Product::strVerbalEqualTops(
      sumProfile, 
      ranksNames, 
      verbal, 
      symmFlag, 
      canonicalShift);
  }
  else if (verbal == VERBAL_SINGULAR_EITHER ||
      verbal == VERBAL_SINGULAR_WEST ||
      verbal == VERBAL_SINGULAR_EAST)
  {
    return Product::strVerbalSingular(
      sumProfile, 
      ranksNames, 
      verbal, 
      symmFlag, 
      canonicalShift);
  }
  else
  {
    assert(false);
    return "";
  }
}

