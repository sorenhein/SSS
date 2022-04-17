/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Covers.h"

#include "ProductMemory.h"
#include "CoverTableau.h"
#include "ResExpl.h"
#include "Tricks.h"

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
  rowsOld.clear();
  store.reset();
  tableauCache.reset();
}


CoverRowOld& Covers::addRow()
{
  rowsOld.emplace_back(CoverRowOld());
  return rowsOld.back();
}


void Covers::sortRows()
{
  rowsOld.sort([](
    const CoverRowOld& coverRow1, const CoverRowOld& coverRow2)
  {
    return (coverRow1.getWeight() >= coverRow2.getWeight());
  });
}


void Covers::prepareNew(
  ProductMemory& productMemory,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  const Profile& sumProfileIn)
{
  sumProfile = sumProfileIn;
  const unsigned char maxTricks = sumProfile.getLength();

  timersStrat[20].start();
  list<ProfilePair> stack; // Unfinished expansions
  stack.emplace_back();
  stack.back().init(sumProfile);

  timersStrat[20].stop();

  timersStrat[21].start();
  while (! stack.empty())
  {
    ProfilePair& running = stack.front();

    unsigned char topNumber = running.getNextTopNo(); // Next to write

    const unsigned char topLength = sumProfile.count(topNumber);

    for (unsigned char topLow = 0; topLow <= topLength; topLow++)
    {
      for (unsigned char topHigh = topLow; topHigh <= topLength; topHigh++)
      {
        // Never use the lowest top explicitly.  Maybe it shouldn't
        // be there at all, but it is.
        if (topNumber == 0 && (topLow != 0 || topHigh != topLength))
          continue;

        // Add or restore the "don't care" with respect to length.
        running.setLength(0, sumProfile.getLength());
        running.addTop(topNumber, topLow, topHigh);

        // An unused top was already seen.
        if (topNumber > 0 && topLow == 0 && topHigh == topLength)
        {
          if (! running.last())
          {
            stack.push_back(running);
            stack.back().incrNextTopNo();
          }
          continue;
        }

        store.add(productMemory, sumProfile, running, false,
          distProfiles, cases);

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

            store.add(productMemory, sumProfile, running, false,
              distProfiles, cases);
          }
        }

        if (! running.last())
        {
          stack.push_back(running);
          stack.back().incrNextTopNo();
        }
      }
    }
    assert(! stack.empty());
    stack.pop_front();
  }
  timersStrat[21].stop();

  cout << setw(6) << store.size() << sumProfile.strLine();

  /*
  cout << "Covers\n";
  cout << store.begin()->strHeader();
  for (auto& c: store)
    cout << c.strLine(sumProfile);
  cout << "\n";
  */
}


void Covers::setup(
  const list<Result>& results,
  Tricks& tricks,
  unsigned char& tricksMin) const
{
  tricks.set(results, tricksMin);
}


CoverState Covers::explain(
  const list<Result>& results,
  ResExpl& resExpl) const
{
  CoverState state = COVER_OPEN;
  auto iter = rowsOld.begin();

  Tricks tricks;

  unsigned char tmin;
  Covers::setup(results, tricks, tmin);
  resExpl.setParameters(
    tmin,
    sumProfile.getLength(),
    sumProfile.count(static_cast<unsigned char>(sumProfile.size()-1)));

  while (true)
  {
    if (iter == rowsOld.end())
    {
      cout << Covers::strDebug("Left with", tricks);
      return COVER_IMPOSSIBLE;
    }

    state = iter->explain(tricks);

    if (state == COVER_DONE)
    {
      resExpl.insert(* iter);
      return COVER_DONE;
    }
    else if (state == COVER_OPEN)
    {
      // cout << "Inserting\n" << iter->str() << "\n";
      // cout << iter->strProfile() << "\n";
      // cout << Covers::strDebug("Inserted", tricks);
      resExpl.insert(* iter);
      continue;
    }
    else
    {
      // cout << "Not using\n" << iter->str() << "\n";
      // cout << iter->strProfile() << "\n";
      // cout << Covers::strDebug("Could not use", tricks);
      iter++;
    }
  }

  // Can't happen
  return COVER_STATE_SIZE;
}


const Cover& Covers::lookup(const Cover& cover) const
{
  // Turn a cover into the one we already know.  It must exist.
  return store.lookup(cover);
}


