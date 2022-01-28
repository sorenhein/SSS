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


ResExpl::ResExpl()
{
  ResExpl::reset();
}


void ResExpl::reset()
{
  tricksMin = 0;
  data.clear();
}


void ResExpl::append(
  Cover const * coverPtr,
  const unsigned char weight,
  const unsigned char numDist,
  const unsigned char level)
{
  data.emplace_back(ExplData());
  ExplData& ed = data.back();
  ed.coverPtr = coverPtr;
  ed.weight = weight;
  ed.numDist = numDist;
  ed.level = level;
}


void ResExpl::add(
  Cover const& cover,
  const unsigned char weight,
  const unsigned char numDist,
  const unsigned char level)
{
  ResExpl::append(&cover, weight, numDist, level);
}


string ResExpl::str() const
{
  return "";
}

