/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

/*
   The search for an "optimal" cover can be very expensive.
   I have tried a number of approaches and the best ones are 
   implemented here:

   - Rank candidates by increasing "complexity per weight", where
     complexity is a subjective measure and weight is the number of
     cases of the candidate.  This tends to make it possible to
     cut off stack elements that would bust a known solution.

   - Generate a "greedy" solution first in order to seed the broader
     search.  This is not a major improvement, but it does help.
     I truncate the stack to a relatively small size.

   - Truncate the stack so it doesn't get completely out of control.
     This does cost a bit of accuracy, but complexity is subjective
     anyway and the cutoff is not so bad, at least for depth 2
     (12 strategies with complexity 23/7 instead of 23/5).
 */



#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Covers.h"
#include "CoverStack.h"
#include "Explain.h"
#include "CoverCategory.h"

#include "tableau/TableauStats.h"

#include "product/ProfilePair.h"

#include "../../strategies/result/Result.h"

extern TableauStats tableauStats;

// TODO TMP
#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;


struct HeavyData
{
  Cover const * coverPtr;
  Tricks additions;
  unsigned rawWeightAdder;

  HeavyData(const size_t size)
  {
    coverPtr = nullptr;
    additions.resize(size);
    rawWeightAdder = 0;
  };
};


Covers::Covers()
{
  Covers::reset();
}


void Covers::reset()
{
  cases.clear();

  coverStore.reset();
  tableauCache.reset();

  rowStore.reset();
  tableauRowCache.reset();
}


void Covers::fillStore(
  ProductMemory& productMemory,
  const vector<Profile>& distProfiles,
  ProfilePair& running)
{
  // This add() and the next one consume about 70% of the
  // overall time of the loop.
  coverStore.add(productMemory, sumProfile, running, distProfiles, cases);

  unsigned char westLow, westHigh;
  running.getLengthRange(westLow, westHigh);

  const unsigned char maxTricks = sumProfile.length();

  for (unsigned char lLow = 0; lLow <= westHigh; lLow++)
  {
    // Allow 0 as well as [westLow, westHigh].
    if (lLow > 0 && lLow < westLow)
      continue;

    for (unsigned char lHigh = max(lLow, westLow); 
        lHigh <= maxTricks; lHigh++)
    { 
      // Allow maxTricks as well as [westHigh, westHigh].
      if (lHigh > westHigh && lHigh < maxTricks)
        continue;

      running.setLength(lLow, lHigh);

      if (! running.minimal(sumProfile, westLow, westHigh))
       continue;

      coverStore.add(productMemory, sumProfile, running, 
        distProfiles, cases);
    }
  }
}


void Covers::prepare(
  ProductMemory& productMemory,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& casesIn,
  const Profile& sumProfileIn)
{
  cases = casesIn;
  sumProfile = sumProfileIn;

  list<ProfilePair> stack; // Unfinished expansions
  stack.emplace_back(ProfilePair(sumProfile));

  while (! stack.empty())
  {
    ProfilePair& running = stack.front();

    unsigned char topNumber = running.getNextTopNo(); // Next to write
    const unsigned char topLength = running.getTopLength(sumProfile);

    for (unsigned char topLow = 0; topLow <= topLength; topLow++)
    {
      for (unsigned char topHigh = topLow; topHigh <= topLength; topHigh++)
      {
        // Never use the lowest top explicitly.  Maybe it shouldn't
        // be there at all, but it is.
        if (topNumber == 0 && (topLow != 0 || topHigh != topLength))
          continue;

        // Add or restore the "don't care" with respect to length.
        running.setLength(0, sumProfile.length());
        running.setTop(topNumber, topLow, topHigh);

        // An unused top was already seen.
        if (topNumber > 0 && topLow == 0 && topHigh == topLength)
        {
          if (! running.last())
          {
            stack.push_back(running);
            stack.back().incrTop();
          }
          continue;
        }

        Covers::fillStore(productMemory, distProfiles, running);

        if (! running.last())
        {
          stack.push_back(running);
          stack.back().incrTop();
        }
      }
    }
    assert(! stack.empty());
    stack.pop_front();
  }

  coverStore.admixSymmetric();
}


void Covers::addDirectly(list<Cover const *>& coverPtrs)
{
  rowStore.addDirectly(coverPtrs, cases);
}


const Cover& Covers::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.
  return coverStore.lookup(cover);
}


