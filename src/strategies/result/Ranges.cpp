/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Ranges.h"
#include "Result.h"


Ranges::Ranges()
{
  Ranges::reset();
}


Ranges::~Ranges()
{
}


void Ranges::reset()
{
  ranges.clear();
  winnersFlag = false;
}


bool Ranges::empty() const
{
  return ranges.empty();
}


unsigned Ranges::size() const
{
  return ranges.size();
}


void Ranges::init(
  const list<Result>& results,
  const bool winnersFlagIn)
{
  winnersFlag = winnersFlagIn;

  if (results.empty())
  {
    ranges.clear();
    return;
  }

  ranges.resize(results.size());
  auto resIter = results.begin();

  for (auto& range: ranges)
  {
    range.init(* resIter);
    resIter++;
  }
}


void Ranges::extend(const list<Result>& results)
{
  assert(results.size() == ranges.size());
  if (results.empty())
    return;

  auto resIter = results.begin();

  for (auto& range: ranges)
  {
    range.extend(* resIter);
    resIter++;
  }
}


void Ranges::operator *= (const Ranges& r2)
{
  if (Ranges::empty())
  {
    if (! r2.empty())
    {
      ranges.assign(r2.ranges.begin(), r2.ranges.end());
      winnersFlag = r2.winnersFlag;
    }
    return;
  }
  
  if (r2.empty())
    return;

  auto iter1 = ranges.begin();
  auto iter2 = r2.ranges.begin();

  while (iter2 != r2.ranges.end())
  {
    if (iter1 == ranges.end() || iter1->dist() > iter2->dist())
    {
      ranges.insert(iter1, * iter2);
      iter2++;
    }
    else if (iter1->dist() < iter2->dist())
      iter1++;
    else
    {
      * iter1 *= * iter2;
      iter1++;
      iter2++;
    }
  }
}


string Ranges::strHeader() const
{
  if (ranges.empty())
    return "";
  else
    return ranges.front().strHeader(false);
}


string Ranges::str() const
{
  string s = "";
  for (auto& range: ranges)
    s += range.str(false);
  return s;
}
