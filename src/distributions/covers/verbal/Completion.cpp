/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Completion.h"

#include "../term/Term.h"

#include "../../../ranks/RanksNames.h"

#include "../../../languages/Dictionary.h"
#include "../../../languages/connections/words.h"

#include "../../../utils/table.h"

extern Dictionary dictionary;


void Completion::resize(const size_t numTops)
{
  west.resize(numTops);
  east.resize(numTops);

  used.resize(numTops, false);
  openTopNumbers.clear();

  ranksUsed = 0;
  dataWest.reset();
  dataEast.reset();
}


void Completion::setTop(
  const unsigned char topNo,
  const bool usedFlag,
  const unsigned char countWest,
  const unsigned char maximum)
{
  // Must be an equal top.

  assert(topNo < used.size());
  used[topNo] = usedFlag;

  // Permit a maximum value to be stored even if the top is unused.

  if (usedFlag)
  {
    west[topNo] = countWest;
    east[topNo] = maximum - countWest;

    dataWest.length += countWest;
    dataEast.length += maximum - countWest;

    ranksUsed++;
    lowestRankUsed = topNo;
    dataWest.update(topNo, countWest, maximum);
    dataEast.update(topNo, maximum - countWest, maximum);
  }
  else
  {
    west[topNo] = maximum;
    east[topNo] = maximum;

    openTopNumbers.push_back(topNo);
  }
}


void Completion::updateTop(
  const unsigned char topNo,
  const unsigned char countSide,
  const unsigned char maximum,
  const Opponent side)
{
  // This method does not respect anything except the numbers
  // in west and east, the length and topsUsed.  The consistency in 
  // openTopNumbers etc.  is lost.  It should only be used from a 
  // method such as Product::makeCompletionList().

  assert(topNo < used.size());

  const unsigned char countWest = (side == OPP_WEST ?
    countSide : maximum - countSide);
  const unsigned char countEast = (side == OPP_WEST ?
    maximum - countSide : countSide);

  if (used[topNo])
  {
    dataWest.length += countWest - west[topNo];
    dataEast.length += countEast - east[topNo];

    dataWest.topsUsed += countWest - west[topNo];
    dataEast.topsUsed += countEast - east[topNo];

    if (west[topNo] == 0 && countWest > 0)
      dataWest.ranksActive++;
    else if (west[topNo] != 0 && countWest == 0)
      dataWest.ranksActive--;

    if (east[topNo] == 0 && countEast > 0)
      dataEast.ranksActive++;
    else if (east[topNo] != 0 && countEast == 0)
      dataEast.ranksActive--;
  }
  else
  {
    // Treat as if partialTops were zero, as there might be a
    // maximum value for an unused top stored here.
    dataWest.length += countWest;
    dataEast.length += countEast;

    dataWest.topsUsed += countWest;
    dataEast.topsUsed += countEast;

    if (countWest > 0)
      dataWest.ranksActive++;

    if (countEast > 0)
      dataEast.ranksActive++;

    used[topNo] = true;
  }

  west[topNo] = countWest;
  east[topNo] = maximum - countWest;
}


void Completion::setFree(
  const unsigned char maximum,
  const Term& length)
{
  unsigned char lengthWestLower, lengthWestUpper;
  if (length.used())
  {
    lengthWestLower = length.lower();
    lengthWestUpper = min(maximum, length.upper());
  }
  else
  {
    lengthWestLower = dataWest.topsUsed;
    lengthWestUpper = maximum - dataEast.topsUsed;
  }

  const unsigned char rest = maximum - 
    dataWest.topsUsed - dataEast.topsUsed;

  const unsigned char westLimit = lengthWestUpper - dataWest.topsUsed;
  const unsigned char eastLimit = maximum -
    lengthWestLower - dataEast.topsUsed;

  dataWest.freeUpper = min(rest, westLimit);
  dataEast.freeUpper = min(rest, eastLimit);

  dataWest.freeLower = rest - dataEast.freeUpper;
  dataEast.freeLower = rest - dataWest.freeUpper;
}


const list<unsigned char>& Completion::openTops() const
{
  return openTopNumbers;
}


unsigned char Completion::length(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.length;
  else if (side == OPP_EAST)
    return dataEast.length;
  else
  {
    assert(false);
    return 0;
  }
}


unsigned char Completion::getTopsFull(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.topsFull;
  else if (side == OPP_EAST)
    return dataEast.topsFull;
  else
  {
    assert(false);
    return 0;
  }
}


unsigned char Completion::getTopsUsed(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.topsUsed;
  else if (side == OPP_EAST)
    return dataEast.topsUsed;
  else
  {
    assert(false);
    return 0;
  }
}


unsigned char Completion::getFreeLower(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.freeLower;
  else if (side == OPP_EAST)
    return dataEast.freeLower;
  else
  {
    assert(false);
    return 0;
  }
}


unsigned char Completion::getFreeUpper(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.freeUpper;
  else if (side == OPP_EAST)
    return dataEast.freeUpper;
  else
  {
    assert(false);
    return 0;
  }
}


unsigned char Completion::getTotalLower(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.topsUsed + dataWest.freeLower;
  else if (side == OPP_EAST)
    return dataEast.topsUsed + dataEast.freeLower;
  else
  {
    assert(false);
    return 0;
  }
}


unsigned char Completion::getTotalUpper(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.topsUsed + dataWest.freeUpper;
  else if (side == OPP_EAST)
    return dataEast.topsUsed + dataEast.freeUpper;
  else
  {
    assert(false);
    return 0;
  }
}


unsigned char Completion::getLowestRankUsed() const
{
  return lowestRankUsed;
}


