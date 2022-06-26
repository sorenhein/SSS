/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <sstream>
#include <cassert>

#include "RowMatches.h"

#include "../../utils/table.h"


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


void RowMatches::setVoid(
  const Opponent side,
  VoidInfo& voidInfo)
{
  if (voidInfo.repeats == 0)
    return;

  // TODO Thoughts
  // Look at each match: matches
  //   void must be contiguous low (West) or high (East)
  //   What is the length range that the match could cover if there
  //   were no length explicitly given:
  //   - On the low side, we might have >=1 of something -- add up
  //   - On the high side, <= something below full West length
  //   - That should be enough to say whether it's compatible(?)
  //   - Say >= 2 but also two >= 1 so it's actually obvious;
  //     Could do away with the length >= 2 then (we won't do it
  //     here, but later on it will happen automatically), so then
  //     the new complexity completely avoids this length term
  //   - Otherwise, the length term goes from interval to >=, say
  //   - Look for matches with the best complexity reduction when
  //     merging in the void

  if (side == OPP_WEST)
  {
    CoverRow row;
    row.resize(voidInfo.tricksPtr->size());
    row.add(* voidInfo.coverPtr, * voidInfo.tricksPtr);

    RowMatches::transfer(row, voidInfo.westLength, voidInfo.repeats);
  }
  else if (side == OPP_EAST)
  {
    CoverRow row;
    row.resize(voidInfo.tricksPtr->size());
    row.add(* voidInfo.coverPtr, * voidInfo.tricksPtr);

    RowMatches::transfer(row, voidInfo.westLength, voidInfo.repeats);
  }
  else
    assert(false);
}


string RowMatches::str() const
{
  stringstream ss;

  for (auto& match: matches)
    ss << match.str();

  return ss.str();
}
