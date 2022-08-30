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
  unsigned char ranksActive; // Non-zero rank entry
  unsigned char lowestRankUsed;
  unsigned char lowestRankActive; // Non-zero rank entry
  bool partialFlag; // Entry used (> 0) by both sides
  unsigned char topsFull; // Count of tops in the ranksOver ranks
  unsigned char ranksFull; // Nuber of ranks active, but not on other side
  unsigned char freeLower;
  unsigned char freeUpper;


  string strXes(
    const bool dashFlag,
    const bool expandFlag) const
  {
    // We only have to set the x'es.
    const string text = 
      (expandFlag ?  " as well as " : (dashFlag ? "-" : ""));

    if (freeLower > 0)
    {
      return text + string(freeLower, 'x') +
        "(" + string(freeUpper - freeLower, 'x') + ")";
    }
    else
    {
      return text + "(" + string(freeUpper, 'x') + ")";
    }
  }


  string str(const string& header) const
  {
    stringstream ss;
    ss << header << "\n";;
    ss << "Top cards used     " << +topsUsed << "\n";
    ss << "Ranks used         " << +ranksUsed << "\n";
    ss << "Ranks active       " << +ranksActive << "\n";
    ss << "Lowest rank used   " << +lowestRankUsed << "\n";
    ss << "1+ partial ranks   " << (partialFlag ? "true" : "false") << "\n";
    ss << "Tops of full rank  " << +topsFull << "\n";
    ss << "Full ranks used    " << +ranksFull << "\n";
    ss << "Lowest rank active " << +lowestRankActive << "\n";
    ss << "Free low cards     " << +freeLower << " to " << 
      +freeUpper << "\n";
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

  xesHidden = Product::countBottoms(sumProfile, canonicalShift);


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


unsigned char Product::countBottoms(
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
  dataWest.ranksActive = 0;
  dataWest.lowestRankUsed = 0;
  dataWest.lowestRankActive = 0;
  dataWest.partialFlag = false;
  dataWest.topsFull = 0;
  dataWest.ranksFull = 0;

  dataEast.topsUsed = 0;
  dataEast.ranksUsed = 0;
  dataEast.ranksActive = 0;
  dataEast.lowestRankUsed = 0;
  dataEast.lowestRankActive = 0;
  dataEast.partialFlag = false;
  dataEast.topsFull = 0;
  dataEast.ranksFull = 0;

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    topNo-- > 0; )
  {
    auto& top = tops[topNo];
    if (! top.used())
      continue;

    const unsigned char tlength = sumProfile[topNo + canonicalShift];
    productWest.tops[topNo] = top;
    productEast.tops[topNo].setMirrored(top, tlength);

    dataWest.topsUsed += top.lower();
    dataWest.ranksUsed++;
    dataWest.lowestRankUsed = topNo;
    if (top.lower())
    {
      dataWest.ranksActive++;
      dataWest.lowestRankActive = topNo;
    }
    else
    {
      dataEast.topsFull += tlength;
      dataEast.ranksFull++;
    }

    dataEast.topsUsed += tlength - top.lower();
    dataEast.ranksUsed++;
      dataEast.lowestRankUsed = topNo;
    if (top.lower() < tlength)
    {
      dataEast.ranksActive++;
      dataEast.lowestRankActive = topNo;
    }
    else
    {
      dataWest.topsFull += tlength;
      dataEast.ranksFull++;
    }

    if (top.lower() > 0 && top.lower() < tlength)
    {
      dataWest.partialFlag = true;
      dataEast.partialFlag = true;
    }
  }

  const unsigned char slength = sumProfile.length();

  if (length.used())
    productWest.length = length;
  else
    productWest.length.set(slength, dataWest.topsUsed, 
      slength - dataEast.topsUsed);

  productEast.length.setMirrored(productWest.length, slength);

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
    Product::countBottoms(sumProfile, canonicalShift);

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


string Product::strUsedTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const bool allAvailableFlag, // Not just the ones in Product
  const bool expandFlag, // jack, not J etc.
  const bool singleRankFlag, // Use dashes between expansions
  const bool onlyFullFlag) const // Only ranks where Product == available
{
  string result = "";

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    --topNo > 0; )
  {
    const auto& top = tops[topNo];
    if (! top.used())
      continue;

    const unsigned char available = sumProfile[topNo + canonicalShift];
    if (onlyFullFlag && top.lower() != available)
      continue;

    const unsigned char count = (allAvailableFlag ?
      available : top.lower());

    const string rstr = ranksNames.strOpponents(topNo + canonicalShift,
      count, expandFlag, singleRankFlag);

    if (rstr.empty())
      continue;

    if (expandFlag && ! singleRankFlag && ! result.empty())
      result += "-";

    result += rstr;
  }

  return result;
}


