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
#include "RowMatches.h"

#include "tableau/TableauStats.h"

#include "product/ProfilePair.h"

#include "../../strategies/result/Result.h"

extern TableauStats tableauStats;

// TODO TMP
#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;

// TODO TMP
#include "../../inputs/Control.h"
extern Control control;

// TODO TMP
#include "../../ranks/RanksNames.h"


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
// cout << "Trying cover " << cover.strTricksShort() << endl;
// cout << "  eff " << +cover.effectiveDepth() <<
  // " symm " << +cover.symmetry() << 
  // " comp " << +cover.composition() << endl;
    if (explain.skip(
        cover.effectiveDepth(),
        cover.symmetry(),
        cover.composition()))
    {
      // Select consistent candidates.
      continue;
    }

// cout << "  Past explain " << cover.strTricksShort() << endl;
    if (row.possibleAdd(cover, tricks, cases, additionsLocal, weightLocal))
    {
// cout << "    Is possible " << cover.strTricksShort() << endl;
      if (weightLocal > heavyData.rawWeightAdder)
      {
// cout << "    Is heavier " << cover.strTricksShort() << endl;
        heavyData.coverPtr = &cover;
        heavyData.additions = additionsLocal;
        heavyData.rawWeightAdder = weightLocal;
      }
    }
  }
}


Cover const * Covers::heaviestCover(
  const Tricks& tricks,
  const Explain& explain,
  bool& fullCoverFlag) const
{
  // This find the best (highest-weight) cover consistent with explain.
  // For example, it can find the length-only or tops-only winner.
  // Note that it may return nullptr if nothing is found, which is
  // indeed possible in some calls (e.g. 8/4894, Strategy #1).

  Cover const * coverPtr = nullptr;

  Tricks additions;
  additions.resize(tricks.size());

  unsigned largestWeight = 0;
  unsigned weight;

  // TODO If we only ever want complete covers (as I think we do
  // longer-term), we could just check tricks == cover.getTricks()?
  // Then cover.getWeight().

  CoverRow row;
  row.resize(tricks.size());

  for (auto& cover: coverStore)
  {
// cout << "Attempting " << cover.strNumerical() << "\n";
    if (explain.skip(
        cover.effectiveDepth(),
        cover.symmetry(),
        cover.composition()))
    {
// cout << "  rejected, cover depth " << +cover.effectiveDepth() << 
  // " symm " << +cover.symmetry() << ", comp " << +cover.composition() << "\n";
      // Select consistent candidates.
      continue;
    }

    if (row.possibleAdd(cover, tricks, cases, additions, weight))
    {
      if (weight > largestWeight)
      {
        coverPtr = &cover;
        largestWeight = weight;
      }
    }
  }

  fullCoverFlag = (largestWeight == tricks.getWeight());
  return coverPtr;
}




