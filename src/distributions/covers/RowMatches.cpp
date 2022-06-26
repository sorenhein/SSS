/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <sstream>
#include <cassert>

#include "RowMatches.h"


using namespace std;


void RowMatches::transfer(
  CoverRow& rowIn,
  const size_t westLength,
  const unsigned repeats)
{
  // rowIn gets invalidated!
  unsigned running = repeats;

  for (auto& match: matches)
  {
    const CoverRow& matchingRow = match.getSingleRow();

    // Lengths must be contiguous in order to augment.
    if (! match.contiguous(westLength))
      continue;

    if (! rowIn.sameTops(matchingRow))
      continue;

    match.add(rowIn.getTricks());
    if (--running == 0)
      return;
  }

  // No extension of an existing row, so add a new row.
  matches.emplace_back(RowMatch());
  matches.back().transfer(rowIn, westLength);

  for (unsigned f = 1; f < running; f++)
    matches.push_back(matches.back());
}


string RowMatches::str() const
{
  stringstream ss;

  for (auto& match: matches)
    ss << match.str();

  return ss.str();
}