string Product::strUsedBottoms(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const bool allFlag,
  const bool expandFlag) const
{
  string result = "";

  for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
  {
    const unsigned char count = (allFlag ?
      sumProfile[topNo] : tops[topNo].lower());

    result += ranksNames.strOpponents(topNo, count, expandFlag, false);
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


string Product::strVerbalAnyTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount > 0);

  Product productWest, productEast;
  productWest.resize(tops.size());
  productEast.resize(tops.size());

  OppData dataWest, dataEast;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

#ifdef DEBUG_EQUAL_TOPS
    cout << "Product  " << Product::strLine() << "\n";
    cout << "prodWest " << productWest.strLine() << "\n";
    cout << "prodEast " << productEast.strLine() << "\n";
    cout << dataWest.str("West");
    cout << dataEast.str("East");
#endif

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

  const string avail = Product::strUsedTops(sumProfile, ranksNames,
    canonicalShift, true, false, false, false);

  string exact;

  if (simplestOpponent == OPP_EAST)
    exact = productEast.strUsedTops(sumProfile, ranksNames,
      canonicalShift, false, dataEast.ranksActive == 1, false, false);
  else
    exact = productWest.strUsedTops(sumProfile, ranksNames,
      canonicalShift, false, dataWest.ranksActive == 1, false, false);

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

  string other = " cards of other ranks";

  if (xesMin == xesMax)
    result += " and " + to_string(xesMin) + other;
  else
  {
    result += " and " + to_string(xesMin) + "-" +
      to_string(xesMax) + other;
  }

  return result;
}



/*--------------------------------------------------------------------*/
/*                                                                    */
/*                     Equal high top string methods                  */
/*                                                                    */
/*--------------------------------------------------------------------*/


string Product::strVerbalHighTopsOnlySide(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const string& side,
  const string& sideOther,
  const OppData& data,
  const bool singleActiveRank) const
{
  const unsigned char numInactive = data.lowestRankUsed + canonicalShift;

  if (data.ranksActive < 3 || 
      data.ranksActive <= numInactive ||
      singleActiveRank)
  {
    return side + " has " + 
      Product::strUsedTops(
        sumProfile, ranksNames, canonicalShift, 
        false, true, data.ranksActive == 1, false);
  }
  else
  {
    return sideOther + " has " + "(" + 
      Product::strUsedBottoms(
        sumProfile, ranksNames, canonicalShift, true, false) + ")";
  }
}


string Product::strVerbalHighTopsOnlyBothSides(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const Product& productOther,
  const string& side,
  const OppData& data,
  const OppData& dataOther) const
{
  const unsigned char numInactive = data.lowestRankUsed + canonicalShift;
  const bool simpleFlag = (data.ranksActive == 1); // || data.topsUsed <= 2);

  if (numInactive == 1)
  {
    const string result = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, simpleFlag, data.ranksActive == 1, false);

    return side + " has " + result + 
      data.strXes(data.partialFlag, simpleFlag);
  }
  else
  {
    const string resultOwn = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, simpleFlag, data.ranksActive == 1, false);

    const bool simpleOther = (dataOther.ranksFull == 1);

    const string resultOther = productOther.strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, simpleOther, dataOther.ranksActive == 1, true);

    if (dataOther.freeUpper <= dataOther.topsFull)
    {
      // Prefer to state the low cards.
      return side + " has " + resultOwn + "(" +
        Product::strUsedBottoms(
          sumProfile, ranksNames, canonicalShift, true, false) + ")";
    }
    else if (dataOther.topsFull > 2)
      return side + " has " + resultOwn + " and none of " + resultOther;
    else if (dataOther.topsFull == 2)
      return side + " has " + resultOwn + " and neither of " + resultOther;
    else if (dataOther.topsFull == 1)
      return side + " has " + resultOwn + " and not " + resultOther;
    else
      return side + " has " + resultOwn +
        // TODO Call "lower"
        " and perhaps smaller cards";
  }
}


