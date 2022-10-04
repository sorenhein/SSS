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
#include "../verbal/VerbalCover.h"
#include "../../../utils/table.h"


// Methods for each verbal cover type.

typedef void (Product::*VerbalMethod)(
  const Profile& profile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  const RanksNames& ranksNames,
  VerbalCover& verbalCost) const;

static const vector<VerbalMethod> verbalMethods =
{
  &Product::setVerbalDisaster,        // VERBAL_GENERAL
  &Product::setVerbalDisaster,        // VERBAL_HEURISTIC
  &Product::setVerbalLengthOnly,      // VERBAL_LENGTH_ONLY
  &Product::setVerbalOneTopOnly,      // VERBAL_ONE_TOP_ONLY
  &Product::setVerbalLengthAndOneTop, // VERBAL_LENGTH_AND_ONE_TOP
  &Product::setVerbalHighTopsEqual,   // VERBAL_HIGH_TOPS_EQUAL
  &Product::setVerbalAnyTopsEqual,    // VERBAL_ANY_TOPS_EQUAL
  &Product::setVerbalSingular         // VERBAL_SINGULAR
};


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
/*                         Completion methods                         */
/*                                                                    */
/**********************************************************************/

void Product::makeCompletionBottoms(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  Completion& completion) const
{
  // The zero'th top represents all the actual tops in sumProfile
  // from 0 up to canonicalShift-1.

  if (! tops[0].used())
  {
    // Fill with unused bottoms.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; 0)
      completion.setTop(topNo, false, sumProfile[topNo],
        sumProfile[topNo]);
    return;
  }

  assert(tops[0].getOperator() == COVER_EQUAL);

  const unsigned char bottoms = tops[0].lower();
  const unsigned char allBottoms = sumProfile.numBottoms(canonicalShift);

  if (bottoms == 0)
  {
    // Fill with zeroes.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; 0)
      completion.setTop(topNo, true, 0, sumProfile[topNo]);
  }
  else if (bottoms == allBottoms)
  {
    // Fill with maximum values.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; 0)
      completion.setTop(topNo, true, sumProfile[topNo],
        sumProfile[topNo]);
  }
  else
  {
    // Add a single bottom with the right number.
    assert(canonicalShift == 0);
    completion.setTop(0, true, bottoms, allBottoms);
  }
}


void Product::makeCompletion(
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
      tops[topNo-canonicalShift].lower(),
      sumProfile[topNo]);
  }

  completion.setFree(sumProfile.length(), length);

  Product::makeCompletionBottoms(sumProfile, canonicalShift, completion);
}


Opponent Product::singularUnusedSide(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const Completion& completion) const
{
  // Determine which side gets the unused tops.

  const unsigned char wlength = length.lower();
  const unsigned char numBottoms = sumProfile.numBottoms(canonicalShift);
  const unsigned char topsUsedWest = completion.getTopsUsed(OPP_WEST);

  if (topsUsedWest == wlength ||
      topsUsedWest + numBottoms == wlength)
    return OPP_EAST;
  else
    return OPP_WEST;
}


void Product::makeSingularCompletionBottoms(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const Opponent side,
  Completion& completion) const
{
  const unsigned char slength = sumProfile.length();
  const unsigned char wlength = length.lower();

  const unsigned char topsUsedWest = completion.getTopsUsed(OPP_WEST);
  const unsigned char topsUsedEast = completion.getTopsUsed(OPP_EAST);

  const unsigned char numBottoms = sumProfile.numBottoms(canonicalShift);

  if (topsUsedWest == wlength ||
      topsUsedEast + numBottoms == slength - wlength)
  {
    // All bottoms go to East.
    if (side == OPP_EAST)
    {
      for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
        completion.setTop(topNo, true, sumProfile[topNo],
          sumProfile[topNo]);
    }
  }
  else if (topsUsedEast == slength - wlength ||
      topsUsedWest + numBottoms == wlength)
  {
    // All bottoms go to West.
    if (side == OPP_WEST)
    {
      for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
        completion.setTop(topNo, true, sumProfile[topNo],
          sumProfile[topNo]);
    }
  }
  else if (canonicalShift == 0)
  {
    if (side == OPP_WEST)
      completion.setTop(0, true, wlength - topsUsedWest, sumProfile[0]);
    else
      completion.setTop(0, true, slength - wlength - topsUsedEast,
        sumProfile[0]);
  }
  else
    assert(false);
}


