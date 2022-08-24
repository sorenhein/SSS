/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

// Despite the file name, this file implements Product methods.
// They are separate as there are so many of them.

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Product.h"
#include "Profile.h"

#include "../CoverCategory.h"

#include "../../../ranks/RanksNames.h"

#include "../term/CoverOperator.h"
#include "../term/TopData.h"

#include "../../../utils/table.h"

// TMP
#include "../term/Xes.h"


// #define DEBUG_EQUAL_TOPS


struct OppData
{
  unsigned char topsUsed;
  unsigned char ranksUsed;
  unsigned char oneRankUsed;
  unsigned char freeLower;
  unsigned char freeUpper;

  string str(const string& header) const
  {
    stringstream ss;
    ss << header;
    ss << "Tops  " << +topsUsed << "\n";
    ss << "Ranks " << +ranksUsed << "\n";
    ss << "1rank " << +oneRankUsed << "\n";
    ss << "Free  " << +freeLower << " to " << +freeUpper << "\n";
    return ss.str();
  };
};


/**********************************************************************/
/*                                                                    */
/*                          Side comparators                          */
/*                                                                    */
/**********************************************************************/


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


bool Product::topsSimplerThan(const Product& p2) const
{
  // Prefer more and larger tops.
  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
      topNo-- > 0; )
  {
    // Include the 0 top.
    const auto& top1 = tops[topNo];
    const auto& top2 = p2.tops[topNo];

    if (! top1.used())
    {
      if (top2.used())
        return false;
      else
        continue;
    }
    else if (! top2.used())
      return true;

    assert(top1.used() && top2.used());

    if (top1.lower() > top2.lower())
      return true;
    else if (top2.lower() > top1.lower())
      return false;
  }

  // Backstop.
  return (length.lower() <= p2.length.lower());
}


bool Product::simplerThan(const Product& p2) const
{
  const unsigned char s = static_cast<unsigned char>(tops.size());
  assert(p2.tops.size() == s);

  assert(length.used());
  assert(length.getOperator() == COVER_EQUAL);
  assert(p2.length.used());
  assert(p2.length.getOperator() == COVER_EQUAL);

  unsigned char noLow1, noHigh1;
  unsigned char noLow2, noHigh2;
  Product::topRange(noLow1, noHigh1);
  p2.topRange(noLow2, noHigh2);

  // Normally prefer the shortest.
  if (length.lower() < p2.length.lower())
  {
    if (length.lower() <= 1 ||
        length.lower() + 2 < p2.length.lower())
      return true;
    else
    {
      // But not if it uses the lowest rank.
      // The logic in this is not very strong, but it's hard to
      // say what makes one choice more intuitive than another.
      return (noLow1 == 0 && noLow2 > 0 ? false : true);
    }
  }
  else if (p2.length.lower() < length.lower())
  {
    if (p2.length.lower() <= 1 ||
        p2.length.lower() + 2 < length.lower())
      return false;
    else
      return (noLow2 == 0 && noLow1 > 0 ? true : false);
  }

  return Product::topsSimplerThan(p2);
}


/**********************************************************************/
/*                                                                    */
/*                    Count and numerical methods                     */
/*                                                                    */
/**********************************************************************/


