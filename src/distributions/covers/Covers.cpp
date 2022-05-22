/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Covers.h"
#include "CoverStack.h"

#include "tableau/TableauStats.h"

#include "product/ProfilePair.h"

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
void Covers::explainStack(
  const unsigned numStrategyTops,
  const C& candidates,
  const bool recurseFlag,
  const bool greedyFlag,
  CoverStack<T>& stack,
  CoverTableau& solution)
{
  // A solution may already be set, in which case it may be improved.
  // The stack has one or more starting elements already.
  //
  // If greedyFlag is set, we allow all first-level covers that match
  // the trick profile.  If that yields a solution, we are done.
  // If not, then we iteratively replace each stack element with only
  // the most promising continuation.
  //
  // If recurseFlag is set, we stop expanding the stack and study each
  // stack element to its conclusion until we are below the critical
  // stack size again.  The stack may keep expanding for a quite a
  // while even with this approach.

  while (! stack.empty())
  {
    auto handle = stack.extract(stack.begin());
    StackEntry<T>& stackElem = handle.value();

    if (recurseFlag && stack.size() > 10000)
    {
      // Make a new stack with only this element, and recurse in order
      // to cut down on the stack size.
      CoverStack<T> stackNew;
      stackNew.insert(stackElem);
      Covers::explainStack<C, T>(numStrategyTops, candidates,
        false, false, stackNew, solution);
      continue;
    }

    // Otherwise continue more globally.
    CoverTableau& tableau = stackElem.tableau;
    auto candIter = stackElem.iter;

    size_t stackSize0 = stack.size();
    unsigned numSolutions0 = tableauStats.numSolutions;

    while (candIter != candidates.end())
    {
      if (candIter->effectiveDepth() > numStrategyTops)
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
        // Or: If we were looking for a single descendent, we found one.
        if (tableauStats.firstFix == 0)
          tableauStats.firstFix = tableauStats.numSteps;
        break;
      }

      candIter++;
    }

    tableauStats.numBranches += stack.size() - stackSize0;

    if (stack.size() > tableauStats.stackMax)
      tableauStats.stackMax = stack.size();

    if (greedyFlag)
    {
      // This is reasonably "optimal" in the current setup.
      if (stack.size() > 7)
      {
        // Just keep the most promising, first element.
        auto siter = stack.begin();
        for (size_t i = 0; i < 7; i++, siter++);
        stack.erase(siter, stack.end());
      }
    }

    if (tableauStats.numSolutions > numSolutions0)
      stack.prune(solution);

    tableauStats.stackActual = stack.size();

    tableauStats.numSteps++;
  }
}


template void Covers::explainStack<CoverStore, Cover>(
  const unsigned numStrategyTops,
  const CoverStore& candidates,
  const bool recurseFlag,
  const bool greedyFlag,
  CoverStack<Cover>& stack,
  CoverTableau& solution);

template void Covers::explainStack<RowStore, CoverRow>(
  const unsigned numStrategyTops,
  const RowStore& candidates,
  const bool recurseFlag,
  const bool greedyFlag,
  CoverStack<CoverRow>& stack,
  CoverTableau& solution);


template<class C, class T>
void Covers::explainTemplate(
  const Tricks& tricks,
  const unsigned char tmin,
  const unsigned numStrategyTops,
  const C& candidates,
  CoverStack<T>& stack,
  CoverTableau& solution)
{
  stack.emplace(tricks, tmin, candidates.begin());
  tableauStats.reset();

  // Covers::explainStack<C, T>(numStrategyTops, candidates,
    // true, false, stack, solution);

/* */
  while (! stack.empty())
  {
    auto handle = stack.extract(stack.begin());
    StackEntry<T>& stackElem = handle.value();

    CoverTableau& tableau = stackElem.tableau;
    auto candIter = stackElem.iter;

    size_t stackSize0 = stack.size();
    unsigned numSolutions0 = tableauStats.numSolutions;

// size_t tmp = stack.size();
// unsigned tmpSolutions = tableauStats.numSolutions;
    bool branchFlag = false;
    unsigned branchLimit;
    if (stackSize0 < 10000)
      branchLimit = 5;
    else if (stackSize0 < 30000)
      branchLimit = 3;
    else if (stackSize0 < 50000)
      branchLimit = 2;
    else
      branchLimit = 1;

    while (candIter != candidates.end())
    {
      if (candIter->effectiveDepth() > numStrategyTops)
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

      if (candIter != candidates.end() && 
          stack.size() - stackSize0 > branchLimit)
      {
        branchFlag = true;
        break;
      }
    }

    tableauStats.numBranches += stack.size() - stackSize0;

    if (stack.size() > tableauStats.stackMax)
      tableauStats.stackMax = stack.size();

    if (tableauStats.numSolutions > numSolutions0)
      stack.prune(solution);

    tableauStats.stackActual = stack.size();

    tableauStats.numSteps++;

    if (branchFlag)
    {
      stackElem.iter = candIter;
      const unsigned w = stackElem.tableau.getResidualWeight();
      assert(w > 0);

        const unsigned char minCompAdder = 
          candIter->minComplexityAdder(w);
        stackElem.tableau.project(minCompAdder);

        stack.insert(stackElem);
    }
  }
  /* */

  T t;
  cout << tableauStats.strHeader();
  cout << tableauStats.str(t.ID());
}

// Instantiations

template void Covers::explainTemplate<CoverStore, Cover>(
  const Tricks& tricks,
  const unsigned char tmin,
  const unsigned numStrategyTops,
  const CoverStore& candidates,
  CoverStack<Cover>& stack,
  CoverTableau& solution);

template void Covers::explainTemplate<RowStore, CoverRow>(
  const Tricks& tricks,
  const unsigned char tmin,
  const unsigned numStrategyTops,
  const RowStore& candidates,
  CoverStack<CoverRow>& stack,
  CoverTableau& solution);


void Covers::explain(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& solution,
  bool& newTableauFlag)
{
  // This version uses covers and puts them together into rows,
  // including possibly covers that are OR'ed together in a row.
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

  CoverStack<Cover> stack;

  // Get a greedy solution.
timersStrat[25].start();
  stack.emplace(tricks, tmin, coverStore.begin());
  Covers::explainStack<CoverStore, Cover>(
    numStrategyTops, coverStore, false, true, stack, solution);
timersStrat[25].stop();

  /*
  cout << "Greedy solution\n";
  cout << solution.strBracket() << "\n";
  cout << solution.str(sumProfile);
  */

/*
if (solution.complete())
{
  cout << "GOODGREED\n";
}
else
{
  cout << "BADGREED\n";
}
*/

  // Use this to seed the exhaustive search.
  Covers::explainTemplate<CoverStore, Cover>(
    tricks, tmin, numStrategyTops, coverStore, stack, solution);

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

  CoverStack<CoverRow> stack;
  Covers::explainTemplate<RowStore, CoverRow>(tricks, tmin, 1, 
    rowStore, stack, solution);

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

