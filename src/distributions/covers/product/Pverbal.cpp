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

#include "../verbal/VerbalCover.h"
#include "../verbal/VerbalSide.h"

#include "../../../utils/table.h"


// Methods for each verbal cover type.

typedef void (Product::*VerbalMethod)(
  const Profile& profile,
  const unsigned char canonicalShift,
  const bool symmFlag,
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


Opponent Product::simplerActive(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const Completion& completion) const
{
  const Opponent sideSimple = completion.preferSimpleActive();
  if (sideSimple != OPP_EITHER)
    return sideSimple;

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

  assert(tops[0].isEqual());

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


void Product::makeSingularCompletion(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  [[maybe_unused]] const Opponent side,
  Completion& completion) const
{
  // All given tops are exact.
  assert(length.used() && length.isEqual());

  // Determine which side gets the unused tops.
  const Opponent sideForUnused = Product::singularUnusedSide(
    sumProfile, canonicalShift, completion);

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
      completion.setTop(topNo, true, top.lower(), sumProfile[topNo]);
    }
    else
    {
      const unsigned char wno = 
        (sideForUnused == OPP_WEST ? sumProfile[topNo] : 0);
      completion.setTop(topNo, true, wno, sumProfile[topNo]);
    }
  }

  const unsigned char wlength = length.lower();
  const unsigned char elength = sumProfile.length() - length.lower();

  if (completion.getTopsUsed(OPP_WEST) == wlength)
  {
    // All bottoms go to East.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
      completion.setTop(topNo, true, 0, sumProfile[topNo]);
  }
  else if (completion.getTopsUsed(OPP_EAST) == elength)
  {
    // All bottoms go to West.
    for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
      completion.setTop(topNo, true, sumProfile[topNo], sumProfile[topNo]);
  }
  else if (canonicalShift == 0)
  {
    completion.setTop(
      0, true, wlength - completion.getTopsUsed(OPP_WEST), sumProfile[0]);
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
  completions.clear();

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
  [[maybe_unused]] VerbalCover& verbalCover) const
{
  // This only happens if verbal is out of range in strVerbal.
  assert(false);
}


void Product::setVerbalLengthOnly(
  const Profile& sumProfile,
  [[maybe_unused]] const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  const Opponent side = 
    (symmFlag ? OPP_WEST : length.shorter(sumProfile.length()));

  verbalCover.fillLength(length, sumProfile, {side, symmFlag});
}


void Product::setVerbalOneTopOnly(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(activeCount == 1);

  const unsigned char fullTopNo = 
    verbalCover.getCompletion().getLowestRankUsed();

  const unsigned char topNo = fullTopNo - canonicalShift;
  assert(! tops[topNo].isEqual());

  const VerbalSide vside = 
    {Product::simpler(sumProfile, canonicalShift), symmFlag};

  verbalCover.fillCountTops(tops[topNo], fullTopNo, sumProfile, vside);
}


void Product::setVerbalLengthAndOneTop(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(length.used());
  assert(activeCount == 1);

  const unsigned char fullTopNo = 
    verbalCover.getCompletion().getLowestRankUsed();

  const unsigned char topNo = fullTopNo - canonicalShift;
  assert(! tops[topNo].isEqual());

  const VerbalSide vside = 
    {Product::simpler(sumProfile, canonicalShift), symmFlag};

  verbalCover.fillCountTopsOrdinal(
    length, tops[topNo], fullTopNo, sumProfile, vside);
}


void Product::setVerbalTopsOnly(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  const bool flipAllowedFlag,
  VerbalCover& verbalCover) const
{
  const Completion& completion = verbalCover.getCompletion();

  const VerbalSide vsideSingle = 
    {completion.preferSingleActive(), symmFlag};

  if (vsideSingle.side != OPP_EITHER)
  {
    verbalCover.fillCompletion(vsideSingle);
    return;
  }

  const VerbalSide vsideSimple = 
    {Product::simplerActive(sumProfile, canonicalShift, completion), 
    symmFlag};

  if (flipAllowedFlag && completion.numOptions() == 1)
  {
    // The lowest cards are a single rank of x'es.
    verbalCover.fillExactlyTopsMaybeUnset(vsideSimple);
    return;
  }

  const VerbalSide vsideHigh = {completion.preferHighActive(), symmFlag};

  verbalCover.fillTwosided(vsideHigh);
}


void Product::setVerbalHighTopsEqual(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(activeCount > 0);

  if (! length.used())
  {
    Product::setVerbalTopsOnly(sumProfile, canonicalShift,
      symmFlag, true, verbalCover);
    return;
  }

  const Completion& completion = verbalCover.getCompletion();
  const Opponent side = Product::simpler(sumProfile, canonicalShift);
  const Opponent otherSide = (side == OPP_WEST ? OPP_EAST : OPP_WEST);

  VerbalSide vside = {side, symmFlag};

  const unsigned char numOptions = completion.numOptions();
  if (numOptions == 1)
  {
    verbalCover.fillTopsAndXes(vside);
  }
  else if (numOptions == 2 && completion.getFreeUpper(side) == 1)
  {
    // "West has Q or Qx", so we need up to one low card.
    // We currently never get more than 4 options.
    assert(Product::makeCompletionList(
      sumProfile, canonicalShift, side, 4, verbalCover.getCompletions()));
   
    verbalCover.fillExactlyList(vside);
  }
  else if (completion.getTopsUsed(side) == 0)
  {
    // "West has at most a doubleton completely below the ten".
    verbalCover.fillLengthBelowTops(
      sumProfile.numBottoms(canonicalShift), vside);
  }
  else if (completion.getTopsUsed(otherSide) == 0)
  {
    verbalCover.fillOnesided(length, sumProfile, vside);
  }
  else
  {
    verbalCover.fillTopsAndLowerMultiple(length, sumProfile, vside);
  }
}


void Product::setVerbalAnyTopsEqual(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(activeCount > 0);

  if (! length.used())
  {
    Product::setVerbalTopsOnly(
      sumProfile, canonicalShift, symmFlag, false, verbalCover);
    return;
  }

  const Opponent side = Product::simpler(sumProfile, canonicalShift);

  if (Product::makeCompletionList(
    sumProfile, canonicalShift, side, 4, verbalCover.getCompletions()))
  {
    verbalCover.fillExactlyList({side, symmFlag});
    return;
  }

  // Have to undo the 4+ completions from above.  Ugh.
  verbalCover.getCompletions().resize(1);
  Product::makeCompletion(sumProfile, canonicalShift, 
    verbalCover.getCompletion());

  if (verbalCover.getCompletion().getTopsFull(OPP_WEST) == 0)
  {
    verbalCover.fillOnesided(length, sumProfile, {OPP_EAST, symmFlag});
  }
  else if (verbalCover.getCompletion().getTopsFull(OPP_EAST) == 0)
  {
    verbalCover.fillOnesided(length, sumProfile, {OPP_WEST, symmFlag});
  }
  else
  {
    verbalCover.fillTwosidedLength(length, sumProfile, 
      {Product::simpler(sumProfile, canonicalShift), symmFlag});
  }
}


void Product::setVerbalSingular(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const bool symmFlag,
  VerbalCover& verbalCover) const
{
  assert(length.used());
  assert(activeCount > 0);

  const Opponent side = Product::simpler(sumProfile, canonicalShift);

  const VerbalSide vside = {side, symmFlag};

  Product::makeSingularCompletion(sumProfile, canonicalShift,
    side, verbalCover.getCompletion());

  verbalCover.fillSingular(length, sumProfile, vside);
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
  // Sometimes this is overtaken locally by a list of completions.
  VerbalCover verbalCover;
  Product::makeCompletion(sumProfile, canonicalShift, 
    verbalCover.getCompletion());

// cout << Product::strLine() << endl;
// cout << "verbal " << verbal << endl;

  // Then dereference into the right verbal method (including some
  // error handling).
  (this->*(verbalMethods[verbal]))
    (sumProfile, canonicalShift, symmFlag, verbalCover);

  return verbalCover.str(ranksNames);
}

