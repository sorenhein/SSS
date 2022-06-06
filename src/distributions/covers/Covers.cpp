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
  // const unsigned char tmin,
  const Explain& explain,
  // const unsigned numStrategyTops,
  const C& candidates,
  const size_t pruneTrigger,
  const size_t pruneSize,
  CoverStack<T>& stack,
  CoverTableau& solution)
{
  stack.emplace(tricks, explain.tricksMin(), candidates.begin());
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
      if (explain.skip(candIter->effectiveDepth(), candIter->symmetric()))
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
  const size_t pruneTrigger,
  const size_t pruneSize,
  CoverStack<Cover>& stack,
  CoverTableau& solution);

template void Covers::explainTemplate<RowStore, CoverRow>(
  const Tricks& tricks,
  const Explain& explain,
  const RowStore& candidates,
  const size_t pruneTrigger,
  const size_t pruneSize,
  CoverStack<CoverRow>& stack,
  CoverTableau& solution);


void Covers::partitionResults(
  const list<Result>& results,
  list<unsigned char>& tricksSymm,
  list<unsigned char>& tricksAsymm,
  Explain& explain) const
{
  unsigned char tmin = numeric_limits<unsigned char>::max();
  for (auto& res: results)
    tmin = min(tmin, res.getTricks());

  unsigned weightSymm = 0;
  unsigned weightAsymm = 0;

  tricksSymm.resize(results.size());
  tricksAsymm.resize(results.size());

  auto riterf = results.begin();
  auto riterb = prev(results.end());
  auto titerSymm = tricksSymm.begin();
  auto titerAsymm = tricksAsymm.begin();

  while (riterf != results.end())
  {
    const unsigned char f = riterf->getTricks() - tmin;
    const unsigned char b = riterb->getTricks() - tmin;
    const unsigned char mint = min(f, b);

    * titerSymm = mint;
    * titerAsymm = f - mint;

    weightSymm += mint;
    weightAsymm += f - mint;
    
    riterf++;
    riterb--;
    titerSymm++;
    titerAsymm++;
  }

  explain.setTricks(tmin, weightSymm, weightAsymm);
}


