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
#include "VerbalData.h"

// TODO Whole file needed, or split perhaps?
#include "../verbal/VerbalTemplates.h"

// TODO Probably not needed long-term
#include "../verbal/VerbalBlank.h"

#include "../CoverCategory.h"

#include "../verbal/VerbalCover.h"

#include "../term/CoverOperator.h"
#include "../term/TopData.h"

#include "../../../ranks/RanksNames.h"

#include "../../../utils/table.h"

extern VerbalTemplates verbalTemplates;


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


unsigned char Product::countBottoms(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  // Add any that are hidden by the canonical shift.
  // TODO Could even be a method in sumProfile.
  unsigned char count = 0;

  for (unsigned char topNo = 0; topNo <= canonicalShift; topNo++)
    count += sumProfile[topNo];

  return count;
}


void Product::fillUsedTops(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  Product& productWest,
  Product& productEast,
  VerbalData& dataWest,
  VerbalData& dataEast) const
{
  // Assumes that all used tops are of the equal type.
  dataWest.reset();
  dataEast.reset();

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    topNo-- > 0; )
  {
    auto& top = tops[topNo];
    if (! top.used())
      continue;

    const unsigned char tlength = sumProfile[topNo + canonicalShift];
    productWest.tops[topNo] = top;
    productEast.tops[topNo].setMirrored(top, tlength);

    dataWest.update(topNo, top.lower(), tlength);
    dataEast.update(topNo, tlength - top.lower(), tlength);
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
  const unsigned char numXes,
  Product& productWest,
  Product& productEast) const
{
  // The side specified by fillOpponent gets all unused bottoms.
  // This may span several ranks, depending on canonicalShift.
  if (fillOpponent == OPP_WEST)
  {
    productWest.tops[0].set(numXes, numXes, numXes);
    productEast.tops[0].set(numXes, 0, 0);
  }
  else
  {
    productWest.tops[0].set(numXes, 0, 0);
    productEast.tops[0].set(numXes, numXes, numXes);
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

  VerbalData dataWest, dataEast;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

  const unsigned char slength = sumProfile.length();
  const unsigned char wlength = length.lower();

  const unsigned char numBottoms = 
    Product::countBottoms(sumProfile, canonicalShift);

  if (dataWest.topsUsed == wlength)
  {
    // The unused tops are 0 for West, maximum for East.
    // East also gets any low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_EAST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_EAST, numBottoms, 
      productWest, productEast);
  }
  else if (dataEast.topsUsed == slength - wlength)
  {
    // The unused tops are 0 for East, maximum for West.
    // West also gets any low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_WEST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_WEST, numBottoms, 
      productWest, productEast);
  }
  else if (dataWest.topsUsed + numBottoms == wlength)
  {
    // East gets any unused tops.  West gets all the low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_EAST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_WEST, numBottoms, 
      productWest, productEast);
    Product::fillSideBottoms(OPP_WEST, numBottoms, 
      productWest, productEast);
  }
  else if (dataEast.topsUsed + numBottoms == slength - wlength)
  {
    // West gets any unused tops.  East gets all the low cards.
    Product::fillUnusedTops(sumProfile, canonicalShift, OPP_WEST,
      productWest, productEast);

    Product::fillSideBottoms(OPP_WEST, numBottoms, 
      productWest, productEast);
    Product::fillSideBottoms(OPP_EAST, numBottoms, 
      productWest, productEast);
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
/*                             Stack methods                          */
/*                                                                    */
/**********************************************************************/

void Product::makePartialProfile(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  Completion& completion) const
{
  // We have some top's that are fixed to a single value.
  // We have some explicit, unused tops.
  // We have 1 or more unused implicit bottoms (canonicalShift+1).

  completion.resize(sumProfile.size());

  // Cover from the highest top down to canonicalShift (exclusive).
  // If canonicalShift is 0, then we will stop at 1, but then the 0th
  // real top is by convention unset.
  for (unsigned char topNo = static_cast<unsigned char>(sumProfile.size());
      --topNo > canonicalShift; )
  {
    completion.setTop(
      topNo, 
      tops[topNo-canonicalShift].used(),
      tops[topNo-canonicalShift].lower());
  }

  // The zero'th top represents all the actual tops in sumProfile
  // from 0 up to canonicalShift-1.

  if (! tops[0].used())
  {
    // Fill with unused bottoms.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; 0)
      completion.setTop(topNo, false, 0);
    return;
  }

  const unsigned char bottoms = tops[0].lower();
  assert(tops[0].getOperator() == COVER_EQUAL);

  if (bottoms == 0)
  {
    // Fill with zeroes.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; 0)
      completion.setTop(topNo, true, 0);
  }
  else if (bottoms == Product::countBottoms(sumProfile, canonicalShift))
  {
    // Fill with maximum values.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; 0)
      completion.setTop(topNo, true, sumProfile[topNo]);
  }
  else
  {
    // Add a single bottom with the right number.
    assert(canonicalShift == 0);
    completion.setTop(0, true, bottoms);
  }
}


bool Product::makeCompletions(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const VerbalData& data,
  const unsigned char maxCompletions,
  VerbalCover& completions) const
{
  Completion completion;
  Product::makePartialProfile(sumProfile, canonicalShift, completion);

  list<Completion> stack;
  stack.push_back(completion);

  const unsigned char totalLower = data.topsUsed + data.freeLower;
  const unsigned char totalUpper = data.topsUsed + data.freeUpper;

  // It's OK to have zero of the highest open top.
  // After that, a zero top was already implicit in some earlier
  // completion.
  bool firstOpen = true;

  for (auto openNo: completion.openTops())
  {
    const size_t psize = stack.size();
    size_t pno = 0;

    // As we're sharing the stack between this and the future
    // open numbers, we make sure to stop
    auto piter = stack.begin();
    while (true)
    {
      if (pno == psize)
        break;

      const unsigned char maxCount = min(
        static_cast<unsigned char>(sumProfile[openNo]),
        static_cast<unsigned char>(totalUpper - piter->length()));

      for (unsigned char count = maxCount+1; count-- > 0; )
      {
        piter->updateTop(openNo, count);

        if (piter->length() >= totalLower && (count > 0 || firstOpen))
        {
          if (completions.size() >= maxCompletions)
            return false;

          completions.push_back(* piter);
        }

        if (piter->length() < totalUpper && openNo > 0)
          stack.push_back(* piter);
      }

      piter++;
      pno++;
    }
    firstOpen = false;
    stack.erase(stack.begin(), piter);
  }

  assert(stack.empty());
  completions.stable_sort();
  return true;
}


/**********************************************************************/
/*                                                                    */
/*                             String methods                         */
/*                                                                    */
/**********************************************************************/


string Product::strUsedBottoms(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift) const
{
  string result = "";

  for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
  {
    result += ranksNames.strOpponents(topNo, sumProfile[topNo], 
      false, false);
  }

  return result;
}


string Product::strVerbalLengthOnly(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const bool symmFlag) const
{
  // ranksNames aren't really needed when ranks aren't set.

  assert(activeCount == 0);

  VerbalCover completions;
  completions.setLength(length);

  vector<TemplateData> tdata;

  return completions.strGeneral(
    sumProfile.length(), symmFlag, ranksNames, tdata);
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

  // ---
  Product productWest, productEast;

  // Simpler version of separateSingular where length is given.
  productWest.resize(tops.size());
  productEast.resize(tops.size());
  
  VerbalData dataWest, dataEast; // Thrown away
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

  Completion completion;

  if (simplestOpponent == OPP_EAST)
    productEast.makePartialProfile(
      sumProfile,
      canonicalShift,
      completion);
  else
    productWest.makePartialProfile(
      sumProfile,
      canonicalShift,
      completion);

  VerbalCover completions;
  completions.setSide(completion, simplestOpponent);

  vector<TemplateData> tdata;

  string tnew = completions.strGeneral(
    sumProfile.length(), symmFlag, ranksNames, tdata);
  // ---

  TopData topData;
  unsigned char topNo;

  for (topNo = static_cast<unsigned char>(tops.size()); topNo-- > 0; )
  {
    if (! tops[topNo].used())
      continue;

    assert(tops[topNo].getOperator() != COVER_EQUAL);
    sumProfile.getTopData(topNo + canonicalShift, ranksNames, topData);

    const string told = tops[topNo].strTop(
      topData,
      simplestOpponent, 
      symmFlag);

    return told;
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
    // TODO This doesn't work well as the moment:
    // "The suit splits 3=3 with 1-2 of ace-king-queen"
    // "The suit splits evenly with ..."
    // In these cases we need to state whether West or East has something.

    VerbalCover completions;
    completions.setLength(length);

    vector<TemplateData> tdata;

    return 
      completions.strGeneral(
        sumProfile.length(), symmFlag, ranksNames, tdata) +
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


/*--------------------------------------------------------------------*/
/*                                                                    */
/*                   Equal high/any top string methods                */
/*                                                                    */
/*--------------------------------------------------------------------*/


  // TEMPLATE
  // if (sold == snew)
    // cout << "\n" << setw(40) << left << sold << "X1X " << snew << endl;
  // else
    // cout << "\n" << setw(40) << left << sold << "X2X " << snew << endl;


string Product::strVerbalTops(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const string& side,
  const string& sideOther,
  const VerbalData& data,
  const bool singleActiveRank,
  const bool flipAllowedFlag) const
{
  // The other side is known to use no tops at all.

  const unsigned char numOptions = 
    static_cast<unsigned char>(tops.size()) + 
    canonicalShift - data.ranksUsed;

  if (! flipAllowedFlag ||
      data.ranksActive < 3 || 
      data.ranksActive <= numOptions ||
      singleActiveRank)
  {
    // State it from the intended side.
    Completion completion;
    Product::makePartialProfile(sumProfile, canonicalShift, completion);
    return side + " has " + 
      completion.strSet(ranksNames, true, data.ranksActive == 1);
  }
  else
  {
    // State it from the other side.  If the tops are not all
    // on the high end, but scattered, this output will not make
    // sense.  So flipAllowedFlag should only be set for high tops.
    return sideOther + " has " + "(" + 
      Product::strUsedBottoms(
        sumProfile, ranksNames, canonicalShift) + ")";
  }
}


string Product::strVerbalTopsDual(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const Product& productOther,
  const string& side,
  const VerbalData& data,
  const VerbalData& dataOther) const
{
  Completion completionRown;
  Product::makePartialProfile(
    sumProfile, canonicalShift, completionRown);
  const string resultOwn = completionRown.strSet(ranksNames,
    data.topsUsed == 1, data.ranksActive == 1);

  Completion completionOther;
  productOther.makePartialProfile(
    sumProfile, canonicalShift, completionOther);
  const string resultOther = completionOther.strSet(ranksNames,
    dataOther.topsUsed == 1, dataOther.ranksActive == 1);

  if (resultOther.empty())
    return side + " has " + resultOwn;

  string s = (dataOther.topsFull > 2 ? "none of" :
    (dataOther.topsFull == 2 ? "neither of" : "not"));

  return side + " has " + resultOwn + " and " + s + " " + resultOther;
}


string Product::strVerbalTopsOnly(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const Product& productWest,
  const Product& productEast,
  const VerbalData& dataWest,
  const VerbalData& dataEast,
  const bool flipAllowedFlag) const
{
  const bool singleActiveRank =
    (dataWest.ranksUsed == 1 && dataEast.ranksUsed == 1 &&
     dataWest.lowestRankActive == dataEast.lowestRankActive);

  if (dataEast.ranksActive == 0 || singleActiveRank)
  {
    return productWest.strVerbalTops(
      sumProfile, ranksNames, canonicalShift,
      "West", "East", dataWest, singleActiveRank, flipAllowedFlag);
  }
  else if (dataWest.ranksActive == 0)
  {
    return productEast.strVerbalTops(
      sumProfile, ranksNames, canonicalShift,
      "East", "West", dataEast, singleActiveRank, flipAllowedFlag);
  }

  bool preferWest;
  if (dataWest.ranksActive == 1 && dataEast.ranksActive > 1)
    preferWest = true;
  else if (dataWest.ranksActive > 1 && dataEast.ranksActive == 1)
    preferWest = false;
  else if (dataWest.topsUsed == 1 && dataEast.topsUsed > 1)
    preferWest = true;
  else if (dataEast.topsUsed == 1 && dataWest.topsUsed > 1)
    preferWest = false;
  else
    preferWest = productWest.topsSimplerThan(productEast);

  // TODO This part unchecked concerning any-tops.
  if (preferWest)
  {
    if (flipAllowedFlag)
      return productWest.strVerbalHighTopsOnlyBothSides(
        sumProfile, ranksNames, canonicalShift,
        productEast, "West", dataWest, dataEast);
   else
      return productWest.strVerbalTopsDual(
        sumProfile, ranksNames, canonicalShift, 
        productEast, "West", dataWest, dataEast);
  }
  else
  {
    if (flipAllowedFlag)
      return productEast.strVerbalHighTopsOnlyBothSides(
        sumProfile, ranksNames, canonicalShift,
        productWest, "East", dataEast, dataWest);
    else
      return productEast.strVerbalTopsDual(
        sumProfile, ranksNames, canonicalShift, 
        productWest, "East", dataEast, dataWest);
  }
}


/*--------------------------------------------------------------------*/
/*                                                                    */
/*                      Equal any top string methods                  */
/*                                                                    */
/*--------------------------------------------------------------------*/

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

  VerbalData dataWest, dataEast;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

  if (! length.used())
  {
    // This works for any tops as well.
    // TODO Have we lost symmFlag here?
    return Product::strVerbalTopsOnly(sumProfile, ranksNames,
      canonicalShift, productWest, productEast, dataWest, dataEast, false);
  }

  VerbalCover completions;

  if (dataWest.topsUsed + dataWest.freeUpper <=
    dataEast.topsUsed + dataEast.freeUpper)
  {
    if (productWest.makeCompletions(sumProfile, canonicalShift, dataWest,
      4, completions))
    {
      const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
        BLANK_PLAYER_CAP_WEST);

      vector<TemplateData> tdata;
      completions.makeList(bside, ranksNames, tdata);
      return verbalTemplates.get(TEMPLATES_LIST, tdata);
    }
  }
  else
  {
    if (productEast.makeCompletions(sumProfile, canonicalShift, dataEast,
      4, completions))
    {
      const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
        BLANK_PLAYER_CAP_EAST);

      vector<TemplateData> tdata;
      completions.makeList(bside, ranksNames, tdata);
      return verbalTemplates.get(TEMPLATES_LIST, tdata);
    }
  }

  completions.setLength(length);

  if (dataWest.ranksActive > 0)
  {
    Completion& vcWest = completions.activateSide(OPP_WEST);
    productWest.makePartialProfile(sumProfile, canonicalShift, vcWest);
  }

  if (dataEast.ranksActive > 0)
  {
    Completion& vcEast = completions.activateSide(OPP_EAST);
    productEast.makePartialProfile(sumProfile, canonicalShift, vcEast);
  }

  vector<TemplateData> tdata;
  return completions.strGeneral(
    sumProfile.length(), symmFlag, ranksNames, tdata);
}



