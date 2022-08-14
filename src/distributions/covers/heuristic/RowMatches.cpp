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

#include "../Explain.h"
#include "../Covers.h"
#include "../CoverCategory.h"

#include "../../../utils/table.h"

// TODO TMP
#include "../../../utils/Timer.h"
extern vector<Timer> timersStrat;


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


// #define ROW_DEBUG

void RowMatches::setVoid(
  const Opponent side,
  PartialVoid& partialVoid,
  const Profile& sumProfile)
{
  if (partialVoid.repeats() == 0)
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

#ifdef ROW_DEBUG
cout << "The matches for " << sideName << " are:\n";
cout << RowMatches::str() << "\n";
#endif

  for (auto& match: matches)
  {
    // We prefer ways that make the length dimension of a cover
    // go away completely.  This happens if the lengths end up matching
    // the lengths by tops.
    // Lengths must be contiguous with a void in order to augment.

    if (match.preferred(partialVoid.lengthWest(), side))
      potentialsGreat.push_back(&match);
    else if (match.possible(partialVoid.lengthWest(), side))
      potentialsGood.push_back(&match);
  }

  const unsigned psizeGreat = static_cast<unsigned>(potentialsGreat.size());
  const unsigned psizeGood = static_cast<unsigned>(potentialsGood.size());

#ifdef ROW_DEBUG
cout << "Potentials " << psizeGreat << ", " << psizeGood << endl;
#endif

  if (psizeGreat > partialVoid.repeats())
    cout << sideName << "WARN Picking random great potentials\n";
  else if (psizeGreat + psizeGood > partialVoid.repeats())
    cout << sideName << "WARN Picking random good potentials\n";

  unsigned rest = partialVoid.repeats();

  // Use up any great potentials first.
  const unsigned potsGreat = min(psizeGreat, rest);
  rest -= potsGreat;
  auto pitGreat = potentialsGreat.begin();
  for (size_t p = 0; p < potsGreat; p++)
  {
    RowMatch& rm = ** pitGreat;
    rm.add(partialVoid.tricks(), side);
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
    rm.add(partialVoid.tricks(), side);
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
  row.resize(partialVoid.tricks().size());
  row.add(partialVoid.cover(), partialVoid.tricks(), VERBAL_LENGTH_ONLY);

  RowMatches::transfer(row, partialVoid.lengthWest(), side, rest);
  cout << sideName << "REST " << rest << "\n";

    // cout << "Done after rest\n";
    // cout << "The matches are now:\n";
    // cout << RowMatches::str() << "\n";
}


void RowMatches::incorporateLengths(
  const CoverStore& coverStore,
  const vector<unsigned char>& cases,
  vector<Tricks>& tricksOfLength,
  Explain& explain)
{
  // Add the top covers that apply entirely and only to given lengths.
  explain.setComposition(EXPLAIN_LENGTH_ONLY);

  // The actual maximum East-West length is tlen-1,
  // as the range goes from 0 to this maximum length.
  const size_t tlen = tricksOfLength.size();
  const size_t numDist = tricksOfLength[0].size();

  // Treat the voids separately.

  for (unsigned lenEW = 0; lenEW < tlen; lenEW++)
  {
    Tricks& tricks = tricksOfLength[lenEW];
    if (tricks.getWeight() == 0)
      continue;

    const unsigned factor = tricks.factor();

    if (2*lenEW+1 == tlen)
      explain.setSymmetry(EXPLAIN_SYMMETRIC);
    else
      explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);

    if (2*lenEW+1 == tlen)
    {
      // The cover must be symmetric.
      explain.setSymmetry(EXPLAIN_SYMMETRIC);
    }
    else
    {
      // A cover of such length is always anti-symmetric.
      explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);
    }

    PartialVoid partial;
    coverStore.heaviestPartial(tricks, cases, explain, partial);
    assert(partial.full(tricks.getWeight()));

    // Keep the voids for later, once all other row matches are known.
    if (lenEW == 0)
    {
      voidWest = move(partial);
      // voidWest = partial;
      voidWest.setVoid(0, factor);
    }
    else if (lenEW+1 == tlen)
    {
      voidEast = move(partial);
      // voidEast = partial;
      voidEast.setVoid(lenEW, factor);
    }
    else
    {
      // TODO Can potentially merge this into RowMatches:setVoid
      // with another VoidInfo (OPP_EITHER).
      CoverRow rowTmp;
      rowTmp.resize(numDist);
      rowTmp.add(partial.cover(), tricks, VERBAL_LENGTH_ONLY);

      RowMatches::transfer(rowTmp, lenEW, OPP_EAST, factor);
    }
  }
}


