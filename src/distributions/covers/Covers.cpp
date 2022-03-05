/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "ResExpl.h"
#include "Covers.h"
#include "CoverMemory.h"
#include "CoverTableau.h"

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
  covers.clear();
}


void Covers::prepare(
  const CoverMemory& coverMemory,
  const unsigned char maxLength,
  const unsigned char maxTops,
  const vector<unsigned char>& lengths,
  const vector<unsigned char>& tops,
  const vector<unsigned char>& cases)
{
  assert(lengths.size() == tops.size());
  assert(lengths.size() == cases.size());
  assert(maxLength >= 2);
  assert(maxTops >= 1);

  covers.resize(coverMemory.size(maxLength, maxTops));
  auto citer = covers.begin();

  for (auto miter = coverMemory.begin(maxLength, maxTops);
      miter != coverMemory.end(maxLength, maxTops); miter++)
  {
    assert(citer != covers.end());
    citer->prepare(lengths, tops, cases, * miter);

    if (citer->getWeight() == 0)
    {
      cout << "Covers::prepare: " << 
        +maxLength << ", " << +maxTops << "\n";
      cout << "Adding " << citer->str() << "\n";
      cout << "Adding " << miter->strRaw() << "\n";

for (unsigned i = 0; i< lengths.size(); i++)
  cout << i << " " << +lengths[i]<< " " << +tops[i]<< " " << +cases[i] << "\n";
cout <<endl;

      assert(citer->getWeight() != 0);
    }

    citer++;
  }

  covers.sort([](const Cover& cover1, const Cover& cover2)
  {
    return (cover1.getWeight() >= cover2.getWeight());
  });
}


void Covers::prune()
{
  for (auto citer = coversNew.begin(); citer != coversNew.end(); )
  {
    assert(! citer->empty()); // Would have to remove. Doesn't happen

    if (citer->full())
    {
      citer = coversNew.erase(citer);
      continue;
    }

    for (auto citer2 = next(citer); citer2 != coversNew.end(); )
    {
      if (! citer2->sameWeight(* citer))
        break;
      else if (citer2->sameTricks(* citer))
        citer2 = coversNew.erase(citer2);
      else
        citer2++;
    }
    citer++;
  }
}


void Covers::prepareNew(
  const vector<unsigned char>& lengths,
  vector<vector<unsigned> const *>& topPtrs,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const vector<unsigned char>& topTotals)
{
  timersStrat[20].start();
  list<CoverStackInfo> stack; // Unfinished expansions
  stack.emplace_back(CoverStackInfo(topTotals));

  coversNew.resize(COVER_CHUNK_SIZE);
  for (auto& c: coversNew)
    c.resize(topTotals.size());

  auto citer = coversNew.begin(); // Next one to write
  timersStrat[20].stop();

/*
cout << setw(4) << "t#" <<
  setw(4) << "tlo" <<
  setw(4) << "thi" <<
  setw(4) << "mW" <<
  setw(4) << "mE" <<
  setw(4) << "dif" << "\n";
  */

  timersStrat[21].start();
  while (! stack.empty())
  {
    auto stackIter = stack.begin();

    unsigned char topNumber = stackIter->topNext; // Next to write
    if (topNumber >= topTotals.size())
    {
// cout << "popped front, stack size now " << stack.size() << endl;
      stack.pop_front();
      continue;
    }

    const unsigned char topCountActual = topTotals[topNumber];

    for (unsigned char topCountLow = 0; 
        topCountLow <= topCountActual; topCountLow++)
    {
      for (unsigned char topCountHigh = topCountLow; 
        topCountHigh <= topCountActual; topCountHigh++)
      {
        const unsigned char minWest = stackIter->minWest + topCountLow;
        const unsigned char minEast = stackIter->minEast + 
          topCountActual - topCountHigh;

        unsigned char diff = topCountHigh - topCountLow;
        if (diff < stackIter->maxDiff)
          diff = stackIter->maxDiff;

/*
 cout << setw(4) << +topNumber <<
  setw(4) << +topCountLow <<
  setw(4) << +topCountHigh <<
  setw(4) << +minWest <<
  setw(4) << +minEast <<
  setw(4) << +diff << "\n";
  */

        if (minWest + diff > maxLength)
        {
          // There is no room for this worst-case single maximum,
          // so we skip the entire set, as there will be a more 
          // accurate other set.
          continue;
        }

        if (minEast + diff > maxLength)
          continue;

        if (minWest + minEast > maxLength)
          continue;

        // Never use the last top explicitly.  Maybe it shouldn't
        // be there at all, but it is.
        if (topNumber == 0 &&
            (topCountLow != 0 || topCountHigh != topCountActual))
          continue;

        // If there is an active top that in itself exceeds the 
        // length range,

        const bool usedFlag = 
          (topCountLow != 0 || topCountHigh != topCountActual);

        unsigned char maxWest, maxEast;
        if (usedFlag)
        {
          const unsigned char dtop = topCountActual - topCountLow;
          maxWest = max(topCountHigh, stackIter->maxWest);
          maxEast = max(dtop, stackIter->maxEast);
        }
        else
        {
          maxWest = stackIter->maxWest;
          maxEast = stackIter->maxEast;
        }


        // if (topCountHigh > stackIter->maxWest)
          // stackIter->maxWest = topCountHigh;

        // const unsigned char maxEast = topCountActual - topCountLow;
        // if (maxEast > stackIter->maxEast)
          // stackIter->maxEast = maxEast;

        stackIter->topsLow[topNumber] = topCountLow;
        stackIter->topsHigh[topNumber] = topCountHigh;

        if (citer == coversNew.end())
          assert(false);

        // Add the "don't care" with respect to length.
        citer->set(maxLength, 0, maxLength, 
          topTotals, stackIter->topsLow, stackIter->topsHigh);
// cout << citer->strLine();
        citer++;

        // Add the possible length constraints.
        const unsigned char lenMax = maxLength - minEast;

        for (unsigned char lLow = minWest; lLow <= lenMax; lLow++)
        {
          for (unsigned char lHigh = lLow; 
            lHigh <= lenMax; lHigh++)
          { 
            if (lLow == minWest && lHigh == lenMax)
              continue;

// cout << "len " << +lLow << " to " << +lHigh << 
  // ": maxWest " << +maxWest << "\n";
            // There is a tighter way to specify this cover.
            if (topNumber > 0 && lHigh < maxWest)
              continue;

            if (citer == coversNew.end())
            {

  cout << "OVERFLOW\n";
  cout << "Length " << +maxLength << ", ";
  for (auto t: topTotals)
    cout << +t << " ";
  cout << "\n";

  for (auto c: coversNew)
    cout << c.strLine();

              cout << "C End reached2" << endl;
              assert(false);
            }
            citer->set(maxLength, lLow, lHigh, 
              topTotals, stackIter->topsLow, stackIter->topsHigh);
// cout << citer->strLine();
            citer++;
          }
        }

        stackIter = stack.insert(stackIter, * stackIter);
        auto nextIter = next(stackIter);
        nextIter->minWest = minWest;
        nextIter->minEast = minEast;
        nextIter->maxDiff = diff;
        nextIter->maxWest = maxWest;
        nextIter->maxEast = maxEast;
        nextIter->topNext++;
// cout << "pushed, stack size now " << stack.size() << endl;
      }
    }
    assert(! stack.empty());
    stack.pop_front();
// cout << "popped, stack size now " << stack.size() << endl;
  }
  timersStrat[21].stop();

  timersStrat[22].start();
const unsigned sizeOld = coversNew.size();

  coversNew.erase(citer, coversNew.end());
  assert(! coversNew.empty());
  timersStrat[22].stop();

  cout << "Length " << +maxLength << ", ";
  for (auto t: topTotals)
    cout << +t << " ";
  cout << "\n";


  timersStrat[23].start();
  for (auto& c: coversNew)
    c.prepare(lengths, topPtrs, cases);
  timersStrat[23].stop();

  timersStrat[24].start();
  coversNew.sort([](const CoverNew& cover1, const CoverNew& cover2)
  {
    return cover1.earlier(cover2);
  });
  timersStrat[24].stop();

  /*
  cout << "Covers before pruning\n";
  cout << coversNew.front().strHeader();
  for (auto& c: coversNew)
    cout << c.strLine(maxLength, topTotals);
  cout << "\n";
  */

  // It is not practical to generate the covers without duplicated
  // trick vectors in one pass.  So we eliminate the more complex
  // ways of saying the same thing.  In total across all covers,
  // go from 354,822 to 225,028, so we need to eliminate about a third.

const unsigned sizeMid = coversNew.size();
  timersStrat[25].start();
  Covers::prune();
  timersStrat[25].stop();
  cout << "Used " << sizeOld << " -> " << sizeMid << " -> " <<
    coversNew.size() << "\n";

  /*
  cout << "Covers\n";
  cout << coversNew.front().strHeader();
  for (auto& c: coversNew)
    cout << c.strLine(maxLength, topTotals);
  cout << "\n";
  */
}


