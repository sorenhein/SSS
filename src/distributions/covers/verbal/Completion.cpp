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

#include "Completion.h"

#include "../../../ranks/RanksNames.h"

#include "../../../utils/table.h"


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
  const unsigned char countWest,
  const unsigned char maximum)
{
  // This methods does not respect anything except the numbers
  // in west and east.  The consistency in openTopNumbers, length
  // etc. is lost.  It should only be used from a method  such as 
  // Product::makeCompletionList().

  assert(topNo < used.size());

  if (used[topNo])
  {
    dataWest.length += countWest - west[topNo];
    dataEast.length += maximum - countWest - east[topNo];
  }
  else
  {
    // Treat as if partialTops were zero, as there might be a
    // maximum value for an unused top stored here.
    dataWest.length += countWest;
    dataEast.length += maximum - countWest;
    used[topNo] = true;
    
  }

  west[topNo] = countWest;
  east[topNo] = maximum -countWest;
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


bool Completion::operator < (const Completion& comp2) const
{
  return (dataWest.length > comp2.dataWest.length);
}


bool Completion::operator == (const Completion& comp2) const
{
  if (used.size() != comp2.used.size())
    return false;
  if (west.size() != comp2.west.size())
    return false;
  if (openTopNumbers.size() != comp2.openTopNumbers.size())
    return false;
  if (dataWest.length != comp2.dataWest.length)
    return false;
  if (dataEast.length != comp2.dataEast.length)
    return false;

  for (size_t i = 0; i < west.size(); i++)
    if (used[i] != comp2.used[i])
      return false;

  for (size_t i = 0; i < west.size(); i++)
    if (west[i] != comp2.west[i])
      return false;

  for (size_t i = 0; i < east.size(); i++)
    if (east[i] != comp2.east[i])
      return false;

  auto oiter1 = openTopNumbers.begin();
  auto oiter2 = comp2.openTopNumbers.begin();

  while (oiter1 != openTopNumbers.end())
  {
    if (* oiter1 != * oiter2)
      return false;

    oiter1++;
    oiter2++;
  }

  return true;
}


string Completion::strDebug() const
{
  stringstream ss;

  ss << "West ";
  for (auto w: west)
    ss << +w << " ";
  ss << "\n";

  ss << "East ";
  for (auto e: east)
    ss << +e << " ";
  ss << "\n";

  ss << "open ";
  for (auto o: openTopNumbers)
    ss << +o << " ";
  ss << "\n";

  ss << "length West " << +dataWest.length << "\n";
  ss << "length East " << +dataEast.length << "\n";
  return ss.str();
}


string Completion::strSet(
  const RanksNames& ranksNames,
  const Opponent side,
  const bool expandFlag,           // jack, not J
  const bool singleRankFlag,       // Use dashes between expansions
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
    if (used[topNo])
    {
      if (expandFlag && ! singleRankFlag && ! s.empty())
        s += "-";

      s += ranksNames.strOpponents(topNo, tops[topNo],
        expandFlag, singleRankFlag);
    }
  }
  return s;
}


string Completion::strSetNew(
  const RanksNames& ranksNames,
  const Opponent side,
  const bool enableExpandFlag,     // jack, not J
  const bool enableSingleRankFlag, // Use dashes between expansions
  const bool explicitVoidFlag) const
{
  if ((side == OPP_WEST && dataWest.length == 0) ||
      (side == OPP_EAST && dataEast.length == 0))
    return (explicitVoidFlag ? "void" : "");

  const bool expandFlag = enableExpandFlag && 
    ((side == OPP_WEST && dataWest.topsUsed == 1) ||
     (side == OPP_EAST && dataEast.topsUsed == 1));

  const bool singleRankFlag = enableSingleRankFlag &&
    ((side == OPP_WEST && dataWest.ranksActive == 1) ||
     (side == OPP_EAST && dataEast.ranksActive == 1));

  string s;
  const vector<unsigned char>& tops = (side == OPP_WEST ? west : east);

  for (unsigned char topNo = 
    static_cast<unsigned char>(west.size()); topNo-- > 0; )
  {
    if (used[topNo])
    {
      if (expandFlag && ! singleRankFlag && ! s.empty())
        s += "-";

      s += ranksNames.strOpponents(topNo, tops[topNo],
        expandFlag, singleRankFlag);
    }
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
    s += ranksNames.strOpponents(openNo, tops[openNo], false, false);
  }
  return s;
}