void Product::topRange(
  unsigned char& noLow,
  unsigned char& noHigh) const
{
  bool lowFlag = false;

  for (unsigned char topNo = 0; topNo < tops.size(); topNo++)
  {
    const auto& top = tops[topNo];
    if (top.lower() > 0)
    {
      if (! lowFlag)
      {
        noLow = topNo;
        lowFlag = true;
      }

      noHigh = topNo;
    }
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


void Product::fillUsedTops(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  Product& productWest,
  Product& productEast,
  OppData& dataWest,
  OppData& dataEast) const
{
  dataWest.topsUsed = 0;
  dataWest.ranksUsed = 0;
  dataWest.oneRankUsed = 0;
  dataEast.topsUsed = 0;
  dataEast.ranksUsed = 0;

  for (unsigned char topNo = 0; topNo < tops.size(); topNo++)
  {
    auto& top = tops[topNo];
    if (! top.used())
      continue;

    const unsigned char tlength = sumProfile[topNo + canonicalShift];
    productWest.tops[topNo] = top;
    productEast.tops[topNo].setMirrored(top, tlength);

    dataWest.topsUsed += top.lower();
    dataWest.ranksUsed++;
    if (top.lower())
      dataWest.oneRankUsed = topNo;

    dataEast.topsUsed += tlength - top.lower();
    dataEast.ranksUsed++;
    if (top.lower() == tlength)
      dataEast.oneRankUsed = topNo;
  }

  const unsigned char slength = sumProfile.length();

  if (length.used())
    productWest.length = length;
  else
    productWest.length.set(slength, dataWest.topsUsed, 
      slength - dataEast.topsUsed);

  productEast.length.setMirrored(productWest.length, slength);

// cout << "\nWest " << productWest.strLine() << endl;
// cout << "East " << productEast.strLine() << endl;

  const unsigned char rest = sumProfile.length() - 
    dataWest.topsUsed - dataEast.topsUsed;

  const unsigned char westLimit = 
    productWest.length.upper() - dataWest.topsUsed;
  const unsigned char eastLimit = 
    productEast.length.upper() - dataEast.topsUsed;

  dataWest.freeUpper = min(rest, westLimit);
  dataEast.freeUpper = min(rest, eastLimit);

  dataWest.freeLower = rest - dataEast.freeUpper;
  dataEast.freeLower = rest - dataWest.freeUpper;
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

  OppData dataWest, dataEast;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

  const unsigned char slength = sumProfile.length();
  const unsigned char wlength = length.lower();

  const unsigned char hidden = 
    Product::countHidden(sumProfile, canonicalShift);

  if (dataWest.topsUsed == wlength)
  {
    // The unused tops are 0 for West, maximum for East.
    // East also gets any low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_EAST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_EAST, hidden, productWest, productEast);
  }
  else if (dataEast.topsUsed == slength - wlength)
  {
    // The unused tops are 0 for East, maximum for West.
    // West also gets any low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_WEST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
  }
  else if (dataWest.topsUsed + hidden == wlength)
  {
    // East gets any unused tops.  West gets all the low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_EAST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
  }
  else if (dataEast.topsUsed + hidden == slength - wlength)
  {
    // West gets any unused tops.  East gets all the low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_WEST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_WEST, hidden, productWest, productEast);
    Product::fillSideBottoms(OPP_EAST, hidden, productWest, productEast);
  }
  else if (canonicalShift == 0)
  {
    // There should be no tops left to fill out, as all are used.
    // Add the right number of low cards to each side.
    assert(static_cast<unsigned char>(activeCount+1) == tops.size());

    const unsigned char westXes = wlength - dataWest.topsUsed;
    const unsigned char allXes = sumProfile[0];
    productWest.tops[0].set(allXes, westXes, westXes);
    productEast.tops[0].set(allXes, allXes - westXes, allXes - westXes);
  }
  else
    assert(false);
}


/**********************************************************************/
/*                                                                    */
/*                             String methods                         */
/*                                                                    */
/**********************************************************************/


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


string Product::strExactStart(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  if (! length.used() ||  length.lower() > 2)
    return "exactly";

  // A somewhat involved way to say "the singleton K" but
  // "a singleton H", and "the doubleton HH" if there are two,
  // "a doubleton HH" if there are more.

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    topNo-- > 0; )
  {
    // Find the first non-zero top.
    auto& top = tops[topNo];
    assert(top.used());
    if (top.lower() == 0)
      continue;

    if (length.lower() < sumProfile[topNo + canonicalShift])
      return (length.lower() == 1 ? "a singleton" : "a doubleton");
    else
      return (length.lower() == 1 ? "the singleton" : "the doubleton");
  }

  assert(false);
  return "";
}


string Product::strExactTop(
  [[maybe_unused]] const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const unsigned char topNo,
  const bool expandFlag) const
{
  const auto& top = tops[topNo];
  assert(top.used());

  return ranksNames.strOpponents(topNo + canonicalShift,
    top.lower(), expandFlag);
}


string Product::strExact(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const string& anchor,
  const unsigned char canonicalShift) const
{
  string start = anchor + " has " + 
    Product::strExactStart(sumProfile, canonicalShift) + " ";

  // Fill out the tops from above, but not the 0'th top.
  string result = "";
  unsigned char check = 0;

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    --topNo > 0; )
  {
    result += Product::strExactTop(sumProfile, ranksNames, 
      canonicalShift, topNo, false);
    check += tops[topNo].lower();
  }

  if (canonicalShift == 0)
  {
    // Same principle.
    result += Product::strExactTop(sumProfile, ranksNames, 
      canonicalShift, 0, false);
    check += tops[0].lower();
  }
  else if (tops[0].lower() > 0)
  {
    TopData topData;

    // All the low cards.
    for (unsigned char hiddenNo = canonicalShift+1; hiddenNo-- > 0; )
    {
      sumProfile.getTopData(hiddenNo, ranksNames, topData);
      result += topData.strTops(topData.value);
      check += topData.value;
    }
  }

  assert(check == length.lower());
  return start + result;
}


