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

#include "../product/Profile.h"

#include "../../../ranks/RanksNames.h"


void Completion::resize(const size_t numTops)
{
  partialTops.resize(numTops);
  used.resize(numTops, false);
  openTopNumbers.clear();
  lengthInt = 0;
}


void Completion::setTop(
  const unsigned char topNo,
  const bool usedFlag,
  const unsigned char count)
{
  // Must be an equal top.

  assert(topNo < used.size());
  used[topNo] = usedFlag;

  // Permit a maximum value to be stored even if the top is unused.
  partialTops[topNo] = count;

  if (usedFlag)
  {
    lengthInt += count;
  }
  else
    openTopNumbers.push_back(topNo);
}


void Completion::updateTop(
  const unsigned char topNo,
  const unsigned char count)
{
  assert(topNo < used.size());

  if (used[topNo])
    lengthInt += count - partialTops[topNo];
  else
  {
    // Treat as if partialTops were zero, as there might be a
    // maximum value for an unused top stored here.
    lengthInt += count;
    used[topNo] = true;
  }

  partialTops[topNo] = count;
}


const list<unsigned char>& Completion::openTops() const
{
  return openTopNumbers;
}


unsigned char Completion::length() const
{
  return lengthInt;
}


bool Completion::operator < (const Completion& comp2) const
{
  return (lengthInt > comp2.lengthInt);
}


bool Completion::operator == (const Completion& comp2) const
{
  if (used.size() != comp2.used.size())
    return false;
  if (partialTops.size() != comp2.partialTops.size())
    return false;
  if (openTopNumbers.size() != comp2.openTopNumbers.size())
    return false;
  if (lengthInt != comp2.lengthInt)
    return false;

  for (size_t i = 0; i < partialTops.size(); i++)
    if (used[i] != comp2.used[i])
      return false;

  for (size_t i = 0; i < partialTops.size(); i++)
    if (partialTops[i] != comp2.partialTops[i])
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
}


string Completion::strSet(
  const RanksNames& ranksNames,
  const bool expandFlag,           // jack, not J
  const bool singleRankFlag,       // Use dashes between expansions
  const bool explicitVoidFlag) const
{
  if (lengthInt == 0)
    return (explicitVoidFlag ? "void" : "");

  string s;
  for (unsigned char topNo = 
    static_cast<unsigned char>(partialTops.size()); topNo-- > 0; )
  {
    if (used[topNo])
    {
      if (expandFlag && ! singleRankFlag && ! s.empty())
        s += "-";

      s += ranksNames.strOpponents(topNo, partialTops[topNo],
        expandFlag, singleRankFlag);
    }
  }
  return s;
}


string Completion::strUnset(const RanksNames& ranksNames) const
{
  string s;
  for (auto openNo: openTopNumbers)
  {
    s += ranksNames.strOpponents(openNo, partialTops[openNo],
      false, false);
  }
  return s;
}