void Product::makeSingularCompletion(
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

  const unsigned char numBottoms = sumProfile.numBottoms(canonicalShift);

  // const Opponent sideOther = (side == OPP_WEST ? OPP_EAST : OPP_WEST);
  const unsigned char topsUsedWest = completion.getTopsUsed(OPP_WEST);
  const unsigned char topsUsedEast = completion.getTopsUsed(OPP_EAST);

  // Determine which side gets the unused tops.
  const Opponent sideForUnused = 
    (topsUsedWest == wlength || topsUsedWest + numBottoms == wlength ?
    OPP_EAST : OPP_WEST);

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
        side == OPP_WEST ? top.lower() : sumProfile[topNo] - top.lower(),
        sumProfile[topNo]);
    }
    else if (side == sideForUnused)
    {
      // Flip if we are filling the opposite side.
      completion.setTop(topNo, true,  sumProfile[topNo], sumProfile[topNo]);
    }
  }

  if (topsUsedWest == wlength ||
      topsUsedEast + numBottoms == slength - wlength)
  {
    // All bottoms go to East.
    if (side == OPP_EAST)
    {
      for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
        completion.setTop(topNo, true, sumProfile[topNo],
          sumProfile[topNo]);
    }
  }
  else if (topsUsedEast == slength - wlength ||
      topsUsedWest + numBottoms == wlength)
  {
    // All bottoms go to West.
    if (side == OPP_WEST)
    {
      for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
        completion.setTop(topNo, true, sumProfile[topNo],
          sumProfile[topNo]);
    }
  }
  else if (canonicalShift == 0)
  {
    if (side == OPP_WEST)
      completion.setTop(0, true, wlength - topsUsedWest, sumProfile[0]);
    else
      completion.setTop(0, true, slength - wlength - topsUsedEast,
        sumProfile[0]);
  }
  else
    assert(false);
}


bool Product::makeCompletionList(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const Opponent side,
  const unsigned char maxCompletions,
  list<Completion>& completions) const
{
  // We always put each completion in the West-East order, but if
  // side == OPP_WEST, we generate and sort them in such an order
  // that they go from higher to lower and from longer to shorter.
  Completion completion;
  Product::makeCompletion(sumProfile, canonicalShift, completion);

  // Put the open tops with the other side, as they are unused i.e.
  // zero with the active/primary side.
  for (auto openNo: completion.openTops())
    completion.updateTop(openNo, 0, sumProfile[openNo], side);

  list<Completion> stack;
  stack.push_back(completion);

  const unsigned char totalLower = completion.getTotalLower(side);
  const unsigned char totalUpper = completion.getTotalUpper(side);

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
        static_cast<unsigned char>(totalUpper - piter->length(side)));

      for (unsigned char count = maxCount+1; count-- > 0; )
      {
        piter->updateTop(openNo, count, sumProfile[openNo], side);

        if (piter->length(side) >= totalLower && 
            (count > 0 || firstOpen))
        {
          if (completions.size() >= maxCompletions)
            return false;

          completions.push_back(* piter);
        }

        if (piter->length(side) < totalUpper && openNo > 0)
          stack.push_back(* piter);
      }

      piter++;
      pno++;
    }
    firstOpen = false;
    stack.erase(stack.begin(), piter);
  }

  assert(stack.empty());

  // Sort stably from longer to shorter outputs.
  completions.sort([side](const Completion& c1, const Completion& c2)
  {
    return (side == OPP_WEST ? c1 < c2 : c1 > c2);
  });

  return true;
}


