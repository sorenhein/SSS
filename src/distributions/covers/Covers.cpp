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
    stack.emplace(solution, tricks, explain.tricksMin(), 
      candidates.begin());
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
  list<unsigned char>& tricksAntisymm,
  Explain& explain) const
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

  explain.setTricks(tmin, weightSymm, weightAntisymm);
}


void Covers::findHeaviest(
  const Tricks& tricks,
  const Explain& explain,
  Cover const *& coverPtr,
  Tricks& additions,
  unsigned& rawWeightAdder) const
{
  // This find the best (highest-weight) cover consistent with explain.
  // For example, it can find the length-only or tops-only winner.
  // Note that coverPtr == nullptr if nothing is found, which is
  // indeed possible (e.g. 8/4894, Strategy #1).

  rawWeightAdder = 0;
  coverPtr = nullptr;

  unsigned weightLocal = 0;

  Tricks additionsLocal;
  additionsLocal.resize(tricks.size());

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
      if (weightLocal > rawWeightAdder)
      {
        coverPtr = &cover;
        additions = additionsLocal;
        rawWeightAdder = weightLocal;
      }
    }
  }
}


void Covers::explainByCategory(
  const list<unsigned char>& rawTricks,
  const Explain& explain,
  const bool partialTableauFlag,
  CoverTableau& solution,
  bool& newTableauFlag)
{
  Tricks tricks;
  tricks.setByList(rawTricks, cases);

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
  const unsigned char tmin,
  Explain& explain) const
{
  explain.setComposition(EXPLAIN_LENGTH_ONLY);
  Cover const * coverLengthPtr = nullptr;
  Tricks additionsLength;
  additionsLength.resize(tricks.size());
  unsigned rawWeightAdderLength;
  Covers::findHeaviest(tricks, explain,
    coverLengthPtr, additionsLength, rawWeightAdderLength);
 
  explain.setComposition(EXPLAIN_TOPS_ONLY);
  Cover const * coverTopsPtr = nullptr;
  Tricks additionsTops;
  additionsTops.resize(tricks.size());
  unsigned rawWeightAdderTops;
  Covers::findHeaviest(tricks, explain,
    coverTopsPtr, additionsTops, rawWeightAdderTops);

  if (coverLengthPtr == nullptr && coverTopsPtr == nullptr)
  {
    // cout << "Partial guess: none\n";
    return;
  }
 
  CoverTableau soln;
  soln.init(tricks, tmin);

  if (additionsLength == additionsTops || 
      additionsTops <= additionsLength)
  {
    // Go with length
    // assert(coverLengthPtr != nullptr);
// cout << "Length only" << endl;
    soln.addRow(* coverLengthPtr);
  }
  else if (additionsLength <= additionsTops)
  {
    // Go with tops
    // assert(coverTopsPtr != nullptr);
// cout << "Tops only" << endl;
// cout << "Cover " << coverTopsPtr->str(sumProfile) << endl;
// cout << "length wgt " << rawWeightAdderLength << endl;
// cout << "length tricks\n" << additionsLength.strList() << endl;
// cout << "tops wgt " << rawWeightAdderTops << endl;
// cout << "tops tricks\n" << additionsTops.strList() << endl;
    soln.addRow(* coverTopsPtr);
  }
  else
  {
    // TODO More efficiently?
    Tricks additionsSum = additionsLength;
    additionsSum += additionsTops;

    if (additionsSum <= tricks)
    {
      // Go with two separate rows
      // assert(coverLengthPtr != nullptr);
      // assert(coverTopsPtr != nullptr);
// cout << "Length and tops in two rows" << endl;
      soln.addRow(* coverLengthPtr);
      soln.addRow(* coverTopsPtr);
    }
    else
    {
      // Go with one row and and "OR"
      // assert(coverLengthPtr != nullptr);
      // assert(coverTopsPtr != nullptr);
// cout << "Length and tops in one row" << endl;
      soln.addRow(* coverLengthPtr);
      soln.extendRow(* coverTopsPtr, additionsTops, 
        rawWeightAdderTops, 0);
    }
  }

  // cout << "Partial guess\n";
  // cout << soln.str(sumProfile);

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
  bool symmetricFlag;
  unsigned char tmin;
  tricks.setByResults(results, cases, tmin, symmetricFlag);

  newTableauFlag = true;
  if (tableauCache.lookup(tricks, solution))
  {
    solution.setMinTricks(tmin);
    newTableauFlag = false;
    return;
  }

  if (tricks.getWeight() == 0)
  {
    // TODO tableau.setTrivial(tmin) or something like that.  
    // Then in Slist, if tableau.trivial()
    //   cout << tableau.whatever();
    return;
  }


  // TODO Maybe only when it looks like it's going to get rough?
  list<unsigned char> tricksSymm, tricksAntisymm;
  Explain explain;
  Covers::partitionResults(results, tricksSymm, tricksAntisymm, explain);
  explain.setTops(numStrategyTops);

  if (! explain.symmetricComponent() && ! explain.asymmetricComponent())
  {
assert(false);
    // TODO Set up the actual strategy, or tableau.setTrivial(tmin)
    // or something like that.  Then in Slist, if tableau.trivial()
    //   cout << tmin
    return;
  }

  if (! explain.asymmetricComponent())
  {
    explain.behave(EXPLAIN_SYMMETRIC);
    Covers::explainByCategory(tricksSymm, explain, false,
      solution, newTableauFlag);
    return;
  }

  if (! explain.symmetricComponent())
  {
    explain.behave(EXPLAIN_ANTI_SYMMETRIC);
    Covers::explainByCategory(tricksAntisymm, explain, false,
      solution, newTableauFlag);
    return;
  }

Covers::guessStart(tricks, tmin, explain);

// TODO Now we should probably subtract out the additions and resymmetrize
// and then check again which halves are in use.

  // Do the symmetric component (keep it in solution).
  explain.behave(EXPLAIN_SYMMETRIC);
  Covers::explainByCategory(tricksSymm, explain, false,
    solution, newTableauFlag);

  // Do the asymmetric component.
  CoverTableau solutionAntisymm;
  explain.behave(EXPLAIN_ANTI_SYMMETRIC);
  Covers::explainByCategory(tricksAntisymm, explain, false,
    solutionAntisymm, newTableauFlag);

  // TODO Only use one solution?
  // I guess the first stack element would get solution as
  // its starting point.  But then the symmetric and anti-symmetric
  // parts could start to merge within rows...
  solution += solutionAntisymm;
}


void Covers::explainManually(
  const list<Result>& results,
  CoverTableau& solution)
{
  // This version uses finished rows.
  Tricks tricks;
  unsigned char tmin;
  bool symmetricFlag;
  tricks.setByResults(results, cases, tmin, symmetricFlag);

  if (tableauRowCache.lookup(tricks, solution))
  {
    solution.setMinTricks(tmin);
    return;
  }

  // TODO For now
  Explain explain;
  if (symmetricFlag)
  {
    explain.setTricks(tmin, 1, 0);
    explain.behave(EXPLAIN_SYMMETRIC);
  }
  else
  {
    explain.setTricks(tmin, 1, 1);
    explain.behave(EXPLAIN_GENERAL);
  }

  explain.setTops(1);

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

