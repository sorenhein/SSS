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

// TMP
#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;


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
  ownPtrs.clear();
  count = 0;
}


void Splits::pushDistribution(
  Strategies& strats,
  const StratData& stratData)
{
  auto siter = strats.strategies.begin();
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
  ownPtrs.resize(count);
  auto ownIter = own.strategies.begin();
  for (unsigned i = 0; i < count; i++, ownIter++)
  {
// if (i == 0  || i == 1)
// {
  // cout << "setPointers: old " << i << " element\n";
  // cout << ownIter->str("here", true);
// }
    ownPtrs[i] = &* ownIter;
  }

  splits.resize(count);
  auto ownIterNew = own.strategies.begin();
  auto sharedIter = shared.strategies.begin();
  auto splitIter = splits.begin();
  for (unsigned i = 0; i < count; 
      i++, ownIterNew++, sharedIter++, splitIter++)
  {
    splitIter->ownPtr = &* ownIterNew;
    splitIter->sharedPtr = &* sharedIter;

// if (i == 0  || i == 1)
// {
  // cout << "setPointers: new " << i << " element\n";
  // cout << splitIter->ownPtr->str("here", true);
// }
  }

  // cout << endl;
  // assert(false);
}


void Splits::split(
  Strategies& strategies,
  const Strategy& counterpart,
  const Ranges& ranges)
{
  count = strategies.size();

  own.strategies.resize(count);
  shared.strategies.resize(count);

  Splits::splitDistributions(strategies, counterpart);

  Splits::setPointers();

  own.scrutinize(ranges);
}


const list<Strategy>& Splits::sharedStrategies() const
{
  return shared.strategies;
}


const list<Split>& Splits::splitStrategies() const
{
  return splits;
}


const Strategy& Splits::ownStrategy(const unsigned index) const
{
 return * ownPtrs[index];
}


bool Splits::lessEqualPrimary(
  const unsigned index1,
  const unsigned index2) const
{
  // TODO Aren't they always both empty or none empty?
  if (ownPtrs[index1]->empty())
    return (ownPtrs[index2]->empty() ? true : false);
  else if (ownPtrs[index2]->empty())
    return false;
  else
    return ownPtrs[index1]->lessEqualPrimaryScrutinized(* ownPtrs[index2]);
}


Compare Splits::comparePrimary(
  const unsigned index1,
  const unsigned index2) const
{
  if (ownPtrs[index1]->empty())
    return (ownPtrs[index2]->empty() ? WIN_EQUAL : WIN_FIRST);
  else if (ownPtrs[index2]->empty())
    return WIN_SECOND;
  else
    return ownPtrs[index1]->compareByProfile(* ownPtrs[index2]);
}


Compare Splits::compareSecondary(
  const unsigned index1,
  const unsigned index2) const
{
  // This assumes a primary identity, which could be tested with
  // ownPtrs[index1]->compareByProfile(* ownPtrs[index2]) == WIN_EQUAL

  // Can empty() happen?
  if (ownPtrs[index1]->empty())
    return (ownPtrs[index2]->empty() ? WIN_EQUAL : WIN_FIRST);
  else if (ownPtrs[index2]->empty())
    return WIN_SECOND;
  else
    return ownPtrs[index1]->compareSecondary(* ownPtrs[index2]);
}


unsigned Splits::weight(const unsigned index) const
{
  return ownPtrs[index]->weight();
}

