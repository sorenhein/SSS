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


void Completion::resize(const size_t numTops)
{
  partialTops.resize(numTops);
  openTopNumbers.clear();
  lengthInt = 0;
}


void Completion::setTop(
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
}


void Completion::updateTop(
  const unsigned char topNo,
  const unsigned char count)
{
  lengthInt += count - partialTops[topNo];
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


string Completion::str(
  const RanksNames& ranksNames,
  const bool expandFlag,           // jack, not J
  const bool singleRankFlag) const // Use dashes between expansions
{
  if (lengthInt == 0)
    return "void";

  string s;
  for (unsigned char topNo = 
    static_cast<unsigned char>(partialTops.size()); topNo-- > 0; )
  {
    if (partialTops[topNo])
    {
      if (expandFlag && ! singleRankFlag && ! s.empty())
        s += "-";

      s += ranksNames.strOpponents(topNo, partialTops[topNo],
        expandFlag, singleRankFlag);
    }
  }
  return s;
}