/**********************************************************************/
/*                                                                    */
/*                           Set methods                              */
/*                                                                    */
/**********************************************************************/


void Product::setVerbalDisaster(
  [[maybe_unused]] const Profile& sumProfile,
  [[maybe_unused]] const unsigned char canonicalShift,
  [[maybe_unused]] const bool symmFlag,
  [[maybe_unused]] const RanksNames& ranksNames,
  [[maybe_unused]] VerbalCover& verbalCover) const
{
  // This only happens if verbal is out of range in strVerbal.
  assert(false);
}


void Product::setVerbalLengthOnly(
  const Profile& sumProfile,
  [[maybe_unused]] const unsigned char canonicalShift,
  const bool symmFlag,
  [[maybe_unused]] const RanksNames& ranksNames,
  VerbalCover& verbalCover) const
{
  verbalCover.fillLengthOnly(length, sumProfile.length(), symmFlag);
}


void Product::setVerbalOneTopOnly(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  [[maybe_unused]] const RanksNames& ranksNames,
  VerbalCover& verbalCover) const
{
  assert(activeCount == 1);

  const unsigned char fullTopNo = 
    verbalCover.getCompletion().getLowestRankUsed();

  const unsigned char topNo = fullTopNo - canonicalShift;
  assert(tops[topNo].getOperator() != COVER_EQUAL);

  const VerbalSide vside = 
    {Product::simpler(sumProfile, canonicalShift), symmFlag};

  verbalCover.fillOnetopOnly(
    tops[topNo], sumProfile[fullTopNo], fullTopNo, vside);
}


void Product::setVerbalLengthAndOneTop(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  [[maybe_unused]] const RanksNames& ranksNames,
  VerbalCover& verbalCover) const
{
  assert(length.used());
  assert(activeCount == 1);

  const unsigned char fullTopNo = 
    verbalCover.getCompletion().getLowestRankUsed();

  const unsigned char topNo = fullTopNo - canonicalShift;
  assert(tops[topNo].getOperator() != COVER_EQUAL);

  const VerbalSide vside = 
    {Product::simpler(sumProfile, canonicalShift), symmFlag};

  verbalCover.fillOnetopLength(
    length, tops[topNo], sumProfile, fullTopNo, vside);
}


void Product::setVerbalTopsOnly(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  const RanksNames& ranksNames,
  const bool flipAllowedFlag,
  VerbalCover& verbalCover) const
{
  const VerbalSide vsideSingle = 
    {verbalCover.getCompletion().preferSingleActive(), symmFlag};

  if (vsideSingle.side != OPP_EITHER)
  {
    verbalCover.fillCompletion(vsideSingle, ranksNames);
    return;
  }

  const Opponent opp = verbalCover.getCompletion().preferSimpleActive();
  Opponent simplestOpponent;

  // TODO Could do a version of topsSimpler here where the tops are
  // equals.
  if (opp != OPP_EITHER)
    simplestOpponent = opp;
  else if (Product::topsSimpler(sumProfile, canonicalShift))
    simplestOpponent = OPP_WEST;
  else
    simplestOpponent = OPP_EAST;

  const VerbalSide vsideSimple = {simplestOpponent, symmFlag};

  const unsigned char numOptions = 
    verbalCover.getCompletion().numOptions();

  if (flipAllowedFlag && numOptions == 1)
  {
    // The lowest cards are a single rank of x'es.
    verbalCover.fillCompletionWithLows(vsideSimple, ranksNames);
  }
  else
  {
    verbalCover.fillTopsExcluding(vsideSimple, ranksNames);
  }
}


