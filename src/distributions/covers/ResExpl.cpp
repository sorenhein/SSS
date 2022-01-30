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
#include "Cover.h"
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


void ResExpl::setMinimum(const unsigned char tmin)
{
  tricksMin = tmin;
}


list<ExplData>::iterator ResExpl::dominator(const Cover& cover)
{
  // Returns data.end() if there is no dominator.
  // Returns the dominator with the highest level number,
  // and among these, the one with the lowest weight.

  list<ExplData>::iterator resIter = data.end();
  unsigned char levelBest = 0;
  unsigned weightBest = numeric_limits<unsigned>::max();

  for (auto iter = data.begin(); iter != data.end(); iter++)
  {
    if (! (cover <= * (iter->coverPtr)))
      continue;

    if (cover.getWeight() == iter->weight)
    {
      // TODO
      cout << "Don't know yet how to deal with repeats" << endl;
      cout << "Already have:\n";
      cout << ResExpl::str();
      cout << "Cover:\n";
      cout << cover.str() << "\n";
      cout << cover.strProfile() << "\n";
      cout << "iter:\n";
      cout << iter->coverPtr->str() << "\n";
      cout << iter->coverPtr->strProfile() << endl;
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


void ResExpl::insert(Cover const& cover)
{
  auto domIter = ResExpl::dominator(cover);

  if (domIter == data.end())
  {
    data.emplace_back(ExplData());
    ExplData& ed = data.back();

    ed.coverPtr = &cover;
    ed.weight = cover.getWeight();
    ed.numDist = cover.getNumDist();
    ed.level = 0;
  }
  else
  {
    ExplData& ed = * data.emplace(next(domIter), ExplData());

    ed.coverPtr = &cover;
    ed.weight = cover.getWeight();
    ed.numDist = cover.getNumDist();
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

  unsigned char lengthIndex, tops1Index;
  data.front().coverPtr->getIndices(lengthIndex, tops1Index);

  auto& singles = explStats.singles[lengthIndex][tops1Index];
  auto& pairs = explStats.pairs[lengthIndex][tops1Index];
  auto& lengths = explStats.lengths[lengthIndex][tops1Index];

  lengths[data.size()]++;

// if (lengthIndex == 5 && tops1Index == 3 && data.size() == 4)
  // cout << "HERE 5-3-4\n";

  for (auto iter = data.begin(); iter != data.end(); iter++)
  {
    const unsigned index = iter->coverPtr->index();
    assert(index < singles.size());
    singles[index]++;

    for (auto iter2 = next(iter); iter2 != data.end(); iter2++)
    {
      const unsigned index2 = iter2->coverPtr->index();
      pairs[index][index2]++;
      pairs[index2][index]++;
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

    unsigned char length, tops1;
    ed.coverPtr->getIndices(length, tops1);

    ss << prefix <<
      ed.coverPtr->str() << " [" <<
      +length << "-" << +tops1 << "-" << data.size() << "; " <<
      ed.coverPtr->index() << ": " <<
      +ed.numDist << ", " <<
      +ed.weight << "]\n";
  }

  return ss.str() + "\n";
}