void Covers::setup(
  const list<Result>& results,
  vector<unsigned char>& tricks,
  unsigned char& tricksMin) const
{
  tricks.resize(results.size());
  tricksMin = UCHAR_NOT_SET;
  unsigned i = 0;

  for (auto& res: results)
  {
    tricks[i] = res.getTricks();
    if (tricks[i] < tricksMin)
      tricksMin = tricks[i];

    i++;
  }

  for (i = 0; i < tricks.size(); i++)
    tricks[i] -= tricksMin;
}


CoverState Covers::explain(
  const list<Result>& results,
  ResExpl& resExpl) const
{
  CoverState state = COVER_OPEN;
  auto iter = covers.begin();

  vector<unsigned char> tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);
  resExpl.setMinimum(tmin);

  while (true)
  {
    if (iter == covers.end())
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
  vector<unsigned char> tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  tableau.setTricks(tricks, tmin);

  auto citer = coversNew.begin();
  while (citer != coversNew.end())
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
  CoverTableau& tableau) const
{
  list<StackTableau> stack;
  stack.emplace_back(StackTableau());
  StackTableau& stableau = stack.back();

  vector<unsigned char> tricks;
  unsigned char tmin;
  Covers::setup(results, tricks, tmin);

  stableau.tableau.setTricks(tricks, tmin);

  stableau.coverIter = coversNew.begin();

  list<CoverTableau> solutions;

  auto siter = stack.begin();
  while (siter != stack.end())
  {
    auto& citer = siter->coverIter;

    while (citer != coversNew.end())
    {
      if (citer->getTopSize() > numStrategyTops)
      {
        citer++;
        continue;
      }

      siter->tableau.attemptExhaustive(citer, stack, solutions);
      citer++;
    }

    siter = stack.erase(siter);
  }

  assert(! solutions.empty());
  solutions.sort();

  // TODO Could perhaps swap
  tableau = solutions.front();

  // TODO Maybe MECE and hierarchy again within the tableau.
}


string Covers::strDebug(
  const string& title,
  const vector<unsigned char>& tricks) const
{
  stringstream ss;
  ss << title << "\n";

  for (unsigned i = 0; i < tricks.size(); i++)
    if (tricks[i])
      ss << i << ": " << +tricks[i] << "\n";
  ss << "\n";
  return ss.str();
}