/*--------------------------------------------------------------------*/
/*                                                                    */
/*                     Equal high top string methods                  */
/*                                                                    */
/*--------------------------------------------------------------------*/


string Product::strVerbalHighTopsOnlyBothSides(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const Product& productOther,
  const string& side,
  const VerbalData& data,
  const VerbalData& dataOther) const
{
  const unsigned char numOptions = 
    static_cast<unsigned char>(tops.size()) + 
    canonicalShift - data.ranksUsed;

  if (numOptions == 1)
  {
    // The lowest cards are a single rank of x'es.

    Completion completion;
    Product::makePartialProfile(sumProfile, canonicalShift, completion);
    const string result = completion.strSet(ranksNames, 
      false, data.ranksActive == 1);

    return side + " has " + result + 
      data.strXes(false, false);
  }
  else if (dataOther.freeUpper <= dataOther.topsFull)
  {
    // Prefer to state the low cards.
    Completion completion;
    Product::makePartialProfile(sumProfile, canonicalShift, completion);
    const string resultOwn = completion.strSet(ranksNames, 
      false, data.ranksActive == 1);

    return side + " has " + resultOwn + "(" +
      Product::strUsedBottoms(
        sumProfile, ranksNames, canonicalShift) + ")";
  }
  else if (dataOther.topsFull == 0)
  {
    Completion completion;
    Product::makePartialProfile(sumProfile, canonicalShift, completion);
    const string resultOwn = completion.strSet(ranksNames, 
      data.topsUsed == 1, data.ranksActive == 1);

    return side + " has " + resultOwn + " and perhaps lower cards";
  }
  else
  {
    return Product::strVerbalTopsDual(
      sumProfile, ranksNames, canonicalShift, 
        productOther, side, data, dataOther);
  }
}


