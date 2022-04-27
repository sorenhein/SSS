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
#include "CoverStacks.h"

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
  tableauRowCache.reset();
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
  // TODO Make < method in CoverRow
  rows.sort([](
    const CoverRow& coverRow1, const CoverRow& coverRow2)
  {
    return (coverRow1.getMCPW() < coverRow2.getMCPW());
  });
}


const Cover& Covers::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.
  return store.lookup(cover);
}


void Covers::explain(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& solution,
  bool& newTableauFlag)
{
  Tricks tricks;
  unsigned char tmin;
  tricks.set(results, tmin);

  newTableauFlag = true;
  if (tableauCache.lookup(tricks, solution))
  {
    solution.setMinTricks(tmin);
    newTableauFlag = false;
    return;
  }

  timersStrat[23].start();

  list<StackEntry> stack;
  Covers::explainTemplate(tricks, tmin, numStrategyTops,
    store, stack, solution);

  timersStrat[23].stop();

  tableauCache.store(tricks, solution);
}


void Covers::explainManually(
  const list<Result>& results,
  CoverTableau& solution)
{
  Tricks tricks;
  unsigned char tmin;
  tricks.set(results, tmin);

  if (tableauRowCache.lookup(tricks, solution))
  {
    solution.setMinTricks(tmin);
    return;
  }

  list<RowStackEntry> stack;
  stack.emplace_back(RowStackEntry());

  RowStackEntry& rentry = stack.back();
  rentry.tableau.init(tricks, tmin, cases);
  rentry.iter = rows.begin();

  auto siter = stack.begin();
timersStrat[24].start();
  while (siter != stack.end())
  {
    auto& stackElem = siter->tableau;

    auto riter = siter->iter;
    while (riter != rows.end())
    {
      const unsigned char headroom = stackElem.headroom(solution);

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
timersStrat[24].stop();

  tableauRowCache.store(tricks, solution);

  // TODO No return value I think
  // return COVER_DONE;
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


string Covers::strCache() const
{
  return tableauCache.str(sumProfile);
}

