/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Heuristics.h"

#include "../CoverStore.h"
#include "../Cover.h"
#include "../CoverRow.h"
#include "../Explain.h"

#include "../tableau/CoverTableau.h"



Heuristics::Heuristics()
{
  partials.clear();
}


void Heuristics::emplace(
  Cover const * coverPtr,
  const Tricks& additions,
  const unsigned rawWeightAdder)
{
  // TODO This is quite inefficient.  I suppose partialCovers might
  // instead hold pointers to Partial's, and there would be a
  // list of the actual Partial's with one scratch element that
  // would get swapped in against the deleted element.

  Partial partial;
  partial.set(coverPtr, additions, rawWeightAdder);
  // partial.coverPtr = coverPtr;
  // partial.additions = additions;
  // partial.rawWeightAdder = rawWeightAdder;
  partials.emplace(partial);
}


void Heuristics::findHeaviestN(
  const CoverStore& coverStore,
  const Tricks& tricksIn,
  const vector<unsigned char>& cases,
  const Explain& explain,
  const size_t numHeaviest)
{
  // This method finds the numHeaviest highest-weight covers consistent
  // with explain.  For example, it can find the length-only or
  // tops-only winners.  It is possible that there is no winner at all,
  // e.g. 8/4894, Strategy #1.
  
  Tricks additions;
  additions.resize(tricksIn.size());

  unsigned rawWeightAdder = 0;

  CoverRow row;
  row.resize(tricksIn.size());

  for (auto& cover: coverStore)
  {
    if (explain.skip(
      cover.effectiveDepth(),
      cover.symmetry(),
      cover.composition()))
    {
      // Only use consistent candidates.
      continue;
    }

    if (! row.possibleAdd(cover, tricksIn, cases, additions, rawWeightAdder))
    {
      // Only use fitting candidates.
      continue;
    }

    if (partials.size() < numHeaviest)
    {
      Heuristics::emplace(&cover, additions, rawWeightAdder);
    }
    else
    {
      auto plast = prev(partials.end());
      if (rawWeightAdder > plast->weight())
      {
        // Keep the size down to numHeaviest.
        partials.erase(plast);
        Heuristics::emplace(&cover, additions, rawWeightAdder);
      }
    }
  }
}


bool Heuristics::combineSimply(
  const Heuristics& heur2,
  CoverTableau& partialSolution,
  bool& combinedFlag) const
{
  // Returns true if at least one of the two Heuristics's is empty.

  if (heur2.partials.empty())
  {
    if (partials.empty())
    {
      combinedFlag = false;
      return true;
    }

    partialSolution.addRow(partials.begin()->cover());
    combinedFlag = true;
    return true;
  }
  else if (partials.empty())
  {
    partialSolution.addRow(heur2.partials.begin()->cover());
    combinedFlag = true;
    return true;
  }
  else
  {
    // No simple combination.
    return false;
  }
}


void Heuristics::setPartialSolution(
  const PartialBest& partialBest,
  const vector<unsigned char>& cases,
  CoverTableau& partialSolution) const
{
  if (partialBest.flag1)
  {
    if (! partialBest.flag2)
    {
      // Only the first one.
      // partialSolution.addRow(* partialBest.ptr1->coverPtr);
      partialSolution.addRow(partialBest.ptr1->cover());
    }
    else if (partialBest.flagIndep)
    {
      // Two rows.
      partialSolution.addRow(partialBest.ptr1->cover());
      partialSolution.addRow(partialBest.ptr2->cover());
    }
    else
    {
// cout << "partial, OR\n";
// cout << "pbest1\n" << partialBest.ptr1->additions.strSpaced();
// cout << "pbest2\n" << partialBest.ptr2->additions.strSpaced();
      // One row with an OR.
      partialSolution.addRow(partialBest.ptr1->cover());

      Tricks additionsScratch = partialBest.ptr2->tricks();
      additionsScratch.uniqueOver(partialBest.ptr1->tricks(), cases);
// cout << "u over " << additionsScratch.strSpaced();

      partialSolution.extendRow(
        partialBest.ptr2->cover(),
        additionsScratch,
        partialBest.ptr2->weight(),
        0);
    }
  }
  else if (partialBest.flag2)
  {
    // Only the second one.
    partialSolution.addRow(partialBest.ptr2->cover());
  }
  else
  {
    assert(false);
  }
}


// #define HEUR_DEBUG