void Covers::explain(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& solution,
  bool& newTableauFlag)
{
  // This version uses covers and puts them together into rows,
  // including possibly covers that are OR'ed together in a row.

  // TODO Maybe only when it looks like it's going to get rough?
  list<unsigned char> tricksSymm, tricksAsymm;
  Explain explain;
  Covers::partitionResults(results, tricksSymm, tricksAsymm, explain);
  explain.setTops(numStrategyTops);

  if (! explain.symmetricComponent() && ! explain.asymmetricComponent())
  {
    // TODO Set up the actual strategy, or tableau.setTrivial(tmin)
    // or something like that.  Then in Slist, if tableau.trivial()
    //   cout << tmin
    return;
  }

  if (! explain.asymmetricComponent())
  {
    Tricks tricks;
    tricks.setByList(tricksSymm, cases);

    newTableauFlag = true;
    if (tableauCache.lookup(tricks, solution))
    {
      solution.setMinTricks(explain.tricksMin());
      newTableauFlag = false;
      return;
    }

    explain.behaveSymmetrically();
    CoverStack<Cover> stack;

    // Get a greedy solution.
    // TODO Test again later whether or not this helps on average.
timersStrat[25].start();
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, 7, 7, stack, solution);
timersStrat[25].stop();

    // Use this to seed the exhaustive search.
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, 50000, 25000, stack, solution);

    tableauCache.store(tricks, solution);
  }
  else if (! explain.symmetricComponent())
  {
    Tricks tricks;
    tricks.setByList(tricksAsymm, cases);

    newTableauFlag = true;
    if (tableauCache.lookup(tricks, solution))
    {
      solution.setMinTricks(explain.tricksMin());
      newTableauFlag = false;
      return;
    }

    explain.behaveAntiSymmetrically();
    CoverStack<Cover> stack;

    // Get a greedy solution.
    // TODO Test again later whether or not this helps on average.
timersStrat[25].start();
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, 7, 7, stack, solution);
timersStrat[25].stop();

    // Use this to seed the exhaustive search.
    Covers::explainTemplate<CoverStore, Cover>(
      tricks, explain, coverStore, 50000, 25000, stack, solution);

    tableauCache.store(tricks, solution);
  }
  else
  {
    // TODO ?
    // First test the complete cache.
    Tricks tricks;
    bool symmetricFlag;
    unsigned char tmin;
    tricks.setByResults(results, cases, tmin, symmetricFlag);

    newTableauFlag = true;
    if (tableauCache.lookup(tricks, solution))
    {
      solution.setMinTricks(explain.tricksMin());
      newTableauFlag = false;
      return;
    }

    // Do the symmetric component (keep it in solution).
    newTableauFlag = true;
    tricks.setByList(tricksSymm, cases);

    if (tableauCache.lookup(tricks, solution))
    {
      solution.setMinTricks(explain.tricksMin());
      newTableauFlag = false;
    }
    else
    {
      explain.behaveSymmetrically();
      CoverStack<Cover> stack;

      // Get a greedy solution.
      // TODO Test again later whether or not this helps on average.
timersStrat[25].start();
      Covers::explainTemplate<CoverStore, Cover>(
        tricks, explain, coverStore, 7, 7, stack, solution);
timersStrat[25].stop();

      // Use this to seed the exhaustive search.
      Covers::explainTemplate<CoverStore, Cover>(
        tricks, explain, coverStore, 50000, 25000, stack, solution);

      tableauCache.store(tricks, solution);
    }

    // Do the asymmetric component.
    CoverTableau solutionAsymm;
    tricks.setByList(tricksAsymm, cases);

    if (tableauCache.lookup(tricks, solutionAsymm))
    {
      solutionAsymm.setMinTricks(explain.tricksMin());
      newTableauFlag = false;
    }
    else
    {
      explain.behaveAntiSymmetrically();
      CoverStack<Cover> stack;

      // Get a greedy solution.
      // TODO Test again later whether or not this helps on average.
timersStrat[25].start();
      Covers::explainTemplate<CoverStore, Cover>(
        tricks, explain, coverStore, 7, 7, stack, solutionAsymm);
timersStrat[25].stop();

      // Use this to seed the exhaustive search.
      Covers::explainTemplate<CoverStore, Cover>(
        tricks, explain, coverStore, 50000, 25000, stack, solutionAsymm);

      tableauCache.store(tricks, solutionAsymm);
    }

    // TODO Only use one solution?
    solution += solutionAsymm;
  }

  /*
  Tricks tricks;
  bool symmetricFlag;
  // TODO Make a simpler one with no tmin and no symmetricFlag
  unsigned char tmin;
  tricks.setByResults(results, cases, tmin, symmetricFlag);

  newTableauFlag = true;
  if (tableauCache.lookup(tricks, solution))
  {
    solution.setMinTricks(explain.tricksMin());
    newTableauFlag = false;
    return;
  }

  if (symmetricFlag)
    explain.behaveSymmetrically();
  else if (! explain.symmetricComponent())
    explain.behaveAsymmetrically();
  else
    explain.behaveGenerally();

  CoverStack<Cover> stack;

  // Get a greedy solution.
  // TODO Test again later whether or not this helps on average.
timersStrat[25].start();
  Covers::explainTemplate<CoverStore, Cover>(
    tricks, explain, coverStore, 7, 7, stack, solution);
timersStrat[25].stop();

  // cout << "Greedy solution\n";
  // cout << solution.strBracket() << "\n";
  // cout << solution.str(sumProfile);
  // cout << (solution.complete() ? "GOODGREED\n" : "BADGREED\n");

  // Use this to seed the exhaustive search.
  Covers::explainTemplate<CoverStore, Cover>(
    tricks, explain, coverStore, 50000, 25000, stack, solution);

  tableauCache.store(tricks, solution);
  */
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
    explain.behaveSymmetrically();
  }
  else
  {
    explain.setTricks(tmin, 1, 1);
    explain.behaveGenerally();
  }

  explain.setTops(1);

  CoverStack<CoverRow> stack;
  Covers::explainTemplate<RowStore, CoverRow>(tricks,
    explain, rowStore, 50000, 25000, stack, solution);

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

