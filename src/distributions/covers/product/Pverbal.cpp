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

#include "../../../ranks/RanksNames.h"

#include "../../../utils/table.h"


/**********************************************************************/
/*                                                                    */
/*                          Side comparators                          */
/*                                                                    */
/**********************************************************************/


bool Product::topsSimpler(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  const unsigned char s = static_cast<unsigned char>(tops.size());
  assert(static_cast<unsigned>(s + canonicalShift) == sumProfile.size());

  for (unsigned char topNo = s; topNo-- > 0; )
  {
    if (! tops[topNo].used())
      continue;

    const Opponent lTop = 
      tops[topNo].longer(sumProfile[topNo + canonicalShift]);

    if (lTop != OPP_EITHER)
      return (lTop == OPP_WEST);
  }

  return true;
}


Opponent Product::simpler(
  const Profile& sumProfile,
  const unsigned char canonicalShift) const
{
  const Opponent shorter = length.shorter(sumProfile.length());
  if (shorter != OPP_EITHER)
    return shorter;
  
  return (Product::topsSimpler(sumProfile, canonicalShift) ? 
    OPP_WEST : OPP_EAST);
}



/**********************************************************************/
/*                                                                    */
/*                    Count and numerical methods                     */
/*                                                                    */
/**********************************************************************/

