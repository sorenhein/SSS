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

#include "product/ProfilePair.h"

#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;


Covers::Covers()
{
  Covers::reset();
}


void Covers::reset()
{
  cases.clear();
  store.reset();
  tableauCache.reset();
  rows.clear();
}


void Covers::prepare(
  ProductMemory& productMemory,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& casesIn,
  const Profile& sumProfileIn)
{
  cases = casesIn;
  sumProfile = sumProfileIn;
  const unsigned char maxTricks = sumProfile.length();

  list<ProfilePair> stack; // Unfinished expansions
  stack.emplace_back(ProfilePair(sumProfile));

  timersStrat[21].start();
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

        // This add() and the next one consume about 70% of the
        // overall time of the loop.
        store.add(productMemory, sumProfile, running, distProfiles, cases);

        unsigned char westLow, westHigh;
        running.getLengthRange(westLow, westHigh);

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
  timersStrat[21].stop();

  timersStrat[22].start();
  store.admixSymmetric();
  timersStrat[22].stop();

  cout << setw(6) << store.size() << sumProfile.strLine();

  // cout << "Cover store\n";
  // cout << store.str();
}


CoverRow& Covers::addRow()
{
  rows.emplace_back(CoverRow());
  return rows.back();
}


void Covers::sortRows()
{
  rows.sort([](
    const CoverRow& coverRow1, const CoverRow& coverRow2)
  {
    return (coverRow1.getMCPW() < coverRow2.getMCPW());
  });
}


void Covers::setup(
  const list<Result>& results,
  Tricks& tricks,
  unsigned char& tricksMin) const
{
  tricks.set(results, tricksMin);
}


const Cover& Covers::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.
  return store.lookup(cover);
}


void Covers::explain(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& tableau,
  bool& newTableauFlag)
{
  Tricks tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  CoverTableau const * tableauPtr = nullptr;
  newTableauFlag = true;
  if (tableauCache.lookup(tricks, tableauPtr))
  {
    // Cache hit, potentially with a different number of minimum tricks.
    tableau = * tableauPtr;
    tableau.setMinTricks(tmin);
    newTableauFlag = false;
    return;
  }

  list<StackEntry> stack;
  stack.emplace_back(StackEntry());

  StackEntry& centry = stack.back();
  centry.tableau.init(tricks, tmin, cases);
  centry.coverIter = store.begin();

  CoverTableau solution;
  // list<CoverTableau> solutions;
  // unsigned char lowestComplexity = numeric_limits<unsigned char>::max();

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    auto& stackElem = siter->tableau;
// cout << "\n--- Stack entry now\n" << stackElem.str(sumProfile);

    // const unsigned char tcomp = siter->tableau.getComplexity();

    auto citer = siter->coverIter;
    while (citer != store.end())
    {
      if (citer->effectiveDepth() > numStrategyTops)
      {
        citer++;
        continue;
      }

      const unsigned char headroom = stackElem.complexityHeadroom(solution);

/*
cout << "  Trying:  " << citer->str(sumProfile) << 
  "; cpx " << +citer->getComplexity() << "\n";
cout << "  Soln     " << solution.strBracket() << "\n";
cout << "  Res wght " << +stackElem.getResidualWeight() << "\n";
cout << "  headroom " << +headroom << "\n";
*/

      if (citer->minComplexityAdder(stackElem.getResidualWeight()) > 
          headroom)
      {
// cout << "    fail project\n";
        // As the covers are ordered, later covers have no chance either.
        break;
      }

      if (citer->getComplexity() > headroom)
      {
// cout << "    fail size\n";
        // The current cover may be too complex, but there may be others.
        citer++;
        continue;
      }

      /*
      if (tcomp + 
          citer->minComplexityAdder(siter->tableau.getResidualWeight()) > 
          lowestComplexity + 1)
        break;
        */

      siter->tableau.attempt(cases, citer, stack, solution);
        // solutions, lowestComplexity);

      citer++;
    }

    // Erasing first stack element.
    siter = stack.erase(siter);
  }

  // assert(! solutions.empty());
  // solutions.sort();

  swap(tableau, solution);

  tableauCache.store(tricks, tableau);
}


CoverState Covers::explainManually(
  const list<Result>& results,
  CoverTableau& solution) const
{
  Tricks tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  list<RowStackEntry> stack;
  stack.emplace_back(RowStackEntry());

  RowStackEntry& rentry = stack.back();
  rentry.tableau.init(tricks, tmin, cases);
  rentry.rowIter = rows.begin();

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    auto& stackElem = siter->tableau;

    auto riter = siter->rowIter;
    while (riter != rows.end())
    {
      const unsigned char headroom = stackElem.complexityHeadroom(solution);

      if (riter->minComplexityAdder(stackElem.getResidualWeight()) > 
          headroom)
      {
        // As the rows are ordered, later rows have no chance either.
        break;
      }

      if (riter->getComplexity() > headroom)
      {
        // The current row may be too complex, but there may be others.
        riter++;
        continue;
      }

      if (stackElem.attemptManually(cases, riter, stack, solution))
      {
        // We found a solution.  It may have replaced the previous one.
        break;
      }

      riter++;
    }

    // Erasing first stack element.
    siter = stack.erase(siter);
  }

  return COVER_DONE;
}


void Covers::storeTableau(
  const Tricks& excessTricks,
  const CoverTableau& tableau)
{
  tableauCache.store(excessTricks, tableau);
}


bool Covers::lookupTableau(
  const Tricks& excessTricks,
  CoverTableau const * tableauPtr)
{
  return tableauCache.lookup(excessTricks, tableauPtr);
}


const Profile& Covers::getSumProfile() const
{
  return sumProfile;
}


void Covers::getCoverCounts(
  unsigned& numTableaux,
  unsigned& numUses) const
{
  tableauCache.getCounts(numTableaux, numUses);
}


string Covers::strCached() const
{
  return tableauCache.str(sumProfile);
}

