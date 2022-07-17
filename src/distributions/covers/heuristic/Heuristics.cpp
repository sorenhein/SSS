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
  partials.emplace(partial);
}


void Heuristics::findHeaviestN(
  const CoverStore& coverStore,
  const Tricks& tricksIn,
  const vector<unsigned char>& cases,
  const Explain& explain,
  const size_t numHeaviest)
{
  coverStore.heaviestPartials(tricksIn, cases, explain,
    numHeaviest, partials);
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


bool Heuristics::combineSimply(
  const Heuristics& heur2,
  const Tricks& tricks,
  list<CoverTableau>& partialSolutions,
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

    for (auto& partial: partials)
    {
      partialSolutions.emplace_back(CoverTableau());
      CoverTableau& partialSolution = partialSolutions.back();
      partialSolution.init(tricks, 0);  // tmin comes later
      partialSolution.addRow(partial.cover());
    }

    combinedFlag = true;
    return true;
  }
  else if (partials.empty())
  {
    for (auto& partial: heur2.partials)
    {
      partialSolutions.emplace_back(CoverTableau());
      CoverTableau& partialSolution = partialSolutions.back();
      partialSolution.init(tricks, 0);
      partialSolution.addRow(partial.cover());
    }

    combinedFlag = true;
    return true;
  }
  else
  {
    // No simple combination.
    return false;
  }
}


bool Heuristics::insertDominant(
  list<Dominant>& dominant,
  const Tricks& tricks,
  Partial const * partial1Ptr,
  Partial const * partial2Ptr,
  const bool flag1In,
  const bool flag2In,
  const bool flagIndepIn) const
{
  // TODO This is inefficient!
  // Could sort list by weight.
  // Could know ahead of time whether to test for <= or >=.

  for (auto diter = dominant.begin(); diter != dominant.end(); )
  {
    // TODO Could add a Tricks::operator <
    if (tricks <= diter->tricks && ! (tricks == diter->tricks))
    {
      // Dominated.
      return false;
    }
    else if (diter->tricks <= tricks && ! (tricks == diter->tricks))
    {
      // Erase all dominated entries.
      diter = dominant.erase(diter);
    }
    else
    {
      // TODO If identical, could pick the lower-complexity one.
      diter++;
    }
  }

  dominant.emplace_back(Dominant());
  Dominant& dom = dominant.back();
  dom.tricks = tricks;
  dom.partialBest.set(partial1Ptr, partial2Ptr, 
    flag1In, flag2In, flagIndepIn);
  return true;
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


bool Heuristics::combine(
  const Heuristics& heur2,
  const Tricks& tricks,
  const vector<unsigned char>& cases,
  list<CoverTableau>& partialSolutions) const
{
  // Typically one Heuristics will be the top N partial length covers,
  // and the other (pc2) will be the top N partial tops-only covers.
  // We pick the combination that covers the most weight in total.
  
  bool combinedFlag = false;
  if (Heuristics::combineSimply(heur2, tricks, 
      partialSolutions, combinedFlag))
    return combinedFlag;

  list<Dominant> dominant;

  Tricks additionsScratch;
  bool flag;

  for (auto& pc1: partials)
  {
    for (auto& pc2: heur2.partials)
    {
      // Try adding them together as independent rows.
      additionsScratch = pc1.tricks();
      additionsScratch += pc2.tricks();

      if (additionsScratch <= tricks)
      {
        // TODO Will use partialBest.set(&pc1, &pc2, true, true, true);
        flag = Heuristics::insertDominant(dominant, additionsScratch,
          &pc1, &pc2, true, true, true);
        
#ifdef HEUR_DEBUG
  if (flag)
  {
    cout << "Independent " << additionsScratch.getWeight() << "\n";
    cout << pc1.coverPtr->strNumerical() << endl;
    cout << pc2.coverPtr->strNumerical() << endl;
    cout << additionsScratch.strSpaced() << endl;
  }
#endif
      }
      else if (pc1.tricks() <= pc2.tricks())
      {
        // pc1 is dominated by pc2.
        // TODO Will use partialBest.set(nullptr, &pc2, false, true, false);

        flag = Heuristics::insertDominant(dominant, pc2.tricks(),
          nullptr, &pc2, false, true, false);

#ifdef HEUR_DEBUG
  if (flag)
  {
    cout << "pc2 dominates " << additionsScratch.getWeight() << "\n";
    cout << pc1.coverPtr->strNumerical() << endl;
    cout << pc2.coverPtr->strNumerical() << endl;
    cout << pc1.additions.strSpaced() << endl;
    cout << pc2.additions.strSpaced() << endl << endl;
  }
#endif
      }
      else if (pc2.tricks() <= pc1.tricks())
      {
        // pc2 is dominated by pc1.
        flag = Heuristics::insertDominant(dominant, pc1.tricks(),
          &pc1, nullptr, true, false, false);

#ifdef HEUR_DEBUG
  if (flag)
  {
    cout << "pc1 dominates " << additionsScratch.getWeight() << "\n";
    cout << pc1.coverPtr->strNumerical() << endl;
    cout << pc2.coverPtr->strNumerical() << endl;
    cout << pc1.additions.strSpaced() << endl;
    cout << pc2.additions.strSpaced() << endl << endl;
  }
#endif
      }
      else
      {
        // OR them together.
        additionsScratch = pc1.tricks();
        additionsScratch.orNormal(pc2.tricks(), cases);

        flag = Heuristics::insertDominant(dominant, additionsScratch,
          &pc1, &pc2, true, true, false);

#ifdef HEUR_DEBUG
  if (flag)
  {
    cout << "OR " << additionsScratch.getWeight() << "\n";
    cout << pc1.coverPtr->strNumerical() << endl;
    cout << pc2.coverPtr->strNumerical() << endl;
    cout << pc1.additions.strSpaced() << endl;
    cout << pc2.additions.strSpaced() << endl << endl;
  }
#endif
      }
    }
  }

  for (auto& dom: dominant)
  {
    partialSolutions.emplace_back(CoverTableau());
    CoverTableau& partialSolution = partialSolutions.back();
    partialSolution.init(tricks, 0);  // tmin comes later
    Heuristics::setPartialSolution(dom.partialBest, cases, partialSolution);
  }

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

