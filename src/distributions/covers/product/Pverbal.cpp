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

#include "../CoverCategory.h"

#include "../../../ranks/RanksNames.h"

#include "../term/CoverOperator.h"
#include "../term/TopData.h"

#include "../../../utils/table.h"


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

class VerbalComb
{
  // A single combination such as 97x.

  private:

    vector<unsigned char> partialTops;
    list<unsigned char> openTopNumbers;
    unsigned char lengthInt;

  public:

  void resize(const size_t numTops)
  {
    partialTops.resize(numTops);
    openTopNumbers.clear();
    lengthInt = 0;
  };

  void setTop(
    const unsigned char topNo,
    const bool usedFlag,
    const unsigned char count)
  {
    // Must be an equal top.
    if (usedFlag)
    {
      partialTops[topNo] = count;
      lengthInt += count;
    }
    else
      openTopNumbers.push_back(topNo);
  };

  void updateTop(
    const unsigned char topNo,
    const unsigned char count)
  {
    lengthInt += count - partialTops[topNo];
    partialTops[topNo] = count;
  };

  const list<unsigned char>& openTops() const
  {
    return openTopNumbers;
  };

  unsigned char length() const
  {
    return lengthInt;
  };

  string strDebug() const
  {
    stringstream ss;

    ss << "tops ";
    for (auto p: partialTops)
      ss << +p << " ";
    ss << "\n";

    ss << "open ";
    for (auto o: openTopNumbers)
      ss << +o << " ";
    ss << "\n";

    ss << "length " << +lengthInt << "\n";
    return ss.str();
  };

  string str(const RanksNames& ranksNames) const
  {
    if (lengthInt == 0)
      return "void";

    string s;
    for (unsigned char topNo = 
      static_cast<unsigned char>(partialTops.size()); topNo-- > 0; )
    {
      if (partialTops[topNo])
      {
        s += ranksNames.strOpponents(topNo, partialTops[topNo],
          false, false);
      }
    }
    return s;
  };
};

class VerbalCover
{
  private:
    
    list<VerbalComb> completions;

  public:

    void push_back(const VerbalComb& verbalComb)
    {
      completions.push_back(verbalComb);
    };

    unsigned char size() const
    {
      return static_cast<unsigned char>(completions.size());
    };

    string str(const RanksNames& ranksNames) const
    {
      string s;
      size_t i = 0;

      for (auto& verbalComb: completions)
      {
        if (i > 0)
          s += (i+1 == completions.size() ? " or " : ", ");

        s += verbalComb.str(ranksNames);
        i++;
      }
      return s;
    };
};


void Product::makePartialProfile(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  VerbalComb& verbalComb) const
{
  // We have some top's that are fixed to a single value.
  // We have some explicit, unused tops.
  // We have 1 or more unused implicit bottoms (canonicalShift+1).

  verbalComb.resize(sumProfile.size());

  for (unsigned char topNo = static_cast<unsigned char>(sumProfile.size());
      topNo-- > 0; )
  {
    if (topNo >= canonicalShift)
      verbalComb.setTop(
        topNo, 
        tops[topNo-canonicalShift].used(),
        tops[topNo-canonicalShift].lower());
    else
      verbalComb.setTop(topNo, false, 0);
  }
}


