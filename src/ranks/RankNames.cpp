/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include <vector>
#include <string>

#include "RankNames.h"

#include "../utils/table.h"
#include "../const.h"


const vector<string> CARD_FULL_NAMES =
{
  "deuce",
  "trey",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "jack",
  "queen",
  "king",
  "ace"
};


RankNames::RankNames()
{
  count = 0;
  sideInt = SIDE_NONE;
}


void RankNames::add(
  const Side side,
  const size_t index)
{
  // TODO This means that side for N-S will be one or the other,
  // and their cards are thrown together.  OK for now.

  sideInt = side;

  const char c = static_cast<char>(CARD_NAMES[index]);

  if (count == 0)
  {
    strFullInt = CARD_FULL_NAMES[index];
    strShortInt = c;
  }
  else
  {
    strFullInt = CARD_FULL_NAMES[index] + "-" + strFullInt;
    strShortInt = string(1, c) + strShortInt;
  }

  count++;
}


Side RankNames::side() const
{
  return sideInt;
}


size_t RankNames::size() const
{
  return count;
}


string RankNames::strFull() const
{
  return strFullInt;
}


string RankNames::strShort() const
{
  return strShortInt;
}


string RankNames::strHeader() const
{
  stringstream ss;
  ss << 
    setw(3) << "Top" << "  " <<
    setw(12) << left << "Short" <<
    "Long" << "\n";
  return ss.str();
}


string RankNames::str(const size_t number) const
{
  stringstream ss;
  ss <<
    setw(3) << number << "  " <<
    setw(12) << left << strShortInt <<
    strFullInt << "\n";
  return ss.str();
}
