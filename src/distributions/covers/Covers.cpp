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
    // return (coverRow1.getWeight() >= coverRow2.getWeight());
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

  centry.tableau.init(sumProfile, tricks, tmin, cases);
  centry.coverIter = store.begin();

  list<CoverTableau> solutions;
  unsigned char lowestComplexity = numeric_limits<unsigned char>::max();

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    const unsigned char tcomp = siter->tableau.getComplexity();

    auto citer = siter->coverIter;
    while (citer != store.end())
    {
      if (citer->effectiveDepth() > numStrategyTops)
      {
        citer++;
        continue;
      }

      // The covers are ordered by increasing "complexity per weight"
      // (micro-cpw).  We round up the minimum number of covers needed
      // unless we hit an exact divisor.
      // TODO Maybe put the >> 20 stuff in Cover -- make a method.
      const unsigned char restWeight = siter->tableau.getResidualWeight();
      const unsigned char minComplexityAdder = 
        static_cast<unsigned char>(
        max(static_cast<unsigned>(citer->getComplexity()),
        1 + (((restWeight * citer->getMCPW() - 1) >> 20))));

      if (tcomp + minComplexityAdder > lowestComplexity + 1)
        break;

      siter->tableau.attempt(cases, citer, stack, 
        solutions, lowestComplexity);

      citer++;
    }

    // Erasing first stack element.
    siter = stack.erase(siter);
  }

  assert(! solutions.empty());
  solutions.sort();

  swap(tableau, solutions.front());

  tableauCache.store(tricks, tableau);

/*
cout << "SOLUTIONS\n\n";
unsigned i = 0;
for (auto s: solutions)
{
  cout << "Solution " << i << ", complex " << + s.getComplexity() << "\n";
  cout << s.str();
  if (++i >= 10)
    break;
}
*/
}


CoverState Covers::explainManually(
  const list<Result>& results,
  CoverTableau& tableau) const
{
  Tricks tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  list<RowStackEntry> stack;
  stack.emplace_back(RowStackEntry());
  RowStackEntry& rentry = stack.back();

  rentry.tableau.init(sumProfile, tricks, tmin, cases);
  rentry.rowIter = rows.begin();

  list<CoverTableau> solutions;
  unsigned char lowestComplexity = numeric_limits<unsigned char>::max();

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    const unsigned char tcomp = siter->tableau.getComplexity();

    auto riter = siter->rowIter;
    while (riter != rows.end())
    {
      // The rows are ordered by increasing "complexity per weight"
      // (micro-cpw).  We round up the minimum number of rows needed
      // unless we hit an exact divisor.
      // TODO Maybe put the >> 20 stuff in Cover -- make a method.
      /* */
      const unsigned char restWeight = siter->tableau.getResidualWeight();
      const unsigned char minComplexityAdder = 
        static_cast<unsigned char>(
        max(static_cast<unsigned>(riter->getComplexity()),
        1 + (((restWeight * riter->getMCPW() - 1) >> 20))));

      if (tcomp + minComplexityAdder > lowestComplexity + 1)
        break;

      siter->tableau.attemptManually(cases, riter, stack, 
        solutions, lowestComplexity);

      riter++;
    }

    // Erasing first stack element.
    siter = stack.erase(siter);
  }

  assert(! solutions.empty());
  solutions.sort();

  swap(tableau, solutions.front());

  return COVER_DONE;

/*
unsigned i = 0;
for (auto s: solutions)
{
  cout << "Solution " << i << ", compl " << + s.getComplexity() << "\n";
  cout << s.str();
  if (++i >= 20)
    break;
}
*/
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


void Covers::getCoverCounts(
  unsigned& numTableaux,
  unsigned& numUses) const
{
  tableauCache.getCounts(numTableaux, numUses);
}


string Covers::strCached() const
{
  return tableauCache.str();
}