string Product::strVerbalHighTopsSide(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const string& side,
  const BlankPlayerCap blankSide,
  const VerbalData& data,
  const unsigned char canonicalShift) const
{
  const unsigned char numOptions = 
     static_cast<unsigned char>(tops.size()) +
     canonicalShift - data.ranksUsed;

  if (numOptions == 1)
  {
    Completion completion;
    Product::makePartialProfile(sumProfile, canonicalShift, completion);
    const string result = completion.strSet(ranksNames, 
      false, data.ranksActive == 1);

    // We only have to set the x'es.
    return side + " has " + result + data.strXes(false, false);
  }
  else if (numOptions == 2 && data.freeUpper == 1)
  {
    // We need up to one low card.
    VerbalCover completions;
    if (Product::makeCompletions(sumProfile, canonicalShift, data,
      4, completions))
    {
      vector<TemplateData> tdata;
      completions.makeList(blankSide, ranksNames, tdata);
      return verbalTemplates.get(TEMPLATES_LIST, tdata);
    }

    // This would only fail if we had more than 4 options.
    assert(false);
    return "";
  }
  else if (data.topsUsed == 0)
  {
    string result = data.strFreeSemantic();

    if (data.freeUpper != 1)
      result += " completely";

    return side + " has " + result + " below the " +
        ranksNames.lowestCard(numOptions);
  }
  else
  {
    // General case.
    Completion completion;
    Product::makePartialProfile(sumProfile, canonicalShift, completion);
    string result = completion.strSet(ranksNames, 
      data.topsUsed == 1, data.ranksActive == 1);

    result += " and " + data.strFreeCount();

    const string cards = (data.freeUpper == 1 ? "card" : "cards");

    if (data.lowestRankActive == data.lowestRankUsed)
      result += ", lower-ranked " + cards;
    else
      result += " " + cards + " below the " + 
        ranksNames.lowestCard(numOptions);

    return side + " has " + result;
  }
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

  VerbalData dataWest, dataEast;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

  if (! length.used())
  {
    return Product::strVerbalTopsOnly(sumProfile, ranksNames,
      canonicalShift, productWest, productEast, dataWest, dataEast, true);
  }
  else if (dataWest.topsUsed + dataWest.freeUpper <=
    dataEast.topsUsed + dataEast.freeUpper)
  {
    const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
      BLANK_PLAYER_CAP_WEST);

    return productWest.strVerbalHighTopsSide(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "West"), 
      bside, dataWest, canonicalShift);
  }
  else
  {
    const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
      BLANK_PLAYER_CAP_EAST);

    return productEast.strVerbalHighTopsSide(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "East"), 
      bside, dataEast, canonicalShift);
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

  Completion completion;
  Product::makePartialProfile(sumProfile, canonicalShift, completion);
  result += completion.strSet(ranksNames, false, false);

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
      ranksNames,
      symmFlag);
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