template<class C, class T>
void Covers::explainTemplate(
  const Tricks& tricks,
  const Explain& explain,
  const C& candidates,
  const bool partialTableauFlag,
  const size_t pruneTrigger,
  const size_t pruneSize,
  CoverStack<T>& stack,
  CoverTableau& solution)
{
  if (partialTableauFlag)
  {
    // The solution is a partial one and it is placed onto the stack
    // as a given start to the optimization.
    // TODO Probably only needs solution and begin() here?
    stack.emplace(solution, tricks, explain.tricksMin(), 
      candidates.begin());

/*
cout << "partial solution\n";
cout << solution.str(sumProfile);
cout << "residuals\n";
cout << solution.strResiduals() << "\n";
cout << stack.begin()->tableau.str(sumProfile);
cout << "residuals\n";
cout << stack.begin()->tableau.strResiduals() << "\n";
*/

    // Then start with a clean slate.
    solution.reset();
  }
  else
  {
    // The solution is complete, and it is only used as a frame of
    // reference.  The optimization proceeds from scratch.
    stack.emplace(tricks, explain.tricksMin(), candidates.begin());
  }

  tableauStats.reset();

  while (! stack.empty())
  {
    auto handle = stack.extract(stack.begin());
    StackEntry<T>& stackElem = handle.value();

    CoverTableau& tableau = stackElem.tableau;
    auto candIter = stackElem.iter;

    size_t stackSize0 = stack.size();
    unsigned numSolutions0 = tableauStats.numSolutions;

    if (stack.size() > pruneTrigger)
    {
      // Just keep the most promising ones to save capacity.
      auto siter = stack.begin();
      for (size_t i = 0; i < pruneSize; i++, siter++) ;
      stack.erase(siter, stack.end());
    }

    while (candIter != candidates.end())
    {
      if (explain.skip(candIter->effectiveDepth(), candIter->symmetry()))
      {
        candIter++;
        continue;
      }

      const unsigned char headroom = tableau.headroom(solution);

      if (candIter->minComplexityAdder(tableau.getResidualWeight()) > 
          headroom)
      {
        // As the covers are ordered, later covers have no chance either.
        break;
      }

      if (candIter->getComplexity() > headroom)
      {
        // The current cover may be too complex, but there may be others.
        candIter++;
        continue;
      }

      if (tableau.attempt(cases, candIter, stack, solution))
      {
        // We found a solution.  It may have replaced the previous one.
        if (tableauStats.firstFix == 0)
          tableauStats.firstFix = tableauStats.numSteps;
        break;
      }

      candIter++;
    }

    tableauStats.numBranches += stack.size() - stackSize0;

    if (stack.size() > tableauStats.stackMax)
      tableauStats.stackMax = stack.size();

    if (tableauStats.numSolutions > numSolutions0)
      stack.prune(solution);

    tableauStats.stackActual = stack.size();

    tableauStats.numSteps++;
  }

  if (pruneTrigger > 10)
  {
    // It was the full run.
    T t;
    cout << tableauStats.strHeader();
    cout << tableauStats.str(t.ID());
  }
}

// Instantiations

template void Covers::explainTemplate<CoverStore, Cover>(
  const Tricks& tricks,
  const Explain& explain,
  const CoverStore& candidates,
  const bool partialTableauFlag,
  const size_t pruneTrigger,
  const size_t pruneSize,
  CoverStack<Cover>& stack,
  CoverTableau& solution);

template void Covers::explainTemplate<RowStore, CoverRow>(
  const Tricks& tricks,
  const Explain& explain,
  const RowStore& candidates,
  const bool partialTableauFlag,
  const size_t pruneTrigger,
  const size_t pruneSize,
  CoverStack<CoverRow>& stack,
  CoverTableau& solution);


void Covers::partitionResults(
  const list<Result>& results,
  list<unsigned char>& tricksSymm,
  list<unsigned char>& tricksAntisymm) const
{
  unsigned char tmin = numeric_limits<unsigned char>::max();
  for (auto& res: results)
    tmin = min(tmin, res.getTricks());

  unsigned weightSymm = 0;
  unsigned weightAntisymm = 0;

  tricksSymm.resize(results.size());
  tricksAntisymm.resize(results.size());

  auto riterf = results.begin();
  auto riterb = prev(results.end());
  auto titerSymm = tricksSymm.begin();
  auto titerAntisymm = tricksAntisymm.begin();

  while (riterf != results.end())
  {
    const unsigned char f = riterf->getTricks() - tmin;
    const unsigned char b = riterb->getTricks() - tmin;
    const unsigned char mint = min(f, b);

    * titerSymm = mint;
    * titerAntisymm = f - mint;

    weightSymm += mint;
    weightAntisymm += f - mint;
    
    riterf++;
    riterb--;
    titerSymm++;
    titerAntisymm++;
  }
}