void Product::study(
   const Profile& sumProfile,
   const unsigned char canonicalShift,
  VerbalData& data) const
{
  // TODO This is an attempt at a more streamlined study.

  data.reset();

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    topNo-- > 0; )
  {
    auto& top = tops[topNo];
    if (top.used())
    {
      const unsigned char t = tops[topNo].lower();
      data.topsUsed += t;
      data.topsUsedOther += sumProfile[topNo + canonicalShift] - t;
      data.lowestRankUsed = topNo;
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


void Product::completeSingular(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const Opponent side,
  Completion& completion) const
{
  // All given tops are exact.  Length must be set.

  // TODO Make some smaller methods.

  assert(length.used());
  assert(length.getOperator() == COVER_EQUAL);

  const unsigned char slength = sumProfile.length();
  const unsigned char wlength = length.lower();

  const unsigned char numBottoms = 
    Product::countBottoms(sumProfile, canonicalShift);

  VerbalData data;
  Product::study(sumProfile, canonicalShift, data);

  // Determine which side gets the unused tops.
  const Opponent sideForUnused = 
    (data.topsUsed == wlength || data.topsUsed + numBottoms == wlength ?
    OPP_EAST : OPP_WEST);

/*
cout << "Product " << Product::strLine() << endl;
cout << "side " << side << "\n";
cout << data.str("data");
cout << "numBottoms " << +numBottoms << endl;
cout << "side for unused " << sideForUnused << endl;
 */

  // Cover from the highest top down to canonicalShift (exclusive).
  // If canonicalShift is 0, then we will stop at 1, but then the 0th
  // real top is by convention unset.
  completion.resize(sumProfile.size());
  for (unsigned char topNo = static_cast<unsigned char>(sumProfile.size());
      --topNo > canonicalShift; )
  {
    const auto& top = tops[topNo - canonicalShift];

    if (top.used())
    {
      completion.setTop(topNo, true, 
        side == OPP_WEST ? top.lower() : sumProfile[topNo] - top.lower());
    }
    else if (side == sideForUnused)
    {
      // Flip if we are filling the opposite side.
      completion.setTop(topNo, true,  sumProfile[topNo]);
    }
  }

  if (data.topsUsed == wlength ||
      data.topsUsedOther + numBottoms == slength - wlength)
  {
    // All bottoms go to East.
    if (side == OPP_EAST)
    {
      for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
        completion.setTop(topNo, true, sumProfile[topNo]);
    }
  }
  else if (data.topsUsedOther == slength - wlength ||
      data.topsUsed + numBottoms == wlength)
  {
    // All bottoms go to West.
    if (side == OPP_WEST)
    {
      for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
        completion.setTop(topNo, true, sumProfile[topNo]);
    }
  }
  else if (canonicalShift == 0)
  {
    if (side == OPP_WEST)
      completion.setTop(0, true, wlength - data.topsUsed);
    else
      completion.setTop(0, true, slength - wlength - data.topsUsedOther);
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
      completion.setTop(topNo, false, sumProfile[topNo]);
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
  list<Completion>& completions) const
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
  completions.sort();
  return true;
}


/**********************************************************************/
/*                                                                    */
/*                             String methods                         */
/*                                                                    */
/**********************************************************************/


void Product::setVerbalLengthOnly(
  const Profile& sumProfile,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(activeCount == 0);

  verbalCover.fillLengthOnly(length, sumProfile.length(), symmFlag);
}


void Product::setVerbalOneTopOnly(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(activeCount == 1);

  VerbalData data;
  Product::study(sumProfile, canonicalShift, data);
  const unsigned char topNo = data.lowestRankUsed;
  assert(tops[topNo].getOperator() != COVER_EQUAL);

  const Opponent simplestOpponent = Product::simpler(
    sumProfile, canonicalShift);

  verbalCover.fillOnetopOnly(
    tops[topNo],
    sumProfile[topNo + canonicalShift],
    topNo + canonicalShift,
    simplestOpponent,
    symmFlag);
}


void Product::setVerbalLengthAndOneTop(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(length.used());
  assert(activeCount == 1);

  VerbalData data;
  Product::study(sumProfile, canonicalShift, data);
  const unsigned char topNo = data.lowestRankUsed;
  assert(tops[topNo].getOperator() != COVER_EQUAL);

  const Opponent simplestOpponent = Product::simpler(
    sumProfile, canonicalShift);

  verbalCover.fillOnetopLength(
    length, tops[topNo], sumProfile, topNo + canonicalShift,
    simplestOpponent, symmFlag);
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
  const Opponent simplestOpponent,
  [[maybe_unused]] const string& side,
  const bool symmFlag,
  const VerbalData& data) const
{
  // The other side is known to use no tops at all.

  Completion completion;
  Product::makePartialProfile(sumProfile, canonicalShift, completion);

  VerbalCover verbalCover;
  verbalCover.fillCompletion(simplestOpponent, symmFlag,
    ranksNames, completion, data);

  return verbalCover.str(TEMPLATES_LIST, ranksNames);
}


void Product::setVerbalTopsExcluding(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const RanksNames& ranksNames,
  const Product& productOther,
  const Opponent simplestOpponent,
  const bool symmFlag,
  const VerbalData& data,
  const VerbalData& dataOther,
  VerbalCover& verbalCover) const
{
  Completion completionRown;
  Product::makePartialProfile(sumProfile, canonicalShift, completionRown);

  Completion completionOther;
  productOther.makePartialProfile(
    sumProfile, canonicalShift, completionOther);

  verbalCover.fillTopsExcluding(simplestOpponent, symmFlag,
    completionRown, completionOther, data, dataOther, ranksNames);
}


string Product::strVerbalTopsOnly(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  const RanksNames& ranksNames,
  const Product& productWest,
  const Product& productEast,
  const VerbalData& dataWest,
  const VerbalData& dataEast,
  const bool flipAllowedFlag) const
{
  const bool singleActiveRank =
    (dataWest.ranksUsed == 1 && dataEast.ranksUsed == 1 &&
     dataWest.lowestRankActive == dataEast.lowestRankActive);

  // TODO symmFlag relevant here too?!
  if (dataEast.ranksActive == 0 || singleActiveRank)
  {
    return productWest.strVerbalTops(
      sumProfile, ranksNames, canonicalShift, OPP_WEST, "West", 
      symmFlag, dataWest);
  }
  else if (dataWest.ranksActive == 0)
  {
    return productEast.strVerbalTops(
      sumProfile, ranksNames, canonicalShift, OPP_EAST, "East", 
      symmFlag, dataEast);
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
    preferWest = productWest.topsSimpler(sumProfile, canonicalShift);

  // TODO This part unchecked concerning any-tops.
  if (preferWest)
  {
    if (flipAllowedFlag)
    {
      return productWest.strVerbalHighTopsOnlyBothSides(
        sumProfile, ranksNames, canonicalShift,
        productEast, OPP_WEST, symmFlag, dataWest, dataEast);
    }
    else
    {
      VerbalCover verbalCover;

      productWest.setVerbalTopsExcluding(
        sumProfile, canonicalShift, ranksNames, productEast, 
        OPP_WEST, symmFlag, dataWest, dataEast, verbalCover);

      return verbalCover.str(TEMPLATES_TOPS_EXCLUDING, ranksNames);
    }
  }
  else
  {
    if (flipAllowedFlag)
    {
      return productEast.strVerbalHighTopsOnlyBothSides(
        sumProfile, ranksNames, canonicalShift,
        productWest, OPP_EAST, symmFlag, dataEast, dataWest);
    }
    else
    {
      VerbalCover verbalCover;

      productEast.setVerbalTopsExcluding(
        sumProfile, canonicalShift, ranksNames, productWest, 
        OPP_EAST, symmFlag, dataEast, dataWest, verbalCover);

      return verbalCover.str(TEMPLATES_TOPS_EXCLUDING, ranksNames);
    }
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
    return Product::strVerbalTopsOnly(sumProfile, canonicalShift,
      symmFlag, ranksNames,
      productWest, productEast, dataWest, dataEast, false);
  }

  list<Completion> completions;

  if (dataWest.topsUsed + dataWest.freeUpper <=
    dataEast.topsUsed + dataEast.freeUpper)
  {
    if (productWest.makeCompletions(sumProfile, canonicalShift, dataWest,
      4, completions))
    {
      // const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
        // BLANK_PLAYER_CAP_WEST);

      VerbalCover verbalCover;
      verbalCover.fillList(OPP_WEST, symmFlag, ranksNames, completions);
      return verbalCover.str(TEMPLATES_LIST, ranksNames);
    }
  }
  else
  {
    if (productEast.makeCompletions(sumProfile, canonicalShift, dataEast,
      4, completions))
    {
      // const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
        // BLANK_PLAYER_CAP_EAST);

      VerbalCover verbalCover;
      verbalCover.fillList(OPP_EAST, symmFlag, ranksNames, completions);
      return verbalCover.str(TEMPLATES_LIST, ranksNames);
    }
  }

  VerbalCover verbalCover;
  verbalCover.setLength(length);

  if (dataWest.ranksActive > 0)
  {
    Completion& vcWest = verbalCover.activateSide(OPP_WEST);
    productWest.makePartialProfile(sumProfile, canonicalShift, vcWest);
  }

  if (dataEast.ranksActive > 0)
  {
    Completion& vcEast = verbalCover.activateSide(OPP_EAST);
    productEast.makePartialProfile(sumProfile, canonicalShift, vcEast);
  }

  vector<TemplateData> tdata;
  return verbalCover.strGeneral(
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
  const Opponent simplestOpponent,
  const bool symmFlag,
  const VerbalData& data,
  const VerbalData& dataOther) const
{
  // TODO numOptions could even to in data?
  const unsigned char numOptions = 
    static_cast<unsigned char>(tops.size()) + 
    canonicalShift - data.ranksUsed;

  if (numOptions == 1)
  {
    // The lowest cards are a single rank of x'es.

    Completion completion;
    Product::makePartialProfile(sumProfile, canonicalShift, completion);

    VerbalCover verbalCover;
    verbalCover.fillCompletionWithLows(simplestOpponent, symmFlag,
      ranksNames, completion, data);

    return verbalCover.str(TEMPLATES_LIST, ranksNames);
  }
  else
  {
    // "West has X and not Y".
    VerbalCover verbalCover;

    Product::setVerbalTopsExcluding(
      sumProfile, canonicalShift, ranksNames, productOther, 
      simplestOpponent, symmFlag, data, dataOther, verbalCover);

    return verbalCover.str(TEMPLATES_TOPS_EXCLUDING, ranksNames);
  }
}


string Product::strVerbalHighTopsSide(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const Opponent simplestOpponent,
  const string& side,
  const bool symmFlag,
  [[maybe_unused]] const BlankPlayerCap blankSide,
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
    const string resultHigh = completion.strSet(ranksNames, 
      false, data.ranksActive == 1);

    const string sold = data.strXes(false, false);

    // This is not the same, e.g. x(x) vs (xx), (xx) vs (QJT987).
    // It also confuses T(98) with x(xx).  Look at ranksNames.

    /*
    const string snew = "(" + completion.strUnset(ranksNames) + ")";

  if (sold == snew)
    cout << "\n" << setw(40) << left << sold << "X1X " << snew << endl;
  else
    cout << "\n" << setw(40) << left << sold << "X2X " << snew << endl;
    */

    // We only have to set the x'es.
    return side + " has " + resultHigh + sold;
  }
  else if (numOptions == 2 && data.freeUpper == 1)
  {
    // We need up to one low card.
    // "West has Q or Qx".
    list<Completion> completions;
    if (! Product::makeCompletions(sumProfile, canonicalShift, 
      data, 4, completions))
    {
      // We currently never get more than 4 options.
      assert(false);
    }

    VerbalCover verbalCover;
    verbalCover.fillList(simplestOpponent, symmFlag, 
      ranksNames, completions);

    return verbalCover.str(TEMPLATES_LIST, ranksNames);
  }
  else if (data.topsUsed == 0)
  {
    // "West has at most a doubleton completely below the ten".
    VerbalCover verbalCover;

    verbalCover.fillBelow(
      data.freeLower, 
      data.freeUpper,
      Product::countBottoms(sumProfile, canonicalShift),
      ranksNames,
      numOptions,
      simplestOpponent,
      symmFlag);

    return verbalCover.str(TEMPLATES_ONLY_BELOW, ranksNames);
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
    return Product::strVerbalTopsOnly(sumProfile, canonicalShift,
      symmFlag, ranksNames,
      productWest, productEast, dataWest, dataEast, true);
  }
  else if (dataWest.topsUsed + dataWest.freeUpper <=
    dataEast.topsUsed + dataEast.freeUpper)
  {
    const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
      BLANK_PLAYER_CAP_WEST);

    return productWest.strVerbalHighTopsSide(sumProfile, ranksNames, 
      OPP_WEST,
      (symmFlag ? "Either opponent" : "West"), 
      symmFlag,
      bside, dataWest, canonicalShift);
  }
  else
  {
    const BlankPlayerCap bside = (symmFlag ? BLANK_PLAYER_CAP_EITHER :
      BLANK_PLAYER_CAP_EAST);

    return productEast.strVerbalHighTopsSide(sumProfile, ranksNames, 
      OPP_EAST,
      (symmFlag ? "Either opponent" : "East"), 
      symmFlag,
      bside, dataEast, canonicalShift);
  }
}


/*--------------------------------------------------------------------*/
/*                                                                    */
/*                       Singular string methods                      */
/*                                                                    */
/*--------------------------------------------------------------------*/


void Product::setVerbalSingular(
  const Profile& sumProfile,
  const bool symmFlag,
  const unsigned char canonicalShift,
  VerbalCover& verbalCover) const
{
  assert(length.used());
  assert(activeCount > 0);

  const Opponent simplestOpponent = Product::simpler(
    sumProfile, canonicalShift);

  const unsigned char len = (simplestOpponent == OPP_WEST ?
    length.lower() : sumProfile.length() - length.lower());

  Completion completion;
  Product::completeSingular(sumProfile, canonicalShift,
    simplestOpponent, completion);

  verbalCover.fillSingular(completion, len, simplestOpponent, symmFlag);
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

  VerbalCover verbalCover;

 // cout << "\nProduct " << Product::strLine() << "\n";
  if (verbal == VERBAL_LENGTH_ONLY)
  {
    Product::setVerbalLengthOnly(sumProfile, symmFlag, verbalCover);
    
    return verbalCover.str(TEMPLATES_LENGTH_ONLY, ranksNames);
  }
  else if (verbal == VERBAL_TOPS_ONLY)
  {
    Product::setVerbalOneTopOnly(
      sumProfile, canonicalShift, symmFlag, verbalCover);

    return verbalCover.str(TEMPLATES_ONETOP, ranksNames);
  }
  else if (verbal == VERBAL_LENGTH_AND_ONE_TOP)
  {
    Product::setVerbalLengthAndOneTop(
      sumProfile, canonicalShift, symmFlag, verbalCover);

    return verbalCover.str(TEMPLATES_TOPS_LENGTH, ranksNames);
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
    Product::setVerbalSingular(
      sumProfile, symmFlag, canonicalShift, verbalCover);

    return verbalCover.str(TEMPLATES_TOPS_LENGTH, ranksNames);
  }
  else
  {
    assert(false);
    return "";
  }
}