void Product::setVerbalAnyTopsEqual(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  const RanksNames& ranksNames,
  VerbalCover& verbalCover) const
{
  assert(activeCount > 0);

  if (! length.used())
  {
    // This works for any tops as well.
    Product::setVerbalTopsOnly(sumProfile, canonicalShift,
      symmFlag, ranksNames, false, verbalCover);
    return;
  }

  const Opponent side = (
    verbalCover.getCompletion().getTotalUpper(OPP_WEST) <=
    verbalCover.getCompletion().getTotalUpper(OPP_EAST) ? 
    OPP_WEST : OPP_EAST);

  const VerbalSide vside = {side, symmFlag};

  verbalCover.setLength(length);

  list<Completion> completions;
  if (Product::makeCompletionList(
    sumProfile, canonicalShift, side, 4, completions))
  {
    verbalCover.fillList(vside, ranksNames, completions);
  }
  else
  {
    // TODO This needs to be expanded somehow
    verbalCover.setGeneral(sumProfile.length(), symmFlag, ranksNames);
  }
}

  // TEMPLATE
  // if (sold == snew)
    // cout << "\n" << setw(40) << left << sold << "X1X " << snew << endl;
  // else
    // cout << "\n" << setw(40) << left << sold << "X2X " << snew << endl;



void Product::setVerbalHighTopsEqual(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  const RanksNames& ranksNames,
  VerbalCover& verbalCover) const
{
  assert(activeCount > 0);

  if (! length.used())
  {
    Product::setVerbalTopsOnly(sumProfile, canonicalShift,
      symmFlag, ranksNames, true, verbalCover);
    return;
  }

  const Opponent side = (
    verbalCover.getCompletion().getTotalUpper(OPP_WEST) <=
    verbalCover.getCompletion().getTotalUpper(OPP_EAST) ? 
    OPP_WEST : OPP_EAST);

  VerbalSide vside = {side, symmFlag};

  const unsigned char numOptions = verbalCover.getCompletion().numOptions();
  if (numOptions == 1)
  {
    verbalCover.fillBottoms(vside, ranksNames);
  }
  else if (numOptions == 2 && 
      verbalCover.getCompletion().getFreeUpper(side) == 1)
  {
    // "West has Q or Qx", so we need up to one low card.
    list<Completion> completions;
    if (! Product::makeCompletionList(sumProfile, canonicalShift, 
      side, 4, completions))
    {
      // We currently never get more than 4 options.
      assert(false);
    }
   
    verbalCover.fillList(vside, ranksNames, completions);
  }
  else if (verbalCover.getCompletion().getTopsUsed(side) == 0)
  {
    // "West has at most a doubleton completely below the ten".
    verbalCover.fillBelow(
      sumProfile.numBottoms(canonicalShift),
      ranksNames,
      numOptions,
      vside);
  }
  else
  {
    verbalCover.fillTopsAndLower(vside, ranksNames, numOptions);
  }
}


void Product::setVerbalSingular(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  [[maybe_unused]] const RanksNames& ranksNames,
  VerbalCover& verbalCover) const
{
  assert(length.used());
  assert(activeCount > 0);

  const VerbalSide vside = 
    {Product::simpler(sumProfile, canonicalShift), symmFlag};

  Product::makeSingularCompletion(sumProfile, canonicalShift,
    vside.side, verbalCover.getCompletion());

  const unsigned char len = (vside.side == OPP_WEST ?
    length.lower() : sumProfile.length() - length.lower());

  verbalCover.fillSingular(len, vside);
}


/*--------------------------------------------------------------------*/
/*                                                                    */
/*                           Overall method                           */
/*                                                                    */
/*--------------------------------------------------------------------*/

string Product::strVerbal(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  const RanksNames& ranksNames,
  const CoverVerbal verbal) const
{
  // Turn the product into a verbal string describing the cover.

  // First, make a Completion with some data about the product.
  VerbalCover verbalCover;
  Product::makeCompletion(sumProfile, canonicalShift, 
    verbalCover.getCompletion());

  // cout << "\nProduct " << Product::strLine() << ", symm " <<
    // symmFlag << "\n";

  // Then dereference into the right verbal method (including some
  // error handling).
  // TODO Later on, ranksNames should only be in string methods.
  (this->*(verbalMethods[verbal]))
    (sumProfile, canonicalShift, symmFlag, ranksNames, verbalCover);

  return verbalCover.str(ranksNames);
}

