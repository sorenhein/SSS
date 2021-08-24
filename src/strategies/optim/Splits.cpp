/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Splits.h"
#include "../StratData.h"


Splits::Splits()
{
  Splits::reset();
}


Splits::~Splits()
{
}


void Splits::reset()
{
  own.reset();
  shared.reset();
  splits.clear();
  count = 0;
}


void Splits::pushDistribution(
  Strategies& strats,
  const StratData& stratData)
{
  auto siter = strats.slist.begin();
  for (auto& sd: stratData.data)
  {
    siter->push_back(* sd.iter);
    siter++;
  }
}


void Splits::splitDistributions(
  Strategies& strategies,
  const Strategy& counterpart)
{
  // Split our strategies by distribution into one group (own) with
  // those distributions that are unique to us, and another (shared)
  // with distributions that overlap.  This is relative to counterpart.

  // List of iterators to a Result of each Strategy in stratsToSplit.
  // All are in sync to point to a given distribution.
  // They move in sync down across the Strategy's.
  StratData stratData;
  stratData.data.resize(count);
  strategies.getLoopData(stratData);

  auto riter = counterpart.begin();
  while (true)
  {
    if (riter == counterpart.end())
    {
      // A unique distribution.
      Splits::pushDistribution(own, stratData);

      if (stratData.advance() == STRATSTATUS_END)
        break;
      else
        continue;
    }

    if (riter->dist() < stratData.dist())
    {
      // Distribution that is only in strat2.
      riter++;
      continue;
    }

    bool endFlag = false;
    while (stratData.dist() < riter->dist())
    {
      // A unique distribution.
      Splits::pushDistribution(own, stratData);

      if (stratData.advance() == STRATSTATUS_END)
      {
        endFlag = true;
        break;
      }
    }

    if (endFlag)
      break;

    if (riter->dist() == stratData.dist())
    {
      // A shared distribution.
      Splits::pushDistribution(shared, stratData);

      if (stratData.advance() == STRATSTATUS_END)
        break;
    }

    riter++;
  }
}


void Splits::setPointers()
{
  // Pointers to each of the own, unique partial Strategy's,
  // for later use.

  splits.resize(count);
  auto ownIter = own.slist.begin();
  auto sharedIter = shared.slist.begin();
  auto splitIter = splits.begin();

  for (unsigned i = 0; i < count; 
      i++, ownIter++, sharedIter++, splitIter++)
  {
    splitIter->ownPtr = &* ownIter;
    splitIter->sharedPtr = &* sharedIter;
  }
}


void Splits::split(
  Strategies& strategies,
  const Strategy& counterpart,
  const Ranges& ranges)
{
  count = strategies.size();

  own.slist.resize(count);
  shared.slist.resize(count);

  Splits::splitDistributions(strategies, counterpart);

  Splits::setPointers();

  own.scrutinize(ranges);
}


const list<Split>& Splits::splitStrategies() const
{
  return splits;
}

