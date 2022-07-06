/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Heuristic.h"

#include "CoverStore.h"
#include "Cover.h"
#include "CoverRow.h"
#include "Explain.h"

#include "tableau/CoverTableau.h"



Heuristic::Heuristic()
{
  partials.clear();
}


void Heuristic::emplace(
  Cover const * coverPtr,
  const Tricks& additions,
  const unsigned rawWeightAdder)
{
  // TODO This is quite inefficient.  I suppose partialCovers might
  // instead hold pointers to PartialCover's, and there would be a
  // list of the actual PartialCovers with one scratch element that
  // would get swapped in against the deleted element.

  PartialCover pc;
  pc.coverPtr = coverPtr;
  pc.additions = additions;
  pc.rawWeightAdder = rawWeightAdder;
  partials.emplace(pc);
}


void Heuristic::findHeaviestN(
  const CoverStore& coverStore,
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  const Explain& explain,
  const size_t numHeaviest)
{
  // This method finds the numHeaviest highest-weight covers consistent
  // with explain.  For example, it can find the length-only or
  // tops-only winners.  It is possible that there is no winner at all,
  // e.g. 8/4894, Strategy #1.
  
  Tricks additions;
  additions.resize(tricks.size());

  unsigned rawWeightAdder = 0;

  CoverRow row;
  row.resize(tricks.size());

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

    if (! row.possibleAdd(cover, tricks, cases, additions, rawWeightAdder))
    {
      // Only use fitting candidates.
      continue;
    }

    if (partials.size() < numHeaviest)
    {
      Heuristic::emplace(&cover, additions, rawWeightAdder);
    }
    else
    {
      auto plast = prev(partials.end());
      if (rawWeightAdder > plast->rawWeightAdder)
      {
        // Keep the size down to numHeaviest.
        partials.erase(plast);
        Heuristic::emplace(&cover, additions, rawWeightAdder);
      }
    }
  }
}


bool Heuristic::combineSimply(
  const Heuristic& heur2,
  CoverTableau& partialSolution,
  bool& combinedFlag) const
{
  // Returns true if at least one of the two Heuristic's is empty.

  if (heur2.partials.empty())
  {
    if (partials.empty())
    {
      combinedFlag = false;
      return true;
    }

    partialSolution.addRow(* partials.begin()->coverPtr);
    combinedFlag = true;
    return true;
  }
  else if (partials.empty())
  {
    partialSolution.addRow(* heur2.partials.begin()->coverPtr);
    combinedFlag = true;
    return true;
  }
  else
  {
    // No simple combination.
    return false;
  }
}


void Heuristic::setPartialSolution(
  const PartialBest& partialBest,
  const vector<unsigned char>& cases,
  CoverTableau& partialSolution) const
{
  if (partialBest.flag1)
  {
    if (! partialBest.flag2)
    {
      // Only the first one.
      partialSolution.addRow(* partialBest.ptr1->coverPtr);
    }
    else if (partialBest.flagIndep)
    {
      // Two rows.
      partialSolution.addRow(* partialBest.ptr1->coverPtr);
      partialSolution.addRow(* partialBest.ptr2->coverPtr);
    }
    else
    {
      // One row with an OR.
      partialSolution.addRow(* partialBest.ptr1->coverPtr);

      Tricks additionsScratch = partialBest.ptr1->additions;
      additionsScratch.uniqueOver(partialBest.ptr1->additions, cases);

      partialSolution.extendRow(
        * partialBest.ptr2->coverPtr,
        partialBest.ptr2->additions,
        partialBest.ptr2->rawWeightAdder,
        0);
    }
  }
  else if (partialBest.flag2)
  {
    // Only the second one.
    partialSolution.addRow(* partialBest.ptr2->coverPtr);
  }
  else
  {
    assert(false);
  }
}


bool Heuristic::combine(
  const Heuristic& heur2,
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  CoverTableau& partialSolution) const
{
  // Typically one Heuristic will be the top N partial length covers,
  // and the other (pc2) will be the top N partial tops-only covers.
  // We pick the combination that covers the most weight in total.
  
  bool combinedFlag = false;
  if (Heuristic::combineSimply(heur2, partialSolution, combinedFlag))
    return combinedFlag;

  PartialBest partialBest;
  unsigned bestWeightAdder = 0;
  Tricks additionsScratch;

  for (auto& pc1: partials)
  {
    for (auto& pc2: heur2.partials)
    {
      // Try adding them together as independent rows.
      additionsScratch = pc1.additions;
      additionsScratch += pc2.additions;

      if (additionsScratch <= tricks)
      {
        // Go with independent rows.
        if (additionsScratch.getWeight() > bestWeightAdder)
        {
          partialBest.set(&pc1, &pc2, true, true, true);
        }
      }
      else if (pc1.additions == pc2.additions ||
          pc1.additions <= pc2.additions)
      {
        // pc1 is dominated by pc2.
        if (pc2.rawWeightAdder > bestWeightAdder)
        {
          partialBest.set(nullptr, &pc2, false, true, false);
        }
      }
      else if (pc2.additions <= pc1.additions)
      {
        // pc2 is dominated by pc1.
        if (pc1.rawWeightAdder > bestWeightAdder)
        {
          partialBest.set(&pc1, nullptr, true, false, false);
        }
      }
      else
      {
        // OR them together.
        additionsScratch = pc1.additions;
        additionsScratch.orNormal(pc2.additions, cases);

        if (additionsScratch.getWeight() > bestWeightAdder)
        {
          partialBest.set(&pc1, &pc1, true, true, false);
        }
      }
    }
  }

  Heuristic::setPartialSolution(partialBest, cases, partialSolution);
  return true;
}

