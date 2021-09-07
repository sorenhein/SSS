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
    ss << +s.fullNo << ", " << +s.reducedNo << endl;
  return ss.str() + "\n";
}