void Covers::explainByCategory(
  const Tricks& tricks,
  const Explain& explain,
  const bool partialTableauFlag,
  CoverTableau& solution,
  bool& newTableauFlag)
{
  newTableauFlag = true;
  // TODO In the partial case this may overwrite a partial solution.
  // I guess in TableauCache::lookup we would += the solutions.
  // For now I just strike this efficiency.
  /*
  if (tableauCache.lookup(tricks, solution))
  {
    solution.setMinTricks(explain.tricksMin());
    newTableauFlag = false;
    return;
  }
  */

  CoverStack<Cover> stack;

  if (partialTableauFlag)
  {
    // Go directly to the full solve, using solution as the start.
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, true, 50000, 25000, stack, solution);
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
      // Go with one row and an "OR"
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

  const CoverSymmetry tricksSymmetry = tricks.symmetry();
  solution.initStrData(numStrategyTops, tricksSymmetry);

  newTableauFlag = true;
  if (tricks.getWeight() == 0)
  {
    solution.setTrivial(tmin);
    newTableauFlag = false;
    return;
  }

  if (tableauCache.lookup(tricks, solution))
  {
    solution.setMinTricks(tmin);
    // TODO This may in fact not be the right depth, but it's a start.
    solution.initStrData(numStrategyTops, tricksSymmetry);
    newTableauFlag = false;
    return;
  }

  solution.init(tricks, tmin);

  Explain explain;
  explain.setParameters(numStrategyTops, tmin);
  explain.setSymmetry(tricksSymmetry);

  if (tricksSymmetry == EXPLAIN_SYMMETRIC)
  {
    // No need for anything fancy if tricks itself is constrained.
    Covers::explainByCategory(tricks, explain, false,
      solution, newTableauFlag);
    solution.initStrData(numStrategyTops, tricksSymmetry);
    tableauCache.store(tricks, solution);
    return;
  }

  if (tricksSymmetry == EXPLAIN_ANTI_SYMMETRIC)
  {
    Covers::explainByCategory(tricks, explain, false,
      solution, newTableauFlag);
    solution.initStrData(numStrategyTops, tricksSymmetry);
    tableauCache.store(tricks, solution);
    return;
  }

  // TODO This is a misuse of the goal input parameter, but
  // it's not forever.
  const unsigned mode = control.goal();

  if (mode == 0)
  {
    // Exhaustive run.  No guessing, no use of symmetry.
    explain.setSymmetry(EXPLAIN_GENERAL);

    Covers::explainByCategory(tricks, explain, false,
      solution, newTableauFlag);
  }
  else if (mode == 1)
  {
    // No guess.  Direct split by symmetry.
    Tricks tricksSymm, tricksAntisymm;
    solution.partitionResiduals(tricksSymm, tricksAntisymm, cases);

    if (tricksSymm.getWeight())
    {
      solution.init(tricksSymm, tmin);
      explain.setSymmetry(EXPLAIN_SYMMETRIC);

      Covers::explainByCategory(tricksSymm, explain, true,
        solution, newTableauFlag);
    }

    if (tricksAntisymm.getWeight())
    {
      solution.init(tricksAntisymm, tmin);
      CoverTableau solutionAntisymm;

      explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);
      Covers::explainByCategory(tricksAntisymm, explain, true,
        solution, newTableauFlag);
    }
  }
  else if (mode == 2)
  {
    // Guess followed by exhaustive run.
    Covers::guessStart(tricks, solution, explain);
    if (solution.complete())
    {
      solution.initStrData(numStrategyTops, tricksSymmetry);
      return;
    }

    explain.setSymmetry(EXPLAIN_GENERAL);

    Covers::explainByCategory(tricks, explain, true,
      solution, newTableauFlag);
  }
  else if (mode == 3)
  {
    // Guess followed by split of the remainder by symmetry.
    Covers::guessStart(tricks, solution, explain);
    if (solution.complete())
    {
      solution.initStrData(numStrategyTops, tricksSymmetry);
      return;
    }

    Tricks tricksSymm, tricksAntisymm;
    solution.partitionResiduals(tricksSymm, tricksAntisymm, cases);

    if (tricksSymm.getWeight())
    {
      solution.init(tricksSymm, tmin);
      explain.setSymmetry(EXPLAIN_SYMMETRIC);

      Covers::explainByCategory(tricksSymm, explain, true,
        solution, newTableauFlag);
    }

    if (tricksAntisymm.getWeight())
    {
      solution.init(tricksAntisymm, tmin);

      explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);
      Covers::explainByCategory(tricksAntisymm, explain, true,
        solution, newTableauFlag);
    }
  }
  else if (mode == 4)
  {
    // TODO Experimental
    vector<Tricks> tricksOfLength;
    vector<Tricks> tricksWithinLength;

    // TODO May want to split even the symmetrics.  Then the
    // partitioning should also somehow be symmetric.
    tricks.partitionGeneral(tricksWithinLength, tricksOfLength, cases);

    // Add the length-only covers arising from a minimum trick number
    // for a given length.
    const size_t tlen = tricksOfLength.size();
    const size_t numDist = tricksOfLength[0].size();

    unsigned numVoidsWest = 0;
    unsigned numVoidsEast = 0;

    CoverTableau solutionTmp;
    // CoverRow rowTmp;

    list<RowMatch> rowMatches;

    explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);
    explain.setComposition(EXPLAIN_LENGTH_ONLY);

    for (unsigned lenEW = 0; lenEW < tlen; lenEW++)
    {
      if (tricksOfLength[lenEW].getWeight() == 0)
        continue;

      const unsigned factor = tricksOfLength[lenEW].factor();

      if (2*lenEW+1 == tlen)
        explain.setSymmetry(EXPLAIN_SYMMETRIC);
      else
        explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);

      Cover const * coverPtr;
      bool fullCoverFlag;
      coverPtr = Covers::heaviestCover(tricksOfLength[lenEW], explain, 
        fullCoverFlag);
      if (! fullCoverFlag)
      {
        cout << "Tried matching " << lenEW << ":\n" << 
          tricksOfLength[lenEW].strList() << endl;
        cout << "factor " << factor << endl;
      assert(fullCoverFlag);
      }
      assert(coverPtr != nullptr);

      // TODO We don't really need either data point, but we need to
      // resize tricks in solutions.
      solutionTmp.resize(tricks.size());
      // rowTmp.resize(tricks.size());

      for (unsigned f = 0; f < factor; f++)
        solutionTmp.addRow(* coverPtr);

        solutionTmp.destroyIntoMatches(rowMatches, lenEW);

      // TODO Treat the voids separately?
      if (lenEW == 0)
        numVoidsWest = factor;
      else if (lenEW+1 == tlen)
        numVoidsEast = factor;
    }


    vector<CoverTableau> solutionsLength;
    solutionsLength.resize(tlen);

    explain.setComposition(EXPLAIN_MIXED_TERMS);

    for (unsigned lenEW = 0; lenEW < tlen; lenEW++)
    {
      const Tricks& tricksL = tricksWithinLength[lenEW];
      if (tricksL.getWeight() == 0)
        continue;

      // A cover of a given length is always anti-symmetric.
      if (2*lenEW+1 == tlen)
        explain.setSymmetry(EXPLAIN_GENERAL);
      else
        explain.setSymmetry(EXPLAIN_ANTI_SYMMETRIC);

      solutionsLength[lenEW].init(tricksL, 0); // Minimum doesn't matter yet
   
// cout << "varEW " << lenEW << endl;
      // TODO Limit covers to those with the specific length
      Covers::explainByCategory(tricksL, explain, true,
        solutionsLength[lenEW], newTableauFlag);

      // TODO Probably due to a rank being needed that doesn't seem
      // to take any tricks?
      if (! solutionsLength[lenEW].complete())
      {
        cout << "FAILED g = 4" << endl;
        return;
      }

      // cout << "row matches before\n";
      // for (auto& rowMatch: rowMatches)
        // cout << rowMatch.str();
// cout << "About to partition " << lenEW << endl;
      solutionsLength[lenEW].destroyIntoMatches(rowMatches, lenEW);
    }

