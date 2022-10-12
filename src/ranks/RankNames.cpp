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
#include "../languages/connections/words.h"

#include "../utils/table.h"


extern Dictionary dictionary;


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

    // Only the lowest card of that rank.
    names[RANKNAME_ACTUAL_LOW_DEF] = 
      dictionary.cardsDefinite.get(index).text;

    // Only used if we need one card.
    names[RANKNAME_ACTUAL_FULL_DEF] = 
      dictionary.cardsDefinite.get(index).text;

    // Only used if we need one card.
    names[RANKNAME_ACTUAL_FULL_DEF_OF] = 
      dictionary.cardsPrepositionOf.get(index).text;
  }
  else
  {
    names[RANKNAME_ACTUAL_FULL] = 
      dictionary.cardsIndefinite.get(index).text + "-" + 
      names[RANKNAME_ACTUAL_FULL];

    names[RANKNAME_ACTUAL_SHORT] = 
      dictionary.cardsShort.get(index).text +
      names[RANKNAME_ACTUAL_SHORT];

    names[RANKNAME_ACTUAL_FULL_DEF] = 
      dictionary.cardsShort.get(index).text + "-" +
      names[RANKNAME_ACTUAL_FULL_DEF];
  }
  names[RANKNAME_RELATIVE_SHORT] = 
    "(" + dictionary.numerals.get(0).text + ")";


  count++;
}


void RankNames::completeOpps(
  size_t& noAbs,
  size_t& noRel)
{
  assert(count > 0);
  if (count == 1)
  {
    names[RANKNAME_ABSOLUTE_SHORT] = names[RANKNAME_ACTUAL_SHORT];
  }
  else
  {
    // Repeat the short honor symbol count times.
    const string& h = dictionary.honorsShort.get(noAbs).text;
    names[RANKNAME_ABSOLUTE_SHORT] = h;
    for (size_t i = 1; i < count; i++)
      names[RANKNAME_ABSOLUTE_SHORT] += h;

    noAbs++;
  }

  // Repeat the short honor symbol count times.
  const string& h = dictionary.honorsShort.get(noRel).text;
  names[RANKNAME_RELATIVE_SHORT] = h;
  for (size_t i = 1; i < count; i++)
    names[RANKNAME_RELATIVE_SHORT] += h;

  noRel++;
}


void RankNames::makeXes()
{
  const string c = names[RANKNAME_ACTUAL_SHORT].substr(0, 1);

  if (c >= "2" && c <= "8")
  {
    // If the last rank is small enough, show it as x'es.
    const string& x = dictionary.words.get(WORDS_SMALL).text;
    string xes = x;
    for (size_t i = 1; i < count; i++)
      xes += x;

    names[RANKNAME_ACTUAL_SHORT] = xes;
    names[RANKNAME_ABSOLUTE_SHORT] = xes;
    names[RANKNAME_RELATIVE_SHORT] = xes;
  }
}


void RankNames::completeNS()
{
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

  if (rankName == RANKNAME_ACTUAL_FULL_DEF)
  {
    if (count == 1)
      return names[rankName]; // Article built in
    else
      return dictionary.words.get(WORDS_PARTICLE_DEF_PLURAL).text + " " +
        names[RANKNAME_ACTUAL_FULL];
  }
  else if (rankName == RANKNAME_ACTUAL_FULL_DEF_OF)
  {
    if (count == 1)
      return names[rankName]; // Preposition and article built in
    else
      return dictionary.words.get(WORDS_DEF_PLURAL_OF).text + " " +
        names[RANKNAME_ACTUAL_FULL];
  }
  else
    return names[rankName];
}


string RankNames::strOpponents(const unsigned char numCards) const
{
  if (numCards == 0)
    return "";
  else if (numCards == count)
    return names[RANKNAME_ACTUAL_SHORT];
  else
    return names[RANKNAME_ABSOLUTE_SHORT].substr(0, numCards);
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

