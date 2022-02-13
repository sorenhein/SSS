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

#include "../../strategies/result/Result.h"
#include "../../const.h"

// TODO Find a more elegant way
#define COVER_CHUNK_SIZE 1000


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


void Covers::prepareNew(
  const vector<unsigned char>& lengths,
  vector<vector<unsigned> const *>& topPtrs,
  const vector<unsigned char>& cases,
  const unsigned char maxLength,
  const vector<unsigned char>& topTotals)
{
  list<CoverStackInfo> stack; // Unfinished expansions
  // stack.emplace_back(CoverStackInfo(lengths.size(), maxLength));
  stack.emplace_back(CoverStackInfo(topTotals.size(), maxLength));

  coversNew.resize(COVER_CHUNK_SIZE);
  for (auto& c: coversNew)
    c.resize(topTotals.size());

  auto citer = coversNew.begin(); // Next one to write

  while (! stack.empty())
  {
    auto stackIter = stack.begin();

    unsigned char topNumber = stackIter->topNext; // Next to write
// cout << "Looking up " << +topNumber << " vs. " << comp.size() << endl;
    if (topNumber >= topTotals.size())
      break;

    const unsigned char topCountActual = topTotals[topNumber];

// cout << "top number " << +topNumber << ", count " <<
  // +topCountActual << endl;

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

        // If there is a top that in itself exceeds the length range,
        // there is a more economical version of this entry.
        if (topCountHigh > stackIter->maxWest)
          stackIter->maxWest = topCountHigh;

        const unsigned char maxEast = topCountActual - topCountLow;
        if (maxEast > stackIter->maxEast)
          stackIter->maxEast = maxEast;

assert(topNumber < stackIter->topsLow.size());
assert(topNumber < stackIter->topsHigh.size());

        stackIter->topsLow[topNumber] = topCountLow;
        stackIter->topsHigh[topNumber] = topCountHigh;

// cout << "top number " << +topNumber << ": (" << +topCountLow << ", " << +topCountHigh << ")" << endl;

        // Add the "don't care" with respect to length.
        if (citer == coversNew.end())
        {
          cout << "C End reached1" << endl;
          cout << "maxLength " << +maxLength << endl;
          cout << "lengths\n";
          for (unsigned j = 0; j < lengths.size(); j++)
            cout << j << ": " << +lengths[j] << endl;
          cout << "cases\n";
          for (unsigned j = 0; j < cases.size(); j++)
            cout << j << ": " << +cases[j] << endl;
          cout << "top totals\n";
          for (unsigned j = 0; j < topTotals.size(); j++)
            cout << j << ": " << +topTotals[j] << endl;
          cout << "top pointers\n";
          for (unsigned j = 0; j < topPtrs.size(); j++)
          {
            const auto& t = * topPtrs[j];
            for (unsigned k = 0; k < t.size(); k++)
              cout << k << ": " << +t[k] << endl;
          }

          cout << coversNew.begin()->strHeader();
          for (auto& c: coversNew)
            cout << c.strLine(maxLength);
          cout << endl;

          assert(false);
        }
// cout << "Adding top without length constraint" << endl;
        citer->set(maxLength, 0, maxLength, 
          stackIter->topsLow, stackIter->topsHigh);
// cout << "Added" << endl;
        citer++;

        // Add the possible length constraints.
        const unsigned char lenMax = maxLength - minEast;

// cout << "L  (" << +minWest << ", " << +lenMax << ")" << endl;

        for (unsigned char lenLow = minWest; lenLow <= lenMax; lenLow++)
        {
          for (unsigned char lenHigh = lenLow; 
            lenHigh <= lenMax; lenHigh++)
          { 
            if (lenLow == minWest && lenHigh == lenMax)
              continue;

 // cout << "  C  (" << +lenLow << ", " << +lenHigh << ")" << 
   // ", maxes " << +stackIter->maxWest << ", " << +stackIter->maxEast << "\n";
 // cout << "comp1: " << +stackIter->maxWest << " vs. " << +lenHigh << endl;
 // cout << "comp2: " << +stackIter->maxEast << " vs. " << +(length-lenLow) <<
   "\n";
            if (stackIter->maxWest > lenHigh)
              continue;
            if (stackIter->maxEast > maxLength - lenLow)
              continue;

// cout << "    storing\n";

            if (citer == coversNew.end())
            {
              cout << "C End reached2" << endl;
              assert(false);
            }
            citer->set(maxLength, lenLow, lenHigh, 
              stackIter->topsLow, stackIter->topsHigh);
            citer++;
          }
        }

        stackIter = stack.insert(stackIter, * stackIter);
        auto nextIter = next(stackIter);
        nextIter->minWest = minWest;
        nextIter->minEast = minEast;
        nextIter->maxDiff = diff;
        nextIter->topNext++;
      }
    }
    assert(! stack.empty());
    stack.pop_front();
  }

  assert(! coversNew.empty());
  cout << "Length " << +maxLength << ", ";
  for (auto t: topTotals)
    cout << +t << " ";
  cout << "\n";

  cout << coversNew.front().strHeader();
  for (auto cit = coversNew.begin(); cit != citer; cit++)
    cout << cit->strLine(maxLength);
  cout << "\n";
// cout << "DONE " << endl;
  
  /*
  for (auto cit = coversNew.begin(); cit != citer; cit++)
  {
    cit->prepare(lengths, topPtrs, cases);
    cout << cit->strLine(maxLength);
    cout << cit->strProfile();
  }
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