void Covers::explainGreedy(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& tableau) const
{
  Tricks tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  // tableau.setBoundaries(maxLength, topTotals);
  tableau.setBoundaries(sumProfile);
  tableau.setTricks(tricks, tmin);

  auto citer = store.begin();
  while (citer != store.end())
  {
/*
cout << citer->strHeader();
cout << citer->strLine();
cout << citer->strProfile();
cout << citer->strTricksShort();
cout << "Top size " << +citer->getTopSize() << endl << endl;
*/

    if (citer->getTopSize() > numStrategyTops)
    {
// cout << "Too detailed\n";
      // A cover should not use distributions more granularly than
      // the strategy itself does.
      citer++;
      continue;
    }
      
    if (! tableau.attemptGreedy(* citer))
    {
// cout << "No match\n";
      citer++;
      continue;
    }

/*
cout << "Tableau now:\n";
cout << tableau.str();
cout << "Residuals now:\n";
cout << tableau.strResiduals();
*/

    if (tableau.complete())
      return;
  }
// cout << "Ran out\n";
}


void Covers::explainExhaustive(
  const list<Result>& results,
  const unsigned numStrategyTops,
  CoverTableau& tableau)
{
  Tricks tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  CoverTableau const * tableauPtr = nullptr;
  // cout << "CACHE when looking up: " << tableauCache.size() << endl;
  // if (tableauCache.size() > 0)
  // {
    // cout << "HERE\n";
  // }
  if (tableauCache.lookup(tricks, tableauPtr))
  {
    // cout << "CACHEHIT" << endl;
    // assert(tableauPtr != nullptr);
    tableau = * tableauPtr;
    tableau.setMinTricks(tmin);
    // cout << "DEREFFED" << endl;
    return;
  }

  list<StackTableau> stack;
  stack.emplace_back(StackTableau());
  StackTableau& stableau = stack.back();

  stableau.tableau.setBoundaries(sumProfile);
  stableau.tableau.setTricks(tricks, tmin);

  stableau.coverIter = store.begin();
  stableau.coverNumber = 0;

const unsigned coverSize = store.size();
unsigned coverNo;

  list<CoverTableau> solutions;
  unsigned char lowestComplexity = numeric_limits<unsigned char>::max();

  /*
  cout << 
    setw(6) << "Stack" <<
    setw(6) << "RunCx" <<
    setw(6) << "Resid" <<
    setw(6) << "BestC" <<
    setw(6) << "Cno" <<
    setw(6) << "CCplx" <<
    setw(6) << "Cwgt" <<
    setw(6) << "Proj" <<
    "\n";
    */

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    auto citer = siter->coverIter;
    coverNo = siter->coverNumber;
const unsigned char comp = (solutions.empty() ? 0 : lowestComplexity);

    // The lowest complexity that is still achievable is
    // Tableau complexity + round up(residual / cover weight).
    unsigned char minCovers = 
      1 + (siter->tableau.getResidual() / citer->getNumDist());

    const unsigned char tcomp = siter->tableau.getComplexity();
    const unsigned char projected = tcomp + minCovers;

/*
cout << 
  setw(6) << stack.size() <<
  setw(6) << +siter->tableau.getComplexity() <<
  setw(6) << +siter->tableau.getResidual() <<
  setw(6) << +comp <<
  setw(6) << coverNo <<
  setw(6) << +citer->getComplexity() <<
  setw(6) << +citer->getNumDist() <<
  setw(6) << +projected <<
  endl;
  */


    if (solutions.empty() || projected <= lowestComplexity + 1)
    {
    while (citer != store.end())
    {
      if (citer->getTopSize() > numStrategyTops)
      {
        citer++;
coverNo++;
        continue;
      }

/*
cout << "Attempting:\n";
cout << citer->strHeader();
cout << citer->strLine();
// cout << citer->strProfile();
// cout << citer->strTricksShort() << "\n";
// cout << "Top size " << +citer->getTopSize() << endl << endl;
*/
// unsigned s0 = solutions.size();
// unsigned st0 = stack.size();

      // TODO Could test projected again here

      siter->tableau.attemptExhaustive(citer, coverNo, stack, 
        solutions, lowestComplexity);

// unsigned s1 = solutions.size();
// unsigned st1 = stack.size();
// cout << "solutions: " << s0 << " -> " << s1 << endl;
// cout << "stack    : " << st0 << " -> " << st1 << endl;
      citer++;
coverNo++;
    }
    }
    // else
      // cout << "SKIP\n";

    siter = stack.erase(siter);
// cout << "erasing first stack element\n";
/*
unsigned i = 0;
for (auto& t: stack)
{
  cout << "Stack element " << i << endl;
  cout << t.tableau.str();
  cout << t.tableau.strResiduals();
  i++;
}
*/
  }

// cout << "DONE WITH SOLUTIONS\n";

  assert(! solutions.empty());
  solutions.sort();

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

  // TODO Could perhaps swap
  tableau = solutions.front();

  tableauCache.store(tricks, tableau);
  // cout << "CACHE NOW\n";
  // cout << tableauCache.str();

  // TODO Maybe MECE and hierarchy again within the tableau.
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

