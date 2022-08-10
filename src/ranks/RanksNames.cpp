/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "RanksNames.h"

#include "../utils/table.h"


RanksNames::RanksNames()
{
  runningIndex = 0;
  RanksNames::reset();
}


void RanksNames::setCards(const unsigned char cards)
{
  assert(cards <= 13);
  runningIndex = 13 - cards;
  RanksNames::reset();
}


void RanksNames::reset()
{
  oppsRanks = 0;
  sidePrev = SIDE_NONE;
  names.clear();
  indexByTop.clear();
}


void RanksNames::add(const Side side)
{
  if (side == SIDE_NORTH || side == SIDE_SOUTH)
  {
    if (sidePrev == SIDE_OPPS || sidePrev == SIDE_NONE)
      names.emplace_back(RankNames());
  }
  else if (sidePrev != SIDE_OPPS)
  {
    names.emplace_back(RankNames());
    oppsRanks++;
  }

  names.back().add(side, runningIndex);

  sidePrev = side;
  runningIndex++;
}


void RanksNames::finish()
{
  indexByTop.resize(oppsRanks);

  size_t itop = 0;
  for (size_t ifull = 0; ifull < names.size(); ifull++)
  {
    if (names[ifull].side() == SIDE_OPPS)
      indexByTop[itop++] = ifull;
  }

assert(itop == oppsRanks);

  if (names.empty())
    return;

  size_t noAbs = 0; // Only the opponent holding with two+ cards
  size_t noRel = 0; // All opponent holdings;

  for (auto i = names.size(); --i > 0; )
  {
    auto& rnames = names[i];
    if (rnames.side() == SIDE_OPPS)
      rnames.completeOpps(noAbs, noRel);
    else
      rnames.completeNS();
  }
}


bool RanksNames::used() const
{
  return ! names.empty();
}


const RankNames& RanksNames::getOpponents(const unsigned topNumber) const
{
  assert(topNumber < indexByTop.size());

  const auto itop = indexByTop[topNumber];
  assert(itop < names.size());

  return names[itop];
}


string RanksNames::str() const
{
  if (names.empty())
    return "";

  stringstream ss;
  ss << names.front().strHeader();

  size_t i = 0;
  for (auto& rnames: names)
    ss << rnames.str(i++);

  return ss.str();
}


string RanksNames::strMap() const
{
  if (names.empty())
    return "";

  string s = " where opponents hold ";

  for (auto riter = names.rbegin(); riter != names.rend(); riter++)
  {
    if (riter->side() == SIDE_OPPS)
      s += riter->strComponent(RANKNAME_RELATIVE_SHORT);
  }

  s += ".";
  return s;
}