// cout << "Done tricks" << endl;
      /*
      cout << "after partitions, row matches now\n";
      for (auto& rowMatch: rowMatches)
        cout << rowMatch.str();
        */

    solution.init(tricks, tmin);

    // Score those row matches anew that involves more than one row.
    explain.setSymmetry(EXPLAIN_GENERAL);
    explain.setComposition(EXPLAIN_MIXED_TERMS);
    for (auto& rowMatch: rowMatches)
    {
      if (rowMatch.singleCount())
        solution.addRow(rowMatch.getSingleRow());
      else
      {
        Cover const * coverPtr;
        bool fullCoverFlag;
        coverPtr = Covers::heaviestCover(rowMatch.getTricks(), explain, 
          fullCoverFlag);
        if (! fullCoverFlag)
        {
          cout << "Tried\n" << rowMatch.str() << endl;
        assert(fullCoverFlag);
        }
      assert(coverPtr != nullptr);

// cout << "final" << endl;
        solution.addRow(* coverPtr);
      }
    }

    // Set the actual minimum.
    solution.setMinTricks(tmin);
  }
  else
    assert(false);

  // This tends to get destroyed when solving with partial solutions,
  // so we just reset it.
  solution.initStrData(numStrategyTops, EXPLAIN_GENERAL);
  tableauCache.store(tricks, solution);
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


string Covers::strCache(const RanksNames& ranksNames) const
{
  return tableauCache.str(sumProfile, ranksNames);
}


string Covers::strSignature() const
{
  stringstream ss;
  ss << setw(6) << coverStore.size() << sumProfile.strLine();
  return ss.str();
}