unsigned char Completion::getLowestRankActive(const Opponent side) const
{
  if (side == OPP_WEST)
    return dataWest.lowestRankActive;
  else if (side == OPP_EAST)
    return dataEast.lowestRankActive;
  else
  {
    assert(false);
    return 0;
  }
}


bool Completion::lowestRankIsUsed(const Opponent side) const
{
  if (side == OPP_WEST)
    return (dataWest.lowestRankActive == lowestRankUsed);
  else
    return (dataEast.lowestRankActive == lowestRankUsed);
}


bool Completion::expandable(const Opponent side) const
{
  return(
     (side == OPP_WEST && dataWest.ranksActive == 1) ||
     (side == OPP_EAST && dataEast.ranksActive == 1));
}


bool Completion::expandableBoth() const
{
  return Completion::expandable(OPP_WEST) &&
      Completion::expandable(OPP_EAST);
}


bool Completion::fullRanked(const Opponent side) const
{
  return(
     (side == OPP_WEST && dataWest.topsUsed == dataWest.topsFull) ||
     (side == OPP_EAST && dataEast.topsUsed == dataEast.topsFull));
}


bool Completion::highRanked(const Opponent side) const
{
  if (side == OPP_WEST)
  {
    if (! dataWest.highestRankFlag)
      return false;
    else if (! dataEast.highestRankFlag)
      return true;
    else
      return (dataWest.highestRankActive >= dataEast.highestRankActive);
  }
  else
  {
    if (! dataEast.highestRankFlag)
      return false;
    else if (! dataWest.highestRankFlag)
      return true;
    else
      return (dataEast.highestRankActive >= dataWest.highestRankActive);
  }
}


bool Completion::secondRanked(const Opponent side) const
{
  if (! dataWest.highestRankFlag || ! dataEast.highestRankFlag)
    return false;
  else if (side == OPP_WEST)
    return (dataWest.highestRankActive+1 == dataEast.highestRankActive);
  else
    return (dataEast.highestRankActive+1 == dataWest.highestRankActive);
}


unsigned char Completion::numOptions() const
{
  return static_cast<unsigned char>(used.size()) - ranksUsed;
}


Opponent Completion::preferSingleActive() const
{
  // Return OPP_WEST if West, OPP_EAST if East, OPP_EITHER if
  // not a relevant match.

  const bool singleActiveRank = 
    (ranksUsed == 1 &&
     dataWest.lowestRankActive == dataEast.lowestRankActive);

  if (dataEast.ranksActive == 0 || singleActiveRank)
    return OPP_WEST;
  else if (dataWest.ranksActive == 0)
    return OPP_EAST;
  else
    return OPP_EITHER;
}


Opponent Completion::preferSimpleActive() const
{
  // Return OPP_WEST if West, OPP_EAST if East, OPP_EITHER if
  // not a relevant match.

  if (dataWest.ranksActive == 1 && dataEast.ranksActive > 1)
    return OPP_WEST;
  else if (dataWest.ranksActive > 1 && dataEast.ranksActive == 1)
    return OPP_EAST;
  else if (dataWest.ranksActive == 1 && dataEast.ranksActive == 1 &&
    dataWest.lowestRankActive != dataEast.lowestRankActive)
  {
    // Prefer the one with the higher single rank.
    return (dataWest.lowestRankActive > dataEast.lowestRankActive ?
      OPP_WEST : OPP_EAST);
  }
  else if (dataWest.topsUsed == 1 && dataEast.topsUsed > 1)
    return OPP_WEST;
  else if (dataEast.topsUsed == 1 && dataWest.topsUsed > 1)
    return OPP_EAST;
  else
    return OPP_EITHER;
}


Opponent Completion::preferHighActive() const
{
  // Prefer the one with the higher rank.
  return (dataWest.highestRankActive >= dataEast.highestRankActive ?
    OPP_WEST : OPP_EAST);
}


bool Completion::operator < (const Completion& comp2) const
{
  return (dataWest.length > comp2.dataWest.length);
}


bool Completion::operator > (const Completion& comp2) const
{
  return (dataWest.length < comp2.dataWest.length);
}


string Completion::strSet(
  const RanksNames& ranksNames,
  const Opponent side,
  const bool explicitVoidFlag) const
{
  if ((side == OPP_WEST && dataWest.length == 0) ||
      (side == OPP_EAST && dataEast.length == 0))
    return (explicitVoidFlag ? "void" : "");

  string s;
  const vector<unsigned char>& tops = (side == OPP_WEST ? west : east);

  for (unsigned char topNo = 
    static_cast<unsigned char>(west.size()); topNo-- > 0; )
  {
    if (used[topNo] && tops[topNo] > 0)
      s += ranksNames.strOpponents(topNo, tops[topNo]);
  }
  return s;
}


string Completion::strUnset(
  const RanksNames& ranksNames,
  const Opponent side) const
{
  string s;
  const vector<unsigned char>& tops = (side == OPP_WEST ? west : east);

  for (auto openNo: openTopNumbers)
  {
    s += ranksNames.strOpponents(openNo, tops[openNo]);
  }
  return s;
}


string Completion::strXes(const Opponent side) const
{
  const string& x = dictionary.words.get(WORDS_SMALL).text;

  string xes = "";
  const CompData& data = (side == OPP_WEST ? dataWest : dataEast);

  for (unsigned char i = 0; i < data.freeLower; i++)
    xes += x;

  if (data.freeUpper > data.freeLower)
  {
    xes += "(";
    for (unsigned char i = data.freeLower; i < data.freeUpper; i++)
      xes += x;
    xes += ")";
  }

  return xes;
}
