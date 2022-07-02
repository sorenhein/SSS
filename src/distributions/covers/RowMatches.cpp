/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "RowMatches.h"

#include "../../utils/table.h"


using namespace std;


void RowMatches::transfer(
  CoverRow& rowIn,
  const size_t westLength,
  const Opponent towardVoid,
  const unsigned repeats)
{
  // rowIn gets invalidated!
  unsigned running = repeats;

  for (auto& match: matches)
  {
    const CoverRow& matchingRow = match.getSingleRow();

    // Lengths must be contiguous in order to augment.
    if (! match.contiguous(westLength, towardVoid))
      continue;

    if (! rowIn.sameTops(matchingRow))
      continue;

    match.add(rowIn.getTricks(), towardVoid);
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
  const VoidInfo& voidInfo,
  const Profile& sumProfile)
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

  const string sideName = (side == OPP_WEST ? "WEST" : "EAST");
  assert(side == OPP_WEST || side == OPP_EAST);

  list<RowMatch *> potentials;

// cout << "The matches are:\n";
// cout << RowMatches::str() << "\n";
  for (auto& match: matches)
  {
    // Lengths must be contiguous with a void in order to augment.
    if (! match.contiguous(voidInfo.westLength, side))
      continue;

    const CoverRow& matchingRow = match.getSingleRow();
    const unsigned char minSide = matchingRow.minimumByTops(
      side, sumProfile);

    if (minSide == 0)
      potentials.push_back(&match);
  }

  const unsigned psize = static_cast<unsigned>(potentials.size());

  if (psize > voidInfo.repeats)
    cout << sideName << "WARN Picking random potentials\n";

  const unsigned pots = min(psize, voidInfo.repeats);
  if (pots > 0)
    cout << sideName << "USE " << pots << "\n";
  auto pit = potentials.begin();
  for (size_t p = 0; p < pots; p++)
  {
    RowMatch& rm = ** pit;
    rm.add(* voidInfo.tricksPtr, side);
    pit++;
  }

  if (psize >= voidInfo.repeats)
  {
    cout << sideName << "EXHAUSTED\n";
// cout << "The matches now:\n";
// cout << RowMatches::str() << "\n";
    return;
  }

  const unsigned rest = voidInfo.repeats - psize;

  CoverRow row;
  row.resize(voidInfo.tricksPtr->size());
  row.add(* voidInfo.coverPtr, * voidInfo.tricksPtr);

  RowMatches::transfer(row, voidInfo.westLength, side, rest);
  cout << sideName << "REST " << rest << "\n";
}


string RowMatches::str() const
{
  stringstream ss;

  for (auto& match: matches)
    ss << match.str();

  return ss.str();
}