string Product::strEqualTopsOnly(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const OppData& oppData) const
{
  string result = "";

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    --topNo > 0; )
  {
    result += Product::strExactTop(sumProfile, ranksNames, 
      canonicalShift, topNo, oppData.ranksUsed == 1);
  }

  return result;
}


string Product::strEqualTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const string& anchor,
  const OppData& data,
  const unsigned char canonicalShift) const
{
  string start = anchor + " has ";

  // Fill out the tops from above, but not the 0'th top.
  // Find the last used top.

  unsigned char lowestUsed = 0;
  unsigned char lowestSet = 0;
  string result = "";

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    --topNo > 0; )
  {
    result += Product::strExactTop(sumProfile, ranksNames, 
      canonicalShift, topNo, false);
      // canonicalShift, topNo, data.ranksUsed == 1);

    if (tops[topNo].used())
    {
      lowestUsed = topNo;
      if (tops[topNo].lower() > 0)
        lowestSet = topNo;
    }
  }

  // TODO Probably the same as lowestRank == 1
  if (canonicalShift == 0 && static_cast<unsigned>(activeCount+1) == tops.size())
  {
    // We only have to set the x'es.
    if (data.freeLower > 0)
      result += string(data.freeLower, 'x');

    result += "(" + string(data.freeUpper - data.freeLower, 'x') + ")";
  }
  else
  {
    TopData topData;
    const unsigned char numOptions = lowestUsed + canonicalShift;

    if (numOptions <= 2 && data.freeUpper == 1)
    {
      // There are at most two options for the low cards.
      // We need 1 low card.
      string rcopy = result;
      result =  "";

      // Show the version without any low card.
      if (data.freeLower == 0)
        result = rcopy;

      for (unsigned char topNo = numOptions; topNo-- > 0; 0)
      {
        if (! result.empty())
        {
          if (topNo == 0)
            result += " or ";
          else
            result += ", ";
        }

        // sumProfile.getTopData(topNo, ranksNames, topData);
        // result += rcopy + topData.strTops(1);

        result += rcopy + ranksNames.strOpponents(topNo, 1, false);
      }
    }
    else
    {
      /* */
      sumProfile.getTopData(lowestUsed + canonicalShift, ranksNames, 
        topData);
      const string lowestRank = topData.strTops(topData.value);
      assert(! lowestRank.empty());

      // const string& lowestCard = lowestRank.substr(lowestRank.size()-1, 1);
      /* */


      // const string lowestRankStr = Product::strExactTop(
        // sumProfile, ranksNames, canonicalShift, lowestUsed, false);
      const string lowestRankStr = 
        ranksNames.strOpponents(lowestUsed + canonicalShift,
          sumProfile[lowestUsed + canonicalShift], false);

/*
if (lowestRankStr != lowestRank)
{
  cout << "lowestRank " << lowestRank << endl;
  cout << "lowestRankStr " << lowestRankStr << endl;
}
*/
      assert(lowestRankStr == lowestRank);

      const string& lowestCard = 
        lowestRankStr.substr(lowestRankStr.size()-1, 1);



      // The unused tops.
      const unsigned char hidden = Product::countHidden(sumProfile,
        canonicalShift);

#ifdef DEBUG_EQUAL_TOPS
      cout << "cshift " << +canonicalShift << endl;
      cout << data.str("Data") << endl;
      cout << "hidden " << +hidden << endl;
#endif

      if (result.empty())
        result = "only ";
      else
        result += " and ";

      if (data.freeLower == 0 && data.freeUpper == hidden)
      {
        // More like "West has K" or "West has exactly one H"
        result += "any number of ";
      }
      else if (data.freeLower == data.freeUpper)
      {
        result += to_string(data.freeLower);
      }
      else if (data.freeLower == 0)
      {
        result += "at most " + to_string(data.freeUpper);
      }
      else
      {
        result += to_string(data.freeLower) + "-" + 
          to_string(data.freeUpper);
      }

      if (lowestSet == lowestUsed)
        result += " lower-ranked cards";
      else
        result += " cards lower than the " + lowestCard;
    }
  }

