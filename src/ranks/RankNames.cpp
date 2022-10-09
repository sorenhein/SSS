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

#include "RankNames.h"

#include "../languages/Dictionary.h"

#include "../utils/table.h"


extern Dictionary dictionary;


const vector<vector<string>> CARD_ABSOLUTE_NAMES =
{
  {"honor", "honors", "H"},
  {"lower honor", "lower honors", "h"},
  {"minor honor", "minor honors", "G"},
  {"tiny honor", "tiny honors", "g"},
  {"micro honor", "micro honors", "F"}
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

  if (count == 0)
  {
    names[RANKNAME_ACTUAL_FULL] = 
      dictionary.cardsIndefinite.get(index).text;

    names[RANKNAME_ACTUAL_SHORT] =
      dictionary.cardsShort.get(index).text;
  }
  else
  {
    names[RANKNAME_ACTUAL_FULL] = 
      dictionary.cardsIndefinite.get(index).text + "-" + 
      names[RANKNAME_ACTUAL_FULL];

    names[RANKNAME_ACTUAL_SHORT] = 
      dictionary.cardsShort.get(index).text +
      names[RANKNAME_ACTUAL_SHORT];
  }
  names[RANKNAME_RELATIVE_SHORT] = "(none)";

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
      names[RANKNAME_ABSOLUTE_SHORT] += h2;
    }

    noAbs++;
  }

  // Repeat the short honor symbol count times.
  const string lookup = dictionary.honorsShort.get(noRel).text;
  names[RANKNAME_RELATIVE_SHORT] = lookup;
  for (size_t i = 1; i < count; i++)
    names[RANKNAME_RELATIVE_SHORT] += lookup;

  noRel++;
}


void RankNames::makeXes()
{
  const string c = names[RANKNAME_ACTUAL_SHORT].substr(0, 1);

  if (c >= "2" && c <= "8")
  {
    names[RANKNAME_ACTUAL_SHORT] = string(count, 'x');
    names[RANKNAME_ABSOLUTE_SHORT] = string(count, 'x');
    names[RANKNAME_RELATIVE_SHORT] = string(count, 'x');
  }
}


void RankNames::completeNS()
{
  names[RANKNAME_ABSOLUTE_FULL] = names[RANKNAME_ACTUAL_FULL];
  names[RANKNAME_ABSOLUTE_SHORT] = names[RANKNAME_ACTUAL_SHORT];
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


string RankNames::strOpponents(
  const unsigned char numCards,
  const bool expandFlag,
  const bool singleRankFlag) const
{
  if (numCards == 0)
    return "";
  else if (numCards == count)
  {
    if (expandFlag)
      return (singleRankFlag ? "the " : "") + names[RANKNAME_ACTUAL_FULL];
    else
      return names[RANKNAME_ACTUAL_SHORT];
  }
  else
  {
    if (expandFlag && singleRankFlag)
    {
      return 
        "exactly " + 
        dictionary.numerals.get(numCards).text +
        " of " +
        names[RANKNAME_ACTUAL_FULL];
    }
    else
      // TODO Is this right, or should it be something with honors?
      return names[RANKNAME_ABSOLUTE_SHORT].substr(0, numCards);
  }
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