bool Product::makeCompletions(
  const Profile& sumProfile,
  const unsigned char canonicalShift,
  const VerbalData& data,
  const unsigned char maxCompletions,
  VerbalCover& completions) const
{
  VerbalComb verbalComb;

  Product::makePartialProfile(sumProfile, canonicalShift, verbalComb);

  list<VerbalComb> stack;
  stack.push_back(verbalComb);

  const unsigned char totalLower = data.topsUsed + data.freeLower;
  const unsigned char totalUpper = data.topsUsed + data.freeUpper;

  // It's OK to have zero of the highest open top.
  // After that, a zero top was already implicit in some earlier
  // completion.
  bool firstOpen = true;

  // for (auto openNo: pstack.openTopNumbers)
  for (auto openNo: verbalComb.openTops())
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

      // for (unsigned char count = 0; count <= maxCount; count++)
      for (unsigned char count = maxCount+1; count-- > 0; )
      {
        piter->updateTop(openNo, count);

        if (piter->length() >= totalLower && (count > 0 || firstOpen))
        {
          completions.push_back(* piter);
          if (completions.size() > maxCompletions ||
              (data.freeLower > 0 && completions.size() == maxCompletions))
            return false;
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
  return true;
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
  // This method doesn't only do bottoms, but "the opposite of" tops.
  // In any event it expands cards hidden by a canonical shift.
  // If we have AQT86 and the queen is not used but AT8 are,
  // then we get Q6.  If AQT86 is stored as AQTx with a shift of 1,
  // and if AQT are used, we get 8x.

  string result = "";

  for (unsigned char topNo = static_cast<unsigned char>(tops.size()); 
    topNo-- > 0; )
  {
    const auto& top = tops[topNo];
    if (top.used() && topNo > canonicalShift)
      continue;

    const unsigned char count = (allFlag ?
      sumProfile[topNo] : top.lower());

    result += ranksNames.strOpponents(topNo, count, expandFlag, false);
  }

  return result;
}


string Product::strAddBottom(
  const RanksNames& ranksNames,
  const unsigned char canonicalShift,
  const string& base,
  const bool emptyFlag) const
{
  // Adds either no bottom or exactly one of each possible bottom.

  string result = "";

  // Show the version without any low card.
  if (emptyFlag)
    result = base;

  for (unsigned char topNo = canonicalShift+1; topNo-- > 0; )
  {
    if (! result.empty())
      result += (topNo == 0 ? " or " : ", ");

    result += base + ranksNames.strOpponents(topNo, 1, false, false);
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


/*--------------------------------------------------------------------*/
/*                                                                    */
/*                   Equal high/any top string methods                */
/*                                                                    */
/*--------------------------------------------------------------------*/


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
    return side + " has " + 
      Product::strUsedTops(
        sumProfile, ranksNames, canonicalShift, 
        false, true, data.ranksActive == 1, false);
  }
  else
  {
    // State it from the other side.  If the tops are not all
    // on the high end, but scattered, this output will not make
    // sense.  So flipAllowedFlag should only be set for high tops.
    return sideOther + " has " + "(" + 
      Product::strUsedBottoms(
        sumProfile, ranksNames, canonicalShift, true, false) + ")";
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
  const string resultOwn = Product::strUsedTops(
    sumProfile, ranksNames, canonicalShift, 
    false, data.topsUsed == 1, data.ranksActive == 1, false);

  const string resultOther = productOther.strUsedTops(
    sumProfile, ranksNames, canonicalShift, 
    false, dataOther.topsFull == 1, dataOther.ranksFull == 1, true);

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

string Product::strVerbalAnyTopsSide(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const string& side,
  const VerbalData& data,
  const unsigned char canonicalShift) const
{
  string result = Product::strUsedTops(
    sumProfile, ranksNames, canonicalShift, 
    false, data.ranksActive == 1, false, false);

  if (result.empty())
    result = "none";

  result += " out of " + Product::strUsedTops(
    sumProfile, ranksNames, canonicalShift, 
    true, false, false, false);

  if (data.freeLower == data.freeUpper)
    result += " and " + to_string(data.freeLower);
  else
  {
    result += " and " + to_string(data.freeLower) + "-" + 
      to_string(data.freeUpper);
  }

  return side + " has " + result + " cards of other ranks";
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

  VerbalData dataWest, dataEast;
  Product::fillUsedTops(sumProfile, canonicalShift, 
    productWest, productEast, dataWest, dataEast);

  if (! length.used())
  {
    // This works for any tops as well.
    return Product::strVerbalTopsOnly(sumProfile, ranksNames,
      canonicalShift, productWest, productEast, dataWest, dataEast, false);
  }
  else if (dataWest.topsUsed + dataWest.freeUpper <=
    dataEast.topsUsed + dataEast.freeUpper)
  {
    VerbalComb verbalComb;
    VerbalCover completions;
    const string side = (symmFlag ? "Either opponent" : "West");

    productWest.makePartialProfile(sumProfile, canonicalShift, verbalComb);
    if (productWest.makeCompletions(sumProfile, canonicalShift, dataWest,
      4, completions))
    {
      return side + " has " + completions.str(ranksNames);
    }
    else
      return productWest.strVerbalAnyTopsSide(sumProfile, ranksNames, 
        side, dataWest, canonicalShift);
  }
  else
  {
    VerbalComb verbalComb;
    VerbalCover completions;
    const string side = (symmFlag ? "Either opponent" : "East");

    productEast.makePartialProfile(sumProfile, canonicalShift, verbalComb);
    if (productEast.makeCompletions(sumProfile, canonicalShift, dataEast,
      4, completions))
    {
      return side + " has " + completions.str(ranksNames);
    }
    else
      return productEast.strVerbalAnyTopsSide(sumProfile, ranksNames, 
        side, dataEast, canonicalShift);
  }
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

    const string result = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, false, data.ranksActive == 1, false);

    return side + " has " + result + 
      data.strXes(false, false);
  }
  else if (dataOther.freeUpper <= dataOther.topsFull)
  {
    // Prefer to state the low cards.
    const string resultOwn = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, false, data.ranksActive == 1, false);

    return side + " has " + resultOwn + "(" +
      Product::strUsedBottoms(
        sumProfile, ranksNames, canonicalShift, true, false) + ")";
  }
  else if (dataOther.topsFull == 0)
  {
    const string resultOwn = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, data.topsUsed == 1, data.ranksActive == 1, false);

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
  const VerbalData& data,
  const unsigned char canonicalShift) const
{
  const unsigned char numOptions = 
     static_cast<unsigned char>(tops.size()) +
     canonicalShift - data.ranksUsed;

  if (numOptions == 1)
  {
    string result = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, false, data.ranksActive == 1, false);

    // We only have to set the x'es.
    return side + " has " + result + data.strXes(false, false);
  }
  else if (numOptions == 2 && data.freeUpper == 1)
  {
    // We need up to one low card.
    string result = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, false, data.ranksActive == 1, false);

    return side + " has " +
      Product::strAddBottom(ranksNames, canonicalShift, 
        result, data.freeLower == 0);
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
    string result = Product::strUsedTops(
      sumProfile, ranksNames, canonicalShift, 
      false, data.topsUsed == 1, data.ranksActive == 1, false);

    result += " and " + data.strFreeCount();

    if (data.lowestRankActive == data.lowestRankUsed)
      result += ", lower-ranked cards";
    else
      result += " cards below the " + ranksNames.lowestCard(numOptions);

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
    return productWest.strVerbalHighTopsSide(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "West"), 
      dataWest, canonicalShift);
  }
  else
  {
    return productEast.strVerbalHighTopsSide(sumProfile, ranksNames, 
      (symmFlag ? "Either opponent" : "East"), 
      dataEast, canonicalShift);
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

