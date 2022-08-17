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


void Product::characterize(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  unsigned char& westActualTops,
  unsigned char& eastActualTops,
  unsigned char& hidden,
  unsigned char& unsetTops) const
{
  westActualTops = 0;
  eastActualTops = 0;
  hidden = 0;
  unsetTops = 0;
  
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
    else
    {
      unsetTops += topData.value;
    }
  }

  // Add any that are hidden by the canonical shift.
  for (unsigned char r = 0; r <= canonicalShift; r++)
  {
    sumProfile.getTopData(r, ranksNames, topData);
    hidden += topData.value;
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

  // unsigned char freeWestMax, freeWestActual;
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
  else if (verbal ==VERBAL_SINGULAR_WEST)
    simplestOpponent = OPP_WEST;
  else
    simplestOpponent = OPP_EAST;

  if (activeCount == 0)
  {
    // This can, happen, e.g. 9/18975, JT96/7 missing AKQ8.
    // One cover applies to d == 3 or 4, so Hx/HH or HH/Hx.
    // This gets classified as a VERBAL_SINGULAR_EITHER as it is
    // symmetric and covers two distributions.
    return length.strLength(
      sumProfile.length(), 
      simplestOpponent, 
      symmFlag);
  }

  if (length.used())
    assert(length.getOperator() == COVER_EQUAL);

  if (length.getOperator() != COVER_EQUAL)
    cout << "UNEXPECTED: " << Product::strLine() << endl;

  string start;
  if (symmFlag)
    start = "Either opponent";
  else if (simplestOpponent == OPP_WEST)
    start = "West";
  else
    start = "East";

  start += " has exactly ";

  // TODO This part is quite hideous and should go somewhere else,
  // possibly in Top.cpp

  unsigned char westActualTops, eastActualTops;
  unsigned char hidden, unsetTops;
  Product::characterize(
    sumProfile, 
    ranksNames, 
    canonicalShift,
    westActualTops,
    eastActualTops,
    hidden, 
    unsetTops);

  const unsigned char actualTops = (simplestOpponent == OPP_EAST ?
    eastActualTops : westActualTops);
  const unsigned char otherActualTops = (simplestOpponent == OPP_EAST ?
    westActualTops : eastActualTops);
  const unsigned char actualLength = (simplestOpponent == OPP_EAST ?
    sumProfile.length() - length.lower() : length.lower());
  const unsigned char otherActualLength = (simplestOpponent == OPP_EAST ?
    length.lower() : sumProfile.length() - length.lower());

  string result;

  // if (westActualTops == length.lower())
  if (actualTops == actualLength)
  {
cout << "CASE 1\n";
    // Just fill out the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);
  }
  // else if (eastActualTops == sumProfile.length() - length.lower())
  else if (otherActualTops == otherActualLength)
  {
cout << "CASE 2\n";
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
  // else if (unsetTops == 0)
  else
  {
    // Start with the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);

    if (actualTops + hidden == actualLength)
    {
cout << "CASE 3a\n";
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
cout << "CASE 3b\n";
      // Add the right number of low cards.
      TopData topData;
      sumProfile.getTopData(canonicalShift, ranksNames, topData);

      // TODO Use 'x' not the actual name?  Fix RankNames?
      // In 11/136397 #4 we have Hh and we have A and 4 available.
      // This code picks 4 which is OK, but perhaps A is easier
      // to comprehend.
      const string str = 
        topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);

      result += str.substr(0, actualLength - actualTops);
    }
    else if (symmFlag &&
      2 * length.lower() == sumProfile.length() &&
      ((static_cast<unsigned>(activeCount+1) == tops.size() && canonicalShift == 1) ||
       (static_cast<unsigned>(activeCount+2) == tops.size() && canonicalShift == 0)))
    {
cout << "Case 3c\n" << endl;
      // TODO
      // This is quite a special case and should perhaps be avoided
      // when generating covers: HT / H8 or H8 / HT can both be
      // described as length 2 with exactly 1 top (symmetrical).
      // If we go through the normal expansion, we would say that
      // we are missing one card, but we have two choices (T and 8).
      // Pick the higher one.

      // First fill out the used equals.
      result = Product::strExactTops(sumProfile, ranksNames, 
        simplestOpponent, canonicalShift, true);

cout << "tmp " << result << endl;

      // TODO Use 'x' not the actual name?  Fix RankNames?
      // In 11/136397 #4 we have Hh and we have A and 4 available.
      // This code picks 4 which is OK, but perhaps A is easier
      // to comprehend.

      TopData topData;
      sumProfile.getTopData(canonicalShift, ranksNames, topData);
      const string str = 
        topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);

      const unsigned char gap = actualLength - actualTops;
      if (gap <= str.size())
      {
cout << "Case 3cI\n" << endl;
        result += str.substr(0, gap);
      }
      else
      {
cout << "Case 3cII\n" << endl;
        // Find the single other unused top.
        for (unsigned char topNo = static_cast<unsigned char>(tops.size()); --topNo > 0; )
        {
          const auto& top = tops[topNo];
          if (top.used())
            continue;

          sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);
          const string str2 = 
            topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);

          assert(gap <= str2.size());
          result += str2.substr(gap);
cout << "HIT\n";
          break;
        }
      }