string Product::strVerbalHighTopsOnly(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const Product& productWest,
  const Product& productEast,
  const OppData& dataWest,
  const OppData& dataEast) const
{
  const unsigned char numInactive = 
    dataWest.lowestRankUsed + canonicalShift;

  const bool singleActiveRank =
    (dataWest.ranksUsed == 1 && dataEast.ranksUsed == 1 &&
     dataWest.lowestRankActive == dataEast.lowestRankActive);

  if (dataEast.ranksActive == 0 || singleActiveRank)
  {
    return productWest.strVerbalHighTopsOnlySide(
      sumProfile, ranksNames, canonicalShift,
      "West", "East", dataWest, singleActiveRank);
  }
  else if (dataWest.ranksActive == 0)
  {
    return productEast.strVerbalHighTopsOnlySide(
      sumProfile, ranksNames, canonicalShift,
      "East", "West", dataEast, singleActiveRank);
  }

  const bool simpleWest = 
    (dataWest.ranksActive == 1 || dataWest.topsUsed <= 2);
  const bool simpleEast = 
    (dataEast.ranksActive == 1 || dataEast.topsUsed <= 2);
  const bool simpleBoth = simpleWest && simpleEast;

  const bool partialWest = (dataWest.ranksActive == 1);
  const bool partialEast = (dataEast.ranksActive == 1);
  const bool partialBoth = partialWest && partialEast;

/*
cout << "Product " << Product::strLine() << endl;
cout << dataWest.str("West");
cout << dataEast.str("East");
cout << "simpleBoth " << simpleBoth << endl;
cout << "partialBoth " << partialBoth << endl;
*/

  bool preferWest;
  if (simpleWest && ! simpleEast)
    preferWest = true;
  else if (! simpleWest && simpleEast)
    preferWest = false;
  else if (dataWest.topsUsed == 1 && dataEast.topsUsed > 1)
    preferWest = true;
  else if (dataEast.topsUsed == 1 && dataWest.topsUsed > 1)
    preferWest = false;
  else
    preferWest = productWest.topsSimplerThan(productEast);

  if (preferWest)
  {
    return productWest.strVerbalHighTopsOnlyBothSides(
      sumProfile, ranksNames, canonicalShift,
      productEast, "West", dataWest, dataEast);
  }
  else
  {
    return productEast.strVerbalHighTopsOnlyBothSides(
      sumProfile, ranksNames, canonicalShift,
      productWest, "East", dataEast, dataWest);
  }
}


string Product::strVerbalHighTopsSide(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const string& side,
  const OppData& data,
  const unsigned char canonicalShift) const
{
  string start = side + " has ";

  string result = Product::strUsedTops(
    sumProfile, ranksNames, canonicalShift, 
    false, data.ranksActive == 1, data.ranksActive == 1, false);

  const unsigned char numOptions = data.lowestRankUsed + canonicalShift;

  if (numOptions == 1)
  {
cout << "P10" << endl;
// cout << data.str("data");
    // We only have to set the x'es.
    result += data.strXes(data.partialFlag, data.ranksActive == 1);
  }
  else if (numOptions == 2 && data.freeUpper == 1)
  {
cout << "P11" << endl;
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

      result += rcopy + ranksNames.strOpponents(topNo, 1, false, false);
    }
  }
  else
  {
cout << "P12" << endl;
    const string lowestRankStr = 
      ranksNames.strOpponents(data.lowestRankUsed + canonicalShift,
        sumProfile[data.lowestRankUsed + canonicalShift], 
        false, false);

    const string& lowestCard = 
      lowestRankStr.substr(lowestRankStr.size()-1, 1);

    // The unused tops.
    const unsigned char hidden = Product::countBottoms(sumProfile,
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

    if (data.lowestRankActive == data.lowestRankUsed)
      result += " lower-ranked cards";
    else
      result += " cards lower than the " + lowestCard;
  }

// cout << "s+r " << start + result << endl;

  return start + result;
}