void Covers::findHeaviest(
  const Tricks& tricks,
  const Explain& explain,
  HeavyData& heavyData) const
{
  // This find the best (highest-weight) cover consistent with explain.
  // For example, it can find the length-only or tops-only winner.
  // Note that coverPtr == nullptr if nothing is found, which is
  // indeed possible (e.g. 8/4894, Strategy #1).

  Tricks additionsLocal;
  additionsLocal.resize(tricks.size());
  unsigned weightLocal = 0;

  CoverRow row;
  row.resize(tricks.size());

  for (auto& cover: coverStore)
  {
    if (explain.skip(
        cover.effectiveDepth(),
        cover.symmetry(),
        cover.composition()))
    {
      // Select consistent candidates.
      continue;
    }

    if (row.possibleAdd(cover, tricks, cases, additionsLocal, weightLocal))
    {
      if (weightLocal > heavyData.rawWeightAdder)
      {
        heavyData.coverPtr = &cover;
        heavyData.additions = additionsLocal;
        heavyData.rawWeightAdder = weightLocal;
      }
    }
  }
}


void Covers::explainByCategory(
  const Tricks& tricks,
  const Explain& explain,
  const bool partialTableauFlag,
  CoverTableau& solution,
  bool& newTableauFlag)
{
  newTableauFlag = true;
  if (tableauCache.lookup(tricks, solution))
  {
    solution.setMinTricks(explain.tricksMin());
    newTableauFlag = false;
    return;
  }

  CoverStack<Cover> stack;

  if (partialTableauFlag)
  {
    // Go directly to the full solve, using solution as the start.
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, true, 50000, 25000, stack, solution);

    tableauCache.store(tricks, solution);
  }
  else
  {
    // Get a greedy solution.
    // TODO Test again later whether or not this helps on average.
    timersStrat[25].start();
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, false, 7, 7, stack, solution);
    timersStrat[25].stop();

    // cout << "Greedy solution\n";
    // cout << solution.strBracket() << "\n";
    // cout << solution.str(sumProfile);
    // cout << (solution.complete() ? "GOODGREED\n" : "BADGREED\n");

    // Use this to seed the exhaustive search.
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, false, 50000, 25000, stack, solution);

    tableauCache.store(tricks, solution);
  }
}


void Covers::guessStart(
  const Tricks& tricks,
  CoverTableau& partialSolution,
  Explain& explain) const
{
  explain.setComposition(EXPLAIN_LENGTH_ONLY);
  HeavyData heaviestLength(tricks.size());
  Covers::findHeaviest(tricks, explain, heaviestLength);
 
  explain.setComposition(EXPLAIN_TOPS_ONLY);
  HeavyData heaviestTops(tricks.size());
  Covers::findHeaviest(tricks, explain, heaviestTops);

  if (heaviestLength.coverPtr == nullptr && 
      heaviestTops.coverPtr == nullptr)
    return;
 
  if (heaviestTops.additions == heaviestLength.additions || 
      heaviestTops.additions <= heaviestLength.additions)
  {
    // Go with length
    partialSolution.addRow(* heaviestLength.coverPtr);
  }
  else if (heaviestLength.additions <= heaviestTops.additions)
  {
    // Go with tops
    partialSolution.addRow(* heaviestTops.coverPtr);
  }
  else
  {
    // TODO More efficiently? Just destroy heaviestLength?
    // Or can we call Tricks::possible after first addRow somehow?
    Tricks additionsSum = heaviestLength.additions;
    additionsSum += heaviestTops.additions;

    if (additionsSum <= tricks)
    {
      // Go with two separate rows
      partialSolution.addRow(* heaviestLength.coverPtr);
      partialSolution.addRow(* heaviestTops.coverPtr);
    }
    else
    {
      // Go with one row and and "OR"
      partialSolution.addRow(* heaviestLength.coverPtr);

      heaviestTops.additions.uniqueOver(heaviestLength.additions, cases);

      partialSolution.extendRow(
        * heaviestTops.coverPtr, 
        heaviestTops.additions, 
        heaviestTops.rawWeightAdder, 
        0);
    }
  }
}

