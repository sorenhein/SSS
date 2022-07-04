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


void RowMatches::emplace(
  CoverRow& rowIn,
  const size_t westLength)
{
  matches.emplace_back(RowMatch());
  matches.back().transfer(rowIn, 0, westLength);
}


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
  // RowMatches::emplace(rowIn, westLength);
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

  // Get the length range that the tops of each match could have if
  // we disregard any length constraint
  for (auto& match: matches)
    match.setLengthsByTops(sumProfile);

  // Eye candy.
  const string sideName = (side == OPP_WEST ? "WEST" : "EAST");
  assert(side == OPP_WEST || side == OPP_EAST);


  // Look for ways to use the void(s) to complete matches.

  list<RowMatch *> potentialsGreat;
  list<RowMatch *> potentialsGood;

// cout << "The matches for " << sideName << " are:\n";
// cout << RowMatches::str() << "\n";

  for (auto& match: matches)
  {
    // We prefer ways that make the length dimension of a cover
    // go away completely.  This happens if the lengths end up matching
    // the lengths by tops.
    // Lengths must be contiguous with a void in order to augment.

    if (match.preferred(voidInfo.westLength, side))
      potentialsGreat.push_back(&match);
    else if (match.possible(voidInfo.westLength, side))
      potentialsGood.push_back(&match);
  }

  const unsigned psizeGreat = static_cast<unsigned>(potentialsGreat.size());
  const unsigned psizeGood = static_cast<unsigned>(potentialsGood.size());

cout << "Potentials " << psizeGreat << ", " << psizeGood << endl;

  if (psizeGreat > voidInfo.repeats)
    cout << sideName << "WARN Picking random great potentials\n";
  else if (psizeGreat + psizeGood > voidInfo.repeats)
    cout << sideName << "WARN Picking random good potentials\n";

  unsigned rest = voidInfo.repeats;

  // Use up any great potentials first.
  const unsigned potsGreat = min(psizeGreat, rest);
  rest -= potsGreat;
  auto pitGreat = potentialsGreat.begin();
  for (size_t p = 0; p < potsGreat; p++)
  {
    RowMatch& rm = ** pitGreat;
    rm.add(* voidInfo.tricksPtr, side);
    pitGreat++;
  }

  if (rest == 0)
  {
    // cout << "Done after great potentials\n";
    // cout << "The matches are now:\n";
    // cout << RowMatches::str() << "\n";
    return;
  }

  // Then use up any good potentials.
  const unsigned potsGood = min(psizeGood, rest);
  rest -= potsGood;
  auto pitGood = potentialsGood.begin();
  for (size_t p = 0; p < potsGood; p++)
  {
    RowMatch& rm = ** pitGood;
    rm.add(* voidInfo.tricksPtr, side);
    pitGood++;
  }

  if (rest == 0)
  {
    // cout << "Done after good potentials\n";
    // cout << "The matches are now:\n";
    // cout << RowMatches::str() << "\n";
    return;
  }

  // Then keep the rest.
  CoverRow row;
  row.resize(voidInfo.tricksPtr->size());
  row.add(* voidInfo.coverPtr, * voidInfo.tricksPtr);

  RowMatches::transfer(row, voidInfo.westLength, side, rest);
  cout << sideName << "REST " << rest << "\n";

    // cout << "Done after rest\n";
    // cout << "The matches are now:\n";
    // cout << RowMatches::str() << "\n";
}


void RowMatches::symmetrize(const Profile& sumProfile)
{
  if (matches.size() < 2)
    return;

  for (auto rit1 = matches.begin(); rit1 != matches.end(); rit1++)
  {
    if (! rit1->symmetrizable(sumProfile))
      continue;
// cout << "rit1 is symmetrizable\n";
// cout << rit1->str() << endl;

    for (auto rit2 = next(rit1); rit2 != matches.end(); )
    {
      if (! rit2->symmetrizable(sumProfile))
      {
        rit2++;
        continue;
      }

// cout << "rit2 is symmetrizable\n";
// cout << rit2->str() << endl;
      if (rit1->symmetricWith(* rit2))
      {
// cout << "Symmetrized!\n";
// cout << "Before\n";
// cout << RowMatches::str();
        rit1->symmetrize();
        rit2 = matches.erase(rit2);
// cout << "After\n";
// cout << RowMatches::str();
      }
      else
        rit2++;
    }
  }
}


string RowMatches::str() const
{
  stringstream ss;

  for (auto& match: matches)
    ss << match.str();

  return ss.str();
}
