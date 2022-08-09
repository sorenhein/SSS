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

const vector<vector<string>> CARD_ABSOLUTE_NAMES =
{
  {"honor", "honors", "H"},
  {"lower honor", "lower honors", "h"},
  {"minor honor", "minor honors", "G"}
};

const vector<string> CARD_RELATIVE_NAMES =
{
  "H", "h", "G", "g", "F", "f"
  
};


RankNames::RankNames()
{
  count = 0;
  sideInt = SIDE_NONE;

  for (size_t i = 0; i < RANKNAME_SIZE; i++)
    names[i] = "";
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
    names[RANKNAME_ACTUAL_FULL] = CARD_FULL_NAMES[index];
    names[RANKNAME_ACTUAL_SHORT] = c;
  }
  else
  {
    names[RANKNAME_ACTUAL_FULL] = 
      CARD_FULL_NAMES[index] + "-" + names[RANKNAME_ACTUAL_FULL];

    names[RANKNAME_ACTUAL_SHORT] = 
      string(1, c) + names[RANKNAME_ACTUAL_SHORT];
  }

  count++;
}


void RankNames::completeOpps(
  size_t& noAbs,
  size_t& noRel)
{
  assert(count > 0);
  if (count == 1)
  {
    names[RANKNAME_ABSOLUTE_FULL] = names[RANKNAME_ACTUAL_FULL];
    names[RANKNAME_ABSOLUTE_SHORT] = names[RANKNAME_ACTUAL_SHORT];
  }
  else
  {
    assert(noAbs < CARD_ABSOLUTE_NAMES.size());
    const string& h0 = CARD_ABSOLUTE_NAMES[noAbs][0];
    const string& h2 = CARD_ABSOLUTE_NAMES[noAbs][2];

    names[RANKNAME_ABSOLUTE_FULL] = h0;
    names[RANKNAME_ABSOLUTE_SHORT] = h2;

    for (size_t i = 1; i < count; i++)
    {
      names[RANKNAME_ABSOLUTE_FULL] += "-" + h0;
      names[RANKNAME_ABSOLUTE_SHORT] += "-" + h2;
    }

    noAbs++;
  }

  assert(noRel < CARD_RELATIVE_NAMES.size());
  const string& hr = CARD_RELATIVE_NAMES[noRel];

  names[RANKNAME_RELATIVE_SHORT] = hr;

  for (size_t i = 1; i < count; i++)
    names[RANKNAME_RELATIVE_SHORT] += "-" + hr;

  noRel++;
}


void RankNames::completeNS()
{
  names[RANKNAME_ABSOLUTE_FULL] = names[RANKNAME_ACTUAL_FULL];
  names[RANKNAME_ABSOLUTE_SHORT] = names[RANKNAME_ACTUAL_SHORT];
  names[RANKNAME_RELATIVE_SHORT] = "";
}


Side RankNames::side() const
{
  return sideInt;
}


size_t RankNames::size() const
{
  return count;
}


string RankNames::strComponent(const RankName rankName) const
{
  assert(rankName < RANKNAME_SIZE);
  return names[rankName];
}


string RankNames::strHeader() const
{
  stringstream ss;
  ss << 
    setw(5) << "Index" << 
    setw(6) << "Side" << 
    setw(11) << "Act. full" << 
    setw(11) << "Act. short" << 
    setw(11) << "Abs. full" << 
    setw(11) << "Abs. short" << 
    setw(11) << "Rel. short" << 
    "\n";
  return ss.str();
}


string RankNames::str(const size_t number) const
{
  stringstream ss;
  ss <<
    setw(5) << right << number << 
    setw(6) << (sideInt == SIDE_OPPS ? "EW" : "NS");

  for (size_t i = 0; i < RANKNAME_SIZE; i++)
    ss << setw(11) << RankNames::strComponent(static_cast<RankName>(i));

  ss << "\n";

  return ss.str();
}

