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

  strategies.splitDistributions(counterpart, own, shared);

  Splits::setPointers();

  own.scrutinize(ranges);
}


const list<Split>& Splits::splitStrategies() const
{
  return splits;
}

