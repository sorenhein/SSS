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
  oppsRanks = 0;
  sidePrev = SIDE_NONE;
}


void RanksNames::setCards(const unsigned char cards)
{
  assert(cards <= 13);
  runningIndex = 13 - cards;
  oppsRanks = 0;
  sidePrev = SIDE_NONE;
}


void RanksNames::add(const Side side)
{
  if (side == SIDE_NORTH || side == SIDE_SOUTH)
  {
    if (sidePrev == SIDE_OPPS)
      names.emplace_back(RankNames());
  }
  else if (sidePrev == SIDE_NORTH || sidePrev == SIDE_SOUTH)
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
  oppsPtrsByTop.resize(oppsRanks);

  size_t i = 0;
  for (auto riter = names.rbegin(); riter != names.rend(); riter++)
  {
    if (riter->side() == SIDE_OPPS)
      oppsPtrsByTop[i++] = &* riter;
  }
}


const RankNames& RanksNames::getOpponents(const unsigned topNumber) const
{
  assert(topNumber < oppsPtrsByTop.size());
  return * oppsPtrsByTop[topNumber];
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
