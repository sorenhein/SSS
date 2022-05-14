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
#include "product/ProfilePair.h"


Covers::Covers()
{
  Covers::reset();
}


void Covers::reset()
{
  cases.clear();

  store.reset();
  tableauCache.reset();

  rows.reset();
  tableauRowCache.reset();
}


void Covers::fillStore(
  ProductMemory& productMemory,
  const vector<Profile>& distProfiles,
  ProfilePair& running)
{
  // This add() and the next one consume about 70% of the
  // overall time of the loop.
  store.add(productMemory, sumProfile, running, distProfiles, cases);

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

      store.add(productMemory, sumProfile, running, 
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

  store.admixSymmetric();
}


void Covers::addDirectly(list<Cover const *>& coverPtrs)
{
  rows.addDirectly(coverPtrs, cases);
}


const Cover& Covers::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.
  return store.lookup(cover);
}



/////////////////////////////////////////////

// TODO
extern Edata edata;

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

edata.stackActual = 0;
edata.firstFix = 0;
edata.stackMax = 0;
edata.numSteps = 0;
edata.numCompares = 0;
edata.numSolutions = 0;
edata.numBranches = 0;
// cout << edata.strHeader();

  while (! stack.empty())
  {
    auto handle = stack.extract(stack.begin());
    StackEntry<T>& stackElem = handle.value();
// Can probably avoid overwriting in CoverTableau, but for now
// we make a copy
StackEntry<T> stackElemCopy = stackElem;

    CoverTableau& tableau = stackElem.tableau;
    auto candIter = stackElem.iter;

size_t tmp = stack.size();
unsigned tmpSolutions = edata.numSolutions;
bool branchFlag = false;
unsigned branchLimit;
if (tmp < 10000)
  branchLimit = 5;
else if (tmp < 30000)
  branchLimit = 3;
else if (tmp < 50000)
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

      // if (stackIter->tableau.attempt(cases, candIter, stack, solution))
      if (tableau.attempt(cases, candIter, stack, solution))
      {
        // We found a solution.  It may have replaced the previous one.
if (edata.firstFix == 0)
  edata.firstFix = edata.numSteps;
        break;
      }

      candIter++;

      if (candIter != candidates.end() && stack.size() - tmp > branchLimit)
      {
        branchFlag = true;
        break;
      }
    }

edata.numBranches += stack.size() - tmp;

if (stack.size() > edata.stackMax)
  edata.stackMax = stack.size();

if (edata.numSolutions > tmpSolutions)
{
// unsigned cs = stack.size();
  stack.prune(solution);
// cout << "Erased " << cs - stack.size() << " << elements\n";


}

edata.stackActual = stack.size();

edata.numSteps++;
if (edata.numSteps % 100 == 0)
{
  T t;
  // cout << edata.str(t.ID());
}

    if (branchFlag)
    {
      // cout << "Branch\n";
      stackElemCopy.iter = candIter;
      const unsigned w = stackElemCopy.tableau.getResidualWeight();
      if (w == 0)
      {
        assert(false);
      }
      const unsigned char minCompAdder = 
        candIter->minComplexityAdder(w);
      stackElemCopy.tableau.project(minCompAdder);
      stack.insert(stackElemCopy);
    }


  }

/* */
T t;
cout << edata.strHeader();
cout << edata.str(t.ID());
/* */

}

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

/////////////////////////////////////////////




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
  Covers::explainTemplate<CoverStore, Cover>(
    tricks, tmin, numStrategyTops, store, stack, solution);

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
    rows, stack, solution);

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
  ss << setw(6) << store.size() << sumProfile.strLine();
  return ss.str();
}