cout << "done " << result << endl;
  }



    else
    {
  cout << "\nProduct      " << Product::strLine() << endl;
  cout << "sum profile " << sumProfile.strLine() << endl;
  cout << "simplestOpponent " << simplestOpponent << "\n";
  cout << "canonicalShift   " << +canonicalShift << "\n";
  cout << "westActualTops   " << +westActualTops << "\n";
  cout << "eastActualTops   " << +eastActualTops << "\n";
  cout << "actualTops       " << +actualTops << "\n";
  cout << "otherActualTops  " << +otherActualTops << "\n";
  cout << "hidden           " << +hidden << "\n";
  cout << "unsetTops        " << +unsetTops << "\n";
  cout << "length.lower     " << +length.lower() << "\n";
  cout << "sum length       " << +sumProfile.length() << "\n";
  cout << "actual length    " << +actualLength << "\n";
  cout << "other actual len " << +otherActualLength << "\n";
  cout << "result           " << result << "\n";
      assert(false);
    }
  }
  /*
  else
  {
  cout << "\nProduct      " << Product::strLine() << endl;
  cout << "sum profile " << sumProfile.strLine() << endl;
  cout << "simplestOpponent " << simplestOpponent << "\n";
  cout << "canonicalShift   " << +canonicalShift << "\n";
  cout << "westActualTops   " << +westActualTops << "\n";
  cout << "eastActualTops   " << +eastActualTops << "\n";
  cout << "actualTops       " << +actualTops << "\n";
  cout << "otherActualTops  " << +otherActualTops << "\n";
  cout << "hidden           " << +hidden << "\n";
  cout << "unsetTops        " << +unsetTops << "\n";
  cout << "length.lower     " << +length.lower() << "\n";
  cout << "sum length       " << +sumProfile.length() << "\n";
  cout << "actual length    " << +actualLength << "\n";
  cout << "other actual len " << +otherActualLength << "\n";
  cout << "result           " << result << "\n";
    assert(false);
  }
  */