void Covers::explain(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& solution,
  bool& newTableauFlag)
{
  // This version uses covers and puts them together into rows,
  // including possibly covers that are OR'ed together in a row.

  // First test the complete cache.
  Tricks tricks;
  unsigned char tmin;
  tricks.setByResults(results, cases, tmin);

  newTableauFlag = true;
  if (tableauCache.lookup(tricks, solution))
  {
    solution.setMinTricks(tmin);
    newTableauFlag = false;
    return;
  }

  if (tricks.getWeight() == 0)
  {
    solution.setTrivial(tmin);
    return;
  }

  // Symmetric tricks use a smaller pool of covers, so this should
  // be sufficient.
  Explain explain;
  explain.setParameters(numStrategyTops, tmin);

  CoverSymmetry tricksSymmetry = tricks.symmetry();
  if (tricksSymmetry == EXPLAIN_SYMMETRIC)
  {
    explain.setSymmetry(EXPLAIN_SYMMETRIC);
    Covers::explainByCategory(tricks, explain, false,
      solution, newTableauFlag);
    return;
  }

  if (tricksSymmetry == EXPLAIN_ANTI_SYMMETRIC)
  {
    explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);
    Covers::explainByCategory(tricks, explain, false,
      solution, newTableauFlag);
    return;
  }


  // TODO
  // Use solution all the way (no partialSolution).
  //
  // Call explainByCategory with "true".
  //
  // Time and complexity spreadsheet:
  // Original 4-day effort
  // Split symmetry and anti-symmetry, 45 minutes but poor
  // Using guessStart and then symmetry/anti-symmetry
  // (Using guessStart and the original effort)
  // Storing the products, covers and tableaux used and reading in
  // (so much fewer comparisons).
  // Should we try a couple of guesses?
  // Do we know ahead of time it's going to get rough?

  // CoverTableau partialSolution;
  solution.init(tricks, tmin);

  explain.setSymmetry(EXPLAIN_GENERAL);
  Covers::guessStart(tricks, solution, explain);

  if (solution.complete())
  {
    // TODO Inefficient
    // solution = partialSolution;
    return;
  }

  // cout << "Partial guess\n";
  // cout << partialSolution.str(sumProfile);


  Tricks tricksSymm, tricksAntisymm;
  solution.partitionResiduals(tricksSymm, tricksAntisymm, cases);
  // tricks.partition(tricksSymm, tricksAntisymm, cases);

// TODO Now we should probably subtract out the additions and resymmetrize
// and then check again which halves are in use.

  if (tricksSymm.getWeight())
  {
    // Do the symmetric component (keep it in solution).
    explain.setSymmetry(EXPLAIN_SYMMETRIC);
    Covers::explainByCategory(tricksSymm, explain, true,
      solution, newTableauFlag);

  cout << "solution after first half\n";
  cout << solution.str(sumProfile);
  }

  if (tricksAntisymm.getWeight())
  {
    // Do the asymmetric component.
    CoverTableau solutionAntisymm;
    explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);
    Covers::explainByCategory(tricksAntisymm, explain, true,
      solution, newTableauFlag);
  }

  // TODO Only use one solution?
  // I guess the first stack element would get solution as
  // its starting point.  But then the symmetric and anti-symmetric
  // parts could start to merge within rows...
  // solution += solutionAntisymm;
}


void Covers::explainManually(
  const list<Result>& results,
  CoverTableau& solution)
{
  // This version uses finished rows.
  Tricks tricks;
  unsigned char tmin;
  tricks.setByResults(results, cases, tmin);

  if (tableauRowCache.lookup(tricks, solution))
  {
    solution.setMinTricks(tmin);
    return;
  }

  // TODO For now
  Explain explain;
  if (tricks.symmetric())
    explain.setSymmetry(EXPLAIN_SYMMETRIC);
  else
    explain.setSymmetry(EXPLAIN_GENERAL);

  explain.setParameters(1, tmin);

  CoverStack<CoverRow> stack;
  Covers::explainTemplate<RowStore, CoverRow>(tricks,
    explain, rowStore, false, 50000, 25000, stack, solution);

  tableauRowCache.store(tricks, solution);
}


const Profile& Covers::getSumProfile() const
{
  return sumProfile;
}


void Covers::getCoverCounts(
  size_t& numTableaux,
  size_t& numUses) const
{
  tableauCache.getCounts(numTableaux, numUses);
}


string Covers::strCache() const
{
  return tableauCache.str(sumProfile);
}


string Covers::strSignature() const
{
  stringstream ss;
  ss << setw(6) << coverStore.size() << sumProfile.strLine();
  return ss.str();
}

