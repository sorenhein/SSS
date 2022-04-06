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

#include "CoverMemory.h"
#include "ProductMemory.h"
#include "CoverTableau.h"
#include "ResExpl.h"
#include "Tricks.h"

#include "../../strategies/result/Result.h"

#include "../../const.h"

#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;

// TODO Find a more elegant way
#define COVER_CHUNK_SIZE 40000


Covers::Covers()
{
  Covers::reset();
}


void Covers::reset()
{
  rowsOld.clear();
  covers.clear();
  tableauCache.reset();
}


void Covers::prepareRow(
  const CoverSpec& coverSpec,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  assert(distProfiles.size() == cases.size());

  rowsOld.emplace_back(CoverRowOld());
  CoverRowOld& coverRowOld = rowsOld.back();
  coverRowOld.prepare(distProfiles, cases, coverSpec);

  assert(coverRowOld.getWeight() != 0);
}


void Covers::sortRows()
{
  rowsOld.sort([](
    const CoverRowOld& coverRow1, const CoverRowOld& coverRow2)
  {
    return (coverRow1.getWeight() >= coverRow2.getWeight());
  });
}


/*
void Covers::prepare(
  const CoverMemory& coverMemory,
  const unsigned char maxLengthIn,
  const unsigned char maxTops,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  assert(distProfiles.size() == cases.size());
  assert(maxLengthIn >= 2);
  assert(maxTops >= 1);

  rowsOld.resize(coverMemory.size(maxLengthIn, maxTops));
  auto riter = rowsOld.begin();

  for (auto miter = coverMemory.begin(maxLengthIn, maxTops);
      miter != coverMemory.end(maxLengthIn, maxTops); miter++)
  {
    assert(riter != rowsOld.end());
    riter->prepare(distProfiles, cases, * miter);

    assert(riter->getWeight() != 0);
    riter++;
  }

  rowsOld.sort([](
    const CoverRowOld& coverRow1, const CoverRowOld& coverRow2)
  {
    return (coverRow1.getWeight() >= coverRow2.getWeight());
  });
}
*/


void Covers::prune()
{
  for (auto citer = covers.begin(); citer != covers.end(); )
  {
    if (citer->empty() || citer->full())
    {
      citer = covers.erase(citer);
      continue;
    }

    for (auto citer2 = next(citer); citer2 != covers.end(); )
    {
      if (! citer2->sameWeight(* citer))
        break;
      else if (citer2->sameTricks(* citer))
        citer2 = covers.erase(citer2);
      else
        citer2++;
    }
    citer++;
  }
}


void Covers::prepareNew(
  ProductMemory& productMemory,
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases,
  const Profile& sumProfileIn)
{
  sumProfile = sumProfileIn;

  timersStrat[20].start();
  list<ProfilePair> stack; // Unfinished expansions
  stack.emplace_back(ProfilePair(sumProfile));

  covers.resize(COVER_CHUNK_SIZE);

  auto citer = covers.begin(); // Next one to write
  timersStrat[20].stop();

  RunningBounds bounds;
  bounds.reset(sumProfile.getLength());

  timersStrat[21].start();
  while (! stack.empty())
  {
    auto stackIter = stack.begin();

    unsigned char topNumber = stackIter->getNextTopNo(); // Next to write
    if (topNumber >= sumProfile.size())
    {
      stack.pop_front();
      continue;
    }

    const unsigned char topCountActual = sumProfile.count(topNumber);

    for (unsigned char topCountLow = 0; 
        topCountLow <= topCountActual; topCountLow++)
    {
      for (unsigned char topCountHigh = topCountLow; 
        topCountHigh <= topCountActual; topCountHigh++)
      {
        // Never use the lowest top explicitly.  Maybe it shouldn't
        // be there at all, but it is.
        if (topNumber == 0 &&
            (topCountLow != 0 || topCountHigh != topCountActual))
          continue;

        bounds.step(
          stackIter->getBounds(),
          topCountActual,
          topCountLow,
          topCountHigh);

        if (bounds.busted())
          continue;

        assert(citer != covers.end());

        // Add the "don't care" with respect to length.
        stackIter->setLength(0, sumProfile.getLength()); // ?
        stackIter->addTop(topNumber, topCountLow, topCountHigh);

        citer->set(productMemory, sumProfile, * stackIter, false);
        citer++;

        const unsigned char westLow = bounds.lengthWestLow();
        const unsigned char westHigh = bounds.lengthWestHigh();

        for (unsigned char lLow = westLow; lLow <= westHigh; lLow++)
        {
          for (unsigned char lHigh = lLow; lHigh <= westHigh; lHigh++)
          { 
            if (lLow == westLow && lHigh == westHigh)
            {
              // No point in specifying length explicitly.
              continue;
            }

            // There is a tighter way to specify this cover.
            if (topNumber > 0 && bounds.unnecessaryLength(lLow, lHigh))
              continue;

            assert(citer != covers.end());

            stackIter->setLength(lLow, lHigh);

            citer->set(productMemory, sumProfile, * stackIter, false);
            citer++;
          }
        }

        stackIter = stack.insert(stackIter, * stackIter);
        auto nextIter = next(stackIter);
        nextIter->setBounds(bounds);
        nextIter->incrNextTopNo();
      }
    }
    assert(! stack.empty());
    stack.pop_front();
  }
  timersStrat[21].stop();

  timersStrat[22].start();
const unsigned sizeOld = covers.size();

  covers.erase(citer, covers.end());
  assert(! covers.empty());
  timersStrat[22].stop();

  cout << "Length " << sumProfile.strLine() << "\n";

  timersStrat[23].start();
  for (auto& c: covers)
    c.prepare(distProfiles, cases);
  timersStrat[23].stop();

  timersStrat[24].start();
  covers.sort([](const Cover& cover1, const Cover& cover2)
  {
    return (cover1 < cover2);
    // return cover1.earlier(cover2);
  });
  timersStrat[24].stop();

  /*
  cout << "Covers before pruning\n";
  cout << covers.front().strHeader();
  for (auto& c: covers)
    cout << c.strLine(sumProfile);
  cout << "\n";
  */

  // It is not practical to generate the covers without duplicated
  // trick vectors in one pass.  So we eliminate the more complex
  // ways of saying the same thing.  In total across all covers,
  // go from 354,822 to 225,028, so we need to eliminate about a third.

const unsigned sizeMid = covers.size();
// cout << "sizeMid " << +sizeMid << endl;
  timersStrat[25].start();
  Covers::prune();
  timersStrat[25].stop();
  cout << "Used " << sizeOld << " -> " << sizeMid << " -> " <<
    covers.size() << "\n";

  /*
  cout << "Covers\n";
  cout << covers.front().strHeader();
  for (auto& c: covers)
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
  resExpl.setMinimum(tmin);

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
      resExpl.insert(* iter);
      continue;
    }
    else
    {
      // cout << iter->str() << "\n";
      // cout << iter->strProfile() << "\n";
      // cout << Covers::strDebug("Could not use", tricks);
      iter++;
    }
  }

  // Can't happen
  return COVER_STATE_SIZE;
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

  auto citer = covers.begin();
  while (citer != covers.end())
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

  stableau.coverIter = covers.begin();
  stableau.coverNumber = 0;

const unsigned coverSize = covers.size();
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
    while (citer != covers.end())
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