// cout << "s+r " << start + result << endl;

  return start + result;
}


string Product::strVerbalEqualTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const CoverVerbal verbal,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount > 0);

  if (verbal == VERBAL_HIGH_TOPS_EQUAL)
  {
    Product productWest, productEast;
    productWest.resize(tops.size());
    productEast.resize(tops.size());
  
    OppData dataWest, dataEast;
    Product::fillUsedTops(sumProfile, canonicalShift, 
      productWest, productEast, dataWest, dataEast);

#ifdef DEBUG_EQUAL_TOPS
    cout << "Product " << Product::strLine() << "\n";
    cout << dataWest.str("West");
    cout << dataEast.str("East");
#endif

    if (! length.used())
    {
      string resWest = productWest.strEqualTopsOnly(sumProfile,
        ranksNames, canonicalShift, dataWest);
      string resEast = productEast.strEqualTopsOnly(sumProfile,
        ranksNames, canonicalShift, dataEast);

      if (resWest.empty())
      {
        // Just state that East has the tops.
        return "East has " + resEast;
      }
      else if (resEast.empty())
      {
        // Just state that West has the tops.
        return "West has " + resWest;
      }
      else if (productWest.topsSimplerThan(productEast))
      {
        if (dataWest.ranksUsed == 1 && dataEast.ranksUsed == 1 &&
            dataWest.oneRankUsed == dataEast.oneRankUsed)
          return "West has " + resWest;
        else
          return "West has " + resWest + " and not " + resEast;
      }
      else
      {
        if (dataWest.ranksUsed == 1 && dataEast.ranksUsed == 1 &&
            dataWest.oneRankUsed == dataEast.oneRankUsed)
          return "East has " + resEast;
        else
          return "East has " + resEast + " and not " + resWest;
      }
    }
    else
    {
// cout << "\nProduct NEQ " << Product::strLine() << "\n";
    }

    // const bool westSimplerFlag = 
      // (productWest.topsSimplerThan(productEast));

    const bool westSimplerFlag =
      (dataWest.topsUsed + dataWest.freeUpper <=
       dataEast.topsUsed + dataEast.freeUpper);

    string resNew;
    // const unsigned char smallCards = sumProfile.length() - 
      // dataWest.topsUsed - dataEast.topsUsed;

    // unsigned char westDisplayCount, eastDisplayCount;

  if (westSimplerFlag)
  {
    resNew = productWest.strEqualTops(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "West"), 
      dataWest, canonicalShift);
  }
  else
  {
    resNew = productEast.strEqualTops(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "East"), 
      dataEast, canonicalShift);
  }

// cout << "NEQ returns " << resNew << endl;

  return resNew;
  }


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


string Product::strVerbalSingular(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount > 0);

  Product productWest, productEast;
  Product::separateSingular(sumProfile, canonicalShift, 
    productWest, productEast);

  if (productWest.simplerThan(productEast))
  {
    return productWest.strExact(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "West"), canonicalShift);
  }
  else
  {
    return productEast.strExact(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "East"), canonicalShift);
  }
}


string Product::strVerbal(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const CoverVerbal verbal,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(verbal != VERBAL_GENERAL && verbal != VERBAL_HEURISTIC);

// cout << "\nProduct " << Product::strLine() << "\n";
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
  else if (verbal == VERBAL_SINGULAR)
  {
    return Product::strVerbalSingular(
      sumProfile, 
      ranksNames, 
      symmFlag, 
      canonicalShift);
  }
  else
  {
    assert(false);
    return "";
  }
}

