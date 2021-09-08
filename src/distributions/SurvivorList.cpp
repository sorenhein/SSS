/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "SurvivorList.h"
#include "DistHelp.h"


SurvivorList::SurvivorList()
{
  SurvivorList::clear();
}


void SurvivorList::clear()
{
  distNumbers.clear();
  reducedSize = 0;
}


void SurvivorList::resize(const unsigned len)
{
  distNumbers.resize(len);
}


void SurvivorList::push_back(const Survivor& survivor)
{
  distNumbers.push_back(survivor);
  reducedSize++;
}


const Survivor& SurvivorList::front() const
{
  return distNumbers.front();
}


list<Survivor>::const_iterator SurvivorList::begin() const
{
  return distNumbers.begin();
}


list<Survivor>::const_iterator SurvivorList::end() const
{
  return distNumbers.end();
}


bool SurvivorList::operator == (const SurvivorList& sl2) const
{
  if (reducedSize != sl2.reducedSize)
    return false;

  if (distNumbers.size() != distNumbers.size())
    return false;

  auto iter1 = distNumbers.begin();
  auto iter2 = sl2.distNumbers.begin();

  while (iter1 != distNumbers.end())
  {
    if (* iter1 != * iter2)
      return false;
  }
  return true;
}


void SurvivorList::setVoid(const unsigned char fullDistNo)
{
  SurvivorList::clear();
  // distNumbers.clear();
  // reducedSize = 0;
  SurvivorList::push_back({fullDistNo, 0});
  // reducedSize = 1;
}


void SurvivorList::collapse(
  const vector<SideInfo>& distCollapses,
  const SurvivorList& survivorsUnreduced)
{
  // Start by copying the uncollapsed list (if this is the first collapse)
  // or the once-collapsed list (if this is the second collapse).
  * this = survivorsUnreduced;

  // If there is no prospect of collapsing anything, continue.
  if (distNumbers.size() <= 1)
    return;

  auto iter = next(distNumbers.begin());
  unsigned char rankLastReduced = 0;
  unsigned char rankCurrentReduced;

  while (iter != distNumbers.end())
  {
    const unsigned char dno = iter->fullNo;

    // Look back until we run out of distributions with the same
    // length.  The distributions are not necessarily in perfect
    // order for a rank collapse, but they are in length order for sure.
    auto iterPrev = prev(iter);
    while (true)
    {
      const unsigned dnoPrev = iterPrev->fullNo;
      const SideCompare compare = distCollapses[dno].compare(
        distCollapses[dnoPrev]);

      if (compare == SC_DIFFERENT_LENGTH ||
          (compare == SC_DIFFERENT_VALUES &&
            iterPrev == distNumbers.begin()))
      {
        // There is no identical previous rank collapse.
        rankLastReduced++;
        rankCurrentReduced = rankLastReduced;
        break;
      }
      else if (compare == SC_SAME)
      {
        rankCurrentReduced = iterPrev->reducedNo;
        break;
      }
      else
        iterPrev--;
    }

    iter->reducedNo = rankCurrentReduced;
    iter++;
  }

  reducedSize = rankLastReduced + 1;
}


unsigned SurvivorList::sizeFull() const
{
  return distNumbers.size();
}


unsigned char SurvivorList::sizeReduced() const
{
  return reducedSize;
}


string SurvivorList::str() const
{
  stringstream ss;
  ss << "Survivor list\n";
  for (auto& s: distNumbers)
    ss << +s.fullNo << ", " << +s.reducedNo << "\n";
  return ss.str() + "\n";
}

