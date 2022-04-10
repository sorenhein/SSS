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
#include "CoverRowOld.h"
#include "ExplStats.h"


ResExpl::ResExpl()
{
  ResExpl::reset();
}


void ResExpl::reset()
{
  tricksMin = 0;
  data.clear();
}


void ResExpl::setParameters(
  const unsigned char tmin,
  const unsigned char maxTricksIn,
  const unsigned char maxTopsIn)
{
  tricksMin = tmin;
  maxLength = maxTricksIn;
  maxTops = maxTopsIn;
}


list<ExplData>::iterator ResExpl::dominator(const CoverRowOld& coverRow)
{
  // Returns data.end() if there is no dominator.
  // Returns the dominator with the highest level number,
  // and among these, the one with the lowest weight.

  list<ExplData>::iterator resIter = data.end();
  unsigned char levelBest = 0;
  unsigned weightBest = numeric_limits<unsigned>::max();

  for (auto iter = data.begin(); iter != data.end(); iter++)
  {
    if (! (coverRow <= * (iter->coverRowPtr)))
      continue;

    if (coverRow.getWeight() == iter->weight)
    {
      // TODO
      cout << "Don't know yet how to deal with repeats" << endl;
      cout << "Already have:\n";
      cout << ResExpl::str();
      cout << "Cover:\n";
      cout << coverRow.str() << "\n";
      cout << coverRow.strProfile() << "\n";
      cout << "iter:\n";
      cout << iter->coverRowPtr->str() << "\n";
      cout << iter->coverRowPtr->strProfile() << endl;
      assert(false);
    }

    if (resIter == data.end() ||
        iter->level > levelBest ||
        (iter->level == levelBest && iter->weight < weightBest))
    {
      resIter = iter;
      levelBest = iter->level;
      weightBest = iter->weight;
    }
  }

  return resIter;
}


void ResExpl::insert(CoverRowOld const& coverRow)
{
  auto domIter = ResExpl::dominator(coverRow);

  if (domIter == data.end())
  {
    data.emplace_back(ExplData());
    ExplData& ed = data.back();

    ed.coverRowPtr = &coverRow;
    ed.weight = coverRow.getWeight();
    ed.numDist = coverRow.getNumDist();
    ed.level = 0;
  }
  else
  {
    ExplData& ed = * data.emplace(next(domIter), ExplData());

    ed.coverRowPtr = &coverRow;
    ed.weight = coverRow.getWeight();
    ed.numDist = coverRow.getNumDist();
    ed.level = domIter->level + 1;
  }
}


bool ResExpl::empty() const
{
  return data.empty();
}


void ResExpl::updateStats(ExplStats& explStats) const
{
  // Look up the indices.
  if (data.empty())
    return;

  ExplStat& explStat = explStats.getEntry(maxLength, maxTops);
  explStat.incrLengths(data.size());

  for (auto iter = data.begin(); iter != data.end(); iter++)
  {
    const unsigned index = iter->coverRowPtr->index();
    explStat.incrSingles(index);

    for (auto iter2 = next(iter); iter2 != data.end(); iter2++)
    {
      const unsigned index2 = iter2->coverRowPtr->index();
      explStat.incrPairs(index, index2);
    }
  }
}


string ResExpl::str() const
{
  stringstream ss;
  ss << "Always take at least " << +tricksMin << 
    " tricks, and more when\n";

  string prefix;
  for (auto& ed: data)
  {
    if (ed.level == 0)
      prefix = "* ";
    else if (ed.level == 1)
      prefix = "  - ";
    else if (ed.level == 2)
      prefix = "    - ";
    else
      prefix = "      - ";

    // unsigned char length, tops1;
    // ed.coverRowPtr->getID(length, tops1);

    ss << prefix <<
      ed.coverRowPtr->str() << "\n";
  }

  return ss.str() + "\n";
}