/*
  // Actual maximum number of x's for the sumProfile and the Product.
  // unsigned char lowestWestMax, lowestWestActual;
  unsigned char xesMin, xesMax, xesHidden, xesAvailable, 
    topsExact, topsAvailable;
  // TODO I think the x'es are actually for simplestOpponent and
  // not for West.
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

  string result;

  const unsigned char lenSimplest = (simplestOpponent == OPP_EAST ?
    sumProfile.length() - length.lower() : length.lower());

  // if (xesMin == 0)
  if (topsExact == lenSimplest)
  {
// cout << "FILL USED" << endl;
    // Just fill out the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);
  }
  else if (topsExact + xesHidden == lenSimplest)
  {
// cout << "FILL ALL" << endl;
    // Fill out the used and all the unused equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);

    // Add any that are hidden by the canonical shift.
    for (unsigned char r = 0; r <= canonicalShift; r++)
    {
      TopData topData;
      sumProfile.getTopData(canonicalShift - r, ranksNames, topData);
      result +=
        topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);
    }
  }
  else if (topsAvailable + xesHidden == length.lower())
  {
// cout << "FILL ALL" << endl;
    // Fill out the used and all the unused equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, false);

    // Add any that are hidden by the canonical shift.
    for (unsigned char r = 0; r <= canonicalShift; r++)
    {
      TopData topData;
      sumProfile.getTopData(canonicalShift - r, ranksNames, topData);
      result +=
        topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);
    }
  }
  else if (// canonicalShift == 1 &&
      symmFlag && 
      xesMin == 1 &&
      2 * length.lower() == sumProfile.length())
  {
cout << "FILL SPECIAL" << endl;
    // TODO
    // This is quite a special case and should perhaps be avoided
    // when generating covers: HT / H8 or H8 / HT can both be
    // described as length 2 with exactly 1 top (symmetrical).
    // If we go through the normal expansion, we would say that
    // we are missing one card, but we have two choices (T and 8).
    // Pick the higher one.

    // First fill out the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);

cout << "tmp " << result << endl;
    TopData topData;
    sumProfile.getTopData(canonicalShift, ranksNames, topData);

    // TODO Use 'x' not the actual name?  Fix RankNames?
    // In 11/136397 #4 we have Hh and we have A and 4 available.
    // This code picks 4 which is OK, but perhaps A is easier
    // to comprehend.
    const string str = 
      topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);

    result += str.substr(0, 1);
cout << "done " << result << endl;
  }
  else if (static_cast<unsigned>(activeCount+1) == tops.size())
  {
// cout << "FILL LOWEST" << endl;
    // First fill out the used equals.
    result = Product::strExactTops(sumProfile, ranksNames, 
      simplestOpponent, canonicalShift, true);

    // Fill out the lowest rank.
    TopData topData;
    sumProfile.getTopData(canonicalShift, ranksNames, topData);
    const string str 
      = topData.rankNamesPtr->strComponent(RANKNAME_ACTUAL_SHORT);

    assert(xesMin <= str.size());
    result += str.substr(0, xesMin);
  }
  else
  {
    cout << "Product " << Product::strLine() << endl;
    cout << "length lower " << +length.lower() << endl;
    cout << "result size " << result.size() << endl;
    cout << "start " << start << endl;
    cout << "result " << result << endl;
    cout << "xes " << +xesMin << " to " << +xesMax << endl;
    cout << "xes avail " << +xesAvailable << endl;
    cout << "xes hidden " << +xesHidden << endl;
    cout << "canonicalShift " << +canonicalShift << endl;
    cout << "topsExact " << +topsExact << endl;
    cout << "topsAvailable " << +topsAvailable << endl;
    cout << "lenSimplest " << +lenSimplest << endl;
    assert(false);
  }
  */



// cout << "result " << result << endl;

if ((simplestOpponent != OPP_EAST && length.lower() != result.size()) ||
    (simplestOpponent == OPP_EAST && 
      static_cast<unsigned char>(sumProfile.length() - length.lower()) != result.size()))
{
  /*
  cout << "\nProduct " << Product::strLine() << endl;
  cout << "length lower " << +length.lower() << endl;
  cout << "result size " << result.size() << endl;
  cout << "start " << start << endl;
  cout << "result " << result << endl;
  cout << "xes " << +xesMin << " to " << +xesMax << endl;
  cout << "xes avail " << +xesAvailable << endl;
  cout << "xes hidden " << +xesHidden << endl;
  cout << "canonicalShift " << +canonicalShift << endl;
  cout << "topsExact " << +topsExact << endl;
  cout << "topsAvailable " << +topsAvailable << endl;
  cout << "lenSimplest " << +lenSimplest << endl;
  */

  cout << "\nProduct      " << Product::strLine() << endl;
  cout << "sum profile " << sumProfile.strLine() << endl;
  cout << "simplestOpponent " << simplestOpponent << "\n";
  cout << "canonicalShift   " << +canonicalShift << "\n";
  cout << "westActualTops   " << +westActualTops << "\n";
  cout << "eastActualTops   " << +eastActualTops << "\n";
  cout << "actualTops       " << +actualTops << "\n";
  cout << "otherActualTops  " << +otherActualTops << "\n";
  cout << "hidden           " << +hidden << "\n";
  cout << "unsetTops        " << +unsetTops << "\n";
  cout << "length.lower     " << +length.lower() << "\n";
  cout << "sum length       " << +sumProfile.length() << "\n";
  cout << "actual length    " << +actualLength << "\n";
  cout << "other actual len " << +otherActualLength << "\n";
  cout << "result           " << result << "\n";



  assert(length.lower() == result.size());
}

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