bool RowMatches::incorporateTops(
  Covers& covers,
  const vector<Tricks>& tricksWithinLength,
  const Profile& sumProfile,
  Explain& explain)
{
  // Add the top covers for a given length.
  explain.setComposition(EXPLAIN_MIXED_TERMS);

  // The actual maximum East-West length is tlen-1,
  // as the range goes from 0 to this maximum length.
  const size_t tlen = tricksWithinLength.size();

  bool newTableauFlag; // Not really used

  for (size_t lenEW = 0; lenEW < tlen; lenEW++)
  {
    const Tricks& tricks = tricksWithinLength[lenEW];
    if (tricks.getWeight() == 0)
      continue;

    if (2*lenEW+1 == tlen)
    {
      // The cover might be symmetric or general.
      // Here we just assume it's general.  TODO Could sharpen.
      explain.setSymmetry(EXPLAIN_GENERAL);
    }
    else
    {
      // A cover of such length is always anti-symmetric.
      explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);
    }

    explain.setSpecificLength(static_cast<unsigned char>(lenEW));

timersStrat[46].start();
    CoverTableau solution;
    solution.init(tricks, 0); // Minimum doesn't matter yet
timersStrat[46].stop();

timersStrat[47].start();
    // TODO Limit covers to those with the specific length
    covers.explainByCategory(tricks, explain, true,
      solution, newTableauFlag);
timersStrat[47].stop();

    if (! solution.complete())
    {
      // TODO This does happen, but rarely.  Probably due to a rank
      // being needed that doesn't seem to take any tricks?
      // So it's really a rank error.
      cout << "FAILED g = 4" << endl;
      explain.unsetSpecificLength();
      return false;
    }

    // TODO Maybe solution gets begin/end instead and we run here?
    // solution.destroyIntoMatches(* this, lenEW);
timersStrat[48].start();
    for (auto& row: solution.rows)
    {
      row.setVerbal(sumProfile);
      RowMatches::transfer(row, lenEW, OPP_EAST);
    }
timersStrat[48].stop();
  }

  explain.unsetSpecificLength();
  return true;
}


void RowMatches::incorporateVoids(const Profile& sumProfile)
{
  // Add in the voids, completing row matches a bit cleverly.
  RowMatches::setVoid(OPP_WEST, voidWest, sumProfile);
  RowMatches::setVoid(OPP_EAST, voidEast, sumProfile);
}


void RowMatches::symmetrize(const Profile& sumProfile)
{
  if (matches.size() < 2)
    return;

  for (auto rit1 = matches.begin(); rit1 != matches.end(); rit1++)
  {
    for (auto rit2 = next(rit1); rit2 != matches.end(); )
    {
      if (rit1->symmetricWith(* rit2))
      {
        if (rit1->symmetrizable(sumProfile))
        {
          // rit1 has the lower length, so anything is symmetrizable,
          // it is this.
          rit1->symmetrize();
          rit2 = matches.erase(rit2);
        }
        else
          rit2++;
      }
      else
        rit2++;
    }
  }
}


void RowMatches::makeSolution(
  const CoverStore& coverStore,
  const vector<unsigned char>& cases,
  const Profile& sumProfile,
  Explain& explain,
  CoverTableau& solution)
{
  // Score those row matches anew that involve more than one row
  // in RowMatches, i.e. multiple lengths but the same tops.
  explain.setSymmetry(EXPLAIN_GENERAL);
  explain.setComposition(EXPLAIN_MIXED_TERMS);

  for (auto& match: matches)
  {
    if (match.singleCount())
      solution.addRow(match.getSingleRow());
    else
    {
      Partial partial;
      coverStore.heaviestPartial(match.getTricks(), cases,
        explain, partial);

      if (! partial.full(match.getTricks().getWeight()))
      {
        cout << "No full match for:\n" << match.str() << endl;
        cout << "The cover store contains:\n";
        cout << coverStore.str() << endl;
        assert(false);
      }

      solution.addRow(partial.cover(), partial.cover().verbal(sumProfile));
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
