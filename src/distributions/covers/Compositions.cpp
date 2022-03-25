/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Compositions.h"


Compositions::Compositions()
{
  Compositions::reset();
}


void Compositions::reset()
{
  compositions.clear();
}


void Compositions::makeLength(const unsigned char length)
{
  compositions[length].resize(1 << (length-1));

  struct StackInfo
  {
    vector<unsigned char> tops;
    unsigned char lengthRunning;
    unsigned char topNext;

    StackInfo(const unsigned char numTops)
    {
      tops.resize(numTops);
      lengthRunning = 0;
      topNext = 0;
    };
  };

  list<StackInfo> stack; // Unfinished expansions
  stack.emplace_back(StackInfo(length));

  auto iter = compositions[length].begin();  // Next one to write

  while (! stack.empty())
  {
    auto stackIter = stack.begin();
    unsigned char topNumber = stackIter->topNext; // Next to write
    const unsigned char remaining = length - stackIter->lengthRunning;

    for (unsigned char topCount = 1; topCount <= remaining; topCount++)
    {
      stackIter->tops[topNumber] = topCount;
      if (topCount == remaining)
      {
        iter->set(stackIter->tops, topNumber);
        iter++;

      }
      else
      {
        stackIter = stack.insert(stackIter, * stackIter);
        next(stackIter)->topNext++;
        next(stackIter)->lengthRunning += topCount;
      }
    }

    stack.pop_front();
  }
}


void Compositions::make(const unsigned char maxLength)
{
  compositions.resize(maxLength+1);
   for (unsigned char length = 2; length <= maxLength; length++)
     Compositions::makeLength(length);
}


list<Profile>::const_iterator Compositions::begin(
  const unsigned char len) const
{
  assert(len >= 2 && len < compositions.size());
  return compositions[len].begin();
}


list<Profile>::const_iterator Compositions::end(
  const unsigned char len) const
{
  assert(len >= 2 && len < compositions.size());
  return compositions[len].end();
}


string Compositions::str(const unsigned char lenSpecific) const
{
  stringstream ss;

  unsigned char lenLow, lenHigh;
  if (lenSpecific == 0)
  {
    assert(compositions.size() > 2);
    lenLow = 2;
    lenHigh = static_cast<unsigned char>(compositions.size()-1);
  }
  else
  {
    assert(compositions.size() > lenSpecific);
    lenLow = lenSpecific;
    lenHigh = lenSpecific;
  }

  // ss << compositions[lenLow].front().strHeader(lenHigh);
  ss << compositions[lenLow].front().strHeader();
  for (unsigned char len = lenLow; len <= lenHigh; len++)
    for (auto& c: compositions[len])
      ss << c.strLine();

  return ss.str() + "\n";
}
