/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <sstream>

#include "RowMatches.h"


using namespace std;


void RowMatches::transfer(
  CoverRow& rowIn,
  const size_t westLength)
{
  // rowIn gets invalidated!
  for (auto& match: matches)
  {
    const CoverRow& matchingRow = match.getSingleRow();

    // Lengths must be contiguous in order to augment.
    if (! match.contiguous(westLength))
      continue;

    if (! rowIn.sameTops(matchingRow))
      continue;

    match.add(rowIn.getTricks());
    return;
  }

  // No extension of an existing row, so add a new row.
  matches.emplace_back(RowMatch());
  matches.back().transfer(rowIn, westLength);
}


string RowMatches::str() const
{
  stringstream ss;

  for (auto& match: matches)
    ss << match.str();

  return ss.str();
}