bool Heuristics::combine(
  const Heuristics& heur2,
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  CoverTableau& partialSolution) const
{
  // Typically one Heuristics will be the top N partial length covers,
  // and the other (pc2) will be the top N partial tops-only covers.
  // We pick the combination that covers the most weight in total.
  
  bool combinedFlag = false;
  if (Heuristics::combineSimply(heur2, partialSolution, combinedFlag))
    return combinedFlag;

  PartialBest partialBest;

  unsigned bestWeightAdder = 0;
  Complexity bestComplexity;
  bestComplexity.reset();
  bestComplexity.addRow(0xff, 0xffff);
  Complexity runningComplexity;

  Tricks additionsScratch;

  for (auto& pc1: partials)
  {
    for (auto& pc2: heur2.partials)
    {
      // Try adding them together as independent rows.
      additionsScratch = pc1.tricks();
      additionsScratch += pc2.tricks();

      if (additionsScratch <= tricks)
      {
        runningComplexity.reset();
        pc1.addRowToComplexity(runningComplexity);
        pc2.addRowToComplexity(runningComplexity);

#ifdef HEUR_DEBUG
  cout << "Independent " << additionsScratch.getWeight() << "\n";
  cout << pc1.coverPtr->strNumerical() << endl;
  cout << pc2.coverPtr->strNumerical() << endl;
  cout << additionsScratch.strSpaced() << endl;
#endif
        // Go with independent rows.
        if (runningComplexity < bestComplexity)
        // if (additionsScratch.getWeight() > bestWeightAdder)
        {
#ifdef HEUR_DEBUG
 cout << "Two rows, complexity now " << runningComplexity.strFull() << endl << endl;
#endif
          partialBest.set(&pc1, &pc2, true, true, true);
          bestWeightAdder = additionsScratch.getWeight();
          bestComplexity = runningComplexity;
        }
      }
      else if (pc1.tricks() <= pc2.tricks())
      {
#ifdef HEUR_DEBUG
  cout << "pc2 dominates " << additionsScratch.getWeight() << "\n";
  cout << pc1.coverPtr->strNumerical() << endl;
  cout << pc2.coverPtr->strNumerical() << endl;
  cout << pc1.additions.strSpaced() << endl;
  cout << pc2.additions.strSpaced() << endl << endl;
#endif
        // pc1 is dominated by pc2.
        if (pc2.weight() > bestWeightAdder)
        {
#ifdef HEUR_DEBUG
 cout << "  better\n";
#endif
          partialBest.set(nullptr, &pc2, false, true, false);
          bestWeightAdder = additionsScratch.getWeight();
        }
      }
      else if (pc2.tricks() <= pc1.tricks())
      {
#ifdef HEUR_DEBUG
 cout << "pc1 dominates " << additionsScratch.getWeight() << "\n";
#endif
        // pc2 is dominated by pc1.
        if (pc1.weight() > bestWeightAdder)
        {
#ifdef HEUR_DEBUG
 cout << "  better\n";
#endif
          partialBest.set(&pc1, nullptr, true, false, false);
          bestWeightAdder = additionsScratch.getWeight();
        }
      }
      else
      {
#ifdef HEUR_DEBUG
 cout << "OR " << additionsScratch.getWeight() << "\n";
#endif
        // OR them together.
        additionsScratch = pc1.tricks();
// cout << "First  " << additionsScratch.strSpaced() << "\n";
        additionsScratch.orNormal(pc2.tricks(), cases);
// cout << "Second " << additionsScratch.strSpaced() << "\n";

        runningComplexity.reset();
        pc1.addCoverToComplexity(runningComplexity);
        pc2.addCoverToComplexity(runningComplexity);
 // cout << "One row, complexity now " << runningComplexity.strFull() << endl;

        // if (additionsScratch.getWeight() > bestWeightAdder)
        if (runningComplexity < bestComplexity)
        {
#ifdef HEUR_DEBUG
  cout << "  better\n";
#endif
          partialBest.set(&pc1, &pc2, true, true, false);
          bestWeightAdder = additionsScratch.getWeight();
          bestComplexity = runningComplexity;
        }
      }
    }
  }

// cout << "p bef   " << partialSolution.strResiduals();
  Heuristics::setPartialSolution(partialBest, cases, partialSolution);
// cout << "p aft   " << partialSolution.strResiduals();
  return true;
}


string Heuristics::str() const
{
  stringstream ss;
  ss << "Heuristics:\n";
  for (auto& partial: partials)
    ss << partial.str();
  return ss.str() + "\n";
}