string Product::strVerbalHighTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const bool symmFlag,
  const unsigned char canonicalShift) const
{
  assert(activeCount > 0);

  Product productWest, productEast;
  productWest.resize(tops.size());
  productEast.resize(tops.size());

  OppData dataWest, dataEast;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

#ifdef DEBUG_EQUAL_TOPS
    cout << "Product  " << Product::strLine() << "\n";
    cout << "prodWest " << productWest.strLine() << "\n";
    cout << "prodEast " << productEast.strLine() << "\n";
    cout << dataWest.str("West");
    cout << dataEast.str("East");
#endif

  if (! length.used())
  {
cout << "P0" << endl;
    return Product::strVerbalHighTopsOnly(sumProfile, ranksNames,
      canonicalShift, productWest, productEast, dataWest, dataEast);
  }
  else
  {
    // const bool westSimplerFlag = 
      // (productWest.topsSimplerThan(productEast));

    const bool westSimplerFlag =
      (dataWest.topsUsed + dataWest.freeUpper <=
       dataEast.topsUsed + dataEast.freeUpper);

    if (westSimplerFlag)
    {
cout << "P7" << endl;
      return productWest.strVerbalHighTopsSide(sumProfile, ranksNames, 
        (symmFlag ? "Either opponent" : "West"), 
        dataWest, canonicalShift);
    }
    else
    {
cout << "P8" << endl;
      return productEast.strVerbalHighTopsSide(sumProfile, ranksNames, 
        (symmFlag ? "Either opponent" : "East"), 
        dataEast, canonicalShift);
    }
  }
}


/*--------------------------------------------------------------------*/
/*                                                                    */
/*                       Singular string methods                      */
/*                                                                    */
/*--------------------------------------------------------------------*/


string Product::strVerbalSingularQualifier(
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


string Product::strVerbalSingularSide(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const string& side,
  const unsigned char canonicalShift) const
{
  string result = side + " has ";

  // Add some verbal cues.
  result += Product::strVerbalSingularQualifier(sumProfile, 
    canonicalShift) + " ";

  // Fill out the tops from above, but not the 0'th top.
  result += Product::strUsedTops(
    sumProfile, ranksNames, canonicalShift, 
    false, false, false, false);

  // Fill out the low cards.
  if (canonicalShift == 0)
  {
    // The right number of the single lowest rank.
    result += Product::strUsedBottoms(sumProfile, ranksNames,
      canonicalShift, false, false);
  }
  else if (tops[0].lower() > 0)
  {
    // All the low cards.
    result += Product::strUsedBottoms(sumProfile, ranksNames,
      canonicalShift, true, false);
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
    return productWest.strVerbalSingularSide(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "West"), canonicalShift);
  }
  else
  {
    return productEast.strVerbalSingularSide(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "East"), canonicalShift);
  }
}


/*--------------------------------------------------------------------*/
/*                                                                    */
/*                           Overall method                           */
/*                                                                    */
/*--------------------------------------------------------------------*/

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
  else if (verbal == VERBAL_ANY_TOPS_EQUAL)
  {
    return Product::strVerbalAnyTops(
      sumProfile, 
      ranksNames, 
      symmFlag, 
      canonicalShift);
  }
  else if (verbal == VERBAL_HIGH_TOPS_EQUAL)
  {
    return Product::strVerbalHighTops(
      sumProfile, 
      ranksNames, 
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

