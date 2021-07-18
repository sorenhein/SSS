#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "RangesNew.h"
#include "Result.h"


RangesNew::RangesNew()
{
  RangesNew::reset();
}


RangesNew::~RangesNew()
{
  if (rangesPtr)
    delete rangesPtr;
}


void RangesNew::reset()
{
  if (rangesPtr)
    rangesPtr = nullptr;

  winnersFlag = false;
}


bool RangesNew::empty() const
{
  return (rangesPtr == nullptr || rangesPtr->empty());
}


void RangesNew::init(
  const list<Result>& results,
  const bool winnersFlagIn)
{
  winnersFlag = winnersFlagIn;

  if (winnersFlag)
    // TODO Differentiate
    rangesPtr = new list<Range>;
  else
    rangesPtr = new list<Range>;

  rangesPtr->resize(results.size());
  auto resIter = results.begin();

  for (auto& range: * rangesPtr)
  {
    range.init(* resIter);
    resIter++;
  }
}


void RangesNew::extend(const list<Result>& results)
{
  assert(rangesPtr);
  assert(results.size() == rangesPtr->size());

  auto resIter = results.begin();

  for (auto& range: * rangesPtr)
  {
    range.extend(* resIter);
    resIter++;
  }
}


void RangesNew::operator *= (const RangesNew& r2)
{
  if (rangesPtr == nullptr || rangesPtr->empty())
  {
    * this = r2;
    return;
  }
  
  if (r2.rangesPtr == nullptr)
    return;

  auto iter1 = rangesPtr->begin();
  auto iter2 = r2.rangesPtr->begin();

  while (iter2 != r2.rangesPtr->end())
  {
    if (iter1 == rangesPtr->end() || iter1->dist() > iter2->dist())
    {
      rangesPtr->insert(iter1, * iter2);
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


string RangesNew::str() const
{
  if (rangesPtr == nullptr)
    return "";

  string s = "";
  for (auto& range: * rangesPtr)
    s += range.str();
  return s;
}
