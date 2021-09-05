/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Splits.h"


Splits::Splits()
{
  Splits::reset();
}


void Splits::reset()
{
  own.clear();
  shared.clear();
  splits.clear();
  count = 0;
}


void Splits::setPointers()
{
  // Pointers to each of the own, unique partial Strategy's,
  // for later use.

  splits.resize(count);
  auto ownIter = own.begin();
  auto sharedIter = shared.begin();
  auto splitIter = splits.begin();

  for (unsigned i = 0; i < count; 
      i++, ownIter++, sharedIter++, splitIter++)
  {
    splitIter->ownPtr = &* ownIter;
    splitIter->sharedPtr = &* sharedIter;
  }
}


void Splits::split(
  Slist& slist,
  const Strategy& counterpart,
  const Ranges& ranges)
{
  count = slist.size();

  own.resize(count);
  shared.resize(count);

  slist.splitDistributions(counterpart, own, shared);

  Splits::setPointers();

  for (auto& strategy: own)
    strategy.scrutinize(ranges);
}


const list<Split>& Splits::splitStrategies() const
{
  return splits;
}

