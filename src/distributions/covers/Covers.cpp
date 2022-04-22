/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverTableau.h"
#include "Covers.h"

#include "Tricks.h"

#include "product/ProductMemory.h"
#include "product/ProfilePair.h"

// #include "ResExpl.h"

#include "../../strategies/result/Result.h"

#include "../../const.h"

#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;


Covers::Covers()
{
  Covers::reset();
}


void Covers::reset()
{
  rows.clear();
  store.reset();
  tableauCache.reset();
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
    return (coverRow1.getWeight() >= coverRow2.getWeight());
  });
}


void Covers::prepareNew(
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


/*
void Covers::explainGreedy(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& tableau) const
{
  Tricks tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  tableau.setBoundaries(sumProfile);
  tableau.setTricks(tricks, tmin, cases);

  auto coverIter = store.begin();
  while (coverIter != store.end())
  {
    if (coverIter->effectiveDepth() > numStrategyTops)
    {
      // A cover should not use distributions more granularly than
      // the strategy itself does.
      coverIter++;
      continue;
    }
      
    if (! tableau.attemptGreedy(* coverIter, cases))
    {
      coverIter++;
      continue;
    }

    if (tableau.complete())
      return;
  }
}
*/


CoverState Covers::explainExhaustiveRows(
  const list<Result>& results,
  CoverTableau& tableau) const
{
  Tricks tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  list<ResTableau> stack;
  stack.emplace_back(ResTableau());
  ResTableau& stableau = stack.back();

  stableau.tableau.setBoundaries(sumProfile);
  stableau.tableau.setTricks(tricks, tmin, cases);

  stableau.rowIter = rows.begin();

const unsigned coverSize = rows.size();

  list<CoverTableau> solutions;
  unsigned char lowestComplexity = numeric_limits<unsigned char>::max();

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    auto riter = siter->rowIter;
const unsigned char comp = (solutions.empty() ? 0 : lowestComplexity);

    // The lowest complexity that is still achievable is
    // Tableau complexity + round up(residual / cover weight).
    unsigned char minCovers = 
      static_cast<unsigned char>(
        (siter->tableau.getResidualWeight() + riter->getWeight()-1) /
        riter->getWeight());

    const unsigned char tcomp = siter->tableau.getComplexity();
    // The minimum complexity of anything is 2.
    const unsigned char projected = tcomp + 2*minCovers;

    if (solutions.empty() || projected <= lowestComplexity + 1)
    {
      while (riter != rows.end())
      {
        siter->tableau.attemptExhaustiveRow(cases, riter, stack, 
          solutions, lowestComplexity);

        riter++;
      }
    }

    // Erasing first stack element.
    siter = stack.erase(siter);
  }


  assert(! solutions.empty());
  solutions.sort();

// Make partially into a CoverTableau method
/*
unsigned i = 0;
for (auto s: solutions)
{
  cout << "Solution " << i << 
    ", complexity " << + s.getComplexity() << 
    ", overlap " << + s.getOverlap() << 
    "\n";
  cout << s.str();
  i++;
  if (i >= 20)
    break;
}
*/

  tableau = solutions.front();

  return COVER_DONE;
}


void Covers::explainExhaustive(
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
    // Cache hit.
    tableau = * tableauPtr;
    tableau.setMinTricks(tmin);
    newTableauFlag = false;
    return;
  }

  list<StackTableau> stack;
  stack.emplace_back(StackTableau());
  StackTableau& stableau = stack.back();

  stableau.tableau.setBoundaries(sumProfile);
  stableau.tableau.setTricks(tricks, tmin, cases);

  stableau.coverIter = store.begin();

  list<CoverTableau> solutions;
  unsigned char lowestComplexity = numeric_limits<unsigned char>::max();

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    auto citer = siter->coverIter;
const unsigned char comp = (solutions.empty() ? 0 : lowestComplexity);

    // The lowest complexity that is still achievable is
    // Tableau complexity + round up(residual / cover weight).
    unsigned char minCovers = 
      static_cast<unsigned char>(
        (siter->tableau.getResidualWeight() + citer->getWeight()-1) /
        citer->getWeight());

    const unsigned char tcomp = siter->tableau.getComplexity();
    // The minimum complexity of anything is 2.
    const unsigned char projected = tcomp + 2*minCovers;
/*
cout << "coverNo " << +coverNo << ", stack " << stack.size() << 
  ", solns " << solutions.size() <<
  ", minCovers " << +minCovers <<
  ", proj " << +projected <<
  ", low " << +lowestComplexity <<
endl;
*/

    if (solutions.empty() || projected <= lowestComplexity + 1)
    {
      while (citer != store.end())
      {
        if (citer->effectiveDepth() > numStrategyTops)
        {
          citer++;
          continue;
        }

        // TODO Could test projected again here

        siter->tableau.attemptExhaustive(cases, citer, stack, 
          solutions, lowestComplexity);

        citer++;
      }
    }
    // else
      // cout << "SKIP\n";

    // Erasing first stack element.
    siter = stack.erase(siter);
  }


  assert(! solutions.empty());
  solutions.sort();

// Make partially into a CoverTableau method
/*
cout << "SOLUTIONS\n\n";
unsigned i = 0;
for (auto s: solutions)
{
  cout << "Solution " << i << 
    ", complexity " << + s.getComplexity() << 
    ", overlap " << + s.getOverlap() << 
    "\n";
  cout << s.str();
  i++;
  if (i >= 20)
    break;
}
*/

/*
cout << "COVERS\n\n";
i = 0;
for (auto& c: store)
{
  cout << "Cover " << i << 
    ", complexity " << + c.getComplexity() << 
    "\n";
  cout << c.strLine();
  i++;
}
*/

  // TODO Could perhaps swap
  tableau = solutions.front();

  tableauCache.store(tricks, tableau);
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


string Covers::strDebug(
  const string& title,
  const Tricks& tricks) const
{
  stringstream ss;
  ss << title << "\n";
  ss << tricks.strList();
  return ss.str();
}


string Covers::strCached() const
{
  return tableauCache.str();
}


/*
string Covers::strExpl(const ResExpl& resExpl) const
{
  return resExpl.str(sumProfile);
}
*/
